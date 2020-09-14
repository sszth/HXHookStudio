#include "pch.h"
#include "CHXHook.h"

CHXAPIHook::CHXAPIHook(std::string strCallModName, std::string strFuncName, PROC pfnHook)
{

}

CHXAPIHook::~CHXAPIHook()
{
}

FARPROC __stdcall CHXAPIHook::GetProcAddressRaw(HMODULE hmod, PCSTR pszProcName)
{
	return ::GetProcAddress(hmod, pszProcName);
}

HMODULE __stdcall CHXAPIHook::ModuleFromAddress(PVOID pv)
{
	MEMORY_BASIC_INFORMATION mbi;
	return ((VirtualQuery(pv, &mbi, sizeof(mbi) != 0)) ? (HMODULE)mbi.AllocationBase : NULL);
}

void __stdcall CHXAPIHook::ReplaceIATEntryInAllMods(PCSTR pszCalleeModName, PROC pfnOrig, PROC pfnHook)
{
	HANDLE hmodThisMod = sm_bExcludeAPIHookMod ? ModuleFromAddress(ReplaceIATEntryInAllMods) : NULL;
}
