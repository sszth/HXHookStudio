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

DEFINE_GUID (GUID_DEVINTERFACE_KMDFDriver1,
    0x7ca34803,0xeca9,0x4514,0xa9,0x2c,0xd5,0x9e,0x34,0x4d,0x1d,0x07);
// {7ca34803-eca9-4514-a92c-d59e344d1d07}
