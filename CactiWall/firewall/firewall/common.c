#include"common.h"
#include"crc32.h"
#include "memtrace.h"

UINT32   HashUnicodeString( IN PUNICODE_STRING s )
/*++
--*/
{
    return crc32_encode( (char*)s->Buffer,s->Length );
}

NTSTATUS
GetLocalTime( OUT PTIME_FIELDS  timeFields )
/*++
--*/
{
    NTSTATUS        status = STATUS_SUCCESS;
    LARGE_INTEGER   sysTime,locTime;

    KeQuerySystemTime( &sysTime );
    ExSystemTimeToLocalTime( &sysTime,&locTime );
    RtlTimeToTimeFields( &locTime,timeFields );
    
    return STATUS_SUCCESS;

}

VOLUME_LINK_ITEM    gVolumeLinkTable[26] = {0};// the arrray index is mapped to a....z

VOID
CreateVolumeLinkTable()
/*++
--*/
{
    NTSTATUS    status = STATUS_SUCCESS;
    OBJECT_ATTRIBUTES   objAttr;
    UNICODE_STRING      uniStrLinkName;
    UNICODE_STRING      uniStrTarName;
    HANDLE              linkHandle = NULL;
    WCHAR               linkNameBuffer[] = L"\\DosDevices\\c:";
    WCHAR               tarNameBuffer[MAX_PATH_LEN];
    ULONG               i;

    LOG("into\n");

    RtlInitEmptyUnicodeString( &uniStrTarName,tarNameBuffer,MAX_PATH_LEN * sizeof(WCHAR));
    RtlInitUnicodeString( &uniStrLinkName,linkNameBuffer );

    for( linkNameBuffer[12] = L'a';linkNameBuffer[12] <= L'z';linkNameBuffer[12]++ ) //12指向盘符在数组中序号
    {
        WCHAR       ch = linkNameBuffer[12];
        
        gVolumeLinkTable[ ch - L'a' ].VolumeLetter = ch;
        gVolumeLinkTable[ ch - L'a' ].crcMappedDevName = 0;

        InitializeObjectAttributes ( &objAttr,&uniStrLinkName,OBJ_KERNEL_HANDLE,NULL,NULL );
        status = ZwOpenSymbolicLinkObject(&linkHandle,GENERIC_READ,&objAttr );
        if( !NT_SUCCESS(status ))
        {
            //KdPrint(("open symbolic link(%wZ) object failed!\n",&uniStrLinkName));
            continue;
        }

        status = ZwQuerySymbolicLinkObject(linkHandle,&uniStrTarName,NULL );
        if( !NT_SUCCESS(status ))
        {
            //KdPrint(("query symbolic link(%wZ) object failed!\n",&uniStrLinkName));
            goto next;
        }
        for( i = 0;i < uniStrTarName.Length;i++)
            if( uniStrTarName.Buffer[i] >= L'A' && uniStrTarName.Buffer[i] <= L'Z' )
            {
                uniStrTarName.Buffer[i] = uniStrTarName.Buffer[i] - L'A' + L'a';
            }
        gVolumeLinkTable[ ch - L'a' ].crcMappedDevName = HashUnicodeString( &uniStrTarName );
next:
        ZwClose( linkHandle );
        linkHandle = NULL;
    }

    return;
}

NTSTATUS
DevicePathToDosPath(
    IN PUNICODE_STRING devicePath,
    OUT PUNICODE_STRING dosPath)
{
    NTSTATUS            status = STATUS_SUCCESS;
    HANDLE              linkHandle = NULL;
    ULONG               i,j,tmp;
    UCHAR               count=0;
    UINT32              crcDevName = 0;

    LOG("into\n");

    

    if( devicePath == NULL || dosPath == NULL ||
        devicePath->Buffer == NULL ||
        dosPath->Buffer == NULL )
    {
        status = STATUS_UNSUCCESSFUL;
        return status;
    }

    KdPrint(("1:devPath:%wZ\n",devicePath ));

    tmp = devicePath->Length;

    for( i = 0,count = 0;i < devicePath->Length / sizeof(WCHAR);i++)
    {
        if( devicePath->Buffer[i] == L'\\' )count++;
        if( count == 3 )break;
    }
    if( i == devicePath->Length / sizeof(WCHAR) )
    {
        status = STATUS_UNSUCCESSFUL;
        goto exit;
    }

    devicePath->Length = (USHORT)(i * sizeof(WCHAR));
    KdPrint((" i = %d,tmp = %d,devicePath.Volume = %wZ\n",i,tmp,devicePath ));

    if( (tmp - i * sizeof(WCHAR) + 2 * sizeof(WCHAR)) > devicePath->MaximumLength )
    {
        status = STATUS_BUFFER_TOO_SMALL;
        goto exit;
    }

    crcDevName = HashUnicodeString( devicePath );
    for( j = 0;j < 26;j++)
    {
        if( crcDevName == gVolumeLinkTable[j].crcMappedDevName )
            break;
    }

    if( 26 == j )
    {
        status = STATUS_UNSUCCESSFUL;
        goto exit;
    }


    RtlCopyMemory( dosPath->Buffer,&gVolumeLinkTable[j].VolumeLetter,sizeof(WCHAR));
    RtlCopyMemory( dosPath->Buffer + 1,L":" ,sizeof(WCHAR));
    RtlCopyMemory( dosPath->Buffer + 2,devicePath->Buffer + i,tmp - i * sizeof(WCHAR));
    dosPath->Length = (USHORT)(tmp - i * sizeof(WCHAR) + 2 * sizeof(WCHAR ));

exit:

    devicePath->Length = (USHORT)tmp;

    if( linkHandle != NULL )
        ZwClose( linkHandle );
    
    KdPrint(("2:devPath:%wZ\n",devicePath ));

    return status;
}

PWORKER_THREAD          gWorkerThread = NULL;
BOOLEAN                 bUnloading = FALSE;

KSTART_ROUTINE WorkerThreadProc;
VOID 
WorkerThreadProc(
    IN PVOID    Context
    )
/*++
--*/
{
    PWORKER_PROCESS_NODE    node;

    LOG("into\n");
    for(;;){
        
        KeWaitForSingleObject( &gWorkerThread->thread_event,
                           Executive,
                           KernelMode,
                           FALSE,
                           NULL);

        while( node = ( PWORKER_PROCESS_NODE )ExInterlockedRemoveHeadList(
                                                                    &gWorkerThread->list,
                                                                    &gWorkerThread->spinLock))
        {
            node->routineAddress( node->Context );
            MyExFreePool( node );
        }
        if( bUnloading )
            break;
    }

    LOG("worker thread exit.......!\n");

    return;
}

NTSTATUS
CreateWorkerThread(
    )
/*++
--*/
{
    PWORKER_THREAD      pWallWorkerThread = NULL;
    NTSTATUS            status;

    LOG("into\n");

    bUnloading = FALSE;
    pWallWorkerThread = MyExAllocatePool( sizeof( WORKER_THREAD));
    if( pWallWorkerThread == NULL ){
        KdPrint(("thread pool create failed!\n"));
        goto exit;
    }

    InitializeListHead( &pWallWorkerThread->list );
    KeInitializeSpinLock( &pWallWorkerThread->spinLock );
    KeInitializeEvent( &pWallWorkerThread->thread_event,SynchronizationEvent,FALSE );

    status = PsCreateSystemThread( &pWallWorkerThread->thread_id,
                                     THREAD_ALL_ACCESS,
                                     NULL,
                                     0,
                                     NULL,
                                     (PKSTART_ROUTINE)WorkerThreadProc,
                                     NULL);
    if( !NT_SUCCESS( status ))
    {
        KdPrint((" system thread create failed!\n"));
        MyExFreePool( pWallWorkerThread );
        goto exit;
    }

    gWorkerThread = pWallWorkerThread;

exit:
    return    status;
}

VOID
DestroyWorkerThread()
/*++
--*/
{
    LARGE_INTEGER       delayTime;

    LOG("into\n");

    ASSERT( gWorkerThread != NULL );

    bUnloading = TRUE;
    KeSetEvent( &gWorkerThread->thread_event,IO_NO_INCREMENT,FALSE );

    //等待一定的时间确认工作线程退出
    delayTime.QuadPart = 2 * DELAY_ONE_SECOND;
    KeDelayExecutionThread( KernelMode,FALSE,&delayTime );

    MyExFreePool( gWorkerThread );
    gWorkerThread = NULL;
}

NTSTATUS
RunMyProcess(
     MY_PROCESS    MyProcess,
     PVOID    Context
    )
/*++
--*/
{
    PWORKER_PROCESS_NODE        node;

    LOG("into\n");

    ASSERT( gWorkerThread != NULL );

    node = MyExAllocatePool( sizeof( WORKER_PROCESS_NODE ));
    if( node == NULL ){
        KdPrint(( " allocate node pool failed!\n"));
        return STATUS_UNSUCCESSFUL;
    }

    node->Context = Context;
    node->routineAddress = MyProcess;
    ExInterlockedInsertTailList(
                            &gWorkerThread->list,
                            &node->list,
                            &gWorkerThread->spinLock);

    KeSetEvent( &gWorkerThread->thread_event,IO_NO_INCREMENT,FALSE );

    return    STATUS_SUCCESS;
}

__inline
void
GetFlagsIndexesForLayer(
   IN UINT16 layerId,
   OUT UINT* flagsIndex
   )
{
   switch (layerId)
   {
   case FWPS_LAYER_ALE_AUTH_CONNECT_V4:
      *flagsIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_FLAGS;
      break;
   case FWPS_LAYER_ALE_AUTH_CONNECT_V6:
      *flagsIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_FLAGS;
      break;
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V4:
      *flagsIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_FLAGS;
      break;
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V6:
      *flagsIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_FLAGS;
      break;
   case FWPS_LAYER_OUTBOUND_TRANSPORT_V4:
      *flagsIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V4_FLAGS;
      break;
   case FWPS_LAYER_OUTBOUND_TRANSPORT_V6:
      *flagsIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V6_FLAGS;
      break;
   case FWPS_LAYER_INBOUND_TRANSPORT_V4:
      *flagsIndex = FWPS_FIELD_INBOUND_TRANSPORT_V4_FLAGS;
      break;
   case FWPS_LAYER_INBOUND_TRANSPORT_V6:
      *flagsIndex = FWPS_FIELD_INBOUND_TRANSPORT_V6_FLAGS;
      break;
   default:
      *flagsIndex = UINT_MAX;
      ASSERT(0);
      break;
   }
}

BOOLEAN IsAleReauthorize(
   IN const FWPS_INCOMING_VALUES0* inFixedValues
   )
{
   UINT flagsIndex;

   GetFlagsIndexesForLayer(
      inFixedValues->layerId,
      &flagsIndex
      );

   if((flagsIndex != UINT_MAX) && ((inFixedValues->incomingValue\
      [flagsIndex].value.uint32 & FWP_CONDITION_FLAG_IS_REAUTHORIZE) != 0))
   {
      return TRUE;
   }

   return FALSE;
}

NTSTATUS
UnicodeStringToUint32( IN PUNICODE_STRING str,OUT UINT32 *result )
/*
*/
{
    int         i,len;
    WCHAR       c;
    UINT32      tmp = 0;

    LOG("into\n");

    if( str == NULL || result == NULL)return STATUS_UNSUCCESSFUL;

    if( str->Buffer == NULL)return STATUS_UNSUCCESSFUL;
    
    len = str->Length / sizeof(WCHAR);
    if( len >  8 )return STATUS_UNSUCCESSFUL;
    
    for( i = 0;i < len;i++)
    {
        UCHAR   digit;

        c = str->Buffer[i];
        if( c >= L'0' && c <= L'9' )
        {
            digit = (UCHAR)(c - L'0');
        }
        else if( c >=L'a' && c <= L'f')
        {
            digit = (UCHAR)(c - L'a') + 10;
        }
        else if( c >= L'A' && c <= L'F')
        {
            digit = (UCHAR)(c - L'A') + 10;
        }
        else
            return STATUS_UNSUCCESSFUL;
        tmp <<= 4;
        tmp |= digit;
    }

    *result = tmp;
    return STATUS_SUCCESS;

}                                                                                                                                                                                                                                                                                        