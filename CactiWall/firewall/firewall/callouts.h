#pragma once


#include "common.h"

// {6812FC83-7D3E-499a-A012-55E0D85F348B}
DEFINE_GUID(
    WALL_ALE_AUTH_CONNECT_CALLOUT_V4, 
    0x6812fc83, 
    0x7d3e, 
    0x499a, 
    0xa0, 0x12, 0x55, 0xe0, 0xd8, 0x5f, 0x34, 0x8b
);

// {B438CEAE-FF2A-484f-9CB8-F425A288594C}
DEFINE_GUID(WALL_ALE_AUTH_RECV_ACCEPT_CALLOUT_V4, 
    0xb438ceae, 
    0xff2a, 
    0x484f, 
    0x9c, 0xb8, 0xf4, 0x25, 0xa2, 0x88, 0x59, 0x4c);

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
   );


NTSTATUS 
NTAPI
WallALEConnectNotify(
    IN FWPS_CALLOUT_NOTIFY_TYPE  notifyType,
    IN const GUID  *filterKey,
    IN const FWPS_FILTER  *filter
    );



VOID 
NTAPI
WallALEConnectFlowDelete(
    IN UINT16  layerId,
    IN UINT32  calloutId,
    IN UINT64  flowContext
    );

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
   );


NTSTATUS 
NTAPI
WallALERecvAcceptNotify(
    IN FWPS_CALLOUT_NOTIFY_TYPE  notifyType,
    IN const GUID  *filterKey,
    IN const FWPS_FILTER  *filter
    );



VOID 
NTAPI
WallALERecvAcceptFlowDelete(
    IN UINT16  layerId,
    IN UINT32  calloutId,
    IN UINT64  flowContext
    );










                                                                                                                                                                                                                                                                                                                                                    