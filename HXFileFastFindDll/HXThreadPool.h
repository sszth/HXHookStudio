#pragma once
#include <atlutil.h>
#include <unordered_map>
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

