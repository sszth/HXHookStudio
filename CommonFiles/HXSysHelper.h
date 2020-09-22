#pragma once
// 定义一些与Windows操作系统相关的函数  方便直接使用
#include <Windows.h>
#define HX_THREAD_MAX_COUNT	500

// 获得CPU处理器个数
DWORD HXGetSysNumberOfProcessors()
{
	SYSTEM_INFO sysInfo;
	GetNativeSystemInfo(&sysInfo);
	return sysInfo.dwNumberOfProcessors;
}

// 获取页面大小
DWORD HXGetSysPageSize()
{
	SYSTEM_INFO sysInfo;
	GetNativeSystemInfo(&sysInfo);
	return sysInfo.dwPageSize;
}

// 获得默认线程大小 
DWORD HXGetDefaultWorkerThreadCout()
{
	// i/o密集型为cpu核心数*2+2; cpu密集型为cpu核心数*1+1
	static DWORD s_dwtDefaultWorkerThreadCout = min((HXGetSysNumberOfProcessors() * 2 + 2), HX_THREAD_MAX_COUNT);
	return s_dwtDefaultWorkerThreadCout;
}

// 获得默认buffer大小
DWORD GetDefaultBufferSize()
{
	static DWORD s_dtsbs = HXGetSysPageSize();
	return s_dtsbs;
}