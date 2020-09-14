#include "stdafx.h"
#include <Windows.h>
#include <atlstr.h>
#include "resource.h"
#include "CHXSpyManage.h"
#include "CHXMessageShowDlg.h"


#define READ_PIPE   L"\\\\.\\pipe\\ReadPipe"
#define WRITE_PIPE  L"\\\\.\\pipe\\WritePipe"      //   管道命名

CHXMessageShowDlg::CHXMessageShowDlg(DWORD procID)
{
	m_dwProcessID = procID;
}
CString g_str = TEXT("312");
HANDLE g_event = NULL;
DWORD __stdcall ThreadProc1(LPVOID pParam)
{
    WaitForSingleObject(g_event, INFINITE);
    DWORD dwProcessID = *((DWORD*)pParam);

    TCHAR szLibFile[MAX_PATH];
    GetModuleFileName(NULL, szLibFile, _countof(szLibFile));
    PTSTR pFilename = _tcsrchr(szLibFile, TEXT('\\')) + 1;
    _tcscpy_s(pFilename, _countof(szLibFile) - (pFilename - szLibFile),
        TEXT("HXInjectionGetMessage.DLL"));
    CHXSpyManage::InjectLib(dwProcessID, szLibFile);
    return 0;
}
LRESULT CHXMessageShowDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	SetWindowText(L"123");
	m_editMessage = GetDlgItem(IDC_LIST1);

    //m_editMessage.
    //TCHAR* p = new TCHAR[10];
    //for (size_t i = 0; i < 10; i++)
    //{
    //    *(p + i) = i + 65;
    //}
    //*(p + 9) = TEXT('\0');
    //::SendMessage(this->m_hWnd, WM_SETTEXT, 0, LPARAM(p));
    //Invalidate();
    g_event = CreateEvent(0, TRUE, FALSE, NULL);

	CreateThread(NULL, 0, ThreadProc2, &m_editMessage, 0, NULL);
    CreateThread(NULL, 0, ThreadProc1, (LPVOID)&m_dwProcessID, 0, NULL);
	//if (CHXSpyManage::InjectLib(m_dwProcessID, szLibFile))
	//{
	//}
	//else {
	//	chMB("DLL Injection/Ejection failed.");
	//}

	return TRUE;
}

LRESULT CHXMessageShowDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

void CHXMessageShowDlg::Layout()
{
	RECT rt;
	GetClientRect(&rt);
	m_editMessage.MoveWindow(&rt);
}

DWORD __stdcall ThreadProc2(LPVOID pParam)
{
    CEdit* pThis = (CEdit*)pParam;
    SendMessage(pThis->m_hWnd, WM_CLEAR, 0, 0);
    CString str = TEXT("捣蛋鬼笨笨!\r\n");
    int x = pThis->GetWindowTextLength();
    ::SendMessage(pThis->m_hWnd, EM_SETSEL, x, x);
    ::SendMessage(pThis->m_hWnd, EM_SCROLLCARET, 0, 0L);
    ::SendMessage(pThis->m_hWnd, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)str.GetBuffer());

    {

    }
	HANDLE hWritePipe = NULL;

	hWritePipe = CreateNamedPipe(
        L"\\\\.\\pipe\\Communication",
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE |
		PIPE_READMODE_MESSAGE |
		PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		MAX_PATH,
		MAX_PATH,
		0,
		NULL);
	int fd;
	int len;
	char buf[1024];

	if (hWritePipe == INVALID_HANDLE_VALUE)
	{
		return -1;
	}

    HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (NULL == hEvent)
    {
        //cout << "创建事件对象失败！" << endl;
        CloseHandle(hWritePipe);
        hWritePipe = NULL;
        //system("pause");
        return -1;
    }

    OVERLAPPED ovlap;
    ZeroMemory(&ovlap, sizeof(OVERLAPPED));
    ovlap.hEvent = hEvent;

    //2. 创建管道连接

    SetEvent(g_event);
    if (!ConnectNamedPipe(hWritePipe, &ovlap))
    {
        if (ERROR_IO_PENDING != GetLastError())
        {
            //cout << "等待客户端连接失败！" << endl;
            CloseHandle(hWritePipe);
            CloseHandle(hEvent);
            hWritePipe = NULL;
            //system("pause");
            return -1;
        }
    }

    //3. 等待客户端连接
    if (WAIT_FAILED == WaitForSingleObject(hEvent, INFINITE))
    {
        //cout << "等待对象失败！" << endl;
        CloseHandle(hWritePipe);
        CloseHandle(hEvent);
        hWritePipe = NULL;
        //system("pause");
        return -1;
    }
    CloseHandle(hEvent);

    //4. 读写管道数据
    //4.1 读取数据
    TCHAR rebuf[100];
    DWORD  dwReadLen = 0;
    if (ReadFile(hWritePipe, rebuf, 100, &dwReadLen, NULL))
    {
        CString str = rebuf;
        int x = pThis->GetWindowTextLength();
        ::SendMessage(pThis->m_hWnd, EM_SETSEL, x, x);
        ::SendMessage(pThis->m_hWnd, EM_SCROLLCARET, 0, 0L);
        ::SendMessage(pThis->m_hWnd, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)str.GetBuffer());

        Sleep(1000);
    }
    //cout << rebuf << endl;
	return 0;
}


// 服务端.cpp : Defines the entry point for the console application.
//
//
//#include "stdafx.h"
//#include <Windows.h>
//#include <iostream>
//using namespace std;
//
//int main()
//{
//    HANDLE hPipe = NULL;
//    HANDLE hEvent = NULL;
//    DWORD  dwReadLen = 0;
//    DWORD  dwWriteLen = 0;
//    OVERLAPPED ovlap;
//    char senbuf[] = "This is server!";
//    char rebuf[100];
//
//    //1. 创建命名管道
//    hPipe = CreateNamedPipe(L"\\\\.\\pipe\\Communication", PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 0, 1, 1024, 1024, 0, NULL);
//    if (INVALID_HANDLE_VALUE == hPipe)
//    {
//        cout << "创建命名管道失败！" << endl;
//        hPipe = NULL;
//        system("pause");
//        return -1;
//    }
//
//    hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//    if (NULL == hEvent)
//    {
//        cout << "创建事件对象失败！" << endl;
//        CloseHandle(hPipe);
//        hPipe = NULL;
//        system("pause");
//        return -1;
//    }
//
//    ZeroMemory(&ovlap, sizeof(OVERLAPPED));
//    ovlap.hEvent = hEvent;
//
//    //2. 创建管道连接
//    if (!ConnectNamedPipe(hPipe, &ovlap))
//    {
//        if (ERROR_IO_PENDING != GetLastError())
//        {
//            cout << "等待客户端连接失败！" << endl;
//            CloseHandle(hPipe);
//            CloseHandle(hEvent);
//            hPipe = NULL;
//            system("pause");
//            return -1;
//        }
//    }
//
//    //3. 等待客户端连接
//    if (WAIT_FAILED == WaitForSingleObject(hEvent, INFINITE))
//    {
//        cout << "等待对象失败！" << endl;
//        CloseHandle(hPipe);
//        CloseHandle(hEvent);
//        hPipe = NULL;
//        system("pause");
//        return -1;
//    }
//    CloseHandle(hEvent);
//
//    //4. 读写管道数据
//    //4.1 读取数据
//    if (!ReadFile(hPipe, rebuf, 100, &dwReadLen, NULL))
//    {
//        cout << "读取数据失败！" << endl;
//        system("pause");
//        return -1;
//    }
//    cout << rebuf << endl;
//
//    //4.2 写入数据
//    if (!WriteFile(hPipe, senbuf, (DWORD)strlen(senbuf) + 1, &dwWriteLen, NULL))
//    {
//        cout << "写入数据失败！" << endl;
//        system("pause");
//        return -1;
//    }
//
//    system("pause");
//    return 0;
//}
//LRESULT WINAPI CHXMessageShowDlg::InjectLib(DWORD dwProcessId, PWCHAR pszLibFile)
//{
//	LRESULT nRet = S_FALSE;
//	HANDLE hProcess = NULL, hThread = NULL;
//	PWSTR pszLibFileRemote = nullptr;
//	__try
//	{
//		hProcess = OpenProcess(
//			PROCESS_QUERY_INFORMATION |
//			PROCESS_CREATE_THREAD |
//			PROCESS_VM_OPERATION |
//			PROCESS_VM_WRITE,
//			FALSE, dwProcessId);
//		if (!hProcess)
//		{
//			__leave;
//		}
//
//		// Calculate the number of bytes needed for the DLL's pathname
//		int cch = 1 + lstrlenW(pszLibFile);
//		int cb = cch * sizeof(wchar_t);
//
//		// Allocate space in the remote process for the pathname
//		pszLibFileRemote = (PWSTR)
//			VirtualAllocEx(hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
//		if (pszLibFileRemote == NULL) __leave;
//
//		// Copy the DLL's pathname to the remote process' address space
//		if (!WriteProcessMemory(hProcess, pszLibFileRemote,
//			(PVOID)pszLibFile, cb, NULL)) __leave;
//
//		// Get the real address of LoadLibraryW in Kernel32.dll
//		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
//			GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
//		if (pfnThreadRtn == NULL) __leave;
//
//		// Create a remote thread that calls LoadLibraryW(DLLPathname)
//		hThread = CreateRemoteThread(hProcess, NULL, 0,
//			pfnThreadRtn, pszLibFileRemote, 0, NULL);
//		if (hThread == NULL) __leave;
//
//		// Wait for the remote thread to terminate
//		WaitForSingleObject(hThread, INFINITE);
//
//		nRet = S_OK;
//	}
//	__finally { // Now, we can clean everything up
//
//	   // Free the remote memory that contained the DLL's pathname
//		if (pszLibFileRemote != NULL)
//			VirtualFreeEx(hProcess, pszLibFileRemote, 0, MEM_RELEASE);
//
//		if (hThread != NULL)
//			CloseHandle(hThread);
//
//		if (hProcess != NULL)
//			CloseHandle(hProcess);
//	}
//
//	return nRet;
//}
