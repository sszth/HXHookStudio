/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_HXNDIS,
    0xc57622b9,0xaa8d,0x4b3b,0x9a,0xed,0x16,0x19,0xd4,0x18,0x02,0x2e);
// {c57622b9-aa8d-4b3b-9aed-1619d418022e}
