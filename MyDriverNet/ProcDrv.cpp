
#include <ntddk.h>
#include <devioctl.h>
#include "ProcDrv.h"


NTSTATUS DispatchCreateClose(PDEVICE_OBJECT pDevObj, PIRP pIrp);
void DriverUnload(PDRIVER_OBJECT pDriverObj);
NTSTATUS DispatchIoctl(PDEVICE_OBJECT pDevObj, PIRP pIrp);
VOID ProcessCallback(IN HANDLE hParentId, IN HANDLE hProcessID, IN BOOLEAN bCreate);

// �����ڲ����ơ������������ơ��¼���������
#define DRVICE_NAME			L"\\Device\\devHXNTProcDrv"
#define LINK_NAME			L"\\DosDevices\\slHXNTProcDrv"
#define EVENT_NAME			L"\\BaseNamedObjects\\HXNTPocDrvProcessEvent"

typedef struct _DEVICE_EXTENSION
{
	HANDLE	hProcessHandle;
	PKEVENT ProcessEvent;
	HANDLE	hPParentId;
	HANDLE	hPProcessId;
	BOOLEAN bPCreate;
} DEVICE_EXTENSION, *PDEIVICE_EXTENSION;


PDEVICE_OBJECT g_pDeviceObject;

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegistryString)
{
	NTSTATUS status = STATUS_SUCCESS;

	pDriverObj->MajorFunction[IRP_MJ_CREATE] = DispatchCreateClose;
	pDriverObj->MajorFunction[IRP_MJ_CLOSE] = DispatchCreateClose;
	pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoctl;
	pDriverObj->DriverUnload = DriverUnload;

	// ��������ʼ���豸����
	// �豸����
	UNICODE_STRING ustrDevName;
	RtlInitUnicodeString(&ustrDevName, DRVICE_NAME);
	// �����豸����
	PD
}