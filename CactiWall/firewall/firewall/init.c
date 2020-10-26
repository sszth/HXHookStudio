
#include "common.h"
#include "wall.h"
#include "crc32.h"
#include "memtrace.h"
#include "ctlcode.h"

#include<netioapi.h>

DRIVER_INITIALIZE   DriverEntry;
DRIVER_UNLOAD       DriverUnload;

__drv_dispatchType_other            DRIVER_DISPATCH     WallDispatchRequest;
__drv_dispatchType(IRP_MJ_CREATE)   DRIVER_DISPATCH     WallDispatchCreate;
__drv_dispatchType(IRP_MJ_CLOSE)    DRIVER_DISPATCH     WallDispatchClose;
__drv_dispatchType(IRP_MJ_CLEANUP)  DRIVER_DISPATCH     WallDispatchCleanup;
__drv_dispatchType(IRP_MJ_DEVICE_CONTROL) DRIVER_DISPATCH WallDispatchDeviceControl;


NTSTATUS
DriverEntry(
    IN  PDRIVER_OBJECT          driverObject,
    IN  PUNICODE_STRING         registryPath
    );

VOID
DriverUnload(
    IN  PDRIVER_OBJECT          driverObject
    );

//==========================global variables================================

PDEVICE_OBJECT              gDevObj = {0};
BOOLEAN                     gbMonitorOn = FALSE;
extern
    BOOLEAN gbEnableMonitor; // from wall.c

//==========================================================================

NTSTATUS
WallDispatchRequest (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    )
{
    LOG("into\n");

    return STATUS_SUCCESS;
}

NTSTATUS
WallDispatchCreate (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    )
{
    NTSTATUS        status = STATUS_SUCCESS;

    LOG("into\n");

    return STATUS_SUCCESS;
}

NTSTATUS
WallDispatchClose (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    )
{
    LOG("into\n");

    return STATUS_SUCCESS;
}

NTSTATUS
WallDispatchCleanup (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    )
{
    LOG("into\n");

    return STATUS_SUCCESS;
}

NTSTATUS
StartMonitor()
/*++
--*/
{
    NTSTATUS        status = STATUS_SUCCESS;
    BOOLEAN         bWorkerThread = FALSE;
    BOOLEAN         bConnList = FALSE;
    BOOLEAN         bPacketList = FALSE;
    BOOLEAN         bInjectHandle = FALSE;

    LOG("into\n");

    if( gbMonitorOn )
    {
        KdPrint(("monitor has started!\n"));
        return STATUS_SUCCESS;
    }

    status = CreateWorkerThread();
    if( !NT_SUCCESS(status) )
    {
        KdPrint(("Create Worker Thread failed!\n"));
        goto exit;
    }
    bWorkerThread = TRUE;
    
    status = WallCreateConnList();
    if( !NT_SUCCESS(status))
    {
        KdPrint(("Create conn list failed!\n"));
        goto exit;
    }
    bConnList = TRUE;

    status = WallCreatePacketList();
    if( !NT_SUCCESS(status))
    {
        KdPrint(("Create packet list failed!\n"));
        goto exit;
    }
    bPacketList = TRUE;

    status = WallCreateInjectionHandle();
    if( !NT_SUCCESS(status))
    {
        KdPrint(("Create Injection Handle failed!\n"));
        goto exit;
    }
    bInjectHandle = TRUE;

    crc32_init();

    CreateVolumeLinkTable();

    WallLoadConfig();

    status = WallRegisterCallouts();
    if( !NT_SUCCESS( status))
        goto exit;

exit:
    if( !NT_SUCCESS(status))
    {
        LOG("ERROR OCCURED!\n");

        if ( bWorkerThread )
            DestroyWorkerThread();

        if( bConnList )
            WallDestroyConnList();

        if( bPacketList )
            WallDestroyPacketList();

        if( bInjectHandle )
            WallDestroyInjectionHandle();

    }
    else
    {
        gbMonitorOn = TRUE;
        KdPrint(("monitor start success!\n"));
    }

    return status;
}

VOID
StopMonitor()
/*++
--*/
{
    LOG("into\n");

    if( !gbMonitorOn )
    {
        KdPrint(("monitor has already stopped!\n"));
        return;
    }

    WallUnRegisterCallouts();

    DestroyWorkerThread();

    WallDestroyConnList();

    WallDestroyPacketList();

    WallDestroyInjectionHandle();

    WallUnloadConfig();

#if DBG
    if( DbgIsMemLeak() )
        KdPrint(("Mem leak occured!\n"));
#endif

    gbMonitorOn = FALSE;
    KdPrint(("monitor has been stopped!\n"));
}

NTSTATUS
WallDispatchDeviceControl (
    IN PDEVICE_OBJECT deviceObject,
    IN PIRP irp
    )
{
    PIO_STACK_LOCATION  irpSp = NULL;

    LOG("into\n");

    irpSp = IoGetCurrentIrpStackLocation( irp );
    KdPrint(("inBuffer=%x\n",irpSp->Parameters.DeviceIoControl.Type3InputBuffer));

    switch( irpSp->Parameters.DeviceIoControl.IoControlCode )
    {
        case IOCTL_LOAD_PROCESS_CONFIG:
            KdPrint(("user request load process config!\n"));

            WallLoadProcessConfig();
            //WallLoadIpConfig();
            irp->IoStatus.Information = 0;
            irp->IoStatus.Status = STATUS_SUCCESS;

            break;
        case IOCTL_UNLOAD_PROCESS_CONFIG:
            KdPrint(("user request unload process config!\n"));

            WallUnloadProcessConfig();
            //WallLoadIpConfig();
            irp->IoStatus.Information = 0;
            irp->IoStatus.Status = STATUS_SUCCESS;

            break;
         case IOCTL_LOAD_IP_CONFIG:
            KdPrint(("user request load ip config!\n"));

            WallLoadIpConfig();
            irp->IoStatus.Information = 0;
            irp->IoStatus.Status = STATUS_SUCCESS;

            break;
        case IOCTL_UNLOAD_IP_CONFIG:
            KdPrint(("user request unload ip config!\n"));

            WallUnloadIpConfig();
            irp->IoStatus.Information = 0;
            irp->IoStatus.Status = STATUS_SUCCESS;

            break;
        case IOCTL_LOAD_DNS_CONFIG:
            KdPrint(("user request unload dns config!\n"));

            WallLoadDnsConfig();
            irp->IoStatus.Information = 0;
            irp->IoStatus.Status = STATUS_SUCCESS;

            break;
        case IOCTL_UNLOAD_DNS_CONFIG:
            KdPrint(("user request unload dns config!\n"));

            WallUnloadDnsConfig();
            irp->IoStatus.Information = 0;
            irp->IoStatus.Status = STATUS_SUCCESS;

            break;
        
        case IOCTL_MONITOR_ON:
            StartMonitor();

            irp->IoStatus.Information = 0;
            irp->IoStatus.Status = STATUS_SUCCESS;

            break;

        case IOCTL_MONITOR_OFF:
            StopMonitor();

            irp->IoStatus.Information = 0;
            irp->IoStatus.Status = STATUS_SUCCESS;

            break;
        case IOCTL_BLOCK_ALL:
            ASSERT( irpSp->Parameters.DeviceIoControl.InputBufferLength
                == sizeof(BOOLEAN));
            ASSERT( irpSp->Parameters.DeviceIoControl.Type3InputBuffer
                != NULL );
            WallBlockAll( *(PBOOLEAN)irpSp->Parameters.DeviceIoControl.Type3InputBuffer);

            irp->IoStatus.Information = 0;
            irp->IoStatus.Status = STATUS_SUCCESS;

            break;


        default:
            irp->IoStatus.Information = 0;
            irp->IoStatus.Status = STATUS_SUCCESS;
            break;
    }

    IoCompleteRequest( irp,IO_NO_INCREMENT );
    return STATUS_SUCCESS;
}

NTSTATUS
DriverEntry(
    IN  PDRIVER_OBJECT          driverObject,
    IN  PUNICODE_STRING         registryPath
    )
/*++
--*/
{
    int             i;
    NTSTATUS        status = STATUS_SUCCESS;
    UNICODE_STRING  deviceName = {0};
    UNICODE_STRING  deviceDosName = {0};

    LOG("into\n");

#if DBG
    DbgMemTraceInit();
#endif

    driverObject->DriverUnload = DriverUnload;
    for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
    {
        driverObject->MajorFunction[i] = WallDispatchRequest;
    }
    driverObject->MajorFunction[IRP_MJ_CREATE] = WallDispatchCreate;
    driverObject->MajorFunction[IRP_MJ_CLOSE] = WallDispatchClose;
    driverObject->MajorFunction[IRP_MJ_CLEANUP] = WallDispatchCleanup;
    driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = WallDispatchDeviceControl;


    RtlInitUnicodeString( &deviceName,DEVICE_NAME );
    status = IoCreateDevice( driverObject,
                            0,
                            &deviceName,
                            FILE_DEVICE_NETWORK,
                            0,
                            FALSE,
                            &gDevObj );
    if( !NT_SUCCESS(status))
        goto exit;

    RtlInitUnicodeString( &deviceDosName,DEVICE_DOSNAME );
    status = IoCreateSymbolicLink( &deviceDosName,&deviceName );
    if( !NT_SUCCESS( status ))
    {
        KdPrint(("Create Symbolink name failed!\n"));
        goto exit;
    }

    WallLoadGlobalConfig();
    if( gbEnableMonitor ){
        status = StartMonitor();
        if( !NT_SUCCESS(status) )
        {
            KdPrint(("Start monitor failed!\n"));
            goto exit;
        }
    }
exit:
    if( !NT_SUCCESS(status))
    {
        LOG("ERROR OCCURED!\n");

        if( gDevObj ){
            IoDeleteDevice( gDevObj );
            gDevObj = NULL;
        }

    }
    return status;
}

VOID
DriverUnload(
    IN  PDRIVER_OBJECT          driverObject
    )
{
    UNICODE_STRING  deviceDosName = {0};
    LOG("into\n");
    
    StopMonitor();
    
    if( gDevObj )
    {
        IoDeleteDevice( gDevObj );
        gDevObj = NULL;
    }

    RtlInitUnicodeString( &deviceDosName,DEVICE_DOSNAME );
    IoDeleteSymbolicLink( &deviceDosName);

#if DBG
    if( DbgIsMemLeak() )
        KdPrint(("Mem leak occured!\n"));
#endif

    return;
}

                                                                                                                                                                                                                                                          