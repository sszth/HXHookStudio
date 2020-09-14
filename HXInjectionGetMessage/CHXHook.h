#pragma once
#include <string>
class CHXAPIHook
{
public:
	// Hook a function in all modules
	CHXAPIHook(std::string strCallModName, std::string strFuncName, PROC pfnHook);
	~CHXAPIHook();

	operator PROC() { return(m_pfnOrig); }

	static BOOL sm_bExcludeAPIHookMod;

public:
	// Calls the real GetProcAddress 
	static FARPROC WINAPI GetProcAddressRaw(HMODULE hmod, PCSTR pszProcName);

	static HMODULE WINAPI ModuleFromAddress(PVOID pv);

private:
	static PVOID sm_pvMaxAppAddr; // Maximum private memory address
	static CHXAPIHook* sm_pHead;    // Address of first object
	CHXAPIHook* m_pNext;            // Address of next  object

	PCSTR m_pszCalleeModName;     // Module containing the function (ANSI)
	PCSTR m_pszFuncName;          // Function name in callee (ANSI)
	PROC  m_pfnOrig;              // Original function address in callee
	PROC  m_pfnHook;              // Hook function address

private:
	static void WINAPI ReplaceIATEntryInAllMods(PCSTR pszCalleeModName,
		PROC pfnOrig, PROC pfnHook);


	static void WINAPI ReplaceIATEntryInOneMod(PCSTR pszCalleeModName,
		PROC pfnOrig, PROC pfnHook, HMODULE hmodCaller);
};

