#include "std"
#include "HXSysHelper.h"

const int g_nWorkThreadMax = 500;

DWORD HXGetDefaultWorkerThreadCout()
{
	// i/o密集型为cpu核心数*2+2; cpu密集型为cpu核心数*1+1
	static DWORD s_dwtDefaultWorkerThreadCout = min((HXGetSysNumberOfProcessors() * 2 + 2), g_nWorkThreadMax);
	return 0;
}

DWORD GetDefaultBufferSize()
{
	static DWORD s_dtsbs = HXGetSysPageSize();
	return s_dtsbs;
}

DWORD HXGetSysNumberOfProcessors()
{
	SYSTEM_INFO sysInfo;
	GetNativeSystemInfo(&sysInfo);
	return sysInfo.dwNumberOfProcessors;
}

DWORD HXGetSysPageSize()
{
	SYSTEM_INFO sysInfo;
	GetNativeSystemInfo(&sysInfo);
	return sysInfo.dwPageSize;
}
