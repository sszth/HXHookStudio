#pragma once
// ����һЩ��Windows����ϵͳ��صĺ���  ����ֱ��ʹ��
#include <Windows.h>
#define HX_THREAD_MAX_COUNT	500

// ���CPU����������
DWORD HXGetSysNumberOfProcessors()
{
	SYSTEM_INFO sysInfo;
	GetNativeSystemInfo(&sysInfo);
	return sysInfo.dwNumberOfProcessors;
}

// ��ȡҳ���С
DWORD HXGetSysPageSize()
{
	SYSTEM_INFO sysInfo;
	GetNativeSystemInfo(&sysInfo);
	return sysInfo.dwPageSize;
}

// ���Ĭ���̴߳�С 
DWORD HXGetDefaultWorkerThreadCout()
{
	// i/o�ܼ���Ϊcpu������*2+2; cpu�ܼ���Ϊcpu������*1+1
	static DWORD s_dwtDefaultWorkerThreadCout = min((HXGetSysNumberOfProcessors() * 2 + 2), HX_THREAD_MAX_COUNT);
	return s_dwtDefaultWorkerThreadCout;
}

// ���Ĭ��buffer��С
DWORD GetDefaultBufferSize()
{
	static DWORD s_dtsbs = HXGetSysPageSize();
	return s_dtsbs;
}