#pragma once
#include <Windows.h>

#define _Acquires_lock_(lock)
#include <unordered_map>
// ATL_THREADPOOLΪ�����汾��������ִ�С��ı��С���ȴ��������ж�
//#define ATL_THREADPOOL
// ATL��ǿ��  ��ӳɹ��ص�
#define HX_ATL_THREADPOOL

#ifdef ATL_THREADPOOL
#include <atlstr.h>
typedef	CString HXString;
#endif
#ifdef HX_ATL_THREADPOOL
#include <string>
typedef	std::wstring HXString;
#endif




#ifdef ATL_THREADPOOL   // ATL CThreadPool ��dll���߳�һֱ����ʧ��
#define HX_ATLS_POOL_SHUTDOWN ((OVERLAPPED*) ((__int64) -1))
#include <atlutil.h>
#include <atomic>
#include <vector>


// ���Ϊ���̸�Ŀ¼����everything��3���ļ�
class HXThreadPool
{
public:
	typedef struct HXDirMeta
	{
		std::wstring m_strDir;
		std::wstring m_strFileName;
	}*PHXDirMeta;
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

	LRESULT Initialize()
    {
		// ��I/O�ܼ�
		//DWORD dw = HXGetDefaultWorkerThreadCout();
		//m_ThreadPool.Initialize(NULL, dw);
		// �����dll�л��������
		m_ThreadPool.Initialize();
		m_llTaskNum = 0L;
		m_llDirNum = 0L;
		auto hx = 0LL;

		m_hResEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!m_hResEvent)
		{

			return E_FAIL;
		}
		m_hResQueue = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (m_hResQueue == NULL)
		{
			// failed creating the Io completion port
			CloseHandle(m_hResEvent);
			return AtlHresultFromLastError();
		}
		UINT dwThreadID;
		ResetEvent(m_hResEvent);
		uintptr_t hTHread = _beginthreadex(NULL, 0, WorkThreadRecvRes, (LPVOID)this, NULL, &dwThreadID);
		if (!hTHread)
		{
			return E_FAIL;
		}
		DWORD dwRet = WaitForSingleObject(m_hResEvent, 60000);
		if (dwRet != WAIT_OBJECT_0)
		{
			if (dwRet == WAIT_TIMEOUT)
			{
				return HRESULT_FROM_WIN32(WAIT_TIMEOUT);
			}
			else
			{
				return AtlHresultFromLastError();
			}
		}
		return S_OK;
    }
    BOOL MapAdd(std::wstring strDir, std::wstring strFileName)
	{
		m_llDirNum++;
		PHXDirMeta p = new HXDirMeta();
		p->m_strDir = strDir;
		p->m_strFileName = strFileName;
		if (!PostQueuedCompletionStatus(m_hResQueue, 0, (ULONG_PTR)p, NULL))
		{
			return FALSE;
		}
		return TRUE;
	}
	LRESULT ShutDown(DWORD dwMaxWait)
	{
		m_ThreadPool.Shutdown(dwMaxWait);
		ResetEvent(m_hResEvent);
		// TODO:WaitForSingleObject�Ż�

		if (!PostQueuedCompletionStatus(m_hResQueue, 0, 0, ATLS_POOL_SHUTDOWN))
		{
			return FALSE;
		}
		DWORD dwRet = WaitForSingleObject(m_hResEvent, dwMaxWait);
		if (WAIT_TIMEOUT == dwRet)
		{
			return E_FAIL;
		}
		else if (WAIT_OBJECT_0 != dwRet)
		{
			return E_FAIL;
		}

		m_mapRes.clear();
		return S_OK;
	}

	void Start(CString strDir)
	{
		m_llTaskNum++;

		HXTask* pTask = new HXTask(strDir);
		m_ThreadPool.QueueRequest((DWORD_PTR)pTask);
	}

	void WaitCurrentTaskEnd()
	{
		// ���ҽ���
		while (m_llTaskNum)
		{
			::SwitchToThread();
		}
		// ���·������
		while (m_llDirNum)
		{
			::SwitchToThread();
		}

		// ��ǰ�������
		ReleaseAll();
	}
private:
	static UINT WINAPI WorkThreadRecvRes(LPVOID pv)
	{
		HXThreadPool* pThis =
			reinterpret_cast<HXThreadPool*>(pv);

		return pThis->ThreadRecvRes();
	}
	UINT WINAPI ThreadRecvRes()
	{
		DWORD dwBytesTransfered;
		ULONG_PTR dwCompletionKey;
		OVERLAPPED* pOverlapped;
		SetEvent(m_hResEvent);
		while (GetQueuedCompletionStatus(m_hResQueue, &dwBytesTransfered, &dwCompletionKey, &pOverlapped, INFINITE))
		{
			if (pOverlapped == HX_ATLS_POOL_SHUTDOWN) // Shut down
			{
				// TODO:shutdown cancelled
				break;
			}
			else
			{
				// TODO:buffer ��
				PHXDirMeta p = (PHXDirMeta)dwCompletionKey;
				std::pair<std::wstring, std::wstring>  pair = std::pair<std::wstring, std::wstring>(p->m_strDir, p->m_strFileName);
				m_mapRes.insert(pair);
				delete p;

				m_llDirNum--;
			}
		}
		SetEvent(m_hResEvent);
		return 0;
	}

	void ReleaseAll()
	{
		if (NULL!=m_hResQueue)
		{
			CloseHandle(m_hResEvent);
			m_hResEvent = NULL;
		}
		if (NULL != m_hResQueue)
		{
			CloseHandle(m_hResQueue);
			m_hResQueue = NULL;
		}
	}

	void FindEnd()
	{
	}
public:
	std::atomic_ullong m_llTaskNum;
	std::atomic_ullong m_llDirNum;

private:
    static HXThreadPool* m_Init;
    CThreadPool<HXWork> m_ThreadPool;

	HANDLE m_hResQueue;
	HANDLE m_hResEvent;
	std::unordered_map<std::wstring, std::wstring>   m_mapRes;

private:
    HXThreadPool();
    HXThreadPool(HXThreadPool&) = delete;
    HXThreadPool& operator=(const HXThreadPool&) = delete;

};


#endif

#ifdef HX_ATL_THREADPOOL   // ����ATLʵ���̳߳�
//Ĭ�ϴ����̸߳���=(-HX_THREAD_DEFAULT_MULTIPLE)*CPU������
#include <process.h>
#include <string>
#include <atomic>
#include "../CommonFiles/HXCriticalSection.h"
#define HX_THREAD_DEFAULT_MULTIPLE	(-1)
#define HX_POOL_SHUTDOWN			((OVERLAPPED*) ((__int64) -1))

class CHXWorkBase
{
public:
	CHXWork()
	{}
	~CHXWork()
	{}

	virtual 
private:

};

class HXThreadPool
{
public:
	typedef std::unordered_map<DWORD, HANDLE> ThreadMap;
	typedef std::unordered_multimap<std::wstring, std::wstring> ResMap;

	static HXThreadPool* Initstance()
	{
		return m_Init;
	}

    HRESULT Initialize(int nNumThreads = 0, DWORD dwStackSize = 0, HANDLE hCompletion = INVALID_HANDLE_VALUE)
    {
        _ASSERT(m_hRequestQueue == INVALID_HANDLE_VALUE);

        if (m_hRequestQueue)
        {
            ::SetLastError(ERROR_ALREADY_INITIALIZED);
            return E_FAIL;
        }

		m_dwThreadEventId = 0;
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
		HXThreadPool* pThreadPool = reinterpret_cast<HXThreadPool*>(lpParam);
		if (!pThreadPool)
		{
			return -1;
		}
		return pThreadPool->ThreadProc();
	}
	void Start(HXString strDir)
	{

	}

	LRESULT ShutDown(DWORD dwMaxWait)
	{
		return S_OK;
	}

	void WaitCurrentTaskEnd()
	{

	}

	bool	InitWork()
	{
		return true;
	}
	bool	TerminateWork()
	{
		return true;
	}
	void	ExecuteWork()
	{}
protected:
	LRESULT ThreadProc() throw()
	{
		DWORD dwNumberOfBytesTransferred = 0;
		ULONG_PTR lpCompletionKey = NULL;
		OVERLAPPED * pOverlapped = NULL;
		// �˿�����ȷ��_beginthread�з���ֵ��
		// TODO��ѧ��ATL::CThreadPool ��λ����֪��ԭ���鷳ָ����С��
		{
			if (!InitWork())
			{
				// TODO:�˴��Գ�ʼ��ʧ�� �ߵȴ�m_hThreadEvent��ʱ���� ���Ż�
				return 1;
			}
			SetEvent(m_hThreadEvent);
			while (GetQueuedCompletionStatus(m_hRequestQueue, &dwNumberOfBytesTransferred, &lpCompletionKey, &pOverlapped, INFINITE))
			{
				if (HX_POOL_SHUTDOWN == pOverlapped)
				{
					// TODO:�ݲ��ṩ��ֹ����
					break;
				}
				else
				{
					ExecuteWork();
				}
			}
			TerminateWork();
		}
		m_dwThreadEventId = GetCurrentThreadId();
		SetEvent(m_hThreadEvent);
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

		m_critSec.Lock();
		int nCurrentThreads = m_threadMap.size();
		if (nNumThreads == nCurrentThreads)
		{
			return S_OK;
		}
		else if (nNumThreads < nCurrentThreads)
		{
			ResetEvent(m_hThreadEvent);
			// ��m_hRequestQueue����ǰ �����˳��˺���
			PostQueuedCompletionStatus(m_hRequestQueue, 0, 0, HX_POOL_SHUTDOWN);
			// ��Ҫ���еȴ���ʱ����
			DWORD dwRet = WaitForSingleObject(m_hThreadEvent, dwMaxWait);
			// ��APC���̴��� APC�������QueueUserAPC
			// WaitForSingleObjectEx(m_hThreadEvent, dwMaxWait, TRUE);
			if (dwRet == WAIT_TIMEOUT)
			{
				return E_FAIL;
			}
			else if(dwRet != WAIT_OBJECT_0)
			{
				return E_FAIL;
			}
			ThreadMap::iterator iter = m_threadMap.find(m_dwThreadEventId);
			if (iter == m_threadMap.end())
			{
				return E_FAIL;
			}

			if (WaitForSingleObject(iter->second, 60 * 1000) == WAIT_TIMEOUT)
			{
				CloseHandle(iter->second);
				m_threadMap.erase(m_dwThreadEventId);
			}
			else
			{
				return E_FAIL;
			}
		}
		else
		{
			int nNumNewThreads = nNumThreads - nCurrentThreads;
			for (int nThreadIndex = 0; nThreadIndex < nNumNewThreads; nThreadIndex++)
			{
				UINT dwThreadID;
				ResetEvent(m_hThreadEvent);				
				uintptr_t nThread = _beginthreadex(NULL, 0, WorkerThreadProc, (LPVOID)this, 0, &dwThreadID);
				if (FAILED(nThread))
				{
					return E_FAIL;
				}

				DWORD dwRet = WaitForSingleObject(m_hThreadEvent, dwMaxWait);
				if (WAIT_OBJECT_0 != dwRet)
				{
					if (WAIT_TIMEOUT == dwRet)
					{
						SetLastError(WAIT_TIMEOUT);
						return E_FAIL;
					}
					else
					{
						return E_FAIL;
					}
				}
				m_threadMap.insert(std::pair< DWORD, HANDLE >(dwThreadID, (HANDLE)nThread));
			}
		}
		m_critSec.Unlock();
		return S_OK;
	}

private:
    static HXThreadPool* m_Init;

    HANDLE	m_hRequestQueue;
	DWORD	m_dwMaxWait;
	HANDLE	m_hThreadEvent;
	DWORD	m_dwThreadEventId;
	CHXAutoCriticalSection	m_critSec;
	ThreadMap	m_threadMap;
	ResMap		m_mapRes;
private:
	HXThreadPool()
	{}
    HXThreadPool(HXThreadPool&) = delete;
    HXThreadPool& operator=(const HXThreadPool&) = delete;
	~HXThreadPool()
	{}
};


#endif // ATL_THREADPOOL
