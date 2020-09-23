#pragma once
#include <Windows.h>

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

    void Initialize()
    {
		// 非I/O密集
		//DWORD dw = HXGetDefaultWorkerThreadCout();
		//m_ThreadPool.Initialize(NULL, dw);
		// 如果在dll中会造成死锁
		m_ThreadPool.Initialize();

		InitializeCriticalSection(&m_listFileSection);
    }
    void MapAdd(std::wstring strDir, std::wstring strFileName);
    void ShutDown(DWORD dwMaxWait);

	void Start(CString strDir)
	{
		HXTask* pTask = new HXTask(strDir);
		m_ThreadPool.QueueRequest((DWORD_PTR)pTask);
	}
private:
	

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
//#endif
//#ifdef HX_ATL_THREADPOOL   // 参照ATL实现线程池
//默认创建线程个数=(-HX_THREAD_DEFAULT_MULTIPLE)*CPU核心数
#include <unordered_map>
#include <process.h>
#define HX_THREAD_DEFAULT_MULTIPLE	(-1)
class HXThreadPool
{
public:
	HXThreadPool()
	{
	}
	~HXThreadPool()
	{

	}

    HRESULT Initialize(int nNumThreads = 0, DWORD dwStackSize = 0, HANDLE hCompletion = INVALID_HANDLE_VALUE)
    {
        _ASSERT(m_hRequestQueue == INVALID_HANDLE_VALUE);

        if (m_hRequestQueue)
        {
            ::SetLastError(ERROR_ALREADY_INITIALIZED);
            return E_FAIL;
        }

		InitializeCriticalSection(&m_critSec);

		m_hThreadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_hThreadEvent == INVALID_HANDLE_VALUE)
		{
			return E_FAIL;
		}

		// Create IO completion port to queue the requests
		m_hRequestQueue = CreateIoCompletionPort(hCompletion, NULL, 0, nNumThreads);
		if (m_hRequestQueue == INVALID_HANDLE_VALUE)
		{
			return E_FAIL;
		}

		HRESULT hr = SetSize(nNumThreads);

    }
	static  UINT _stdcall WorkerThreadProc(LPVOID lpParam)
	{
		HXThreadPool* pThreadPool = (HXThreadPool*)lpParam;
		
		return 0;
	}
private:
	HRESULT SetSize(int nNumThreads)
	{
		if (nNumThreads == 0)
		{
			nNumThreads = HX_THREAD_DEFAULT_MULTIPLE;
		}
		if (nNumThreads < 0)
		{
			SYSTEM_INFO sysInfo;
			GetNativeSystemInfo(&sysInfo);
			nNumThreads = (int)(-nNumThreads)*sysInfo.dwNumberOfProcessors;
		}

		return ResizePool(nNumThreads, m_dwMaxWait);
	}

	HRESULT ResizePool(int nNumThreads, int dwMaxWait)
	{
		if (m_hRequestQueue == INVALID_HANDLE_VALUE)
		{
			return E_FAIL;
		}

		EnterCriticalSection(&m_critSec);
		int nCurrentThreads = m_threadMap.size();
		if (nNumThreads == nCurrentThreads)
		{
			return S_OK;
		}
		else if (nNumThreads < nCurrentThreads)
		{

		}
		else
		{
			int nNumNewThreads = nNumThreads - nCurrentThreads;

			for (int nThreadIndex = 0; nThreadIndex < nNumNewThreads; nThreadIndex++)
			{
				UINT dwThreadID;
				ResetEvent(m_hThreadEvent);				
				UINT_PTR nThread = _beginthreadex(NULL, 0, WorkerThreadProc, (LPVOID)this, 0, &dwThreadID);
				if (NULL != nThread)
				{
					return E_FAIL;
				}

				DWORD dwRet = WaitForSingleObject(m_hThreadEvent, dwMaxWait);
				if (WAIT_OBJECT_0 != dwRet)
				{
					if (WAIT_TIMEOUT != dwRet)
					{
						SetLastError(WAIT_TIMEOUT);
						return E_FAIL;
					}
					else
					{
						return E_FAIL;
					}
				}
			}
		}

		LeaveCriticalSection(&m_critSec);
	}

private:
    static HXThreadPool* m_Init;



    HANDLE m_hRequestQueue;
	DWORD m_dwMaxWait;
	HANDLE m_hThreadEvent;
	CRITICAL_SECTION	m_critSec;
	std::unordered_map<DWORD, HANDLE> m_threadMap;


    std::unordered_multimap<std::wstring, std::wstring>   m_mapRes;

    CRITICAL_SECTION m_listFileSection;
private:
    HXThreadPool(HXThreadPool&) = delete;
    HXThreadPool& operator=(const HXThreadPool&) = delete;
};


#endif // ATL_THREADPOOL
