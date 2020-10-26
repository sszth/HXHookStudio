
#include "callouts.h"
#include "wall.h"
#include "memtrace.h"


extern HANDLE               gInjectHandle;
extern PWALL_CONN_LIST      gConnList;
extern PWALL_PACKET_LIST    gPacketList;

extern
VOID
WallInspectWallPackets( PVOID Context );

extern
NTSTATUS
WallRunMyProcess(
    MY_PROCESS      ToyMyProcess,
    PVOID           Context
    );

void 
NTAPI
WallALEConnectClassify(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN OUT void* layerData,
   IN const void* classifyContext,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut
   )
/*++

注意：此回调的Irql <= DISPATCH_LEVEL!!!!!
--*/
{
    NTSTATUS                status = STATUS_SUCCESS;
    PWALL_PENDED_PACKET     pendedConn = NULL;
    BOOLEAN                 bWakeUp = FALSE;
    ADDRESS_FAMILY          addressFamily;
    UNICODE_STRING          devName,dosName;
    WCHAR                   buffer[MAX_PATH_LEN];
    PMY_UNICODE_STRING      logData = NULL;

    LOG("into\n");

    if(!(classifyOut->rights & FWPS_RIGHT_ACTION_WRITE))
    {
        KdPrint(("write right not set!\n"));
        return;
    }

     if( layerData != NULL )
     {
         FWPS_PACKET_INJECTION_STATE state;
         state = FwpsQueryPacketInjectionState( gInjectHandle,
                                        layerData,
                                        NULL);
         if( state == FWPS_PACKET_INJECTED_BY_SELF ||
             state == FWPS_PACKET_PREVIOUSLY_INJECTED_BY_SELF )
         {
             classifyOut->actionType = FWP_ACTION_PERMIT;
             goto exit;
         }
     }

    addressFamily = GetAddressFamilyForLayer(inFixedValues->layerId);

    if(!IsAleReauthorize(inFixedValues))
    {
        pendedConn = WallAllocateAndInitPendedPacket( inFixedValues,
                                                    inMetaValues,
                                                    addressFamily,
                                                    layerData,
                                                    WALL_CONNECT_PACKET,
                                                    FWP_DIRECTION_OUTBOUND );
        if(pendedConn == NULL )
        {
            classifyOut->actionType = FWP_ACTION_BLOCK;
            classifyOut->rights &= ~FWPS_RIGHT_ACTION_WRITE;
            goto exit;
        }

        ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(inMetaValues, 
                                            FWPS_METADATA_FIELD_COMPLETION_HANDLE));

        status = FwpsPendOperation0(
                  inMetaValues->completionHandle,
                  &pendedConn->completionContext
                  );

        if (!NT_SUCCESS(status))
        {
            classifyOut->actionType = FWP_ACTION_BLOCK;
            classifyOut->rights &= ~FWPS_RIGHT_ACTION_WRITE;
            goto exit;
        }

        bWakeUp = IsListEmpty(&gPacketList->list) &&
                                       IsListEmpty(&gConnList->list);

        ExInterlockedInsertTailList( &gConnList->list,&pendedConn->list,&gConnList->lock );
        pendedConn = NULL;

        if( bWakeUp )
        {
            RunMyProcess( WallInspectWallPackets,NULL );
        }

        classifyOut->actionType = FWP_ACTION_BLOCK;
        classifyOut->flags |= FWPS_CLASSIFY_OUT_FLAG_ABSORB;
    }
    else
    {//reauth

        FWP_DIRECTION packetDirection;
        KIRQL         irql,irql2;

        LOG("1\n");

        ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(inMetaValues, 
                                            FWPS_METADATA_FIELD_PACKET_DIRECTION));
        packetDirection = inMetaValues->packetDirection;

        if (packetDirection == FWP_DIRECTION_OUTBOUND)
        {
            LIST_ENTRY* listEntry;
            BOOLEAN authComplete = FALSE;
            
             LOG("2\n");
            KeAcquireSpinLock( &gConnList->lock,&irql );
            LOG("22\n");
            for (listEntry = gConnList->list.Flink;
              listEntry != (PLIST_ENTRY)gConnList;
              )
            {   
                pendedConn = (PWALL_PENDED_PACKET)listEntry;
                listEntry = listEntry->Flink;

                if (IsMatchingConnectPacket(
                     inFixedValues,
                     addressFamily,
                     packetDirection,
                     pendedConn
                  ) && (pendedConn->authConnectDecision != 0))
                {

                    ASSERT((pendedConn->authConnectDecision == FWP_ACTION_PERMIT) ||
                      (pendedConn->authConnectDecision == FWP_ACTION_BLOCK));
                LOG("3\n");
                    classifyOut->actionType = pendedConn->authConnectDecision;
                    if( classifyOut->actionType == FWP_ACTION_BLOCK ){
                        classifyOut->rights &= ~FWPS_RIGHT_ACTION_WRITE;
                        classifyOut->flags |= FWPS_CLASSIFY_OUT_FLAG_ABSORB;
                    }

                    RemoveEntryList(&pendedConn->list);
               
                    if (/*!gDriverUnloading &&*/
                       (pendedConn->netBufferList != NULL) &&
                       (pendedConn->authConnectDecision == FWP_ACTION_PERMIT))
                    {
                  
                        pendedConn->type = WALL_DATA_PACKET;
                         LOG("4\n");
                        KeAcquireSpinLock( &gPacketList->lock,&irql2 );

                        bWakeUp = IsListEmpty(&gPacketList->list) &&
                                       IsListEmpty(&gConnList->list);

                        InsertTailList(&gPacketList->list, &pendedConn->list);
                        pendedConn = NULL; // ownership transferred

                        KeReleaseSpinLock( &gPacketList->lock,irql2 );

                        if (bWakeUp)
                        {
                            RunMyProcess( WallInspectWallPackets,NULL );
                        }
                    }//end if permit

                    authComplete = TRUE;
                    break;
                }//end if match
            }//end if for

            KeReleaseSpinLock( &gConnList->lock,irql );
            if (authComplete)
            {
                 LOG("5\n");
                goto exit;
            }
            else
            {
                pendedConn = NULL;
            }
        }//end if outbound

        classifyOut->actionType = FWP_ACTION_BLOCK;
        classifyOut->rights &= ~FWPS_RIGHT_ACTION_WRITE;
        classifyOut->flags |= FWPS_CLASSIFY_OUT_FLAG_ABSORB;
    }
exit:
    if( pendedConn != NULL )
    {
        LOG("free packet\n");
        WallFreePendedPacket( pendedConn );
        pendedConn = NULL;
    }

    if( classifyOut->actionType == FWP_ACTION_PERMIT )
    {
        RtlInitUnicodeString( &devName,(PWCHAR)inMetaValues->processPath->data );
        RtlInitEmptyUnicodeString( &dosName,buffer,MAX_PATH_LEN * sizeof(WCHAR));
        logData = MyExAllocatePool( sizeof( MY_UNICODE_STRING) + inMetaValues->processPath->size);
        if( logData != NULL)
        {
            logData->str.Buffer = logData->buffer;
            logData->str.MaximumLength = (USHORT)inMetaValues->processPath->size;
            status = DevicePathToDosPath( &devName,&dosName );
            if( NT_SUCCESS( status ))
            {
                RtlCopyUnicodeString( (PUNICODE_STRING)logData,&dosName );
            }
            else
            {
                RtlCopyUnicodeString( (PUNICODE_STRING)logData,&devName );
            }
            KdPrint(("logData:%wZ\n",logData ));

            RunMyProcess( WallWriteConnectLogData,logData );
            logData = NULL;
        }
        
    }
    return;
}

NTSTATUS 
NTAPI
WallALEConnectNotify(
    IN FWPS_CALLOUT_NOTIFY_TYPE  notifyType,
    IN const GUID  *filterKey,
    IN const FWPS_FILTER  *filter
    )
{
    LOG("into\n");

    return STATUS_SUCCESS;
}

VOID 
NTAPI
WallALEConnectFlowDelete(
    IN UINT16  layerId,
    IN UINT32  calloutId,
    IN UINT64  flowContext
    )
{
    LOG("into\n");

    return;
}

void 
NTAPI
WallALERecvAcceptClassify(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN OUT void* layerData,
   IN const void* classifyContext,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut
   )
/*++
--*/
{
    NTSTATUS                status = STATUS_SUCCESS;
    UNICODE_STRING          devName,dosName;
    WCHAR                   buffer[MAX_PATH_LEN];
    PMY_UNICODE_STRING      logData;
    PWALL_PENDED_PACKET     pendedRecv = NULL;
    BOOLEAN                 bWakeUp = FALSE;
    ADDRESS_FAMILY          addressFamily;

    LOG("into\n");


    if(!(classifyOut->rights & FWPS_RIGHT_ACTION_WRITE))
    {
        KdPrint(("write right not set!\n"));
        return;
    }

    if( layerData != NULL )
    {
        FWPS_PACKET_INJECTION_STATE state;
        state = FwpsQueryPacketInjectionState( gInjectHandle,
                                       layerData,
                                       NULL);
        KdPrint(("inject state:%x\n",state ));
        if( state == FWPS_PACKET_INJECTED_BY_SELF ||
            state == FWPS_PACKET_PREVIOUSLY_INJECTED_BY_SELF )
        {
            classifyOut->actionType = FWP_ACTION_PERMIT;
            KdPrint(("inject by self\n"));
            goto exit;
        }
    }
    addressFamily = GetAddressFamilyForLayer(inFixedValues->layerId);

    if(!IsAleReauthorize(inFixedValues))
    {
        pendedRecv = WallAllocateAndInitPendedPacket( inFixedValues,
                                                    inMetaValues,
                                                    addressFamily,
                                                    layerData,
                                                    WALL_DATA_PACKET,
                                                    FWP_DIRECTION_INBOUND );
        if(pendedRecv == NULL )
        {
            classifyOut->actionType = FWP_ACTION_BLOCK;
            classifyOut->rights &= ~FWPS_RIGHT_ACTION_WRITE;
            goto exit;
        }

        ASSERT(FWPS_IS_METADATA_FIELD_PRESENT(inMetaValues, 
                                            FWPS_METADATA_FIELD_COMPLETION_HANDLE));

        status = FwpsPendOperation0(
                  inMetaValues->completionHandle,
                  &pendedRecv->completionContext
                  );

        if (!NT_SUCCESS(status))
        {
            classifyOut->actionType = FWP_ACTION_BLOCK;
            classifyOut->rights &= ~FWPS_RIGHT_ACTION_WRITE;
            classifyOut->flags |= FWPS_CLASSIFY_OUT_FLAG_ABSORB;
            goto exit;
        }

        bWakeUp = IsListEmpty(&gPacketList->list) &&
                                       IsListEmpty(&gConnList->list);

        ExInterlockedInsertTailList( &gPacketList->list,&pendedRecv->list,&gPacketList->lock );
        pendedRecv = NULL;

        if( bWakeUp )
        {
            RunMyProcess( WallInspectWallPackets,NULL );
        }

        classifyOut->actionType = FWP_ACTION_BLOCK;
        classifyOut->flags |= FWPS_CLASSIFY_OUT_FLAG_ABSORB;
        classifyOut->rights &= ~FWPS_RIGHT_ACTION_WRITE;
    }
    else
    {//reauth

        FWP_DIRECTION packetDirection;
        KIRQL         irql,irql2;


        KdPrint(("recv reauth!\n"));
        classifyOut->actionType = FWP_ACTION_BLOCK;
        classifyOut->rights &= ~FWPS_RIGHT_ACTION_WRITE;
        classifyOut->flags |= FWPS_CLASSIFY_OUT_FLAG_ABSORB;
    }


exit:
    if( classifyOut->actionType == FWP_ACTION_PERMIT )
    {
        RtlInitUnicodeString( &devName,(PWCHAR)inMetaValues->processPath->data );
        RtlInitEmptyUnicodeString( &dosName,buffer,MAX_PATH_LEN * sizeof(WCHAR));
        logData = MyExAllocatePool( sizeof( MY_UNICODE_STRING) + inMetaValues->processPath->size);
        if( logData != NULL)
        {
            logData->str.Buffer = logData->buffer;
            logData->str.MaximumLength = (USHORT)inMetaValues->processPath->size;
            status = DevicePathToDosPath( &devName,&dosName );
            if( NT_SUCCESS( status ))
            {
                RtlCopyUnicodeString( (PUNICODE_STRING)logData,&dosName );
            }
            else
            {
                RtlCopyUnicodeString( (PUNICODE_STRING)logData,&devName );
            }
            RunMyProcess( WallWriteConnectLogData,logData );
            logData = NULL;
        }
        
    }
    return;
}


NTSTATUS 
NTAPI
WallALERecvAcceptNotify(
    IN FWPS_CALLOUT_NOTIFY_TYPE  notifyType,
    IN const GUID  *filterKey,
    IN const FWPS_FILTER  *filter
    )
/*++
--*/
{
    LOG("into\n");

    return STATUS_SUCCESS;
}


VOID 
NTAPI
WallALERecvAcceptFlowDelete(
    IN UINT16  layerId,
    IN UINT32  calloutId,
    IN UINT64  flowContext
    )
/*++
--*/
{
    LOG("into\n");

    return;
}
