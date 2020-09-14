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

DEFINE_GUID (GUID_DEVINTERFACE_HXDriver,
    0x01abf8f7,0xadfb,0x42f8,0x9c,0x02,0xb8,0xb2,0x91,0x7e,0xd4,0x32);
// {01abf8f7-adfb-42f8-9c02-b8b2917ed432}
