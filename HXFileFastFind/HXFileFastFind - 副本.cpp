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

void ListPush(CString strFile);
struct HXTask;
class HXWork
{
public:
    typedef DWORD_PTR RequestType;

    BOOL Initialize(void* pvWorkerParam)
    {
        return true;
    }

    void Execute(RequestType request, void* pvWorkerParam, OVERLAPPED* pOverlapped);

    void Terminate(void* pvWorkerParam)
    {
        return;
    }

};
CThreadPool<HXWork> g_ThreadPool;

struct HXTask
{
    CString m_strDir;
    HXTask(CString strDir):m_strDir(strDir){}
    void DoTask(void* pvParam, OVERLAPPED* pOverlapped)
    {
        CString strDir = m_strDir;
        std::vector<HANDLE> listHandle;
        std::list<CString> vecDir;

        WIN32_FIND_DATAW w32FindData;
        HANDLE hd = FindFirstFileEx(strDir, FINDEX_INFO_LEVELS::FindExInfoBasic, &w32FindData, FINDEX_SEARCH_OPS::FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
        if (INVALID_HANDLE_VALUE != hd)
        {
            do
            {
                if (_tcscmp(w32FindData.cFileName, _T(".")) == 0 || _tcscmp(w32FindData.cFileName, _T("..")) == 0)
                {
                    continue;
                }

                if (w32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    //TODO:新线程用于查找文件
                    DWORD dwID;
                    CString strFull;
                    int nValidPath = strDir.ReverseFind(_T('\\'));
                    strFull = strDir.Left(nValidPath);
                    strFull += _T("\\");
                    strFull += w32FindData.cFileName;
                    strFull += _T("\\*");
                    vecDir.push_back(strFull);
                    auto iter = vecDir.rbegin();
                    HXTask* pTask = new HXTask(strFull);
                    g_ThreadPool.QueueRequest((DWORD_PTR)pTask);
                    //HANDLE hd = CreateThread(NULL, 0, ThreadProc, (LPVOID)(&(*iter)), 0, &dwID);
                    //listHandle.push_back(hd);
                    continue;
                }
                ListPush(w32FindData.cFileName);
            } while (FindNextFile(hd, &w32FindData));
            //CloseHandle(hd);
        }
        //for (size_t i = 0; i < listHandle.size(); i++)
        //{
        //    TTEngWaitForObject(listHandle[i]);
        //}

    }
};



CRITICAL_SECTION g_listFileSection;
std::list<CString>  g_listFile;

std::list<HANDLE> g_arrThreadId;
CRITICAL_SECTION g_arrThreadIdSection;

void ListPush(CString strFile)
{
    EnterCriticalSection(&g_listFileSection);
    g_listFile.push_back(strFile);
    LeaveCriticalSection(&g_listFileSection);
}
void PushThreadID(HANDLE hd)
{
    EnterCriticalSection(&g_arrThreadIdSection);
    g_arrThreadId.push_back(hd);
    LeaveCriticalSection(&g_arrThreadIdSection);
}

inline void TTEngWaitForObject(HANDLE hHandle)
{
    if (NULL == hHandle)
        return;

    MSG msg;
    int nTaskThreads = 1;
    while (nTaskThreads > 0 && NULL != hHandle)
    {
        // @warning: 这里的fWaitAll不能设置成TRUE，否则会发生死锁
        int uiRet = (int)MsgWaitForMultipleObjects(nTaskThreads, &hHandle, FALSE, INFINITE, QS_ALLINPUT);
        if (uiRet == WAIT_OBJECT_0 + nTaskThreads)
        {
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else if (uiRet >= WAIT_OBJECT_0 && uiRet < (int)(WAIT_OBJECT_0 + nTaskThreads))
        {
            // @waring: MsgWaitForMultipleObjects不允许m_threadIds数组中有空隙
            int nIndex = uiRet - WAIT_OBJECT_0;
            hHandle = NULL;
            nTaskThreads--;
        }
        else
        {
            uiRet = GetLastError();
            break;
        }
    }
}
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
    CString strDir = *((CString*)lpParam);
    std::vector<HANDLE> listHandle;
    std::list<CString> vecDir;
    
    WIN32_FIND_DATAW w32FindData;
    HANDLE hd = FindFirstFileEx(strDir, FINDEX_INFO_LEVELS::FindExInfoBasic, &w32FindData, FINDEX_SEARCH_OPS::FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
    if (INVALID_HANDLE_VALUE != hd)
    {
        do
        {
            if (_tcscmp(w32FindData.cFileName, _T(".")) == 0 || _tcscmp(w32FindData.cFileName, _T("..")) == 0)
            {
                continue;
            }

            if (w32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                //TODO:新线程用于查找文件
                DWORD dwID;
                CString strFull;
                int nValidPath = strDir.ReverseFind(_T('\\'));
                strFull = strDir.Left(nValidPath);
                strFull += _T("\\");
                strFull += w32FindData.cFileName;
                strFull += _T("\\*");
                vecDir.push_back(strFull);
                auto iter = vecDir.rbegin();
                HANDLE hd = CreateThread(NULL, 0, ThreadProc, (LPVOID)(&(*iter)), 0, &dwID);
                listHandle.push_back(hd);
                continue;
            }
            ListPush(w32FindData.cFileName);
        } while (FindNextFile(hd, &w32FindData));
        //CloseHandle(hd);
    }
    for (size_t i = 0; i < listHandle.size(); i++)
    {
        TTEngWaitForObject(listHandle[i]);
    }
    return 0;
}

int main(int argc, char* argv[])
{
    InitializeCriticalSection(&g_listFileSection);
    InitializeCriticalSection(&g_arrThreadIdSection);
    g_ThreadPool.Initialize();

    CString strDir = L"G:\\20_WindowsSystem\\01——简单TCP_UDP\\*";
    CString strTargetFile = L"*.pdf";
    std::list<CString>  listDir;
    HXTask* pTask = new HXTask(strDir);							// 创建任务
    g_ThreadPool.QueueRequest((DWORD_PTR)pTask); // 执行线程池任务
    DWORD dwID;
    HANDLE hd = CreateThread(NULL, 0, ThreadProc, (LPVOID)(&strDir), 0, &dwID);
    TTEngWaitForObject(hd);
    //g_ThreadPool.Shutdown(INFINITE);
    Sleep(600 * 1000);
    return 0;
}

void HXWork::Execute(RequestType request, void* pvWorkerParam, OVERLAPPED* pOverlapped)
{
    {
        HXTask* p = (HXTask*)request;
        p->DoTask(pvWorkerParam, pOverlapped);
        delete p;
    }
}
