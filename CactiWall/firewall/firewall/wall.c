#include<stdio.h>
#include<stdlib.h>

#include "common.h"
#include "callouts.h"
#include "wall.h"
#include "rules.h"

//////////////////////////引用的外部变量////////////////////////////////////

extern              PDEVICE_OBJECT  gDevObj;

////////////////////////////////////////////////////////////////////////////



///////////////////////////////模块全局变量定义//////////////////////////////
HANDLE              gEngineHandle = 0;
HANDLE              gInjectHandle = 0;
PWALL_CONN_LIST     gConnList = NULL;
PWALL_PACKET_LIST   gPacketList = NULL;
UINT32              gAleConnectCalloutId = 0;
UINT32              gAleRecvAcceptCalloutId = 0;
UINT64              gAleConnectFilterId = 0;
UINT64              gAleRecvAcceptFilterId = 0;
BOOLEAN             gbProcessConfig_other_allow = FALSE;
BOOLEAN             gbIpConfig_other_allow = TRUE;
BOOLEAN             gbDnsConfig_other_allow = TRUE;
BOOLEAN             gbBlockAll = FALSE;
BOOLEAN             gbEnableProcessMonitor=TRUE;
BOOLEAN             gbEnableIpMonitor=FALSE;
BOOLEAN             gbEnableDnsMonitor=FALSE;
BOOLEAN             gbEnableMonitor = FALSE;
WCHAR               gProcessLogFilePath[MAX_PATH_LEN]={0};

/////////////////////////////////////////////////////////////////////////////



///////////////////////////函数定义开始//////////////////////////////////////

NTSTATUS
RegisterCalloutForLayer(
   IN const GUID* layerKey,
   IN const GUID* calloutKey,
   IN FWPS_CALLOUT_CLASSIFY_FN classifyFn,
   IN FWPS_CALLOUT_NOTIFY_FN notifyFn,
   IN FWPS_CALLOUT_FLOW_DELETE_NOTIFY_FN flowDeleteNotifyFn,
   OUT UINT32* calloutId,
   OUT UINT64* filterId
   )
{
    NTSTATUS        status = STATUS_SUCCESS;

    FWPS_CALLOUT    sCallout = {0};

    FWPM_FILTER     mFilter = {0};
    FWPM_FILTER_CONDITION mFilter_condition[1] = {0};

    FWPM_CALLOUT    mCallout = {0};
    FWPM_DISPLAY_DATA mDispData = {0};

    BOOLEAN         bCalloutRegistered = FALSE; //用于失败退出时检测状态，正确释放已经申请的资源

    LOG("into\n");

    sCallout.calloutKey = *calloutKey;
    sCallout.classifyFn = classifyFn;
    sCallout.flowDeleteFn = flowDeleteNotifyFn;
    sCallout.notifyFn = notifyFn;

    status = FwpsCalloutRegister( gDevObj,&sCallout,calloutId );
    if( !NT_SUCCESS(status))
        goto exit;

    bCalloutRegistered = TRUE;

    mDispData.name = L"Wall ALE Callout";
    mDispData.description = L"Callout that capture the wall acquired event";

    mCallout.applicableLayer = *layerKey;
    mCallout.calloutKey = *calloutKey;
    mCallout.displayData = mDispData;

    status = FwpmCalloutAdd( gEngineHandle,&mCallout,NULL,NULL);
    if( !NT_SUCCESS(status))
        goto exit;

    mFilter.action.calloutKey = *calloutKey;
    mFilter.action.type = FWP_ACTION_CALLOUT_TERMINATING;
    mFilter.displayData.name = L"Wall Filter";
    mFilter.displayData.description = L"filter that used to capture the wall needed event";
    mFilter.layerKey = *layerKey;
    mFilter.numFilterConditions = 0;
    mFilter.filterCondition = mFilter_condition;
    mFilter.subLayerKey = FWPM_SUBLAYER_UNIVERSAL;
    mFilter.weight.type = FWP_EMPTY;
    
    //initialize mFilter_condition
    //...........................

    status = FwpmFilterAdd( gEngineHandle,&mFilter,NULL,filterId );

    if( !NT_SUCCESS( status))
        goto exit;

exit:
    if( !NT_SUCCESS(status))
    {
        LOG("ERROR OCCURED!\n");

        if( bCalloutRegistered )
        {
            FwpsCalloutUnregisterById( *calloutId );
        }
    }
    return status;
}

NTSTATUS
WallRegisterCallouts()
/*++
--*/
{
    NTSTATUS    status = STATUS_SUCCESS;

    //用于出错时正确销毁已经申请到的资源
    BOOLEAN     bInTransaction = FALSE;
    BOOLEAN     bEngineOpened = FALSE;

    FWPM_SESSION session = {0};

    LOG("into\n");

    session.flags = FWPM_SESSION_FLAG_DYNAMIC;
    
    status = FwpmEngineOpen( NULL,
                            RPC_C_AUTHN_WINNT,
                            NULL,
                            &session,
                            &gEngineHandle );
    if( !NT_SUCCESS(status))
        goto exit;
    bEngineOpened = TRUE;
    
    status = FwpmTransactionBegin( gEngineHandle,0 );
    if( !NT_SUCCESS(status))
        goto exit;

    bInTransaction = TRUE;

    status = RegisterCalloutForLayer( 
        &FWPM_LAYER_ALE_AUTH_CONNECT_V4 ,
        &WALL_ALE_AUTH_CONNECT_CALLOUT_V4,
        WallALEConnectClassify,
        WallALEConnectNotify,
        WallALEConnectFlowDelete,
        &gAleConnectCalloutId,
        &gAleConnectFilterId);
    if( !NT_SUCCESS(status))
        goto exit;

    status = RegisterCalloutForLayer( 
        &FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4 ,
        &WALL_ALE_AUTH_RECV_ACCEPT_CALLOUT_V4,
        WallALERecvAcceptClassify,
        WallALERecvAcceptNotify,
        WallALERecvAcceptFlowDelete,//没必要
        &gAleRecvAcceptCalloutId,
        &gAleRecvAcceptFilterId);
    if( !NT_SUCCESS(status))
        goto exit;

    status = FwpmTransactionCommit(gEngineHandle );
    if( !NT_SUCCESS(status))
        goto exit;

    bInTransaction = FALSE;

exit:
    if( !NT_SUCCESS(status))
    {
        LOG("ERROR OCCURED!\n");

        if( bInTransaction)
        {
            FwpmTransactionAbort( gEngineHandle );
        }

        if( bEngineOpened )
        {
            FwpmEngineClose( gEngineHandle );
            gEngineHandle = 0;
        }
    }

    return status;
}

NTSTATUS
WallUnRegisterCallouts()
/*++
--*/
{
    LOG("into\n");

    if( gEngineHandle != 0 )
    {
        FwpmFilterDeleteById( gEngineHandle,gAleConnectFilterId );
        FwpmFilterDeleteById( gEngineHandle,gAleRecvAcceptFilterId);
        FwpmCalloutDeleteById( gEngineHandle,gAleConnectCalloutId );
        FwpmCalloutDeleteById( gEngineHandle,gAleRecvAcceptCalloutId );

        FwpmEngineClose( gEngineHandle );
        gEngineHandle = 0;
        gAleRecvAcceptFilterId = 0;
        gAleRecvAcceptFilterId = 0;

    }
    FwpsCalloutUnregisterById( gAleConnectCalloutId );
    gAleConnectCalloutId = 0;
    FwpsCalloutUnregisterById( gAleRecvAcceptCalloutId );
    gAleRecvAcceptCalloutId = 0;

    return STATUS_SUCCESS;
}


VOID
WallWriteConnectLogData( IN PVOID Context )
/*++

注意：此函数运行在PASSIVE_LEVEL ，这里被工作线程调用
--*/
{
    NTSTATUS                    status = STATUS_SUCCESS;
    TIME_FIELDS                 time;
    WCHAR                       buffer[100];
    UNICODE_STRING              uniStr={0};
    OBJECT_ATTRIBUTES           objAttr={0};
    IO_STATUS_BLOCK             ioBlock;
    HANDLE                      hDatabaseLogFile;
    PUNICODE_STRING             data = (PUNICODE_STRING)Context;

    LOG("into\n");
    
    ASSERT( data != NULL );

    //KdPrint(("hash(%wZ)=%x\n",data,HashUnicodeString( data )));
    RtlInitUnicodeString( &uniStr,gProcessLogFilePath );
    InitializeObjectAttributes( &objAttr,&uniStr,OBJ_KERNEL_HANDLE,NULL,NULL );
    status = ZwCreateFile( &hDatabaseLogFile,
                            FILE_APPEND_DATA/*FILE_ALL_ACCESS*/,
                            &objAttr,
                            &ioBlock,
                            NULL,
                            FILE_ATTRIBUTE_NORMAL,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            FILE_OPEN_IF,
                            FILE_SYNCHRONOUS_IO_NONALERT,
                            NULL,
                            0);
    if( !NT_SUCCESS( status ))
    {
        KdPrint(("file create ERROR!\n"));
        goto exit;
    }

    GetLocalTime( &time );
#pragma warning(push)
#pragma warning(disable:28719)              //banned API
    swprintf( buffer,L"[%d-%d-%d-%d-%d-%d]",
            time.Year,
            time.Month,
            time.Day,
            time.Hour,
            time.Minute,
            time.Second);
#pragma warning(pop)

    status = ZwWriteFile( hDatabaseLogFile,
                          NULL,
                          NULL,
                          NULL,
                          &ioBlock,
                          buffer,
                          wcslen( buffer ) * sizeof(WCHAR),
                          NULL,
                          NULL);
    if( !NT_SUCCESS(status))
    {
        KdPrint(("Write to log file failed!\n"));
        goto exit;
    }

    status = ZwWriteFile( hDatabaseLogFile,
                          NULL,
                          NULL,
                          NULL,
                          &ioBlock,
                          data->Buffer,
                          data->Length,
                          NULL,
                          NULL);
    if( !NT_SUCCESS(status))
    {
        KdPrint(("Write to log file failed!\n"));
        goto exit;
    }

    status = ZwWriteFile( hDatabaseLogFile,
                          NULL,
                          NULL,
                          NULL,
                          &ioBlock,
                          L"\r\n",
                          2 * sizeof(WCHAR),
                          NULL,
                          NULL);
    if( !NT_SUCCESS(status))
    {
        KdPrint(("Write to log file failed!\n"));
        goto exit;
    }

exit:
    if( hDatabaseLogFile != NULL )
        ZwClose( hDatabaseLogFile );

    if( data != NULL )
        MyExFreePool( data );

    return;
}


NTSTATUS 
WallCreateConnList()
/*++
--*/
{
    PWALL_CONN_LIST p = NULL;
    NTSTATUS        status = STATUS_SUCCESS;

    LOG("into\n");

    p = (PWALL_CONN_LIST)MyExAllocatePool( sizeof( WALL_CONN_LIST));
    if( p == NULL )
    {
        status = STATUS_UNSUCCESSFUL;
        goto exit;
    }
    
    KeInitializeSpinLock( &p->lock );
    InitializeListHead( &p->list );
    gConnList = p;

exit:

    return status;
}

NTSTATUS 
WallCreatePacketList()
/*++
--*/
{
    PWALL_CONN_LIST p = NULL;
    NTSTATUS        status = STATUS_SUCCESS;

    LOG("into\n");

    p = (PWALL_CONN_LIST)MyExAllocatePool( sizeof( WALL_CONN_LIST));
    if( p == NULL )
    {
        status = STATUS_UNSUCCESSFUL;
        goto exit;
    }
    
    KeInitializeSpinLock( &p->lock );
    InitializeListHead( &p->list );
    gPacketList = p;

exit:

    return status;
}

VOID
WallDestroyConnList()
/*++
--*/
{
    LOG("into\n");

    if( gConnList == NULL)
        return;

    ASSERT( IsListEmpty( &gConnList->list ));
    MyExFreePool( gConnList );
    gConnList = NULL;

}

VOID
WallDestroyPacketList()
/*++
--*/
{
    LOG("into\n");

    if( gPacketList == NULL )
        return;

    ASSERT( IsListEmpty( &gPacketList->list ));
    MyExFreePool( gPacketList );
    gPacketList = NULL;
}

NTSTATUS
WallCreateInjectionHandle()
/*++
--*/
{
    NTSTATUS status = STATUS_SUCCESS;

    LOG("into\n");

    status = FwpsInjectionHandleCreate( AF_UNSPEC,FWPS_INJECTION_TYPE_TRANSPORT,&gInjectHandle);

    return status;
}

NTSTATUS
WallDestroyInjectionHandle()
/*++
--*/
{
    NTSTATUS status = STATUS_SUCCESS;

    LOG("into\n");

    status = FwpsInjectionHandleDestroy( gInjectHandle );
    if( NT_SUCCESS( status ))gInjectHandle = NULL;

    ASSERT( gInjectHandle == NULL);
    return status;
}

__inline
void
GetNetwork5TupleIndexesForLayer(
   IN UINT16 layerId,
   OUT UINT* localAddressIndex,
   OUT UINT* remoteAddressIndex,
   OUT UINT* localPortIndex,
   OUT UINT* remotePortIndex,
   OUT UINT* protocolIndex
   )
{
   switch (layerId)
   {
   case FWPS_LAYER_ALE_AUTH_CONNECT_V4:
      *localAddressIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_PROTOCOL;
      break;
   case FWPS_LAYER_ALE_AUTH_CONNECT_V6:
      *localAddressIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_PROTOCOL;
      break;
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V4:
      *localAddressIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_PROTOCOL;
      break;
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V6:
      *localAddressIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_PROTOCOL;
      break;
   case FWPS_LAYER_OUTBOUND_TRANSPORT_V4:
      *localAddressIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V4_IP_PROTOCOL;
      break;
   case FWPS_LAYER_OUTBOUND_TRANSPORT_V6:
      *localAddressIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V6_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V6_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V6_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V6_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_OUTBOUND_TRANSPORT_V6_IP_PROTOCOL;
      break;
   case FWPS_LAYER_INBOUND_TRANSPORT_V4:
      *localAddressIndex = FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_INBOUND_TRANSPORT_V4_IP_PROTOCOL;
      break;
   case FWPS_LAYER_INBOUND_TRANSPORT_V6:
      *localAddressIndex = FWPS_FIELD_INBOUND_TRANSPORT_V6_IP_LOCAL_ADDRESS;
      *remoteAddressIndex = FWPS_FIELD_INBOUND_TRANSPORT_V6_IP_REMOTE_ADDRESS;
      *localPortIndex = FWPS_FIELD_INBOUND_TRANSPORT_V6_IP_LOCAL_PORT;
      *remotePortIndex = FWPS_FIELD_INBOUND_TRANSPORT_V6_IP_REMOTE_PORT;
      *protocolIndex = FWPS_FIELD_INBOUND_TRANSPORT_V6_IP_PROTOCOL;
      break;
   default:
      *localAddressIndex = UINT_MAX;
      *remoteAddressIndex = UINT_MAX;
      *localPortIndex = UINT_MAX;
      *remotePortIndex = UINT_MAX;
      *protocolIndex = UINT_MAX;      
      ASSERT(0);
   }
}

void
FillNetwork5Tuple(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN ADDRESS_FAMILY addressFamily,
   IN OUT WALL_PENDED_PACKET* packet
   )
{
   UINT localAddrIndex;
   UINT remoteAddrIndex;
   UINT localPortIndex;
   UINT remotePortIndex;
   UINT protocolIndex;

   LOG("into\n");

   GetNetwork5TupleIndexesForLayer(
      inFixedValues->layerId,
      &localAddrIndex,
      &remoteAddrIndex,
      &localPortIndex,
      &remotePortIndex,
      &protocolIndex
      );

   if (addressFamily == AF_INET)
   {
      packet->ipv4LocalAddr = 
         RtlUlongByteSwap( /* host-order -> network-order conversion */
            inFixedValues->incomingValue[localAddrIndex].value.uint32
            );
      packet->ipv4RemoteAddr = 
         RtlUlongByteSwap( /* host-order -> network-order conversion */
            inFixedValues->incomingValue[remoteAddrIndex].value.uint32
            );
   }
   else
   {
      RtlCopyMemory(
         (UINT8*)&packet->localAddr,
         inFixedValues->incomingValue[localAddrIndex].value.byteArray16,
         sizeof(FWP_BYTE_ARRAY16)
         );
      RtlCopyMemory(
         (UINT8*)&packet->remoteAddr,
         inFixedValues->incomingValue[remoteAddrIndex].value.byteArray16,
         sizeof(FWP_BYTE_ARRAY16)
         );
   }

   packet->localPort = 
      RtlUshortByteSwap(
         inFixedValues->incomingValue[localPortIndex].value.uint16
         );
   packet->remotePort = 
      RtlUshortByteSwap(
         inFixedValues->incomingValue[remotePortIndex].value.uint16
         );
   packet->protocol = inFixedValues->incomingValue[protocolIndex].value.uint8;

   return;
}

__inline
void
GetDeliveryInterfaceIndexesForLayer(
   IN UINT16 layerId,
   OUT UINT* interfaceIndexIndex,
   OUT UINT* subInterfaceIndexIndex
   )
{
   switch (layerId)
   {
   case FWPS_LAYER_ALE_AUTH_CONNECT_V4:
#if (NTDDI_VERSION >= NTDDI_WIN6SP1)
      *interfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_CONNECT_V4_INTERFACE_INDEX;
      *subInterfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_CONNECT_V4_SUB_INTERFACE_INDEX;
#else
      //
      // Prior to Vista SP1, sub/interface-index are not being indicated to
      // ALE_AUTH_CONNECT layers. A callout driver would need to derive them
      // from the interface LUID using iphlpapi functions.
      //
      ASSERT(0);
#endif
      break;
   case FWPS_LAYER_ALE_AUTH_CONNECT_V6:
#if (NTDDI_VERSION >= NTDDI_WIN6SP1)
      *interfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_CONNECT_V6_INTERFACE_INDEX;
      *subInterfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_CONNECT_V6_SUB_INTERFACE_INDEX;
#else
      ASSERT(0);
#endif
      break;
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V4:
      *interfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_INTERFACE_INDEX;
      *subInterfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_SUB_INTERFACE_INDEX;
      break;
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V6:
      *interfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_INTERFACE_INDEX;
      *subInterfaceIndexIndex = 
         FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_SUB_INTERFACE_INDEX;
      break;
   case FWPS_LAYER_INBOUND_TRANSPORT_V4:
      *interfaceIndexIndex = 
         FWPS_FIELD_INBOUND_TRANSPORT_V4_INTERFACE_INDEX;
      *subInterfaceIndexIndex = 
         FWPS_FIELD_INBOUND_TRANSPORT_V4_SUB_INTERFACE_INDEX;
      break;
   case FWPS_LAYER_INBOUND_TRANSPORT_V6:
      *interfaceIndexIndex = 
         FWPS_FIELD_INBOUND_TRANSPORT_V6_INTERFACE_INDEX;
      *subInterfaceIndexIndex = 
         FWPS_FIELD_INBOUND_TRANSPORT_V6_SUB_INTERFACE_INDEX;
      break;
   default:
      ASSERT(0);
      break;
   }
}

BOOLEAN
IsMatchingConnectPacket(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN ADDRESS_FAMILY addressFamily,
   IN FWP_DIRECTION direction,
   IN WALL_PENDED_PACKET* pendedPacket
   )
{
   UINT localAddrIndex;
   UINT remoteAddrIndex;
   UINT localPortIndex;
   UINT remotePortIndex;
   UINT protocolIndex;

   ASSERT(pendedPacket->type == WALL_CONNECT_PACKET);

   LOG("into\n");

   GetNetwork5TupleIndexesForLayer(
      inFixedValues->layerId,
      &localAddrIndex,
      &remoteAddrIndex,
      &localPortIndex,
      &remotePortIndex,
      &protocolIndex
      );

   if(localAddrIndex == UINT_MAX)
   {
      return FALSE;
   }

   if (addressFamily != pendedPacket->addressFamily)
   {
      return FALSE;
   }

   if (direction != pendedPacket->direction)
   {
      return FALSE;
   }

   if (inFixedValues->incomingValue[protocolIndex].value.uint8 != 
       pendedPacket->protocol)
   {
      return FALSE;
   }

   if (RtlUshortByteSwap(
         inFixedValues->incomingValue[localPortIndex].value.uint16
         ) != pendedPacket->localPort)
   {
      return FALSE;
   }

   if (RtlUshortByteSwap(
         inFixedValues->incomingValue[remotePortIndex].value.uint16
         ) != pendedPacket->remotePort)
   {
      return FALSE;
   }

   if (addressFamily == AF_INET)
   {
      UINT32 ipv4LocalAddr = 
         RtlUlongByteSwap(
            inFixedValues->incomingValue[localAddrIndex].value.uint32
            );
      UINT32 ipv4RemoteAddr = 
         RtlUlongByteSwap( /* host-order -> network-order conversion */
            inFixedValues->incomingValue[remoteAddrIndex].value.uint32
            );
      if (ipv4LocalAddr != pendedPacket->ipv4LocalAddr)
      {
         return FALSE;
      }

      if (ipv4RemoteAddr != pendedPacket->ipv4RemoteAddr)
      {
         return FALSE;
      }
   }
   else
   {
      if (RtlCompareMemory(
            inFixedValues->incomingValue[localAddrIndex].value.byteArray16, 
            &pendedPacket->localAddr,
            sizeof(FWP_BYTE_ARRAY16)) !=  sizeof(FWP_BYTE_ARRAY16))
      {
         return FALSE;
      }

      if (RtlCompareMemory(
            inFixedValues->incomingValue[remoteAddrIndex].value.byteArray16, 
            &pendedPacket->remoteAddr,
            sizeof(FWP_BYTE_ARRAY16)) !=  sizeof(FWP_BYTE_ARRAY16))
      {
         return FALSE;
      }
   }

   return TRUE;
}


ADDRESS_FAMILY GetAddressFamilyForLayer(
   IN UINT16 layerId
   )
{
   ADDRESS_FAMILY addressFamily;

   LOG("into\n");

   switch (layerId)
   {
   case FWPS_LAYER_ALE_AUTH_CONNECT_V4:
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V4:
   case FWPS_LAYER_OUTBOUND_TRANSPORT_V4:
   case FWPS_LAYER_INBOUND_TRANSPORT_V4:
      addressFamily = AF_INET;
      break;
   case FWPS_LAYER_ALE_AUTH_CONNECT_V6:
   case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V6:
   case FWPS_LAYER_OUTBOUND_TRANSPORT_V6:
   case FWPS_LAYER_INBOUND_TRANSPORT_V6:
      addressFamily = AF_INET6;
      break;
   default:
      addressFamily = AF_UNSPEC;
      ASSERT(0);
   }

   return addressFamily;
}

WALL_PENDED_PACKET*
WallAllocateAndInitPendedPacket(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
   IN ADDRESS_FAMILY addressFamily,
   IN OUT void* layerData,
   IN WALL_PACKET_TYPE packetType,
   IN FWP_DIRECTION packetDirection
   )
/*++
功能描述：缓冲原始的packet，分配相关的PACKET结构变量

参数说明：
    inFixedValues：classifyOut回调中的inFixedValues
    inMetaValues：classifyOut回调中的inMetaValues
    addressFamily：IP地址类型，取值为AF_INET或者AF_INET6
    layerData:classifyOut回调中的layerData
    packetType:packet类型，取值为WALL_CONNECT_PACKET,WALL_DATA_PACKET或者WALL_REAUTH_PACKET
    packetDirection:packet方向，上行为:FWP_DIRECTION_OUTBOUND,下行为：FWP_DIRECTION_INBOUND;

返回值说明：
    所分配的packet的指针，将来必须用WallFreePendedPacket释放
    分配失败返回NULL
--*/
{
    NTSTATUS                status = STATUS_SUCCESS;
    WALL_PENDED_PACKET      *pendedPacket;
    UNICODE_STRING          devName,dosName;
    WCHAR                   buffer[MAX_PATH_LEN];

    LOG("into\n");

    // pendedPacket gets deleted in FreePendedPacket
    #pragma warning( suppress : 28197 )
    pendedPacket = MyExAllocatePool(sizeof( WALL_PENDED_PACKET));
    if (pendedPacket == NULL)
    {
       return NULL;
    }

    RtlZeroMemory(pendedPacket, sizeof(WALL_PENDED_PACKET));

    pendedPacket->type = packetType;
    pendedPacket->direction = packetDirection;

    pendedPacket->addressFamily = addressFamily;

    FillNetwork5Tuple(
       inFixedValues,
       addressFamily,
       pendedPacket
       );

    if (layerData != NULL)
    {
       pendedPacket->netBufferList = layerData;

       //
       // Reference the net buffer list to make it accessible outside of 
       // classifyFn.
       //
       FwpsReferenceNetBufferList0(pendedPacket->netBufferList, TRUE);
    }

    ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(inMetaValues, 
                                          FWPS_METADATA_FIELD_COMPARTMENT_ID));
    pendedPacket->compartmentId = inMetaValues->compartmentId;

    if ((pendedPacket->direction == FWP_DIRECTION_OUTBOUND) &&
        (layerData != NULL))
    {
       ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(
                   inMetaValues, 
                   FWPS_METADATA_FIELD_TRANSPORT_ENDPOINT_HANDLE));
       pendedPacket->endpointHandle = inMetaValues->transportEndpointHandle;

       pendedPacket->remoteScopeId = inMetaValues->remoteScopeId;

       if (FWPS_IS_METADATA_FIELD_PRESENT(
             inMetaValues, 
             FWPS_METADATA_FIELD_TRANSPORT_CONTROL_DATA))
       {
          ASSERT(inMetaValues->controlDataLength > 0);

          // pendedPacket->controlData gets deleted in FreePendedPacket
          #pragma warning( suppress : 28197 )
          pendedPacket->controlData = MyExAllocatePool(inMetaValues->controlDataLength);
         if (pendedPacket->controlData == NULL)
         {
            goto Exit;
         }

         RtlCopyMemory(
            pendedPacket->controlData,
            inMetaValues->controlData,
            inMetaValues->controlDataLength
            );

         pendedPacket->controlDataLength =  inMetaValues->controlDataLength;
      }
   }
   else if (pendedPacket->direction == FWP_DIRECTION_INBOUND)
   {
      UINT interfaceIndexIndex;
      UINT subInterfaceIndexIndex;

      GetDeliveryInterfaceIndexesForLayer(
         inFixedValues->layerId,
         &interfaceIndexIndex,
         &subInterfaceIndexIndex
         );

      pendedPacket->interfaceIndex = 
         inFixedValues->incomingValue[interfaceIndexIndex].value.uint32;
      pendedPacket->subInterfaceIndex = 
         inFixedValues->incomingValue[subInterfaceIndexIndex].value.uint32;
      
      ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(
               inMetaValues, 
               FWPS_METADATA_FIELD_IP_HEADER_SIZE));
      ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(
               inMetaValues, 
               FWPS_METADATA_FIELD_TRANSPORT_HEADER_SIZE));
      pendedPacket->ipHeaderSize = inMetaValues->ipHeaderSize;
      pendedPacket->transportHeaderSize = inMetaValues->transportHeaderSize;

      if (pendedPacket->netBufferList != NULL)
      {
         FWPS_PACKET_LIST_INFORMATION0 packetInfo = {0};
         FwpsGetPacketListSecurityInformation0(
            pendedPacket->netBufferList,
            FWPS_PACKET_LIST_INFORMATION_QUERY_IPSEC |
            FWPS_PACKET_LIST_INFORMATION_QUERY_INBOUND,
            &packetInfo
            );

         pendedPacket->ipSecProtected = 
            (BOOLEAN)packetInfo.ipsecInformation.inbound.isSecure;

         pendedPacket->nblOffset = 
            NET_BUFFER_DATA_OFFSET(\
               NET_BUFFER_LIST_FIRST_NB(pendedPacket->netBufferList));
      }
   }

    ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(inMetaValues, 
                                FWPS_METADATA_FIELD_PROCESS_PATH));

    RtlInitUnicodeString( &devName,(PWCHAR)inMetaValues->processPath->data );
    KdPrint(("tname:%wZ\nlayerData = %x\n",&devName,layerData ));
    RtlInitEmptyUnicodeString( &dosName,buffer,MAX_PATH_LEN * sizeof(WCHAR));

    status = DevicePathToDosPath( &devName,&dosName );//BUG!!!!  irql level must be passive level.
    if( status == STATUS_SUCCESS )
    {
        KdPrint(("PNAME:%wZ\n",&dosName ));
        pendedPacket->crcProcessPath = HashUnicodeString( &dosName );
    }
    else
    {
        KdPrint(("PNAME:%wZ\n",&devName ));
        pendedPacket->crcProcessPath = HashUnicodeString( &devName );
    }
    KdPrint(("crc = %x\n",pendedPacket->crcProcessPath ));

    return pendedPacket;

Exit:

    if (pendedPacket != NULL)
    {
        LOG("free packet\n");
       WallFreePendedPacket(pendedPacket);
    }

    return NULL;
}

VOID
WallFreePendedPacket( IN PWALL_PENDED_PACKET packet )
/*++
功能说明：释放被缓冲的packet

参数说明：
    packet:指向所释放packet的指针

返回值说明：
    无返回值
--*/
{
    LOG("into\n");

   if (packet->netBufferList != NULL)
   {
      FwpsDereferenceNetBufferList0(packet->netBufferList, FALSE);
   }
   if (packet->controlData != NULL)
   {
      MyExFreePool(packet->controlData);
   }
   if (packet->completionContext != NULL)
   {
      //ASSERT(packet->type == WALL_CONNECT_PACKET);
      //ASSERT(packet->direction == FWP_DIRECTION_INBOUND); // complete for ALE connect
                                                          // is done prior to freeing
                                                          // of the packet.
      FwpsCompleteOperation0(packet->completionContext, NULL);
   }
   MyExFreePool(packet);
    
}

VOID
NTAPI
WallInspectInjectComplete(
   IN VOID * context,
   IN OUT NET_BUFFER_LIST* netBufferList,
   IN BOOLEAN dispatchLevel
   )
/*++
功能描述：注入完成事件的回调函数，用来做清除资源的工作

参数说明：
    context:指向注入完成packet的指针
    netBufferList:克隆的NET_BUFFER_LIST结构
    dispatchLevel：执行的中断级，此处未用

返回值说明：无返回值

--*/
{
   WALL_PENDED_PACKET* packet = context;

   UNREFERENCED_PARAMETER(dispatchLevel);   

   LOG("into\n");

   FwpsFreeCloneNetBufferList0(netBufferList, 0);

   LOG("free packet\n");
   WallFreePendedPacket(packet);
}


NTSTATUS
WallInspectCloneReinjectOutbound(
   IN WALL_PENDED_PACKET* packet
   )
/* ++

   This function clones the outbound net buffer list and reinject it back.

-- */
{
   NTSTATUS status = STATUS_SUCCESS;

   NET_BUFFER_LIST* clonedNetBufferList = NULL;
   FWPS_TRANSPORT_SEND_PARAMS0 sendArgs = {0};

   LOG("into\n");

   status = FwpsAllocateCloneNetBufferList0(
               packet->netBufferList,
               NULL,
               NULL,
               0,
               &clonedNetBufferList
               );
   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   sendArgs.remoteAddress = (UINT8*)(&packet->remoteAddr);
   sendArgs.remoteScopeId = packet->remoteScopeId;
   sendArgs.controlData = packet->controlData;
   sendArgs.controlDataLength = packet->controlDataLength;
   //
   // Send-inject the cloned net buffer list.
   //

   status = FwpsInjectTransportSendAsync0(
               gInjectHandle,
               NULL,
               packet->endpointHandle,
               0,
               &sendArgs,
               packet->addressFamily,
               packet->compartmentId,
               clonedNetBufferList,
               WallInspectInjectComplete,
               packet
               );

   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   clonedNetBufferList = NULL; // ownership transferred to the 
                               // completion function.

Exit:

   if (clonedNetBufferList != NULL)
   {
      FwpsFreeCloneNetBufferList0(clonedNetBufferList, 0);
   }

   return status;
}

NTSTATUS
WallInspectCloneReinjectInbound(
   IN OUT WALL_PENDED_PACKET* packet
   )
/* ++

   This function clones the inbound net buffer list and, if needed, 
   rebuild the IP header to remove the IpSec headers and receive-injects 
   the clone back to the tcpip stack.

-- */
{
   NTSTATUS status = STATUS_SUCCESS;

   NET_BUFFER_LIST* clonedNetBufferList = NULL;
   NET_BUFFER* netBuffer;
   ULONG nblOffset;

   LOG("into\n");

   //
   // For inbound net buffer list, we can assume it contains only one 
   // net buffer.
   //
   netBuffer = NET_BUFFER_LIST_FIRST_NB(packet->netBufferList);
   
   nblOffset = NET_BUFFER_DATA_OFFSET(netBuffer);

   //
   // The TCP/IP stack could have retreated the net buffer list by the 
   // transportHeaderSize amount; detect the condition here to avoid
   // retreating twice.
   //
   if (nblOffset != packet->nblOffset)
   {
      ASSERT(packet->nblOffset - nblOffset == packet->transportHeaderSize);
      packet->transportHeaderSize = 0;
   }

   //
   // Adjust the net buffer list offset to the start of the IP header.
   //
   NdisRetreatNetBufferDataStart(
      netBuffer,
      packet->ipHeaderSize + packet->transportHeaderSize,
      0,
      NULL
      );

   //
   // Note that the clone will inherit the original net buffer list's offset.
   //

   status = FwpsAllocateCloneNetBufferList0(
               packet->netBufferList,
               NULL,
               NULL,
               0,
               &clonedNetBufferList
               );

   //
   // Undo the adjustment on the original net buffer list.
   //

   NdisAdvanceNetBufferDataStart(
      netBuffer,
      packet->ipHeaderSize + packet->transportHeaderSize,
      FALSE,
      NULL
      );

   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   if (packet->ipSecProtected)
   {
      //
      // When an IpSec protected packet is indicated to AUTH_RECV_ACCEPT or 
      // INBOUND_TRANSPORT layers, for performance reasons the tcpip stack
      // does not remove the AH/ESP header from the packet. And such 
      // packets cannot be recv-injected back to the stack w/o removing the
      // AH/ESP header. Therefore before re-injection we need to "re-build"
      // the cloned packet.
      // 
#if (NTDDI_VERSION >= NTDDI_WIN6SP1)

      status = FwpsConstructIpHeaderForTransportPacket0(
                  clonedNetBufferList,
                  packet->ipHeaderSize,
                  packet->addressFamily,
                  (UINT8*)&packet->remoteAddr, 
                  (UINT8*)&packet->localAddr,  
                  packet->protocol,
                  0,
                  NULL,
                  0,
                  0,
                  NULL,
                  0,
                  0
                  );
#else
      ASSERT(FALSE); // Prior to Vista SP1, IP address needs to be updated 
                     // manually (including updating IP checksum).

      status = STATUS_NOT_IMPLEMENTED;
#endif

      if (!NT_SUCCESS(status))
      {
         goto Exit;
      }
   }

   if (packet->completionContext != NULL)
   {
      //ASSERT(packet->type == WALL_CONNECT_PACKET);

      FwpsCompleteOperation0(
         packet->completionContext,
         clonedNetBufferList
         );

      packet->completionContext = NULL;
   }

   status = FwpsInjectTransportReceiveAsync0(
               gInjectHandle,
               NULL,
               NULL,
               0,
               packet->addressFamily,
               packet->compartmentId,
               packet->interfaceIndex,
               packet->subInterfaceIndex,
               clonedNetBufferList,
               WallInspectInjectComplete,
               packet
               );

   if (!NT_SUCCESS(status))
   {
      goto Exit;
   }

   clonedNetBufferList = NULL; // ownership transferred to the 
                               // completion function.

Exit:

   if (clonedNetBufferList != NULL)
   {
      FwpsFreeCloneNetBufferList0(clonedNetBufferList, 0);
   }

   return status;
}


VOID
WallInspectWallPackets( IN PVOID Context )
/*++
功能描述：处理被缓冲的packet

参数说明：
    Context:未定义

返回值说明：无返回值

注意：此函数在工作线程中执行，中断级为PASSIVE_LEVEL
--*/
{
    PWALL_PENDED_PACKET     packet = NULL;
    KIRQL                   irql,irql2;
    PLIST_ENTRY             listEntry = NULL;
    NTSTATUS                status = STATUS_SUCCESS;
    LOG("into\n");
    

    for( ;!IsListEmpty( &gConnList->list ) || !IsListEmpty( &gPacketList->list);)
    {
        listEntry = NULL;
        
        if( !IsListEmpty( &gConnList->list ))
        {
            KeAcquireSpinLock( &gConnList->lock,&irql );
            listEntry = gConnList->list.Flink;
            if(((PWALL_PENDED_PACKET)listEntry)->netBufferList != NULL)
            {
                ((PWALL_PENDED_PACKET)listEntry)->type = WALL_DATA_PACKET;
                //((PWALL_PENDED_PACKET)listEntry)->completionContext = NULL;
                RemoveEntryList( listEntry );
            }
            KeReleaseSpinLock( &gConnList->lock,irql );
        }

        if( listEntry == NULL && !IsListEmpty( &gPacketList->list ))
        {
            KeAcquireSpinLock( &gPacketList->lock,&irql2 );
            listEntry = gPacketList->list.Flink;
            RemoveEntryList( listEntry );
            KeReleaseSpinLock( &gPacketList->lock,irql2 );
        }

        ASSERT(listEntry != NULL );

        packet = (PWALL_PENDED_PACKET)listEntry;

        if( gbBlockAll )
            packet->authConnectDecision = FWP_ACTION_BLOCK;
        else if( gbEnableProcessMonitor && !WallIsProcessTrafficPermit(packet))
            packet->authConnectDecision = FWP_ACTION_BLOCK;
        else if ( gbEnableIpMonitor && !WallIsIpTrafficPermit(packet))
            packet->authConnectDecision = FWP_ACTION_BLOCK;
        else if( gbEnableDnsMonitor && !WallIsDnsTrafficPermit( packet ))
            packet->authConnectDecision = FWP_ACTION_BLOCK;
        else
            packet->authConnectDecision = FWP_ACTION_PERMIT;

        if( packet->type == WALL_CONNECT_PACKET )
        {
            if( packet->authConnectDecision == FWP_ACTION_PERMIT )
                FwpsCompleteOperation( packet->completionContext,NULL);
            else
            {
                KeAcquireSpinLock( &gConnList->lock,&irql );
                RemoveEntryList( &packet->list );
                KeReleaseSpinLock( &gConnList->lock,irql );

                WallFreePendedPacket( packet );
                packet = NULL;
                
            }

        }
        else
        {
            ASSERT( packet->type == WALL_DATA_PACKET );

            if( packet->direction == FWP_DIRECTION_OUTBOUND )
            {
                
                FwpsCompleteOperation( packet->completionContext,NULL);
                packet->completionContext = NULL;
                
                if( packet->authConnectDecision == FWP_ACTION_PERMIT )
                    status = WallInspectCloneReinjectOutbound( packet );
                else
                {
                    WallFreePendedPacket( packet );
                    packet = NULL;
                }
            }
            else
            {
                if( packet->authConnectDecision == FWP_ACTION_PERMIT )
                    status = WallInspectCloneReinjectInbound( packet );
                else
                {
                    packet->completionContext = NULL;
                    WallFreePendedPacket( packet );
                    packet = NULL;
                }
            }
            ASSERT(NT_SUCCESS(status));
        }

    }
}

VOID    WallLoadGlobalConfig()
/*++
功能描述：加载全局配置数据

参数说明：无

返回值说明：无
--*/
{
    NTSTATUS                status;
    HANDLE                  hCactiKey = 0,hGlobalRulesKey = 0;
    OBJECT_ATTRIBUTES       KeyObjAttr;
    UNICODE_STRING          RegDirectory;
    ULONG                   i;
    UCHAR                   RetInfor[ 88+ sizeof( KEY_FULL_INFORMATION ) + MAX_PATH_LEN * sizeof( WCHAR) ];//NOTICE!
    PKEY_FULL_INFORMATION   pKeyFullInfor;
    PKEY_BASIC_INFORMATION  pKeyBasicInfor;
    PKEY_VALUE_PARTIAL_INFORMATION pKeyValuePartialInfor;
    PKEY_VALUE_FULL_INFORMATION    pKeyValueFullInfor;
    ULONG                   retLength = 0;
    ULONG                   subKeys = 0,KeyValues = 0;
    UNICODE_STRING          uniKeyName;
    UNICODE_STRING          uniValueName;

    LOG("into\n");

    RtlInitUnicodeString( &RegDirectory,CACTIWALL_REG_DIRECTORY );
    InitializeObjectAttributes( &KeyObjAttr,
                                &RegDirectory,
                                OBJ_KERNEL_HANDLE,
                                0,
                                NULL);
    status = ZwCreateKey( &hCactiKey,
                          KEY_ALL_ACCESS,
                          &KeyObjAttr,
                          0,
                          NULL,
                          0,
                          NULL);
    if( !NT_SUCCESS( status ) ){
        KdPrint(("Create key 1 failed!\n"));
        return;
    }

    RtlInitUnicodeString( &RegDirectory,L"globalrules" );
    InitializeObjectAttributes( &KeyObjAttr,
                                &RegDirectory,
                                OBJ_KERNEL_HANDLE,
                                hCactiKey,
                                NULL);
    status = ZwCreateKey( &hGlobalRulesKey,
                          KEY_ALL_ACCESS,
                          &KeyObjAttr,
                          0,
                          NULL,
                          0,
                          NULL);
    if( !NT_SUCCESS( status ) ){
        KdPrint(("Create key 2 failed!\n"));
        ZwClose( hCactiKey );
        hCactiKey = NULL;
        return;
    }

    RtlInitUnicodeString( &uniValueName,L"processmonitorenable");
    status = ZwQueryValueKey( hGlobalRulesKey,
                                  &uniValueName,
                                  KeyValuePartialInformation,
                                  &RetInfor,
                                  sizeof( RetInfor ),
                                  &retLength );
    if( !NT_SUCCESS( status ))
    {
        KdPrint(("query value key(ProcessMonitorEnable) failed!(%x)\n",status));
        gbEnableProcessMonitor = TRUE;
    }
    else
    {
        pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
        ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
        if( *(UINT32 *)pKeyValuePartialInfor->Data == 0 )
            gbEnableProcessMonitor = FALSE;
        else
            gbEnableProcessMonitor = TRUE;
        KdPrint(("global_config:gbEnableProcessMonitor = %x\n",gbEnableProcessMonitor ));
    }

    RtlInitUnicodeString( &uniValueName,L"IpMonitorEnable");
    status = ZwQueryValueKey( hGlobalRulesKey,
                                  &uniValueName,
                                  KeyValuePartialInformation,
                                  &RetInfor,
                                  sizeof( RetInfor ),
                                  &retLength );
    if( !NT_SUCCESS( status ))
    {
        KdPrint(("query value key(IpMonitorEnable) failed!\n"));
        gbEnableIpMonitor = FALSE;
    }
    else
    {
        pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
        ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
        if( *(UINT32 *)pKeyValuePartialInfor->Data == 0 )
            gbEnableIpMonitor = FALSE;
        else
            gbEnableIpMonitor = TRUE;
        KdPrint(("global_config:gbEnableIpMonitor = %x\n",gbEnableIpMonitor ));
    }

    RtlInitUnicodeString( &uniValueName,L"DnsMonitorEnable");
    status = ZwQueryValueKey( hGlobalRulesKey,
                                  &uniValueName,
                                  KeyValuePartialInformation,
                                  &RetInfor,
                                  sizeof( RetInfor ),
                                  &retLength );
    if( !NT_SUCCESS( status ))
    {
        KdPrint(("query value key(DnsMonitorEnable) failed!\n"));
        gbEnableDnsMonitor = FALSE;
    }
    else
    {
        pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
        ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
        if( *(UINT32 *)pKeyValuePartialInfor->Data == 0 )
            gbEnableDnsMonitor = FALSE;
        else
            gbEnableDnsMonitor = TRUE;
        KdPrint(("global_config:gbEnableDnsMonitor = %x\n",gbEnableDnsMonitor ));
    }

    RtlInitUnicodeString( &uniValueName,L"MonitorEnable");
    status = ZwQueryValueKey( hGlobalRulesKey,
                                  &uniValueName,
                                  KeyValuePartialInformation,
                                  &RetInfor,
                                  sizeof( RetInfor ),
                                  &retLength );
    if( !NT_SUCCESS( status ))
    {
        KdPrint(("query value key(MonitorEnable) failed!\n"));
        gbEnableMonitor = FALSE;
    }
    else
    {
        pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
        ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
        if( *(UINT32 *)pKeyValuePartialInfor->Data == 0 )
            gbEnableMonitor = FALSE;
        else
            gbEnableMonitor = TRUE;
        KdPrint(("global_config:gbEnableMonitor = %x\n",gbEnableMonitor ));
    }

    RtlInitUnicodeString( &uniValueName,L"ProcessLogFile");
    status = ZwQueryValueKey( hGlobalRulesKey,
                                  &uniValueName,
                                  KeyValuePartialInformation,
                                  RetInfor,
                                  sizeof( RetInfor ),
                                  &retLength );
    if( !NT_SUCCESS( status ))
    {
        KdPrint(("query value key(ProcessLogFile) failed!\n"));
        gbEnableIpMonitor = FALSE;
    }
    else
    {
        pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
        ASSERT( pKeyValuePartialInfor->Type == REG_SZ );
        if( pKeyValuePartialInfor->DataLength > MAX_PATH_LEN * sizeof(WCHAR))
        {
            //缓冲区长度检查
            KdPrint(("log file path is too long!\n"));
            wcscpy_s( gProcessLogFilePath,MAX_PATH_LEN,L"c:\\wall_processlog.db");
        }
        else
        {
            RtlCopyMemory( gProcessLogFilePath,pKeyValuePartialInfor->Data,pKeyValuePartialInfor->DataLength);

        }
        KdPrint(("global_config:gProcessLogFilePath = %ws\n",gProcessLogFilePath ));
    }

    ZwClose( hGlobalRulesKey );
    hGlobalRulesKey = NULL;
    ZwClose(  hCactiKey );
    hCactiKey = NULL;

    return;
}

NTSTATUS    WallLoadProcessConfig()
/*++
功能说明：载入进程规则

参数说明：无

返回值：成功返回STATUS_SUCCESS

--*/
{
    NTSTATUS                status;
    HANDLE                  hCactiKey = 0,hProcessRulesKey = 0;
    OBJECT_ATTRIBUTES       KeyObjAttr;
    UNICODE_STRING          RegDirectory;
    ULONG                   i;
    UCHAR                   RetInfor[ sizeof( KEY_FULL_INFORMATION ) + 88 ];//NOTICE!
    PKEY_FULL_INFORMATION   pKeyFullInfor;
    PKEY_BASIC_INFORMATION  pKeyBasicInfor;
    PKEY_VALUE_PARTIAL_INFORMATION pKeyValuePartialInfor;
    PKEY_VALUE_FULL_INFORMATION    pKeyValueFullInfor;
    ULONG                    retLength = 0;
    ULONG                    subKeys = 0,KeyValues = 0;
    UNICODE_STRING          uniKeyName;
    UNICODE_STRING          uniValueName;
    UINT32                  crcProcessPath;
    UINT32                  rule;

    LOG("into\n");

    InitProcessRules();

    RtlInitUnicodeString( &RegDirectory,CACTIWALL_REG_DIRECTORY );
    InitializeObjectAttributes( &KeyObjAttr,
                                &RegDirectory,
                                OBJ_KERNEL_HANDLE,
                                0,
                                NULL);
    status = ZwCreateKey( &hCactiKey,
                          KEY_ALL_ACCESS,
                          &KeyObjAttr,
                          0,
                          NULL,
                          0,
                          NULL);
    if( !NT_SUCCESS( status ) ){
        KdPrint(("Create key 1 failed!\n"));
        return status;
    }

    RtlInitUnicodeString( &RegDirectory,L"processrules" );
    InitializeObjectAttributes( &KeyObjAttr,
                                &RegDirectory,
                                OBJ_KERNEL_HANDLE,
                                hCactiKey,
                                NULL);
    status = ZwCreateKey( &hProcessRulesKey,
                          KEY_ALL_ACCESS,
                          &KeyObjAttr,
                          0,
                          NULL,
                          0,
                          NULL);
    if( !NT_SUCCESS( status ) ){
        KdPrint(("Create key 2 failed!\n"));
        return status;
    }

    RtlInitUnicodeString( &uniValueName,L"other_access");
    status = ZwQueryValueKey( hProcessRulesKey,
                                  &uniValueName,
                                  KeyValuePartialInformation,
                                  &RetInfor,
                                  sizeof( RetInfor ),
                                  &retLength );
    if( !NT_SUCCESS( status ))
    {
        KdPrint(("query value key(other_access) failed!\n"));
        gbProcessConfig_other_allow = TRUE;
    }
    else
    {
        pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
        ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
        if( *(UINT32 *)pKeyValuePartialInfor->Data == 1 )
            gbProcessConfig_other_allow = TRUE;
        else
            gbProcessConfig_other_allow = FALSE;
    }


    status = ZwQueryKey( hProcessRulesKey,
                         KeyFullInformation,
                         &RetInfor,
                         sizeof( RetInfor ),
                         &retLength );
    if( !NT_SUCCESS( status ) ){
        KdPrint(( "QueryKeyfullinfor failed!\n"));
        return status;
    }
    pKeyFullInfor = ( PKEY_FULL_INFORMATION )RetInfor;
    subKeys = pKeyFullInfor->SubKeys;
    
    for( i = 0;i < subKeys; i++)
    {
        HANDLE  hKey;

        RtlZeroMemory( &RetInfor,sizeof(RetInfor));
        status = ZwEnumerateKey( hProcessRulesKey,
                                      i,
                                      KeyBasicInformation,
                                      &RetInfor,
                                      sizeof( RetInfor ),
                                      &retLength);
        if( !NT_SUCCESS( status ) ){
            KdPrint(("Enumerate value key failed!--%d\n",i));
            continue;
        }
        pKeyBasicInfor = (PKEY_BASIC_INFORMATION)RetInfor;
        uniKeyName.Length = uniKeyName.MaximumLength = (USHORT)pKeyBasicInfor->NameLength;
        uniKeyName.Buffer = pKeyBasicInfor->Name;
        UnicodeStringToUint32( &uniKeyName,&crcProcessPath );
        
        InitializeObjectAttributes( &KeyObjAttr,
                                &uniKeyName,
                                OBJ_KERNEL_HANDLE,
                                hProcessRulesKey,
                                NULL);
        status = ZwCreateKey( &hKey,
                                KEY_ALL_ACCESS,
                                &KeyObjAttr,
                                0,
                                NULL,
                                0,
                                NULL);
        if( !NT_SUCCESS( status ) ){
            KdPrint(("create hKey failed!--%d\n",i));
            continue;
        }
        
        RtlInitUnicodeString( &uniValueName,L"rule");
        status = ZwQueryValueKey( hKey,
                                  &uniValueName,
                                  KeyValuePartialInformation,
                                  &RetInfor,
                                  sizeof( RetInfor ),
                                  &retLength );
        if( !NT_SUCCESS( status ))
        {
            KdPrint(("query value key failed!--%d\n",i));
            continue;
        }

        pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
        ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
        rule = *(UINT32 *)pKeyValuePartialInfor->Data;
        
        ZwClose( hKey );
        hKey = NULL;

        KdPrint(("crcProcessPath = 0x%x,rule = %x\n",crcProcessPath,rule ));
        AddProcessRule( crcProcessPath,rule );

    }

    ZwClose( hProcessRulesKey );
    ZwClose( hCactiKey );

    gbEnableProcessMonitor = TRUE;

    return STATUS_SUCCESS;

}

NTSTATUS
WallLoadIpConfig()
/*++
功能说明：载入IP规则

参数说明：无

返回值：成功返回STATUS_SUCCESS

--*/
{
    NTSTATUS                status;
    HANDLE                  hCactiKey = 0,hIpRulesKey = 0;
    OBJECT_ATTRIBUTES       KeyObjAttr;
    UNICODE_STRING          RegDirectory;
    ULONG                   i;
    UCHAR                   RetInfor[ sizeof( KEY_FULL_INFORMATION ) + 88 ];//NOTICE!
    PKEY_FULL_INFORMATION   pKeyFullInfor;
    PKEY_BASIC_INFORMATION  pKeyBasicInfor;
    PKEY_VALUE_PARTIAL_INFORMATION pKeyValuePartialInfor;
    PKEY_VALUE_FULL_INFORMATION    pKeyValueFullInfor;
    ULONG                   retLength = 0;
    ULONG                   subKeys = 0,KeyValues = 0;
    UNICODE_STRING          uniKeyName;
    UNICODE_STRING          uniValueName;
    UINT32                  crcIpRuleName;
    UINT32                  rule;
    IP_RULES_ELEM           elem = {0};

    LOG("into\n");

    InitIpRules();

    RtlInitUnicodeString( &RegDirectory,CACTIWALL_REG_DIRECTORY );
    InitializeObjectAttributes( &KeyObjAttr,
                                &RegDirectory,
                                OBJ_KERNEL_HANDLE,
                                0,
                                NULL);
    status = ZwCreateKey( &hCactiKey,
                          KEY_ALL_ACCESS,
                          &KeyObjAttr,
                          0,
                          NULL,
                          0,
                          NULL);
    if( !NT_SUCCESS( status ) ){
        KdPrint(("Create key 1 failed!\n"));
        return status;
    }

    RtlInitUnicodeString( &RegDirectory,L"iprules" );
    InitializeObjectAttributes( &KeyObjAttr,
                                &RegDirectory,
                                OBJ_KERNEL_HANDLE,
                                hCactiKey,
                                NULL);
    status = ZwCreateKey( &hIpRulesKey,
                          KEY_ALL_ACCESS,
                          &KeyObjAttr,
                          0,
                          NULL,
                          0,
                          NULL);
    if( !NT_SUCCESS( status ) ){
        KdPrint(("Create key 2 failed!\n"));
        return status;
    }

    RtlInitUnicodeString( &uniValueName,L"other_access");
    status = ZwQueryValueKey( hIpRulesKey,
                                  &uniValueName,
                                  KeyValuePartialInformation,
                                  &RetInfor,
                                  sizeof( RetInfor ),
                                  &retLength );
    if( !NT_SUCCESS( status ))
    {
        KdPrint(("query value key(other_access) failed!\n"));
        gbIpConfig_other_allow = TRUE;
    }
    else
    {
        pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
        ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
        if( *(UINT32 *)pKeyValuePartialInfor->Data == 0 )
            gbIpConfig_other_allow = FALSE;
        else
            gbIpConfig_other_allow = TRUE;
        KdPrint(("ip_config:other_access = %x\n",gbIpConfig_other_allow ));
    }


    status = ZwQueryKey( hIpRulesKey,
                         KeyFullInformation,
                         &RetInfor,
                         sizeof( RetInfor ),
                         &retLength );
    if( !NT_SUCCESS( status ) ){
        KdPrint(( "QueryKeyfullinfor failed!\n"));
        return status;
    }
    pKeyFullInfor = ( PKEY_FULL_INFORMATION )RetInfor;
    subKeys = pKeyFullInfor->SubKeys;
    
    for( i = 0;i < subKeys; i++)
    {
        HANDLE  hKey = NULL;
        PIP_RULES_ELEM ipRulesElem = NULL;

        RtlZeroMemory( &RetInfor,sizeof(RetInfor));
        status = ZwEnumerateKey( hIpRulesKey,
                                      i,
                                      KeyBasicInformation,
                                      &RetInfor,
                                      sizeof( RetInfor ),
                                      &retLength);
        if( !NT_SUCCESS( status ) ){
            KdPrint(("Enumerate value key failed!--%d\n",i));
            continue;
        }
        pKeyBasicInfor = (PKEY_BASIC_INFORMATION)RetInfor;
        uniKeyName.Length = uniKeyName.MaximumLength = (USHORT)pKeyBasicInfor->NameLength;
        uniKeyName.Buffer = pKeyBasicInfor->Name;
        UnicodeStringToUint32( &uniKeyName,&crcIpRuleName );
        
        InitializeObjectAttributes( &KeyObjAttr,
                                &uniKeyName,
                                OBJ_KERNEL_HANDLE,
                                hIpRulesKey,
                                NULL);
        status = ZwCreateKey( &hKey,
                                KEY_ALL_ACCESS,
                                &KeyObjAttr,
                                0,
                                NULL,
                                0,
                                NULL);
        if( !NT_SUCCESS( status ) ){
            KdPrint(("create hKey failed!--%d\n",i));
            continue;
        }

        ipRulesElem = &elem;
        RtlZeroMemory( ipRulesElem,sizeof(IP_RULES_ELEM));
        
        RtlInitUnicodeString( &uniValueName,L"rule");
        status = ZwQueryValueKey( hKey,
                                  &uniValueName,
                                  KeyValuePartialInformation,
                                  &RetInfor,
                                  sizeof( RetInfor ),
                                  &retLength );
        if( !NT_SUCCESS( status ))
        {
            KdPrint(("query value key failed!--%d\n",i));
            MyExFreePool( ipRulesElem );
            continue;
        }

        pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
        ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
        rule = *(UINT32 *)pKeyValuePartialInfor->Data;
        
        ipRulesElem->crcRuleName = crcIpRuleName;
        ipRulesElem->rule.u32 = rule;
        
        //根据rule的相关位设置加载相应字段

        if( ipRulesElem->rule.Bits.LocalAddrType != AnyAddr )
        {
            //读入地址1
            RtlInitUnicodeString( &uniValueName,L"LocalAddr");
            status = ZwQueryValueKey( hKey,
                                      &uniValueName,
                                      KeyValuePartialInformation,
                                      &RetInfor,
                                      sizeof( RetInfor ),
                                      &retLength );
            if( !NT_SUCCESS( status ))
            {
                KdPrint(("query value key failed!(LocalAddr)\n"));
            }
            else
            {
                pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
                ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
                ipRulesElem->LocalAddr = *(UINT32 *)pKeyValuePartialInfor->Data;

            }

            if( ipRulesElem->rule.Bits.LocalAddrType == RangeAddr )
            {
                //读入地址2
                RtlInitUnicodeString( &uniValueName,L"LocalAddr2");
                status = ZwQueryValueKey( hKey,
                                          &uniValueName,
                                          KeyValuePartialInformation,
                                          &RetInfor,
                                          sizeof( RetInfor ),
                                          &retLength );
                if( !NT_SUCCESS( status ))
                {
                    KdPrint(("query value key failed!(LocalAddr)\n"));
                }
                else
                {
                    pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
                    ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
                    ipRulesElem->LocalAddr2 = *(UINT32 *)pKeyValuePartialInfor->Data;
                }
            }
        }//end if LocalAddr

        if( ipRulesElem->rule.Bits.RemoteAddrType != AnyAddr )
        {
            //读入地址1
            RtlInitUnicodeString( &uniValueName,L"RemoteAddr");
            status = ZwQueryValueKey( hKey,
                                      &uniValueName,
                                      KeyValuePartialInformation,
                                      &RetInfor,
                                      sizeof( RetInfor ),
                                      &retLength );
            if( !NT_SUCCESS( status ))
            {
                KdPrint(("query value key failed!(LocalAddr)\n"));
            }
            else
            {
                pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
                ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
                ipRulesElem->RemoteAddr = *(UINT32 *)pKeyValuePartialInfor->Data;
            }
            if( ipRulesElem->rule.Bits.RemoteAddrType == RangeAddr )
            {
                //读入地址2
                RtlInitUnicodeString( &uniValueName,L"RemoteAddr2");
                status = ZwQueryValueKey( hKey,
                                          &uniValueName,
                                          KeyValuePartialInformation,
                                          &RetInfor,
                                          sizeof( RetInfor ),
                                          &retLength );
                if( !NT_SUCCESS( status ))
                {
                    KdPrint(("query value key failed!(LocalAddr)\n"));
                }
                else
                {
                    pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
                    ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
                    ipRulesElem->RemoteAddr2 = *(UINT32 *)pKeyValuePartialInfor->Data;
                }
            }
        } //end if RemoteAddr

        if( ipRulesElem->rule.Bits.LocalPortType != AnyAddr )
        {
            //读入端口1
            RtlInitUnicodeString( &uniValueName,L"LocalPort");
            status = ZwQueryValueKey( hKey,
                                      &uniValueName,
                                      KeyValuePartialInformation,
                                      &RetInfor,
                                      sizeof( RetInfor ),
                                      &retLength );
            if( !NT_SUCCESS( status ))
            {
                KdPrint(("query value key failed!(LocalAddr)\n"));
            }
            else
            {
                pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
                ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
                ipRulesElem->LocalPort = *(UINT16 *)pKeyValuePartialInfor->Data;
            }

            if( ipRulesElem->rule.Bits.LocalPortType == RangeAddr )
            {
                //读入端口2
                RtlInitUnicodeString( &uniValueName,L"LocalPort2");
                status = ZwQueryValueKey( hKey,
                                          &uniValueName,
                                          KeyValuePartialInformation,
                                          &RetInfor,
                                          sizeof( RetInfor ),
                                          &retLength );
                if( !NT_SUCCESS( status ))
                {
                    KdPrint(("query value key failed!(LocalAddr)\n"));
                }
                else
                {
                    pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
                    ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
                    ipRulesElem->LocalPort2 = *(UINT16 *)pKeyValuePartialInfor->Data;
                }
            }
        }//end if LocalPort

        if( ipRulesElem->rule.Bits.RemotePortType != AnyAddr )
        {
            //读入端口1
            RtlInitUnicodeString( &uniValueName,L"RemotePort");
            status = ZwQueryValueKey( hKey,
                                      &uniValueName,
                                      KeyValuePartialInformation,
                                      &RetInfor,
                                      sizeof( RetInfor ),
                                      &retLength );
            if( !NT_SUCCESS( status ))
            {
                KdPrint(("query value key failed!(LocalAddr)\n"));
            }
            else
            {
                pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
                ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
                ipRulesElem->RemotePort = *(UINT16 *)pKeyValuePartialInfor->Data;
            }

            if( ipRulesElem->rule.Bits.RemotePortType == RangeAddr )
            {
                //读入端口2
                RtlInitUnicodeString( &uniValueName,L"RemotePort2");
                status = ZwQueryValueKey( hKey,
                                          &uniValueName,
                                          KeyValuePartialInformation,
                                          &RetInfor,
                                          sizeof( RetInfor ),
                                          &retLength );
                if( !NT_SUCCESS( status ))
                {
                    KdPrint(("query value key failed!(LocalAddr)\n"));
                }
                else
                {
                    pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
                    ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
                    ipRulesElem->RemotePort2 = *(UINT16 *)pKeyValuePartialInfor->Data;
                }
            }
        }//end if RemotePort


        ZwClose( hKey );
        hKey = NULL;

        KdPrint(("crcIpRulename = 0x%x\n\
            rule = 0x%x\n\
            LocalAddr=0x%x\n\
            LocalAddr2=0x%x\n\
            RemoteAddr=0x%x\n\
            RemoteAddr2=0x%x\n\
            LocalPort=0x%x\n\
            LocalPort2=0x%x\n\
            RemotePort=0x%x\n\
            RemotePort2=0x%x\n",
            ipRulesElem->crcRuleName,
            ipRulesElem->rule.u32,
            ipRulesElem->LocalAddr,
            ipRulesElem->LocalAddr2,
            ipRulesElem->RemoteAddr,
            ipRulesElem->RemoteAddr2,
            ipRulesElem->LocalPort,
            ipRulesElem->LocalPort2,
            ipRulesElem->RemotePort,
            ipRulesElem->RemotePort2 ));
        AddIpRule( ipRulesElem );
        ipRulesElem = NULL;

    }//end for enum ip rules sub key

    ZwClose( hIpRulesKey );
    ZwClose( hCactiKey );

    gbEnableIpMonitor = TRUE;

    return STATUS_SUCCESS;
}

NTSTATUS WallLoadDnsConfig()
/*++
--*/
{
    
    NTSTATUS                status;
    HANDLE                  hCactiKey = 0,hDnsRulesKey = 0;
    OBJECT_ATTRIBUTES       KeyObjAttr;
    UNICODE_STRING          RegDirectory;
    ULONG                   i;
    UCHAR                   RetInfor[ sizeof( KEY_FULL_INFORMATION ) + 88 ];//NOTICE!
    PKEY_FULL_INFORMATION   pKeyFullInfor;
    PKEY_BASIC_INFORMATION  pKeyBasicInfor;
    PKEY_VALUE_PARTIAL_INFORMATION pKeyValuePartialInfor;
    PKEY_VALUE_FULL_INFORMATION    pKeyValueFullInfor;
    ULONG                   retLength = 0;
    ULONG                   subKeys = 0,KeyValues = 0;
    UNICODE_STRING          uniKeyName;
    UNICODE_STRING          uniValueName;
    UINT32                  crcDnsRuleName;
    UINT32                  rule;
    BOOLEAN                 bAllow = TRUE;

    LOG("into\n");

    InitDnsRules();

    RtlInitUnicodeString( &RegDirectory,CACTIWALL_REG_DIRECTORY );
    InitializeObjectAttributes( &KeyObjAttr,
                                &RegDirectory,
                                OBJ_KERNEL_HANDLE,
                                0,
                                NULL);
    status = ZwCreateKey( &hCactiKey,
                          KEY_ALL_ACCESS,
                          &KeyObjAttr,
                          0,
                          NULL,
                          0,
                          NULL);
    if( !NT_SUCCESS( status ) ){
        KdPrint(("Create key 1 failed!\n"));
        return status;
    }

    RtlInitUnicodeString( &RegDirectory,L"dnsrules" );
    InitializeObjectAttributes( &KeyObjAttr,
                                &RegDirectory,
                                OBJ_KERNEL_HANDLE,
                                hCactiKey,
                                NULL);
    status = ZwCreateKey( &hDnsRulesKey,
                          KEY_ALL_ACCESS,
                          &KeyObjAttr,
                          0,
                          NULL,
                          0,
                          NULL);
    if( !NT_SUCCESS( status ) ){
        KdPrint(("Create key 2 failed!\n"));
        return status;
    }

    RtlInitUnicodeString( &uniValueName,L"other_access");
    status = ZwQueryValueKey( hDnsRulesKey,
                                  &uniValueName,
                                  KeyValuePartialInformation,
                                  &RetInfor,
                                  sizeof( RetInfor ),
                                  &retLength );
    if( !NT_SUCCESS( status ))
    {
        KdPrint(("query value key(other_access) failed!\n"));
        gbDnsConfig_other_allow = TRUE;
    }
    else
    {
        pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
        ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
        if( *(UINT32 *)pKeyValuePartialInfor->Data == 0 )
            gbDnsConfig_other_allow = FALSE;
        else
            gbDnsConfig_other_allow = TRUE;
        KdPrint(("dns_config:other_access = %x\n",gbDnsConfig_other_allow ));
    }


    status = ZwQueryKey( hDnsRulesKey,
                         KeyFullInformation,
                         &RetInfor,
                         sizeof( RetInfor ),
                         &retLength );
    if( !NT_SUCCESS( status ) ){
        KdPrint(( "QueryKeyfullinfor failed!\n"));
        return status;
    }
    pKeyFullInfor = ( PKEY_FULL_INFORMATION )RetInfor;
    subKeys = pKeyFullInfor->SubKeys;
    
    for( i = 0;i < subKeys; i++)
    {
        HANDLE  hKey = NULL;

        RtlZeroMemory( &RetInfor,sizeof(RetInfor));
        status = ZwEnumerateKey( hDnsRulesKey,
                                      i,
                                      KeyBasicInformation,
                                      &RetInfor,
                                      sizeof( RetInfor ),
                                      &retLength);
        if( !NT_SUCCESS( status ) ){
            KdPrint(("Enumerate value key failed!--%d\n",i));
            continue;
        }
        pKeyBasicInfor = (PKEY_BASIC_INFORMATION)RetInfor;
        uniKeyName.Length = uniKeyName.MaximumLength = (USHORT)pKeyBasicInfor->NameLength;
        uniKeyName.Buffer = pKeyBasicInfor->Name;
        UnicodeStringToUint32( &uniKeyName,&crcDnsRuleName );
        
        InitializeObjectAttributes( &KeyObjAttr,
                                &uniKeyName,
                                OBJ_KERNEL_HANDLE,
                                hDnsRulesKey,
                                NULL);
        status = ZwCreateKey( &hKey,
                                KEY_ALL_ACCESS,
                                &KeyObjAttr,
                                0,
                                NULL,
                                0,
                                NULL);
        if( !NT_SUCCESS( status ) ){
            KdPrint(("create hKey failed!--%d\n",i));
            continue;
        }

        
        RtlInitUnicodeString( &uniValueName,L"rule");
        status = ZwQueryValueKey( hKey,
                                  &uniValueName,
                                  KeyValuePartialInformation,
                                  &RetInfor,
                                  sizeof( RetInfor ),
                                  &retLength );
        if( !NT_SUCCESS( status ))
        {
            KdPrint(("query value key failed!--%d\n",i));
            continue;
        }

        pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
        ASSERT( pKeyValuePartialInfor->Type == REG_DWORD );
        rule = *(UINT32 *)pKeyValuePartialInfor->Data;
        if( rule & DNS_RULE_FLAG_ALLOW_ACCESS )
            bAllow = TRUE;
        else
            bAllow = FALSE;

        RtlZeroMemory( &RetInfor,sizeof(RetInfor));
        RtlInitUnicodeString( &uniValueName,L"name");
        status = ZwQueryValueKey( hKey,
                                  &uniValueName,
                                  KeyValuePartialInformation,
                                  &RetInfor,
                                  sizeof( RetInfor ),
                                  &retLength );
        if( !NT_SUCCESS( status ))
        {
            KdPrint(("query value key failed!--%d\n",i));
            continue;
        }

        pKeyValuePartialInfor = (PKEY_VALUE_PARTIAL_INFORMATION)RetInfor;
        ASSERT( pKeyValuePartialInfor->Type == REG_SZ );
        
        KdPrint(("Add dns:%ws\n",pKeyValuePartialInfor->Data));
        if( pKeyValuePartialInfor->DataLength + sizeof(WCHAR)
              + sizeof( KEY_VALUE_PARTIAL_INFORMATION) >sizeof( RetInfor))
        {
            KdPrint(("the dns name is too long!\n"));
            continue;
        }
        AddDnsRule( (PWCHAR)pKeyValuePartialInfor->Data,
                    pKeyValuePartialInfor->DataLength + sizeof(WCHAR),
                    bAllow,
                    crcDnsRuleName );

    }//end for

    gbEnableDnsMonitor = TRUE;

    return STATUS_SUCCESS;
}

VOID    WallUnloadProcessConfig()
/*++
--*/
{
    LOG("into\n");

    gbEnableProcessMonitor = FALSE;

    return;
}

VOID    WallUnloadIpConfig()
/*++
--*/
{
    LOG("into\n");

    gbEnableIpMonitor = FALSE;

    ClearIpRulesList();

    return;
}

VOID    WallUnloadDnsConfig()
/*++
--*/
{
    LOG("into\n");

    gbEnableDnsMonitor = FALSE;

    ClearDnsRulesList();

    return;
}

VOID    WallLoadConfig()
/*++
功能描述：加载配置数据

参数说明：无

返回值说明：无返回值
--*/
{
    NTSTATUS    status;

    LOG("into\n");

    WallLoadGlobalConfig();

    if( gbEnableProcessMonitor )
        WallLoadProcessConfig();

    if( gbEnableIpMonitor )
        WallLoadIpConfig();

    if( gbEnableDnsMonitor )
        WallLoadDnsConfig();
    
}

VOID    WallUnloadConfig()
/*++
功能描述：卸载配置数据，释放资源

参数说明：无

返回值说明：无返回值
--*/
{
    NTSTATUS    status;

    LOG("into\n");


    if( gbEnableProcessMonitor )
        WallUnloadProcessConfig();

    if( gbEnableIpMonitor )
        WallUnloadIpConfig();

    if( gbEnableDnsMonitor)
        WallUnloadDnsConfig();
    
}


BOOLEAN
WallIsProcessTrafficPermit( IN PWALL_PENDED_PACKET packet )
/*++
功能描述：判断一个packet是否符合进程规则的放行条件

参数说明：
    packet:指向被判定的packet结构

返回值说明：符合返回TRUE，否则返回FALSE
--*/
{
    NTSTATUS                status = STATUS_SUCCESS;
    UINT32                  rule;
    
    LOG("into\n");

    //if( packet->direction == FWP_DIRECTION_OUTBOUND )
        KdPrint(( "remote addr:%x\nlocal addr:%x\n",packet->ipv4RemoteAddr,packet->ipv4LocalAddr));
    status = GetProcessRule( packet->crcProcessPath,&rule );
    if( status == STATUS_SUCCESS )
    {
        if(rule & PROCESS_RULE_FLAG_ALLOW_ACCESS )
        {
            KdPrint(("return true\n"));
            return TRUE;
        }
        else 
        {
            KdPrint(("return false\n"));
            return FALSE;
        }
    }

    KdPrint(("return %x\n",gbProcessConfig_other_allow ));
    return gbProcessConfig_other_allow;
}

BOOLEAN
WallIsIpTrafficPermit( IN PWALL_PENDED_PACKET packet )
/*++
功能描述：判断一个packet是否符合IP规则的放行条件

参数说明：
    packet:指向被判定的packet结构

返回值说明：符合返回TRUE，否则返回FALSE
--*/
{
    extern IP_RULES_LIST    gIpRulesList;
    PLIST_ENTRY             list = NULL;
    BOOLEAN                 bMatched = FALSE;

    LOG("into\n");

    KdPrint(("RemoteAddr:%x\nLocalAddr:%x\nRemotePort:%x\nLocalPort:%x\nProtocol:%x\ndirection:%x\n",
        packet->ipv4RemoteAddr,
        packet->ipv4LocalAddr,
        packet->remotePort,
        packet->localPort,
        packet->protocol,
        packet->direction));
    for( list = gIpRulesList.list.Flink;
        list != &gIpRulesList.list;
        list = list->Flink)
    {
        PIP_RULES_ELEM  rule = (PIP_RULES_ELEM)list;

        if(rule->rule.Bits.Direction != RulesDirectionAny )
        {
            if( rule->rule.Bits.Direction == RulesDirectionUp && 
                packet->direction != FWP_DIRECTION_OUTBOUND )continue;
            if( rule->rule.Bits.Direction == RulesDirectionDown && 
                packet->direction != FWP_DIRECTION_INBOUND )continue;

        }

        if( rule->rule.Bits.LocalAddrType == UniqueAddr && 
            rule->LocalAddr != *(PUINT32)packet->localAddr.byteArray16 )
            continue;

        if( rule->rule.Bits.LocalAddrType == RangeAddr && 
            (rule->LocalAddr > *(PUINT32)packet->localAddr.byteArray16  || 
             rule->LocalAddr2 < *(PUINT32)packet->localAddr.byteArray16))
            continue;

        if( rule->rule.Bits.RemoteAddrType == UniqueAddr && 
            rule->RemoteAddr != *(PUINT32)packet->remoteAddr.byteArray16 )
            continue;

        if( rule->rule.Bits.RemoteAddrType == RangeAddr && 
            (rule->RemoteAddr > *(PUINT32)packet->remoteAddr.byteArray16  || 
             rule->RemoteAddr2 < *(PUINT32)packet->remoteAddr.byteArray16))
            continue;

        if( rule->rule.Bits.ProtocolType != RulesProtocolAny)
        {
            if(rule->rule.Bits.ProtocolType != packet->protocol )
                continue;

            if( packet->protocol == IPPROTO_TCP || packet->protocol==IPPROTO_UDP)
            {
                if( rule->rule.Bits.LocalPortType == UniqueAddr && 
                    rule->LocalPort != packet->localPort )
                    continue;

                if( rule->rule.Bits.LocalPortType == RangeAddr && 
                    (rule->LocalPort > packet->localPort  || 
                     rule->LocalPort2 < packet->localPort))
                    continue;

                if( rule->rule.Bits.RemotePortType == UniqueAddr && 
                    rule->RemotePort != packet->remotePort )
                    continue;

                if( rule->rule.Bits.RemotePortType == RangeAddr && 
                    (rule->RemotePort > packet->remotePort  || 
                     rule->RemotePort2 < packet->remotePort))
                    continue;
            }//end if tcp || udp

            if( packet->protocol == IPPROTO_ICMP )
            {
                if( rule->rule.Bits.IcmpType != 0x1f )
                {
                    if(rule->rule.Bits.IcmpType != packet->icmpType )
                        continue;

                    if( rule->rule.Bits.IcmpCode != 0x1f && 
                        rule->rule.Bits.IcmpCode != packet->icmpCode )
                        continue;
                }
            }//end if icmp

        }//end if processType any

        bMatched = TRUE;

        if( rule->rule.Bits.Access == 0 )
        {
            KdPrint(("return false\n"));
            return FALSE;
        }
    }

    if( bMatched )
    {
        KdPrint(("return true\n"));
        return TRUE;
    }
    else
    {
        KdPrint(("return %x\n",gbIpConfig_other_allow));
        return gbIpConfig_other_allow;
    }

}

BOOLEAN
WallIsDnsTrafficPermit( IN PWALL_PENDED_PACKET packet )
/*++
功能描述：判断一个packet是否符合DNS则的放行条件

参数说明：
    packet:指向被判定的packet结构

返回值说明：符合返回TRUE，否则返回FALSE
--*/
{
    extern DNS_RULES_LIST   gDnsRulesList;
    PNET_BUFFER_LIST        netBufferList = NULL;
    PNET_BUFFER             netBuffer = NULL;
    PMDL                    mdl = NULL;
    PBYTE                   va = NULL,dnsName = NULL;
    ULONG                   dataLength = 0,count = 0,offset=0;
    PBYTE                   udpDataBuffer = NULL;
    UNICODE_STRING          uniDnsName = {0};
    ANSI_STRING             ansiDnsName = {0};
    ULONG                   bytesCopied = 0;
    ULONG                   len = 0;
    PLIST_ENTRY             list = NULL;
    BOOLEAN                 bMatched = FALSE;
    int                     i = 0,j = 0;

    LOG("into\n");
    ASSERT( packet != NULL );

    if( packet->protocol != IPPROTO_UDP || 
        packet->remotePort != RtlUshortByteSwap(53) )return TRUE;//53为DNS解析所用端口号

    ASSERT( packet->type == WALL_DATA_PACKET && packet->netBufferList != NULL);

    //从NET_BUFFER_LIST结构中提取UDP包数据

    netBufferList = packet->netBufferList;
    netBuffer = NET_BUFFER_LIST_FIRST_NB( netBufferList );
    mdl = NET_BUFFER_FIRST_MDL( netBuffer );
    dataLength = NET_BUFFER_DATA_LENGTH( netBuffer );
    offset = NET_BUFFER_DATA_OFFSET( netBuffer );
    KdPrint(("data length of net_buffer list is %d\n",dataLength ));
    udpDataBuffer = (PBYTE)MyExAllocatePool( dataLength );
    if( udpDataBuffer == NULL)
    {
        KdPrint(("memory allocated failed! return true!\n"));
        return gbDnsConfig_other_allow;
    }

    for(bytesCopied = 0;
        mdl != NULL;
        mdl = mdl->Next )
    {
        va = (PBYTE)MmGetMdlVirtualAddress( mdl );
        count = MmGetMdlByteCount( mdl );

        if( offset >= count ){
            offset -= count;
            continue;
        }

        len = count - offset > dataLength ? dataLength:count-offset ;
        if( len == 0 )break;

        RtlCopyMemory( udpDataBuffer + bytesCopied,
                        va + offset,
                        len );
        bytesCopied += len;
        dataLength -= len;
        offset = 0;
    }//end for 
    ASSERT( dataLength == 0 );
    KdPrint(("udpDataBuffer = 0x%x,bytesCopied = %d\n",udpDataBuffer,bytesCopied ));

    //从UDP数据报中提取所查询的域名
    //udp header:8 bytes,the first 6 records of dns packet:12 bytes
    //offset of dns name:20 bytes

    for( i = 20;udpDataBuffer[i] != 0; )
    {
        for( j = i + 1;j < i + udpDataBuffer[i]+1;j++)
        {
            //转换为小写
            if( udpDataBuffer[j] >='A' && udpDataBuffer[j] <= 'Z')
                udpDataBuffer[j] = udpDataBuffer[j]-'A' + 'a';
        }
        udpDataBuffer[i] = '.';
        i = j;
    }
    dnsName = udpDataBuffer + 21;

    //转换为UNICODE_STRING
    ansiDnsName.Buffer = dnsName;
    ansiDnsName.Length = ansiDnsName.MaximumLength = strlen( dnsName) +1;
    
    uniDnsName.MaximumLength = (USHORT)RtlAnsiStringToUnicodeSize( &ansiDnsName );
    uniDnsName.Buffer = (PWCHAR)MyExAllocatePool( uniDnsName.MaximumLength);
    if( uniDnsName.Buffer == NULL )
    {
        KdPrint(("Memory allocated failed!(unicode buffer)\n"));
        return gbDnsConfig_other_allow;
    }
    RtlZeroMemory( uniDnsName.Buffer,uniDnsName.MaximumLength );
    KdPrint(("ansi_string = %s\n",ansiDnsName.Buffer ));
    RtlAnsiStringToUnicodeString( &uniDnsName,&ansiDnsName,FALSE );

    MyExFreePool( udpDataBuffer );
    udpDataBuffer = NULL;
    ansiDnsName.MaximumLength = ansiDnsName.Length = 0;
    ansiDnsName.Buffer = NULL;

    KdPrint(("addr:%x\nwsz = %ws\n",&uniDnsName,uniDnsName.Buffer ));
    
    //匹配规则
    bMatched = FALSE;
    for( list = gDnsRulesList.list.Flink;
        list != &gDnsRulesList.list;
        list = list->Flink)
    {
        PDNS_RULES_ELEM  rule = (PDNS_RULES_ELEM)list;
        
        if( NULL != wcsstr(uniDnsName.Buffer,rule->dnsName->str.Buffer))
        {
            bMatched = TRUE;
            if( rule->rule & DNS_RULE_FLAG_ALLOW_ACCESS )
                continue;
            else
                break;
        }
    }

    MyExFreePool( uniDnsName.Buffer );
    uniDnsName.Buffer = NULL;
    uniDnsName.Length = uniDnsName.MaximumLength = 0;

    if( bMatched )
    {
        if( list == &gDnsRulesList.list )
        {
            KdPrint(("return true\n"));
            return TRUE;
        }
        else 
        {
            KdPrint(("return false\n"));
            return FALSE;
        }
    }
    else
    {
        KdPrint(("return other_allow:%x\n",gbDnsConfig_other_allow));
        return gbDnsConfig_other_allow;
    }

}

NTSTATUS
WallBlockAll( BOOLEAN bBlockAll )
/*++
功能描述：设置阻塞所有连接

参数说明：

    bBlockAll:为TRUE则设置阻塞所有连接，否则此功能无效。

返回值：成功返回STATUS_SUCCESS,否则返回STATUS_UNSUCCESSFUL
    
--*/
{
    LOG("into\n");

    gbBlockAll = bBlockAll;
    KdPrint(("gbBlockAll = %d\n",gbBlockAll ));

    return STATUS_SUCCESS;
}

///////////////////////////函数定义结束//////////////////////////////////////

                                                                                                                                                                                                                                                                                                                                                               /*
BUG LIST
1:  WallAllocateAndInitPendedPacket  line:943

*/