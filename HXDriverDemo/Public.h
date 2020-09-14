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

DEFINE_GUID (GUID_DEVINTERFACE_HXDriverDemo,
    0xec231cd7,0x96d5,0x426d,0xa8,0x23,0xd6,0x13,0x8a,0xac,0x7e,0xd6);
// {ec231cd7-96d5-426d-a823-d6138aac7ed6}
