#pragma once

#include <unordered_map>
#define ATL_THREADPOOL  
#ifdef ATL_THREADPOOL   // ATL CThreadPool 在dll中线程一直创建失败

#include <atlutil.h>
class HXThreadPool
{
public:
    static HXThreadPool* Initstance()
    {
        return m_Init;
    }

    class HXWork
    {
    public:
        typedef DWORD_PTR RequestType;

        BOOL Initialize(void* pvWorkerParam);
        void Execute(RequestType request, void* pvWorkerParam, OVERLAPPED* pOverlapped);
        void Terminate(void* pvWorkerParam);
    };
    struct HXTask
    {
        CString m_strDir;
        HXTask(CString strDir) :m_strDir(strDir) {}
        void DoTask(void* pvParam, OVERLAPPED* pOverlapped);
    };

    void Start(CString strDir)
    {
        HXTask* pTask = new HXTask(strDir);
        m_ThreadPool.QueueRequest((DWORD_PTR)pTask);
    }
	void Init();
    void MapAdd(std::wstring strDir, std::wstring strFileName);
    void ShutDown(DWORD dwMaxWait);

private:
    static HXThreadPool* m_Init;
    CThreadPool<HXWork> m_ThreadPool;
    std::unordered_multimap<std::wstring, std::wstring>   m_mapRes;

    CRITICAL_SECTION m_listFileSection;
private:
    HXThreadPool();
    HXThreadPool(HXThreadPool&) = delete;
    HXThreadPool& operator=(const HXThreadPool&) = delete;
};

#else

class HXThreadPool
{
public:
    HXThreadPool();
    ~HXThreadPool();

    HRESULT Initialize(int nNumThreads = 0, DWORD dwStackSize = 0, HANDLE hCompletion = INVALID_HANDLE_VALUE)
    {
        ASSERT(m_hRequestQueue == INVALID_HANDLE_VALUE);

        if (m_hRequestQueue)
        {
            ::SetLastError(ERROR_ALREADY_INITIALIZED);
            return S_FALSE;
        }
    }

private:
    static HXThreadPool* m_Init;



    HANDLE m_hRequestQueue;
    //CThreadPool<HXWork> m_ThreadPool;
    std::unordered_multimap<std::wstring, std::wstring>   m_mapRes;

    CRITICAL_SECTION m_listFileSection;
private:
    HXThreadPool(HXThreadPool&) = delete;
    HXThreadPool& operator=(const HXThreadPool&) = delete;
};


#endif // ATL_THREADPOOL
