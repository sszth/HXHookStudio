#pragma once

#include<ndis.h>
#include<ntddk.h>
#include<fwpmk.h>
#include<limits.h>
#include "memtrace.h"

#pragma warning(disable:28197)

#pragma warning(push)
#pragma warning(disable:4201)       // unnamed struct/union

#include "fwpsk.h"

#pragma warning(pop)

#define INITGUID
#include <guiddef.h>

#if 1
#define LOG(x) \
    KdPrint(("%s(%d).%s:%s",__FILE__,__LINE__,__FUNCTION__,(x)))
#else
#define LOG(X) 
#endif

#define DEVICE_NAME L"\\Device\\Wall_Device"
#define DEVICE_DOSNAME L"\\DosDevices\\Wall_Device"

#define MAX_PATH_LEN    256

#define CACTIWALL_REG_DIRECTORY    L"\\REGISTRY\\Machine\\Software\\lzcj\\CactiWall"

#define DELAY_ONE_MICROSECOND   (-10)
#define DELAY_ONE_MILLISECOND   (DELAY_ONE_MICROSECOND*1000)
#define DELAY_ONE_SECOND        (DELAY_ONE_MILLISECOND*1000)

typedef VOID    ( *MY_PROCESS)( PVOID Context );

typedef struct _MY_UNICODE_STRING
{
    UNICODE_STRING  str;
    WCHAR           buffer[1];
}MY_UNICODE_STRING,*PMY_UNICODE_STRING;

typedef  struct  _WORKER_THREAD
{
    LIST_ENTRY    list;
    HANDLE        thread_id;
    KEVENT        thread_event;
    KSPIN_LOCK    spinLock;
}WORKER_THREAD,*PWORKER_THREAD;

typedef    struct    _WORKER_PROCESS_NODE
{
    LIST_ENTRY  list;
    KEVENT      process_event;
    MY_PROCESS  routineAddress;
    PVOID       Context;
}WORKER_PROCESS_NODE,*PWORKER_PROCESS_NODE;

typedef    struct    _VOLUME_LINK_ITEM
{
    WCHAR   VolumeLetter;
    UINT32  crcMappedDevName;  //crc hash value of the device name mapped by VolumeLetter
}VOLUME_LINK_ITEM,*PVOLUME_LINK_ITEM;

NTSTATUS
GetLocalTime( OUT PTIME_FIELDS  localTime );

VOID
CreateVolumeLinkTable();

NTSTATUS
DevicePathToDosPath(
    IN PUNICODE_STRING devicePath,
    OUT PUNICODE_STRING dosPath);

UINT32
HashUnicodeString( IN PUNICODE_STRING s );

NTSTATUS
UnicodeStringToUint32( IN PUNICODE_STRING str,OUT UINT32 *result );

NTSTATUS  
CreateWorkerThread();

VOID
DestroyWorkerThread();

NTSTATUS
RunMyProcess( IN MY_PROCESS ToyMyProcess, PVOID Context);

__inline
void
GetFlagsIndexesForLayer(
   IN UINT16 layerId,
   OUT UINT* flagsIndex
   );

BOOLEAN IsAleReauthorize(
   IN const FWPS_INCOMING_VALUES0* inFixedValues
   );
