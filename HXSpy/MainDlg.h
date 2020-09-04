// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <atlctrlx.h>

class CHXThreadListViewCtrl :
	public CSortListViewCtrlImpl<CHXThreadListViewCtrl, CListViewCtrl>
{
protected:
	// Typedefs
	typedef CHXThreadListViewCtrl thisClass;
	typedef CSortListViewCtrlImpl<CHXThreadListViewCtrl, CListViewCtrl> baseClass;

public:
	// Enumerations
	enum ListColumnIndex
	{
		ListColumn_ThreadName = 0,
		ListColumn_Pid = 1,
		//ListColumn_State = 2,
		//ListColumn_UserName = 3,
		//ListColumn_SizeBytes = 4,
		//ListColumn_FullPath = 5,
	};
	struct CHXThreadDtata
	{


	};

	// Message Handling
	DECLARE_WND_SUPERCLASS(_T("CThreadListViewCtrl"), CListViewCtrl::GetWndClassName())

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)

		CHAIN_MSG_MAP(baseClass)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// Helpers
	void Initialize(void);
	void InitializeListColumns(void);
	void Uninitialize(void);

	// Overrides for CWindowImpl
	BOOL SubclassWindow(HWND hWnd);
	HWND UnsubclassWindow(BOOL bForce = FALSE);

	// Overrides for CSortListViewImpl
	int CompareItemsCustom(LVCompareParam* pItem1, LVCompareParam* pItem2, int iSortCol);

	// ITestWizardFindFileCB
	//virtual bool OnBeginFindFiles(void) { return true; }
	//virtual void OnEndFindFiles(void) { }
	//virtual bool OnBeginDirectorySearch(LPCTSTR /*directory*/) { return true; }
	//virtual void OnEndDirectorySearch(LPCTSTR /*directory*/) { }
	//virtual void OnFileFound(LPCTSTR directory, LPWIN32_FIND_DATA findFileData);

	// Methods
	//int AddFile(LPCTSTR fileFullPath);
	//int AddFile(LPCTSTR directory, LPCTSTR fileSpec, LPCTSTR fileFullPath, FILETIME lastWriteTimeUTC, ULONGLONG fileSize);
	//void AutoResizeColumns(void);
	//void ClearSortHeaderBitmap(void);
};

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void CloseDialog(int nVal);
private:
	CHXThreadListViewCtrl m_listThread;
};
