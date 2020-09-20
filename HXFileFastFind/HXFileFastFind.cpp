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




//void ListPush(CString strFile)
//{
//    EnterCriticalSection(&g_listFileSection);
//    g_listFile.push_back(strFile);
//    LeaveCriticalSection(&g_listFileSection);
//}
//
//inline void TTEngWaitForObject(HANDLE hHandle)
//{
//    if (NULL == hHandle)
//        return;
//
//    MSG msg;
//    int nTaskThreads = 1;
//    while (nTaskThreads > 0 && NULL != hHandle)
//    {
//        // @warning: 这里的fWaitAll不能设置成TRUE，否则会发生死锁
//        int uiRet = (int)MsgWaitForMultipleObjects(nTaskThreads, &hHandle, FALSE, INFINITE, QS_ALLINPUT);
//        if (uiRet == WAIT_OBJECT_0 + nTaskThreads)
//        {
//            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//            {
//                TranslateMessage(&msg);
//                DispatchMessage(&msg);
//            }
//        }
//        else if (uiRet >= WAIT_OBJECT_0 && uiRet < (int)(WAIT_OBJECT_0 + nTaskThreads))
//        {
//            // @waring: MsgWaitForMultipleObjects不允许m_threadIds数组中有空隙
//            int nIndex = uiRet - WAIT_OBJECT_0;
//            hHandle = NULL;
//            nTaskThreads--;
//        }
//        else
//        {
//            uiRet = GetLastError();
//            break;
//        }
//    }
//}
int main(int argc, char* argv[])
{
    CString strDir = _T("G:\\20_WindowsSystem\\01——简单TCP_UDP\\*");
    //CString strDir = _T("G:\\*");
    HXThreadPool::Initstance()->Start(strDir);
    HXThreadPool::Initstance()->ShutDown(INFINITE);

    return 0;
}

