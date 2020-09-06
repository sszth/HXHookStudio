#include "stdafx.h"

#include <Windows.h>

#include "CHXSpyManage.h"

void CHXSpyManage::Init()
{
	////先删除同名进程
	PROCESSENTRY32 proc;
	THREADENTRY32 thread;
	HEAPLIST32 heapList;
	MODULEENTRY32 moduleEntry;

	proc.dwSize = sizeof(PROCESSENTRY32);
	thread.dwSize = sizeof(THREADENTRY32);
	heapList.dwSize = sizeof(HEAPLIST32);
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	HANDLE hthSnapshot = NULL;
	BOOL theloop;
	//DWORD dwCurrentProcessId = GetCurrentProcessId();//当前进程id

	__try
	{
		hthSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0); //获取进程快照句柄
		if (hthSnapshot == INVALID_HANDLE_VALUE)
			__leave;

		theloop = Process32First(hthSnapshot, &proc); //查找第一个进程
		while (theloop)
		{
			m_MapProcess[proc.th32ProcessID] = proc;
			theloop = Process32Next(hthSnapshot, &proc); //查找下一个进程
		}

		theloop = Thread32First(hthSnapshot, &thread); //查找第一个线程
		while (theloop)
		{
			m_MapThread[thread.th32ThreadID] = thread;
			theloop = Thread32Next(hthSnapshot, &thread);
		}

		theloop = Heap32ListFirst(hthSnapshot, &heapList); //查找第一个堆栈
		while (theloop)
		{
			m_MapHeapList[heapList.th32ProcessID] = heapList;
			theloop = Thread32Next(hthSnapshot, &thread);
		}
		
		theloop = Module32First(hthSnapshot, &moduleEntry);//查找第一个dll
		while (theloop)
		{
			m_MapModuleEntry[moduleEntry.th32ProcessID] = moduleEntry;
			theloop = Module32Next(hthSnapshot, &moduleEntry);
		}
	}
	__finally { // Now we can clean everything up

		if (hthSnapshot != NULL)
			CloseHandle(hthSnapshot);
	}
}

MapProcess CHXSpyManage::GetProcess() const
{
	return m_MapProcess;
}

MapThread CHXSpyManage::GetThread() const
{
	return m_MapThread;
}

MapHeapList CHXSpyManage::GetHeapList() const
{
	return m_MapHeapList;
}

MapModuleEntry CHXSpyManage::GetModuleEntry() const
{
	return m_MapModuleEntry;
}

LRESULT __stdcall CHXSpyManage::InjectLib(DWORD dwProcessId, PWCHAR pszLibFile)
{
	LRESULT nRet = S_FALSE;
	HANDLE hProcess = NULL, hThread = NULL;
	PWSTR pszLibFileRemote = nullptr;
	__try
	{
		hProcess = OpenProcess(
			PROCESS_QUERY_INFORMATION |
			PROCESS_CREATE_THREAD |
			PROCESS_VM_OPERATION |
			PROCESS_VM_WRITE,
			FALSE, dwProcessId);
		if (!hProcess)
		{
			__leave;
		}

		// Calculate the number of bytes needed for the DLL's pathname
		int cch = 1 + lstrlenW(pszLibFile);
		int cb = cch * sizeof(wchar_t);

		// Allocate space in the remote process for the pathname
		pszLibFileRemote = (PWSTR)
			VirtualAllocEx(hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
		if (pszLibFileRemote == NULL) __leave;

		// Copy the DLL's pathname to the remote process' address space
		if (!WriteProcessMemory(hProcess, pszLibFileRemote,
			(PVOID)pszLibFile, cb, NULL)) __leave;

		// Get the real address of LoadLibraryW in Kernel32.dll
		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
		if (pfnThreadRtn == NULL) __leave;

		// Create a remote thread that calls LoadLibraryW(DLLPathname)
		hThread = CreateRemoteThread(hProcess, NULL, 0,
			pfnThreadRtn, pszLibFileRemote, 0, NULL);
		if (hThread == NULL) __leave;

		// Wait for the remote thread to terminate
		WaitForSingleObject(hThread, INFINITE);

		nRet = S_OK;
	}
	__finally { // Now, we can clean everything up

	   // Free the remote memory that contained the DLL's pathname
		if (pszLibFileRemote != NULL)
			VirtualFreeEx(hProcess, pszLibFileRemote, 0, MEM_RELEASE);

		if (hThread != NULL)
			CloseHandle(hThread);

		if (hProcess != NULL)
			CloseHandle(hProcess);
	}

	return nRet;
}
