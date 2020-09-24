
#include "stdafx.h"
#include "HXThreadPool.h"

#ifdef ATL_THREADPOOL
#include "../CommonFiles/HXSysHelper.h"
HXThreadPool* HXThreadPool::m_Init = new HXThreadPool();

BOOL HXThreadPool::HXWork::Initialize(void* pvWorkerParam)
{
    return true;
}

void HXThreadPool::HXWork::Execute(RequestType request, void* pvWorkerParam, OVERLAPPED* pOverlapped)
{
    HXTask* p = (HXTask*)request;
    p->DoTask(pvWorkerParam, pOverlapped);
    delete p;
}

void HXThreadPool::HXWork::Terminate(void* pvWorkerParam)
{
    //TODO:强制关线程
}

void HXThreadPool::HXTask::DoTask(void* pvParam, OVERLAPPED* pOverlapped)
{
    CString strDir = m_strDir;
    std::vector<HANDLE> listHandle;

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
                // 新线程用于查找文件
                DWORD dwID;
                CString strFull;
                int nValidPath = strDir.ReverseFind(_T('\\'));
                strFull = strDir.Left(nValidPath);
                strFull += _T("\\");
                strFull += w32FindData.cFileName;
                strFull += _T("\\*");
                HXThreadPool::Initstance()->Start(strFull);
                continue;
            }
            std::wstring strDir1 = strDir.GetString();
            std::wstring strDir2 = w32FindData.cFileName;
            HXThreadPool::Initstance()->MapAdd(strDir1, strDir2);
        } while (FindNextFile(hd, &w32FindData));
        if (hd != INVALID_HANDLE_VALUE)
        {
            FindClose(hd);
        }
    }
}

HXThreadPool::HXThreadPool()
{
}


#else

#endif // ATL_THREADPOOL
