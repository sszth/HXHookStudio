#pragma once

typedef enum _WALL_PENDED_PACKET_TYPE_
{
    WALL_CONNECT_PACKET,
    WALL_DATA_PACKET,
    WALL_REAUTH_PACKET
} WALL_PACKET_TYPE;

typedef struct _WALL_PENDED_PACKET
{
    LIST_ENTRY          list;

    ADDRESS_FAMILY      addressFamily;
    WALL_PACKET_TYPE    type;
    FWP_DIRECTION       direction;

    UINT32              authConnectDecision;
    HANDLE              completionContext;
    UINT32              crcProcessPath;
    //
    // Common fields for inbound and outbound traffic.
    //
    UINT8 protocol;
    NET_BUFFER_LIST* netBufferList;
    COMPARTMENT_ID compartmentId;
    union
    {
       FWP_BYTE_ARRAY16 localAddr;
       UINT32 ipv4LocalAddr;
    };
    union
    {
       UINT16 localPort;
       UINT16 icmpType;
    };
    union
    {
       UINT16 remotePort;
       UINT16 icmpCode;
    };

    //
    // Data fields for outbound packet re-injection.
    //
    UINT64 endpointHandle;
    union
    {
       FWP_BYTE_ARRAY16 remoteAddr;
       UINT32 ipv4RemoteAddr;
    };

    SCOPE_ID remoteScopeId;
    WSACMSGHDR* controlData;
    ULONG controlDataLength;

    //
    // Data fields for inbound packet re-injection.
    //
    BOOLEAN ipSecProtected;
    ULONG nblOffset;
    UINT32 ipHeaderSize;
    UINT32 transportHeaderSize;
    IF_INDEX interfaceIndex;
    IF_INDEX subInterfaceIndex;

}WALL_PENDED_PACKET,*PWALL_PENDED_PACKET;

typedef struct _WALL_CONN_LIST
{
    LIST_ENTRY  list;
    KSPIN_LOCK  lock;
}WALL_CONN_LIST,WALL_PACKET_LIST,*PWALL_CONN_LIST,*PWALL_PACKET_LIST;

NTSTATUS
RegisterCalloutForLayer(
   IN const GUID* layerKey,
   IN const GUID* calloutKey,
   IN FWPS_CALLOUT_CLASSIFY_FN classifyFn,
   IN FWPS_CALLOUT_NOTIFY_FN notifyFn,
   IN FWPS_CALLOUT_FLOW_DELETE_NOTIFY_FN flowDeleteNotifyFn,
   OUT UINT32* calloutId,
   OUT UINT64* filterId
   );

NTSTATUS
WallRegisterCallouts();

NTSTATUS
WallUnRegisterCallouts();

VOID
WallWriteConnectLogData(  IN PVOID data );

NTSTATUS 
WallCreateConnList();

NTSTATUS 
WallCreatePacketList();

VOID
WallDestroyConnList();

VOID
WallDestroyPacketList();

NTSTATUS
WallCreateInjectionHandle();

NTSTATUS
WallDestroyInjectionHandle();

WALL_PENDED_PACKET*
WallAllocateAndInitPendedPacket(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
   IN ADDRESS_FAMILY addressFamily,
   IN OUT void* layerData,
   IN WALL_PACKET_TYPE packetType,
   IN FWP_DIRECTION packetDirection
   );
VOID
WallFreePendedPacket( IN PWALL_PENDED_PACKET packet );

VOID
WallInspectWallPackets( IN PVOID Context );

BOOLEAN
IsMatchingConnectPacket(
   IN const FWPS_INCOMING_VALUES0* inFixedValues,
   IN ADDRESS_FAMILY addressFamily,
   IN FWP_DIRECTION direction,
   IN WALL_PENDED_PACKET* pendedPacket
   );

ADDRESS_FAMILY GetAddressFamilyForLayer(
   IN UINT16 layerId
   );

VOID
WallLoadGlobalConfig();

NTSTATUS    
WallLoadProcessConfig();

NTSTATUS
WallLoadIpConfig();

NTSTATUS
WallLoadDnsConfig();

VOID
WallUnloadProcessConfig();

VOID
WallUnloadIpConfig();

VOID
WallUnloadDnsConfig();

VOID    WallLoadConfig();
VOID    WallUnloadConfig();

BOOLEAN
WallIsProcessTrafficPermit( IN PWALL_PENDED_PACKET packet );

BOOLEAN
WallIsIpTrafficPermit( IN PWALL_PENDED_PACKET packet );

BOOLEAN
WallIsDnsTrafficPermit( IN PWALL_PENDED_PACKET packet );

NTSTATUS
WallBlockAll( BOOLEAN bBlockAll );
