// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include "HXThreadPool.h"
#include "HXFileFastFind.h"

#ifdef ATL_THREADPOOL   // ATL CThreadPool 在dll中线程一直创建失败
void HXStartFastFind(std::wstring strDir)
{
	CString strDirTmp = strDir.data();
	HXThreadPool::Initstance()->Init(strDirTmp);
}

void HXShutDown(DWORD dwMaxWait)
{
	HXThreadPool::Initstance()->ShutDown(dwMaxWait);
}
#else
//#endif
//#ifdef HX_ATL_THREADPOOL   // 参照ATL实现线程池
HXThreadPool g_ThreadPool;
void HXStartFastFind(std::wstring strDir)
{

}

void HXShutDown(DWORD dwMaxWait)
{
}
#endif


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
#ifndef ATL_THREADPOOL   // ATL CThreadPool 在dll中线程一直创建失败
		//#endif
		//#ifdef HX_ATL_THREADPOOL   // 参照ATL实现线程池
		g_ThreadPool.Initialize();
#endif
		break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

