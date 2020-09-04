// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include <atlstr.h>
#include <TlHelp32.h>         //声明快照函数的头文件
#include "aboutdlg.h"
#include "MainDlg.h"

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	UIUpdateChildWindows();
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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

	m_listThread.SubclassWindow(this->GetDlgItem(IDC_THREAD_LIST));

	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	CloseDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

LRESULT CHXThreadListViewCtrl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	LRESULT result = DefWindowProc();
	if (result == S_FALSE)
	{
		return S_FALSE;
	}

	this->Initialize();
	return result;
}

LRESULT CHXThreadListViewCtrl::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	return S_OK;
}

LRESULT CHXThreadListViewCtrl::OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	bHandled = TRUE;

	int indexSelectedNearMenu = -1;

	POINT ptPopup = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	if (ptPopup.x == -1 && ptPopup.y == -1)
	{
		// They used the context menu key or Shift-F10 to bring up the context menu
		indexSelectedNearMenu = this->GetNextItem(-1, LVNI_SELECTED);
		RECT rect = { 0 };
		if (indexSelectedNearMenu >= 0)
		{
			// If there is a selected item, popup the menu under the first selected item,
			// if not, pop it up in the top left of the list view
			this->GetItemRect(indexSelectedNearMenu, &rect, LVIR_BOUNDS);
			::MapWindowPoints(m_hWnd, NULL, (LPPOINT)&rect, 2);
			ptPopup.x = rect.left;
			ptPopup.y = rect.bottom;
		}
	}
	else
	{
		POINT ptClient = ptPopup;
		::MapWindowPoints(NULL, m_hWnd, &ptClient, 1);

		LVHITTESTINFO hti = { 0 };
		hti.pt = ptClient;
		indexSelectedNearMenu = this->HitTest(&hti);
	}

	if (indexSelectedNearMenu > 0)
	{
		// TODO: Handle multiple selection
		this->GetItemData(indexSelectedNearMenu);

		// Build up the menu to show
		//CMenu mnuContext;
		//if(mnuContext.CreatePopupMenu())
		//{
		//}
	}

	return S_OK;
}

void CHXThreadListViewCtrl::Initialize(void)
{
	this->InitializeListColumns();
}

void CHXThreadListViewCtrl::InitializeListColumns(void)
{
	SetRedraw(FALSE);
	RECT rcList;
	this->GetClientRect(&rcList);

	int width = rcList.right - rcList.left;
	int columnWidth = 0;
	int remainingWidth = width;

	// NOTE: We'll take the default sort type (LVCOLSORT_TEXT)
	// for the "Name" and "Folder" columns.
	// LVCOLSORT_TEXT uses lstrcmp, which uses the currently
	// selected user locale for sorting. This matches the sorting
	// in the file list in Windows Explorer. With lstrcmp:
	//    "a" < "A" and "A" < "b" and "b" < "B"
	// By comparison, with _tcscmp the sort order is in "ASCII" order:
	//    "A" < "a" and "Z" < "a"
	// LVCOLSORT_TEXTNOCASE uses lstrcmpi, which sorts:
	//    "a" == "A" and "A" < "b" and "b" == "B"

	columnWidth = ::MulDiv(width, 20, 100);  // 20%
	this->InsertColumn(ListColumn_ThreadName, _T("Name"), LVCFMT_LEFT, columnWidth, ListColumn_ThreadName);
	this->InsertColumn(ListColumn_Pid, _T("Pid"), LVCFMT_LEFT, columnWidth, ListColumn_Pid);
	////先删除同名进程
	PROCESSENTRY32 proc;
	THREADENTRY32 thread;
	HANDLE snap = NULL;
	BOOL theloop;
	DWORD dwCurrentProcessId = GetCurrentProcessId();//当前进程id

	snap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0); //获取进程快照句柄
	proc.dwSize = sizeof(PROCESSENTRY32);
	theloop = Process32First(snap, &proc); //查找第一个进程
	int nItem = 0;
	while (theloop)
	{
		if (dwCurrentProcessId != proc.th32ProcessID)
		{
			InsertItem(nItem, proc.szExeFile);
			CString strPid;
			strPid.Format(_T("%d"), proc.th32ProcessID);
			SetItemText(nItem, ListColumn_Pid, strPid);	
			nItem++;
		}
		//	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, proc.th32ProcessID);
		//	if (hProcess)
		//	{
		//		TerminateProcess(hProcess, 0);
		//		CloseHandle(hProcess);
		//		hProcess = NULL;
		//	}
		theloop = Process32Next(snap, &proc); //查找下一个进程
	}
	SetRedraw(TRUE);
}

void CHXThreadListViewCtrl::Uninitialize(void)
{
}

BOOL CHXThreadListViewCtrl::SubclassWindow(HWND hWnd)
{
	ATLASSERT(m_hWnd == NULL);
	ATLASSERT(::IsWindow(hWnd));
	BOOL returnValue = baseClass::SubclassWindow(hWnd);
	if (returnValue)
	{
		this->Initialize();
	}
	return returnValue;
}

HWND CHXThreadListViewCtrl::UnsubclassWindow(BOOL bForce)
{
	this->Uninitialize();

	return baseClass::UnsubclassWindow(bForce);
}

int CHXThreadListViewCtrl::CompareItemsCustom(LVCompareParam * pItem1, LVCompareParam * pItem2, int iSortCol)
{
	int result = 0;

	// Deal with all of the custom sort columns
	switch (iSortCol)
	{
	case ListColumn_Pid:
	{
		// Sort based on ListColumn_SizeBytes

		// NOTE: There's other ways to use a "proxy column" for sorting, this is just one
		//  (mainly, just to give an example of CompareItemsCustom).
		//  Another way would be to have DoSortItems run on the hidden column,
		//  but then SetSortColumn for the visible column.

		CString sizeInBytesLHS, sizeInBytesRHS;
		this->GetItemText(pItem1->iItem, ListColumn_Pid, *(BSTR*)(&sizeInBytesLHS));
		this->GetItemText(pItem2->iItem, ListColumn_Pid, *(BSTR*)(&sizeInBytesRHS));

		__int64 difference = _ttoi64(sizeInBytesRHS) - _ttoi64(sizeInBytesLHS);
		if (difference < 0)
			result = 1;
		else if (difference > 0)
			result = -1;
		else
			result = 0;
	}
	break;
	}

	return result;
}
