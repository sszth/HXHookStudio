#pragma once
#include <memory>
#include <list>
#include <windows.h>
#include <string>
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
	// Qualifier: 获取注册表某类服务信息
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

	LRESULT StartService(IN const std::wstring& strServerName);
	//************************************
	// Method:    OpenService
	// FullName:  CHXWinscv::OpenService
	// Access:    public 
	// Returns:   LRESULT
	// Qualifier:	注意驱动服务不会接收这个服务参数
	// Parameter: IN LPCWSTR lpServiceName
	//				服务名称,注意是创建服务时最基本名称
	// Parameter: IN DWORD dwNumServiceArgs
	//				服务参数个数
	// Parameter: LPCWSTR * lpServiceArgVectors
	//				服务参数
	// author:	  SSZTH
	//************************************
	LRESULT OpenService(IN LPCWSTR lpServiceName, IN DWORD dwNumServiceArgs = 0, LPCWSTR* lpServiceArgVectors = NULL);
protected:
private:
	void ResetServiceHandle(IN OUT SC_HANDLE& hHandle);

private:
	SC_HANDLE m_hSCM;
};

