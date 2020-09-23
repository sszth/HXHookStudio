// HXFileFastFind.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
//#include <atlstr.h>
//#include <afxtempl.h>
#include <list>
#include <vector>
#include <thread>
#include <afxwin.h>
#include <Windows.h>
#include <atlutil.h>
#include "HXThreadPool.h"
#include "HXFileFastFind.h"

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

//int main(int argc, char* argv[])
//{
//    CString strDir = _T("G:\\20_WindowsSystem\\01——简单TCP_UDP\\*");
//    //CString strDir = _T("G:\\*");
//    HXThreadPool::Initstance()->Start(strDir);
//    HXThreadPool::Initstance()->ShutDown(INFINITE);
//
//    return 0;
//}

void HXStartFastFind(std::wstring strDir)
{
    CString strDirTmp = strDir.data();
    HXThreadPool::Initstance()->Start(strDirTmp);
}

void HXShutDown(DWORD dwMaxWait)
{
    HXThreadPool::Initstance()->ShutDown(dwMaxWait);
}
