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

	LRESULT CreateService(IN LPCWSTR lpServiceName, IN LPCWSTR lpDisplayName, IN DWORD dwDesiredAccess, IN DWORD dwServiceType, IN DWORD dwStartType, IN DWORD dwErrorControl, IN LPCWSTR lpBinaryPathName, IN LPCWSTR lpLoadOrderGroup, IN LPDWORD lpdwTagId, IN LPCWSTR lpDependencies, IN LPCWSTR lpServiceStartName, IN LPCWSTR lpPassword);

	//************************************
	// 功能:		启动服务，内部无校验,无法准确判断是否成功启动。
	// 参数:
	//				IN LPCWSTR lpServiceName
	//				服务名称，非服务显示名称。
	//				IN DWORD dwNumServiceArgs
	//				服务参数个数
	//				LPCWSTR * lpServiceArgVectors
	//				服务参数，注意驱动服务不会接收这个服务参数。
	// 返回值:
	//            	LRESULT
	//************************************
	LRESULT StartService(IN LPCWSTR lpServiceName, IN DWORD dwNumServiceArgs = 0, LPCWSTR* lpServiceArgVectors = NULL);

	//************************************
	// 功能:		启动服务，内部增加各个流程校验。
	// 参数:
	//				OUT SERVICE_STATUS_PROCESS ssStatus
	//				如果失败此结构体中含有失败信息
	//				IN LPCWSTR lpServiceName
	//				IN DWORD dwNumServiceArgs
	//				LPCWSTR * lpServiceArgVectors
	// 返回值:
	//            	LRESULT
	//************************************
	LRESULT StartServiceEx(OUT SERVICE_STATUS_PROCESS ssStatus, IN LPCWSTR lpServiceName, IN DWORD dwNumServiceArgs = 0, LPCWSTR* lpServiceArgVectors = NULL);
protected:
private:
	void ResetServiceHandle(IN OUT SC_HANDLE& hHandle);

private:
	SC_HANDLE m_hSCM;
};

