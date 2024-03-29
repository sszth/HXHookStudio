#pragma once
class CHXMessageShowDlg : public CDialogImpl<CHXMessageShowDlg>
{
public:
	enum { IDD = IDD_DIALOG1	};

	CHXMessageShowDlg(DWORD procID);

	BEGIN_MSG_MAP(CHXMessageShowDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		//COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void Layout();

	//LRESULT WINAPI InjectLib(DWORD dwProcessId, PWCHAR pszLibFile);
private:
	CEdit m_editMessage;
	DWORD m_dwProcessID;
};

DWORD WINAPI ThreadProc2(LPVOID pParam);
