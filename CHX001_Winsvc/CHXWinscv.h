#pragma once
#include <memory>
#include <list>
#include <windows.h>
#include <string>
typedef std::list<ENUM_SERVICE_STATUS> List_ENUM_SERVICE_STATUS;
typedef std::list<ENUM_SERVICE_STATUS_PROCESS> List_ENUM_SERVICE_STATUS_PROCESS;

class CHXWinscv
{
public:
	CHXWinscv();
	~CHXWinscv();
public:
	LRESULT RefreshServer();

	//************************************
	// Method:    GetService
	// FullName:  CHXWinscv::GetService
	// Access:    public 
	// Returns:   LRESULT
	// Qualifier: ��ȡע���ĳ�������Ϣ
	// Parameter: DWORD dwServiceType
	//		SERVICE_DRIVER:Services of type SERVICE_KERNEL_DRIVER and SERVICE_FILE_SYSTEM_DRIVER
	//		SERVICE_FILE_SYSTEM_DRIVER:File system driver services
	//		SERVICE_KERNEL_DRIVER:Driver services
	//		SERVICE_WIN32:Services of type SERVICE_WIN32_OWN_PROCESS and SERVICE_WIN32_SHARE_PROCESS
	//		SERVICE_WIN32_OWN_PROCESS:Services that run in their own processes
	//		SERVICE_WIN32_SHARE_PROCESS:Services that share a process with one or more other services
	// author:	  SSZTH
	//************************************
	LRESULT GetService(OUT List_ENUM_SERVICE_STATUS& list, IN DWORD dwServiceType = SERVICE_DRIVER | SERVICE_WIN32, IN DWORD dwServiceState = SERVICE_STATE_ALL);
	LRESULT GetServiceEx(OUT List_ENUM_SERVICE_STATUS_PROCESS& list, IN DWORD dwServiceType = SERVICE_DRIVER | SERVICE_WIN32, IN DWORD dwServiceState = SERVICE_STATE_ALL, IN LPCWSTR pszGroupName = NULL);

	LRESULT CreateService(IN LPCWSTR lpServiceName, IN LPCWSTR lpDisplayName, IN DWORD dwDesiredAccess, IN DWORD dwServiceType, IN DWORD dwStartType, IN DWORD dwErrorControl, IN LPCWSTR lpBinaryPathName, IN LPCWSTR lpLoadOrderGroup, IN LPDWORD lpdwTagId, IN LPCWSTR lpDependencies, IN LPCWSTR lpServiceStartName, IN LPCWSTR lpPassword);

	//************************************
	// ����:		���������ڲ���У��,�޷�׼ȷ�ж��Ƿ�ɹ�������
	// ����:
	//				IN LPCWSTR lpServiceName
	//				�������ƣ��Ƿ�����ʾ���ơ�
	//				IN DWORD dwNumServiceArgs
	//				�����������
	//				LPCWSTR * lpServiceArgVectors
	//				���������ע���������񲻻����������������
	// ����ֵ:
	//            	LRESULT
	//************************************
	LRESULT StartService(IN LPCWSTR lpServiceName, IN DWORD dwNumServiceArgs = 0, LPCWSTR* lpServiceArgVectors = NULL);

	//************************************
	// ����:		���������ڲ����Ӹ�������У�顣
	// ����:
	//				OUT SERVICE_STATUS_PROCESS ssStatus
	//				���ʧ�ܴ˽ṹ���к���ʧ����Ϣ
	//				IN LPCWSTR lpServiceName
	//				IN DWORD dwNumServiceArgs
	//				LPCWSTR * lpServiceArgVectors
	// ����ֵ:
	//            	LRESULT
	//************************************
	LRESULT StartServiceEx(OUT SERVICE_STATUS_PROCESS ssStatus, IN LPCWSTR lpServiceName, IN DWORD dwNumServiceArgs = 0, LPCWSTR* lpServiceArgVectors = NULL);
protected:
private:
	void ResetServiceHandle(IN OUT SC_HANDLE& hHandle);

private:
	SC_HANDLE m_hSCM;
};

