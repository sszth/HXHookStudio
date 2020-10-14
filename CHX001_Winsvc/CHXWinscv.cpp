#include "CHXWinscv.h"

inline bool IsNull(HANDLE hHandle)
{
	return hHandle == NULL;
}
#define NULL_RETURN_FALSE(hHandle)	\
	if (IsNull(hHandle)){return S_FALSE;}
#define false_RETURN_FALSE(bRet)	\
	if (false==bRet){return S_FALSE;}
#define FALSE_RETURN_FALSE(bRet)	\
	if (FALSE==bRet){return S_FALSE;}

CHXWinscv::CHXWinscv()
	: m_hSCM(NULL)
	, m_hService(NULL)
{

}

CHXWinscv::~CHXWinscv()
{
	CleanHandle();
}

LRESULT CHXWinscv::GetService(OUT List_ENUM_SERVICE_STATUS& list, IN DWORD dwServiceType /*= SERVICE_DRIVER | SERVICE_WIN32*/, IN DWORD dwServiceState /*= SERVICE_STATE_ALL*/)
{
	// SC_MANAGER_ENUMERATE_SERVICE��ӦEnumServicesStatus/EnumServicesStatusEx/NotifyServiceStatusChangeִ��Ȩ��
	m_hSCM = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ENUMERATE_SERVICE);
	NULL_RETURN_FALSE(m_hSCM);

	// EnumServicesStatus �˺���ʹ��ʱ��Ҫע�⻺����  �����С��һֱʧ��  ����serStatus���Ϊ256K bytes
	LPBYTE				lpServices = NULL;
	DWORD               dwBufSize = 0;
	DWORD               dwBytesNeeded = 0;
	DWORD               dwServicesReturned = 0;
	DWORD               dwResumeHandle = 0;
	BOOL bRet = ::EnumServicesStatus(
		m_hSCM, 
		dwServiceType, 
		dwServiceState,
		(LPENUM_SERVICE_STATUS)lpServices,
		dwBufSize, 
		&dwBytesNeeded, 
		&dwServicesReturned, 
		&dwResumeHandle);

	switch (GetLastError())
	{
	case ERROR_SHUTDOWN_IN_PROGRESS:
		ResetServiceHandle(m_hSCM);
		return E_FAIL;
	case ERROR_MORE_DATA:
	{
		lpServices = (LPBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);
		dwBufSize = dwBytesNeeded;
		bRet = ::EnumServicesStatus(
			m_hSCM,
			dwServiceType,
			dwServiceState,
			(LPENUM_SERVICE_STATUS)lpServices,
			dwBufSize,
			&dwBytesNeeded,
			&dwServicesReturned,
			&dwResumeHandle);

		if (0 == bRet)
		{
			::HeapFree(GetProcessHeap(), 0, lpServices);
			ResetServiceHandle(m_hSCM);
			return S_FALSE;
		}
		LPENUM_SERVICE_STATUS lp = (LPENUM_SERVICE_STATUS)lpServices;
		for (int nIndex = 0; nIndex < dwServicesReturned; ++nIndex)
		{
			list.push_back(*(lp + nIndex));
		}
		break;
	}
	case ERROR_INVALID_PARAMETER:
	case ERROR_INVALID_HANDLE:
	case ERROR_INVALID_LEVEL:
	case ERROR_ACCESS_DENIED:
	default:
		ResetServiceHandle(m_hSCM);
		return S_FALSE;
		break;
	}

	CleanHandle();
	return S_OK;
}

LRESULT CHXWinscv::GetServiceEx(OUT List_ENUM_SERVICE_STATUS_PROCESS& list, IN DWORD dwServiceType /*= SERVICE_DRIVER | SERVICE_WIN32*/, IN DWORD dwServiceState /*= SERVICE_STATE_ALL*/, IN LPCWSTR pszGroupName /*= NULL*/)
{
	// SC_MANAGER_ENUMERATE_SERVICE��ӦEnumServicesStatus/EnumServicesStatusEx/NotifyServiceStatusChangeִ��Ȩ��
	m_hSCM = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ENUMERATE_SERVICE);
	NULL_RETURN_FALSE(m_hSCM);
	SC_ENUM_TYPE        InfoLevel = SC_ENUM_PROCESS_INFO;
	LPBYTE				lpServices = NULL;
	DWORD               dwBufSize = 0;
	DWORD				dwBytesNeeded = 0;
	DWORD				dwServicesReturned = 0;
	DWORD				dwResumeHandle = 0;

	BOOL bRet = ::EnumServicesStatusEx(
			m_hSCM,
			SC_ENUM_PROCESS_INFO,
			dwServiceType,
			dwServiceState,
			lpServices,
			dwBufSize,
			&dwBytesNeeded,
			&dwServicesReturned,
			&dwResumeHandle,
			pszGroupName);
	switch (GetLastError())
	{
	case ERROR_SHUTDOWN_IN_PROGRESS:
		CleanHandle();
		return E_FAIL;
	case ERROR_MORE_DATA:
	{
		lpServices = new BYTE[dwBytesNeeded];
		dwBufSize = dwBytesNeeded;
		bRet = ::EnumServicesStatusEx(
			m_hSCM,
			SC_ENUM_PROCESS_INFO,
			dwServiceType,
			dwServiceState,
			lpServices,
			dwBufSize,
			&dwBytesNeeded,
			&dwServicesReturned,
			&dwResumeHandle,
			pszGroupName);

		if (0 == bRet)
		{
			if (lpServices)
			{
				delete[] lpServices;
			}
			CleanHandle();
			return S_FALSE;
		}
		LPENUM_SERVICE_STATUS_PROCESS lp = (LPENUM_SERVICE_STATUS_PROCESS)lpServices;
		for (int nIndex = 0; nIndex < dwServicesReturned; ++nIndex)
		{
			list.push_back(*(lp+nIndex));
		}
		break;
	}
	case ERROR_INVALID_PARAMETER:
	case ERROR_INVALID_HANDLE:
	case ERROR_INVALID_LEVEL:
	case ERROR_ACCESS_DENIED:
	default:
		CleanHandle();
		return S_FALSE;
		break;
	}

	CleanHandle();
	return S_OK;
}

LRESULT CHXWinscv::CreateService(IN LPCWSTR lpServiceName
	, IN LPCWSTR lpDisplayName
	, IN DWORD dwDesiredAccess
	, IN DWORD dwServiceType
	, IN DWORD dwStartType
	, IN DWORD dwErrorControl
	, IN LPCWSTR lpBinaryPathName
	, IN LPCWSTR lpLoadOrderGroup
	, IN LPDWORD lpdwTagId
	, IN LPCWSTR lpDependencies
	, IN LPCWSTR lpServiceStartName
	, IN LPCWSTR lpPassword)
{
	m_hSCM = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	NULL_RETURN_FALSE(m_hSCM);

	m_hService = ::CreateService(m_hSCM
		, lpServiceName
		, lpDisplayName
		, dwDesiredAccess
		, dwServiceType
		, dwStartType
		, dwErrorControl
		, lpBinaryPathName
		, lpLoadOrderGroup
		, lpdwTagId
		, lpDependencies
		, lpServiceStartName
		, lpPassword);
	//SC_HANDLE hSCService = OpenService(
	//	m_hSCM,
	//	strServerName.data(),
	//	SERVICE_START
	//);
	CleanHandle();
	return S_OK;
}

LRESULT CHXWinscv::StartService(IN LPCWSTR lpServiceName, IN DWORD dwNumServiceArgs /*= 0*/, LPCWSTR* lpServiceArgVectors /*= NULL*/)
{
	m_hSCM = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	NULL_RETURN_FALSE(m_hSCM);

	// ��ȡ������
	m_hService = ::OpenService(
		m_hSCM,
		lpServiceName,
		SERVICE_START);
	if (NULL == m_hService)
	{
		CleanHandle();
	}
	   
	BOOL bRet = ::StartService(m_hService
		, dwNumServiceArgs
		, lpServiceArgVectors);

	if (!bRet)
	{
		switch (GetLastError())
		{
		case ERROR_ACCESS_DENIED:
		case ERROR_INVALID_HANDLE:
		case ERROR_PATH_NOT_FOUND:
			//�÷����ʵ���������С�		
		case ERROR_SERVICE_ALREADY_RUNNING:
		case ERROR_SERVICE_DATABASE_LOCKED:
			//�÷���ȡ���ڲ����ڵķ�����ѱ��Ϊɾ���ķ���
		case ERROR_SERVICE_DEPENDENCY_DELETED:
			//�÷�����������һ������ʧ�ܵķ���
		case ERROR_SERVICE_DEPENDENCY_FAIL:
		case ERROR_SERVICE_DISABLED:
			//���ڵ�¼ʧ�ܣ��÷���δ������ �������������Ϊ��û�С���Ϊ�����¼��Ȩ�޵��ʻ������У���ᷢ���˴���
		case ERROR_SERVICE_LOGON_FAILED:
		case ERROR_SERVICE_MARKED_FOR_DELETE:
		case ERROR_SERVICE_NO_THREAD:
			//����Ľ�������������δ����StartServiceCtrlDispatcher�������ڴ���������п�����������ΪStartServiceCtrlDispatcher���̡߳�
		case ERROR_SERVICE_REQUEST_TIMEOUT:
		default:
			break;
		}
	}

	CleanHandle();
	return S_OK;
}

LRESULT CHXWinscv::StartServiceEx(OUT SERVICE_STATUS_PROCESS& ssStatus,IN LPCWSTR lpServiceName, IN DWORD dwNumServiceArgs /*= 0*/, LPCWSTR* lpServiceArgVectors /*= NULL*/)
{
	ssStatus = { 0 };
	DWORD dwOldCheckPoint = 0;
	DWORD dwWaitTime = 0;
	DWORD dwStartTickCount = 0;
	m_hSCM = ::OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CONNECT);
	NULL_RETURN_FALSE(m_hSCM);

	// ��ȡ������
	m_hService = ::OpenService(
		m_hSCM,
		lpServiceName,
		SERVICE_START | SERVICE_QUERY_STATUS);
	if (NULL == m_hService)
	{
		CleanHandle();
		return S_FALSE;
	}
	DWORD	cbBufSize = sizeof(ssStatus);
	DWORD	cbBytesNeeded = 0;

	// ��˺�����SC_STATUS_PROCESS_INFOʱlpBuffer�����飬ֱ�ӻ�ȡֵ
	BOOL bRet = ::QueryServiceStatusEx(m_hService
		, SC_STATUS_PROCESS_INFO
		, (LPBYTE)&ssStatus
		, sizeof(ssStatus)
		, &cbBytesNeeded);
	if (!bRet)
	{
		switch (GetLastError())
		{
		case ERROR_INSUFFICIENT_BUFFER:
		case ERROR_INVALID_HANDLE:
		case ERROR_ACCESS_DENIED:
		case ERROR_INVALID_PARAMETER:
		case ERROR_INVALID_LEVEL:
		case ERROR_SHUTDOWN_IN_PROGRESS:
			CleanHandle();
			return S_FALSE;
		default:
			break;
		}
	}

	// TODO:����Ӳ������ڿ��Ʒ����Ƿ�����
	if (ssStatus.dwCurrentState != SERVICE_STOPPED
		&& ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
	{
		CleanHandle();
		return S_OK;
	}

	// ��ΪSERVICE_STOP_PENDING״̬���ȵȱ�ΪSERVICE_STOPPED
	dwStartTickCount = ::GetTickCount();
	dwOldCheckPoint	= ssStatus.dwCheckPoint;
	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		// �ڴ�״̬����õȴ�ʱ����ΧΪ1~10s
		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
		{
			dwWaitTime = 1000;
		}
		else if (dwWaitTime > 10000)
		{
			dwWaitTime = 100000;
		}

		Sleep(dwWaitTime);

		bRet = ::QueryServiceStatusEx(m_hService
			, SC_STATUS_PROCESS_INFO
			, (LPBYTE)&ssStatus
			, sizeof(ssStatus)
			, &cbBytesNeeded);
		if (!bRet)
		{
			CleanHandle();
			return S_FALSE;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			dwStartTickCount = ::GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if (::GetTickCount()-dwStartTickCount>ssStatus.dwWaitHint)
			{
				CleanHandle();
				return S_FALSE;
			}
		}
	}

	bRet = ::StartService(m_hService
		, dwNumServiceArgs
		, lpServiceArgVectors);
	if (!bRet)
	{
		switch (GetLastError())
		{
		case ERROR_PATH_NOT_FOUND:
			//�÷����ʵ���������С�
			CleanHandle();
			return S_OK;
		case ERROR_ACCESS_DENIED:
		case ERROR_INVALID_HANDLE:		
		case ERROR_SERVICE_ALREADY_RUNNING:
		case ERROR_SERVICE_DATABASE_LOCKED:
			//�÷���ȡ���ڲ����ڵķ�����ѱ��Ϊɾ���ķ���
		case ERROR_SERVICE_DEPENDENCY_DELETED:
			//�÷�����������һ������ʧ�ܵķ���
		case ERROR_SERVICE_DEPENDENCY_FAIL:
		case ERROR_SERVICE_DISABLED:
			//���ڵ�¼ʧ�ܣ��÷���δ������ �������������Ϊ��û�С���Ϊ�����¼��Ȩ�޵��ʻ������У���ᷢ���˴���
		case ERROR_SERVICE_LOGON_FAILED:
		case ERROR_SERVICE_MARKED_FOR_DELETE:
		case ERROR_SERVICE_NO_THREAD:
			//����Ľ�������������δ����StartServiceCtrlDispatcher�������ڴ���������п�����������ΪStartServiceCtrlDispatcher���̡߳�
		case ERROR_SERVICE_REQUEST_TIMEOUT:
			CleanHandle();
			return S_FALSE;
		default:
			break;
		}
	}

	bRet = ::QueryServiceStatusEx(m_hService
		, SC_STATUS_PROCESS_INFO
		, (LPBYTE)&ssStatus
		, sizeof(ssStatus)
		, &cbBytesNeeded);
	if (!bRet)
	{
		CleanHandle();
		return S_FALSE;
	}

	// У����������״̬
	dwStartTickCount = ::GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;
	while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
	{
		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
		{
			dwWaitTime = 1000;
		}
		else if (dwWaitTime > 10000)
		{
			dwWaitTime = 10000;
		}

		Sleep(dwWaitTime);

		bRet = ::QueryServiceStatusEx(m_hService
			, SC_STATUS_PROCESS_INFO
			, (LPBYTE)&ssStatus
			, sizeof(ssStatus)
			, &cbBytesNeeded);
		if (!bRet)
		{
			CleanHandle();
			return S_FALSE;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			dwOldCheckPoint = ssStatus.dwCheckPoint;
			dwStartTickCount = ::GetTickCount();
		}
		else
		{
			if (::GetTickCount() - ssStatus.dwCheckPoint > ssStatus.dwWaitHint)
			{
				// ������ʱ
				break;
			}
		}
	}

	if (ssStatus.dwCurrentState != SERVICE_RUNNING)
	{
		CleanHandle();
		return S_FALSE;
	}

	CleanHandle();
	return S_OK;
}

LRESULT CHXWinscv::StopServiceEx(OUT SERVICE_STATUS_PROCESS& ssStatus, IN LPCWSTR lpServiceName)
{
	ssStatus = { 0 };
	DWORD dwWaitTime = 0;
	DWORD dwStartTickCount = GetTickCount();
	DWORD cbBytesNeeded = 0;
	DWORD dwTimeout = 30000; // 30-second time-out
	m_hSCM = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_CREATE_SERVICE | SC_MANAGER_ENUMERATE_SERVICE);
	NULL_RETURN_FALSE(m_hSCM);
	m_hService = ::OpenService(m_hSCM, lpServiceName, SERVICE_ENUMERATE_DEPENDENTS | SERVICE_STOP | SERVICE_QUERY_STATUS);
	if (!m_hService)
	{
		ResetServiceHandle(m_hSCM);
	}

	if (!::QueryServiceStatusEx(m_hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &cbBytesNeeded))
	{
		CleanHandle();
		return S_FALSE;
	}

	if (ssStatus.dwCurrentState == SERVICE_STOP)
	{
		return S_OK;
	}

	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime > 10000)
		{
			dwWaitTime = 10000;
		}
		else if (dwWaitTime < 1000)
		{
			dwWaitTime = 1000;
		}
		Sleep(dwWaitTime);

		if (!::QueryServiceStatusEx(m_hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(SERVICE_STATUS_PROCESS), &cbBytesNeeded))
		{
			CleanHandle();
			return S_FALSE;
		}

		if (ssStatus.dwCurrentState == SERVICE_STOP)
		{
			CleanHandle();
			return S_OK;
		}

		if (::GetTickCount() - dwStartTickCount > dwTimeout)
		{
			CleanHandle();
			return S_FALSE;
		}
	}

	// �����ǰ���������У���Ҫֹͣ���� �˺���δֹͣ��������
	StopDependentServices(ssStatus);

	SERVICE_CONTROL_STATUS_REASON_PARAMS scsrParam;
	if (!::ControlServiceEx(m_hService, SERVICE_STOP, SERVICE_CONTROL_STATUS_REASON_INFO, &scsrParam))
	{
		CleanHandle();
		return S_FALSE;
	}

	while (scsrParam.ServiceStatus.dwCurrentState != SERVICE_STOP)
	{
		Sleep(scsrParam.ServiceStatus.dwWaitHint);
		if (!::QueryServiceStatusEx(m_hService, SC_STATUS_PROCESS_INFO))
		{
		}
		if (scsrParam.ServiceStatus.dwCurrentState == SERVICE_STOP)
		{
			break;
		}
		if (::GetTickCount() - dwStartTickCount > dwWaitTime)
		{
			CleanHandle();
			return S_FALSE;
		}
	}
	CleanHandle();
	return S_OK;
}

LRESULT CHXWinscv::StopDependentServices(OUT SERVICE_STATUS_PROCESS& ssStatus)
{
	LPENUM_SERVICE_STATUS lpDependencies = NULL;
	ENUM_SERVICE_STATUS	ess;
	SERVICE_CONTROL_STATUS_REASON_PARAMS scsrParam;
	SC_HANDLE hDependentService = NULL;
	DWORD cbBytesNeeded = 0;
	DWORD dwServicesReturned = 0;
	DWORD dwStartTime = ::GetTickCount();
	if (::EnumDependentServices(m_hService, SERVICE_ACTIVE, lpDependencies, 0, &cbBytesNeeded, &dwServicesReturned))
	{
		// ��������
		return S_OK;
	}
	else
	{
		if (GetLastError() != ERROR_MORE_DATA)
		{
			return S_FALSE;
		}

		lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cbBytesNeeded);
		NULL_RETURN_FALSE(lpDependencies);

		__try
		{
			if (::EnumDependentServices(m_hService, SERVICE_ACTIVE, lpDependencies, cbBytesNeeded, &cbBytesNeeded, &dwServicesReturned))
			{
				return S_FALSE;
			}

			for (int nIndex=0; nIndex < dwServicesReturned; ++nIndex)
			{
				ess = *(lpDependencies + nIndex);
				hDependentService = ::OpenService(m_hSCM, ess.lpServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
				NULL_RETURN_FALSE(hDependentService);

				__try
				{
					if (!::ControlServiceEx(hDependentService, SERVICE_CONTROL_STOP, SERVICE_CONTROL_STATUS_REASON_INFO, &scsrParam))
					{
						ResetServiceHandle(hDependentService);
						return S_FALSE;
					}

					while (scsrParam.ServiceStatus.dwCheckPoint != SERVICE_STOPPED)
					{
						Sleep(scsrParam.ServiceStatus.dwWaitHint);
						if (!::QueryServiceStatusEx(hDependentService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssStatus, sizeof(ssStatus), &cbBytesNeeded))
						{
							ResetServiceHandle(hDependentService);
							return S_FALSE;
						}

						if (ssStatus.dwCurrentState == SERVICE_STOPPED)
						{
							break;
						}
						if (GetTickCount()- dwStartTime > ssStatus.dwWaitHint)
						{
							ResetServiceHandle(hDependentService);
							return S_FALSE;
						}
					}
				}
				__finally
				{
					ResetServiceHandle(hDependentService);
				}
			}

		}
		__finally
		{
			HeapFree(GetProcessHeap(), 0, lpDependencies);
		}
	}
	return S_OK;
}

void CHXWinscv::ResetServiceHandle(IN OUT SC_HANDLE& hHandle)
{
	if (hHandle)
	{
		CloseServiceHandle(hHandle);
		hHandle = NULL;
	}
}

void CHXWinscv::CleanHandle()
{
	ResetServiceHandle(m_hService);
	ResetServiceHandle(m_hSCM);
}
