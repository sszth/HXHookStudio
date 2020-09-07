// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <stdio.h>

#include "..\CommonFiles\CmnHdr.h"
#include <ImageHlp.h>
#pragma comment(lib, "ImageHlp")

#include "..\CommonFiles\Toolhelp.h"
#include <StrSafe.h>
#include <windowsx.h>
#include <string>

class CAPIHook {
public:
	// Hook a function in all modules
	CAPIHook(std::string pszCalleeModName, std::string pszFuncName, PROC pfnHook);

	// Unhook a function from all modules
	~CAPIHook();

	// Returns the original address of the hooked function
	operator PROC() { return(m_pfnOrig); }

	// Hook module w/CAPIHook implementation?
	// I have to make it static because I need to use it 
	// in ReplaceIATEntryInAllMods
	static BOOL ExcludeAPIHookMod;


public:
	// Calls the real GetProcAddress 
	static FARPROC WINAPI GetProcAddressRaw(HMODULE hmod, PCSTR pszProcName);

private:
	static PVOID sm_pvMaxAppAddr; // Maximum private memory address
	static CAPIHook* sm_pHead;    // Address of first object
	CAPIHook* m_pNext;            // Address of next  object

	PCSTR m_pszCalleeModName;     // Module containing the function (ANSI)
	PCSTR m_pszFuncName;          // Function name in callee (ANSI)
	PROC  m_pfnOrig;              // Original function address in callee
	PROC  m_pfnHook;              // Hook function address

private:
	// Replaces a symbol's address in a module's import section
	static void WINAPI ReplaceIATEntryInAllMods(PCSTR pszCalleeModName,
		PROC pfnOrig, PROC pfnHook);

	// Replaces a symbol's address in all modules' import sections
	static void WINAPI ReplaceIATEntryInOneMod(PCSTR pszCalleeModName,
		PROC pfnOrig, PROC pfnHook, HMODULE hmodCaller);

	// Replaces a symbol's address in a module's export sections
	static void ReplaceEATEntryInOneMod(HMODULE hmod, PCSTR pszFunctionName, PROC pfnNew);

private:
	// Used when a DLL is newly loaded after hooking a function
	static void    WINAPI FixupNewlyLoadedModule(HMODULE hmod, DWORD dwFlags);

	// Used to trap when DLLs are newly loaded
	static HMODULE WINAPI LoadLibraryA(PCSTR pszModulePath);
	static HMODULE WINAPI LoadLibraryW(PCWSTR pszModulePath);
	static HMODULE WINAPI LoadLibraryExA(PCSTR pszModulePath,
		HANDLE hFile, DWORD dwFlags);
	static HMODULE WINAPI LoadLibraryExW(PCWSTR pszModulePath,
		HANDLE hFile, DWORD dwFlags);

	// Returns address of replacement function if hooked function is requested
	static FARPROC WINAPI GetProcAddress(HMODULE hmod, PCSTR pszProcName);

private:
	// Instantiates hooks on these functions
	static CAPIHook sm_LoadLibraryA;
	static CAPIHook sm_LoadLibraryW;
	static CAPIHook sm_LoadLibraryExA;
	static CAPIHook sm_LoadLibraryExW;
	static CAPIHook sm_GetProcAddress;
};

/////////////////////////////////////////////////////////////////////////////////

int WINAPI Hook_MessageBoxA(HWND hWnd, PCSTR pszText, PCSTR pszCaption,
	UINT uType);
int WINAPI Hook_MessageBoxW(HWND hWnd, PCWSTR pszText, LPCWSTR pszCaption,
	UINT uType);
// Hook the MessageBoxA and MessageBoxW functions
CAPIHook g_MessageBoxA("User32.dll", "MessageBoxA",
(PROC)Hook_MessageBoxA);

CAPIHook g_MessageBoxW("User32.dll", "MessageBoxW",
(PROC)Hook_MessageBoxW);

// The head of the linked-list of CAPIHook objects
CAPIHook* CAPIHook::sm_pHead = NULL;

// By default, the module containing the CAPIHook() is not hooked
BOOL CAPIHook::ExcludeAPIHookMod = TRUE;


///////////////////////////////////////////////////////////////////////////////


CAPIHook::CAPIHook(std::string pszCalleeModName, std::string pszFuncName, PROC pfnHook) {

	// Note: the function can be hooked only if the exporting module 
	//       is already loaded. A solution could be to store the function
	//       name as a member; then, in the hooked LoadLibrary* handlers, parse
	//       the list of CAPIHook instances, check if pszCalleeModName
	//       is the name of the loaded module to hook its export table and 
	//       re-hook the import tables of all loaded modules.

	m_pNext = sm_pHead;    // The next node was at the head
	sm_pHead = this;        // This node is now at the head

	// Save information about this hooked function
	m_pszCalleeModName = pszCalleeModName.data();
	m_pszFuncName = pszFuncName.data();
	m_pfnHook = pfnHook;
	m_pfnOrig =
		GetProcAddressRaw(GetModuleHandleA(pszCalleeModName.data()), m_pszFuncName);

	// If function does not exit,... bye bye
	// This happens when the module is not already loaded
	if (m_pfnOrig == NULL)
	{
		wchar_t szPathname[MAX_PATH];
		GetModuleFileNameW(NULL, szPathname, _countof(szPathname));
		wchar_t sz[1024];
		StringCchPrintfW(sz, _countof(sz),
			TEXT("[%4u - %s] impossible to find %S\r\n"),
			GetCurrentProcessId(), szPathname, pszFuncName.data());
		OutputDebugString(sz);
		return;
	}

#ifdef _DEBUG
	// This section was used for debugging sessions when Explorer died as 
	// a folder content was requested
	// 
	//static BOOL s_bFirstTime = TRUE;
	//if (s_bFirstTime)
	//{
	//   s_bFirstTime = FALSE;

	//   wchar_t szPathname[MAX_PATH];
	//   GetModuleFileNameW(NULL, szPathname, _countof(szPathname));
	//   wchar_t* pszExeFile = wcsrchr(szPathname, L'\\') + 1;
	//   OutputDebugStringW(L"Injected in ");
	//   OutputDebugStringW(pszExeFile);
	//   if (_wcsicmp(pszExeFile, L"Explorer.EXE") == 0)
	//   {
	//      DebugBreak();
	//   }
	//   OutputDebugStringW(L"\n   --> ");
	//   StringCchPrintfW(szPathname, _countof(szPathname), L"%S", pszFuncName);
	//   OutputDebugStringW(szPathname);
	//   OutputDebugStringW(L"\n");
	//}
#endif

   // Hook this function in all currently loaded modules
	ReplaceIATEntryInAllMods(m_pszCalleeModName, m_pfnOrig, m_pfnHook);
}


///////////////////////////////////////////////////////////////////////////////


CAPIHook::~CAPIHook() {

	// Unhook this function from all modules
	ReplaceIATEntryInAllMods(m_pszCalleeModName, m_pfnHook, m_pfnOrig);

	// Remove this object from the linked list
	CAPIHook* p = sm_pHead;
	if (p == this) {     // Removing the head node
		sm_pHead = p->m_pNext;
	}
	else {

		BOOL bFound = FALSE;

		// Walk list from head and fix pointers
		for (; !bFound && (p->m_pNext != NULL); p = p->m_pNext) {
			if (p->m_pNext == this) {
				// Make the node that points to us point to our next node
				p->m_pNext = p->m_pNext->m_pNext;
				bFound = TRUE;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////


// NOTE: This function must NOT be inlined
FARPROC CAPIHook::GetProcAddressRaw(HMODULE hmod, PCSTR pszProcName) {

	return(::GetProcAddress(hmod, pszProcName));
}


///////////////////////////////////////////////////////////////////////////////


// Returns the HMODULE that contains the specified memory address
static HMODULE ModuleFromAddress(PVOID pv) {

	MEMORY_BASIC_INFORMATION mbi;
	return((VirtualQuery(pv, &mbi, sizeof(mbi)) != 0)
		? (HMODULE)mbi.AllocationBase : NULL);
}


///////////////////////////////////////////////////////////////////////////////


void CAPIHook::ReplaceIATEntryInAllMods(PCSTR pszCalleeModName,
	PROC pfnCurrent, PROC pfnNew) {

	HMODULE hmodThisMod = ExcludeAPIHookMod
		? ModuleFromAddress(ReplaceIATEntryInAllMods) : NULL;

	// Get the list of modules in this process
	CToolhelp th(TH32CS_SNAPMODULE, GetCurrentProcessId());

	MODULEENTRY32 me = { sizeof(me) };
	for (BOOL bOk = th.ModuleFirst(&me); bOk; bOk = th.ModuleNext(&me)) {

		// NOTE: We don't hook functions in our own module
		if (me.hModule != hmodThisMod) {

			// Hook this function in this module
			ReplaceIATEntryInOneMod(
				pszCalleeModName, pfnCurrent, pfnNew, me.hModule);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////


// Handle unexpected exceptions if the module is unloaded
LONG WINAPI InvalidReadExceptionFilter(PEXCEPTION_POINTERS pep) {

	// handle all unexpected exceptions because we simply don't patch
	// any module in that case
	LONG lDisposition = EXCEPTION_EXECUTE_HANDLER;

	// Note: pep->ExceptionRecord->ExceptionCode has 0xc0000005 as a value

	return(lDisposition);
}


void CAPIHook::ReplaceIATEntryInOneMod(PCSTR pszCalleeModName,
	PROC pfnCurrent, PROC pfnNew, HMODULE hmodCaller) {

	// Get the address of the module's import section
	ULONG ulSize;

	// An exception was triggered by Explorer (when browsing the content of 
	// a folder) into imagehlp.dll. It looks like one module was unloaded...
	// Maybe some threading problem: the list of modules from Toolhelp might 
	// not be accurate if FreeLibrary is called during the enumeration.
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = NULL;
	__try {
		pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
			hmodCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
	}
	__except (InvalidReadExceptionFilter(GetExceptionInformation())) {
		// Nothing to do in here, thread continues to run normally
		// with NULL for pImportDesc 
	}

	if (pImportDesc == NULL)
		return;  // This module has no import section or is no longer loaded


	 // Find the import descriptor containing references to callee's functions
	for (; pImportDesc->Name; pImportDesc++) {
		PSTR pszModName = (PSTR)((PBYTE)hmodCaller + pImportDesc->Name);
		if (lstrcmpiA(pszModName, pszCalleeModName) == 0) {

			// Get caller's import address table (IAT) for the callee's functions
			PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)
				((PBYTE)hmodCaller + pImportDesc->FirstThunk);

			// Replace current function address with new function address
			for (; pThunk->u1.Function; pThunk++) {

				// Get the address of the function address
				PROC* ppfn = (PROC*)&pThunk->u1.Function;

				// Is this the function we're looking for?
				BOOL bFound = (*ppfn == pfnCurrent);
				if (bFound) {
					if (!WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
						sizeof(pfnNew), NULL) && (ERROR_NOACCESS == GetLastError())) {
						DWORD dwOldProtect;
						if (VirtualProtect(ppfn, sizeof(pfnNew), PAGE_WRITECOPY,
							&dwOldProtect)) {

							WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
								sizeof(pfnNew), NULL);
							VirtualProtect(ppfn, sizeof(pfnNew), dwOldProtect,
								&dwOldProtect);
						}
					}
					return;  // We did it, get out
				}
			}
		}  // Each import section is parsed until the right entry is found and patched
	}
}


///////////////////////////////////////////////////////////////////////////////


void CAPIHook::ReplaceEATEntryInOneMod(HMODULE hmod, PCSTR pszFunctionName,
	PROC pfnNew) {

	// Get the address of the module's export section
	ULONG ulSize;

	PIMAGE_EXPORT_DIRECTORY pExportDir = NULL;
	__try {
		pExportDir = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(
			hmod, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &ulSize);
	}
	__except (InvalidReadExceptionFilter(GetExceptionInformation())) {
		// Nothing to do in here, thread continues to run normally
		// with NULL for pExportDir 
	}

	if (pExportDir == NULL)
		return;  // This module has no export section or is unloaded

	PDWORD pdwNamesRvas = (PDWORD)((PBYTE)hmod + pExportDir->AddressOfNames);
	PWORD pdwNameOrdinals = (PWORD)
		((PBYTE)hmod + pExportDir->AddressOfNameOrdinals);
	PDWORD pdwFunctionAddresses = (PDWORD)
		((PBYTE)hmod + pExportDir->AddressOfFunctions);

	// Walk the array of this module's function names 
	for (DWORD n = 0; n < pExportDir->NumberOfNames; n++) {
		// Get the function name
		PSTR pszFuncName = (PSTR)((PBYTE)hmod + pdwNamesRvas[n]);

		// If not the specified function, try the next function
		if (lstrcmpiA(pszFuncName, pszFunctionName) != 0) continue;

		// We found the specified function
		// --> Get this function's ordinal value
		WORD ordinal = pdwNameOrdinals[n];

		// Get the address of this function's address
		PROC* ppfn = (PROC*)&pdwFunctionAddresses[ordinal];

		// Turn the new address into an RVA
		pfnNew = (PROC)((PBYTE)pfnNew - (PBYTE)hmod);

		// Replace current function address with new function address
		if (!WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
			sizeof(pfnNew), NULL) && (ERROR_NOACCESS == GetLastError())) {
			DWORD dwOldProtect;
			if (VirtualProtect(ppfn, sizeof(pfnNew), PAGE_WRITECOPY,
				&dwOldProtect)) {

				WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew,
					sizeof(pfnNew), NULL);
				VirtualProtect(ppfn, sizeof(pfnNew), dwOldProtect, &dwOldProtect);
			}
		}
		break;  // We did it, get out
	}
}


///////////////////////////////////////////////////////////////////////////////
// Hook LoadLibrary functions and GetProcAddress so that hooked functions
// are handled correctly if these functions are called.

CAPIHook CAPIHook::sm_LoadLibraryA("Kernel32.dll", "LoadLibraryA",
(PROC)CAPIHook::LoadLibraryA);

CAPIHook CAPIHook::sm_LoadLibraryW("Kernel32.dll", "LoadLibraryW",
(PROC)CAPIHook::LoadLibraryW);

CAPIHook CAPIHook::sm_LoadLibraryExA("Kernel32.dll", "LoadLibraryExA",
(PROC)CAPIHook::LoadLibraryExA);

CAPIHook CAPIHook::sm_LoadLibraryExW("Kernel32.dll", "LoadLibraryExW",
(PROC)CAPIHook::LoadLibraryExW);

CAPIHook CAPIHook::sm_GetProcAddress("Kernel32.dll", "GetProcAddress",
(PROC)CAPIHook::GetProcAddress);


///////////////////////////////////////////////////////////////////////////////


void CAPIHook::FixupNewlyLoadedModule(HMODULE hmod, DWORD dwFlags) {

	// If a new module is loaded, hook the hooked functions
	if ((hmod != NULL) &&   // Do not hook our own module
		(hmod != ModuleFromAddress(FixupNewlyLoadedModule)) &&
		((dwFlags & LOAD_LIBRARY_AS_DATAFILE) == 0) &&
		((dwFlags & LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE) == 0) &&
		((dwFlags & LOAD_LIBRARY_AS_IMAGE_RESOURCE) == 0)
		) {

		for (CAPIHook* p = sm_pHead; p != NULL; p = p->m_pNext) {
			if (p->m_pfnOrig != NULL) {
				ReplaceIATEntryInAllMods(p->m_pszCalleeModName,
					p->m_pfnOrig, p->m_pfnHook);
			}
			else {
#ifdef _DEBUG
				// We should never end up here 
				wchar_t szPathname[MAX_PATH];
				GetModuleFileNameW(NULL, szPathname, _countof(szPathname));
				wchar_t sz[1024];
				StringCchPrintfW(sz, _countof(sz),
					TEXT("[%4u - %s] impossible to find %S\r\n"),
					GetCurrentProcessId(), szPathname, p->m_pszCalleeModName);
				OutputDebugString(sz);
#endif
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryA(PCSTR pszModulePath) {

	HMODULE hmod = ::LoadLibraryA(pszModulePath);
	FixupNewlyLoadedModule(hmod, 0);
	return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryW(PCWSTR pszModulePath) {

	HMODULE hmod = ::LoadLibraryW(pszModulePath);
	FixupNewlyLoadedModule(hmod, 0);
	return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryExA(PCSTR pszModulePath,
	HANDLE hFile, DWORD dwFlags) {

	HMODULE hmod = ::LoadLibraryExA(pszModulePath, hFile, dwFlags);
	FixupNewlyLoadedModule(hmod, dwFlags);
	return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


HMODULE WINAPI CAPIHook::LoadLibraryExW(PCWSTR pszModulePath,
	HANDLE hFile, DWORD dwFlags) {

	HMODULE hmod = ::LoadLibraryExW(pszModulePath, hFile, dwFlags);
	FixupNewlyLoadedModule(hmod, dwFlags);
	return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


FARPROC WINAPI CAPIHook::GetProcAddress(HMODULE hmod, PCSTR pszProcName) {

	// Get the true address of the function
	FARPROC pfn = GetProcAddressRaw(hmod, pszProcName);

	// Is it one of the functions that we want hooked?
	CAPIHook* p = sm_pHead;
	for (; (pfn != NULL) && (p != NULL); p = p->m_pNext) {

		if (pfn == p->m_pfnOrig) {

			// The address to return matches an address we want to hook
			// Return the hook function address instead
			pfn = p->m_pfnHook;
			break;
		}
	}

	return(pfn);
}

// Prototypes for the hooked functions
typedef int (WINAPI *PFNMESSAGEBOXA)(HWND hWnd, PCSTR pszText,
	PCSTR pszCaption, UINT uType);

typedef int (WINAPI *PFNMESSAGEBOXW)(HWND hWnd, PCWSTR pszText,
	PCWSTR pszCaption, UINT uType);



///////////////////////////////////////////////////////////////////////////////

// This function sends the MessageBox info to our main dialog box
void SendLastMsgBoxInfo(BOOL bUnicode,
	PVOID pvCaption, PVOID pvText, int nResult) {

	// Get the pathname of the process displaying the message box
	wchar_t szProcessPathname[MAX_PATH];
	GetModuleFileNameW(NULL, szProcessPathname, MAX_PATH);

	// Convert the return value into a human-readable string
	PCWSTR pszResult = L"(Unknown)";
	switch (nResult) {
	case IDOK:       pszResult = L"Ok";        break;
	case IDCANCEL:   pszResult = L"Cancel";    break;
	case IDABORT:    pszResult = L"Abort";     break;
	case IDRETRY:    pszResult = L"Retry";     break;
	case IDIGNORE:   pszResult = L"Ignore";    break;
	case IDYES:      pszResult = L"Yes";       break;
	case IDNO:       pszResult = L"No";        break;
	case IDCLOSE:    pszResult = L"Close";     break;
	case IDHELP:     pszResult = L"Help";      break;
	case IDTRYAGAIN: pszResult = L"Try Again"; break;
	case IDCONTINUE: pszResult = L"Continue";  break;
	}

	// Construct the string to send to the main dialog box
	wchar_t sz[2048];
	StringCchPrintfW(sz, _countof(sz), bUnicode
		? L"Process: (%d) %s\r\nCaption: %s\r\nMessage: %s\r\nResult: %s"
		: L"Process: (%d) %s\r\nCaption: %S\r\nMessage: %S\r\nResult: %s",
		GetCurrentProcessId(), szProcessPathname,
		pvCaption, pvText, pszResult);

	// Send the string to the main dialog box
	COPYDATASTRUCT cds = { 0, ((DWORD)wcslen(sz) + 1) * sizeof(wchar_t), sz };
	FORWARD_WM_COPYDATA(FindWindow(NULL, TEXT("Last MessageBox Info")),
		NULL, &cds, SendMessage);
}

// This is the MessageBoxW replacement function
int WINAPI Hook_MessageBoxW(HWND hWnd, PCWSTR pszText, LPCWSTR pszCaption,
	UINT uType) {

	// Call the original MessageBoxW function
	int nResult = ((PFNMESSAGEBOXW)(PROC)g_MessageBoxW)
		(hWnd, pszText, pszCaption, uType);

	// Send the information to the main dialog box
	SendLastMsgBoxInfo(TRUE, (PVOID)pszCaption, (PVOID)pszText, nResult);

	// Return the result back to the caller
	return(nResult);
}
// This is the MessageBoxA replacement function
int WINAPI Hook_MessageBoxA(HWND hWnd, PCSTR pszText, PCSTR pszCaption,
	UINT uType) {

	// Call the original MessageBoxA function
	int nResult = ((PFNMESSAGEBOXA)(PROC)g_MessageBoxA)
		(hWnd, pszText, pszCaption, uType);

	// Send the information to the main dialog box
	SendLastMsgBoxInfo(FALSE, (PVOID)pszCaption, (PVOID)pszText, nResult);

	// Return the result back to the caller
	return(nResult);
}



HHOOK g_hhook = NULL;


static LRESULT WINAPI GetMsgProc(int code, WPARAM wParam, LPARAM lParam) {
	return(CallNextHookEx(g_hhook, code, wParam, lParam));
}

BOOL WINAPI LastMsgBoxInfo_HookAllApps(BOOL bInstall, DWORD dwThreadId) {

	BOOL bOk;

	if (bInstall) {

		chASSERT(g_hhook == NULL); // Illegal to install twice in a row

		// Install the Windows' hook
		g_hhook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc,
			ModuleFromAddress(LastMsgBoxInfo_HookAllApps), dwThreadId);

		bOk = (g_hhook != NULL);
	}
	else {

		chASSERT(g_hhook != NULL); // Can't uninstall if not installed
		bOk = UnhookWindowsHookEx(g_hhook);
		g_hhook = NULL;
	}

	return(bOk);
}

//////////////////////////////// End of File //////////////////////////////////

BOOL APIENTRY DllMain( HMODULE hInstDll,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

		LastMsgBoxInfo_HookAllApps(TRUE, 0);
		break;


		//// ok open dlg success
		//TCHAR szqq[MAX_PATH] = TEXT("hun");
		//MessageBox(GetActiveWindow(), szqq, szqq, MB_OK);
		//break;



        //char szBuf[MAX_PATH * 100] = { 0 };

        //PBYTE pb = NULL;
        //MEMORY_BASIC_INFORMATION mbi;
        //while (VirtualQuery(pb, &mbi, sizeof(mbi)) == sizeof(mbi)) {

        //    int nLen;
        //    char szModName[MAX_PATH];

        //    if (mbi.State == MEM_FREE)
        //        mbi.AllocationBase = mbi.BaseAddress;

        //    if ((mbi.AllocationBase == hInstDll) ||
        //        (mbi.AllocationBase != mbi.BaseAddress) ||
        //        (mbi.AllocationBase == NULL)) {
        //        // Do not add the module name to the list
        //        // if any of the following is true:
        //        // 1. If this region contains this DLL
        //        // 2. If this block is NOT the beginning of a region
        //        // 3. If the address is NULL
        //        nLen = 0;
        //    }
        //    else {
        //        nLen = GetModuleFileNameA((HINSTANCE)mbi.AllocationBase,
        //            szModName, __crt_countof(szModName));
        //    }

        //    if (nLen > 0) {
        //        wsprintfA(strchr(szBuf, 0), "\n%p-%s",
        //            mbi.AllocationBase, szModName);
        //    }

        //    pb += mbi.RegionSize;
        //}

        //// NOTE: Normally, you should not display a message box in DllMain
        //// due to the loader lock described in Chapter 20. However, to keep
        //// this sample application simple, I am violating this rule.
        //PCSTR szMsg = &szBuf[1];
        //char szTitle[MAX_PATH];
        //GetModuleFileNameA(NULL, szTitle, __crt_countof(szTitle));
        //MessageBoxA(GetActiveWindow(), szMsg, szTitle, MB_OK);
        //break;

    //case DLL_THREAD_ATTACH:
    //case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:

		LastMsgBoxInfo_HookAllApps(FALSE, 0);
        break;
    }
    return TRUE;
}

