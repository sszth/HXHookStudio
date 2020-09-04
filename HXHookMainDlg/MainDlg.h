// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "atlframe.h"
#include "resource.h"

extern int HXLocalHookProc(int x);
class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateChildWindows();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(ID_LOCAL_HOOK_ON, OnLocalHookOn)
		COMMAND_ID_HANDLER(ID_LOCAL_HOOK_OFF, OnLocalHookOff)
		COMMAND_ID_HANDLER(ID_Dll_HOOK_ON, OnDllHookOn)
		COMMAND_ID_HANDLER(ID_DLL_HOOK_OFF, OnDllHookOff)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// center the dialog on the screen
		CenterWindow();

		// set icons
		HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
		SetIcon(hIcon, TRUE);
		HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		SetIcon(hIconSmall, FALSE);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		UIAddChildWindowContainer(m_hWnd);

		return TRUE;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		return 0;
	}

	HMODULE m_hDll;// , strFuncName.GetBuffer());
	LRESULT OnDllHookOn(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CString strDll;
		strDll = L"HXHookDll.dll";
		CString szFuncName = "HXAddFunc";
		m_hDll = ::LoadLibrary(strDll);// , strFuncName.GetBuffer());
		return 0;
	}
	LRESULT OnDllHookOff(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if (m_hDll)
		{
			::FreeLibrary(m_hDll);
		}
		return 0;
	}
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		//CAboutDlg dlg;
		//dlg.DoModal();
		return 0;
	}
	DWORD oldProtect;
	BYTE  JmpBtye[5];
	BYTE  OldByte[5];
	LRESULT OnLocalHookOn(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CString strDll;
		strDll = L"HXAdd.dll";
		CString szFuncName = "HXAddFunc";
		HMODULE hDll = ::LoadLibrary(strDll);// , strFuncName.GetBuffer());
		if (!hDll)
		{
			return S_FALSE;
		}

		typedef int(*pHXAdd)(int);
		pHXAdd addr = (pHXAdd)::GetProcAddress(hDll, "HXAddFunc");
		if (!addr)
		{
			return S_FALSE;
		}

		VirtualProtect((void*)addr, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
		JmpBtye[0] = 0xE9;
		*(DWORD*)&JmpBtye[1] = (DWORD)((long long)HXLocalHookProc - (long long)addr - 5);
		//保存原先字节
		memcpy(OldByte, (void*)addr, 5);
		//替换原先字节
		memcpy((void*)addr, JmpBtye, 5);
		return 0;
	}

	LRESULT OnLocalHookOff(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CString strDll;
		strDll = L"HXAdd.dll";
		CString szFuncName = "HXAddFunc";
		HMODULE hDll = ::LoadLibrary(strDll);// , strFuncName.GetBuffer());
		if (!hDll)
		{
			return S_FALSE;
		}

		typedef int(*pHXAdd)(int);
		pHXAdd addr = (pHXAdd)::GetProcAddress(hDll, "HXAddFunc");
		if (!addr)
		{
			return S_FALSE;
		}
		//恢复原先字节
		memcpy((void*)addr, OldByte, 5);
		//恢复属性
		DWORD p;
		VirtualProtect((void*)addr, 5, oldProtect, &p);

		return 0;
	}
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: Add validation code 
		int n = GetDlgItemInt(IDC_EDIT1);
		CString strText, strCaption;
		CString strDll;
		strDll = L"HXAdd.dll";
		CString szFuncName = "HXAddFunc";
		HMODULE hDll = ::LoadLibrary(strDll);// , strFuncName.GetBuffer());
		if (!hDll)
		{
			return S_FALSE;
		}

		typedef int(*pHXAdd)(int);
		pHXAdd pFunc = (pHXAdd)::GetProcAddress(hDll, "HXAddFunc");
		if (!pFunc)
		{
			return S_FALSE;
		}
		n = pFunc(n);
		FreeLibrary(hDll);
		strText.Format(L"结果：%d", n);
		strCaption = L"结果";
	
		MessageBox(strText.GetBuffer(), strCaption.GetBuffer());
		return 0;
	}

	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CloseDialog(wID);
		return 0;
	}

	void CloseDialog(int nVal)
	{
		DestroyWindow();
		::PostQuitMessage(nVal);
	}
};
