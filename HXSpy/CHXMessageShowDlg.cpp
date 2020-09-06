#include "stdafx.h"
#include <Windows.h>
#include "resource.h"
#include "CHXSpyManage.h"
#include "CHXMessageShowDlg.h"

CHXMessageShowDlg::CHXMessageShowDlg(DWORD procID)
{
	m_dwProcessID = procID;
}

LRESULT CHXMessageShowDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());
	m_editMessage = GetDlgItem(IDC_LIST1);

	TCHAR szLibFile[MAX_PATH];
	GetModuleFileName(NULL, szLibFile, _countof(szLibFile));
	PTSTR pFilename = _tcsrchr(szLibFile, TEXT('\\')) + 1;
	_tcscpy_s(pFilename, _countof(szLibFile) - (pFilename - szLibFile),
		TEXT("HXInjectionGetMessage.DLL"));
	if (CHXSpyManage::InjectLib(m_dwProcessID, szLibFile))
	{
	//	chVERIFY(EjectLib(dwProcessId, szLibFile));
	//	chMB("DLL Injection/Ejection successful.");
	}
	//else {
	//	chMB("DLL Injection/Ejection failed.");
	//}

	m_editMessage.AppendText(L"123123");
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
