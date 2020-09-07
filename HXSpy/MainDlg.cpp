// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include <algorithm>
#include <atlstr.h>
#include "aboutdlg.h"
#include "CHXSpyManage.h"
#include "CHXMessageShowDlg.h"
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

	m_listThread = this->GetDlgItem(IDC_THREAD_LIST);
	DWORD dwStyle;
	dwStyle = m_listThread.GetStyle();
	dwStyle |= LVS_SINGLESEL;
	m_listThread.ModifyStyle(0, dwStyle);
	dwStyle = m_listThread.GetExStyle();
	dwStyle |= LVS_EX_FULLROWSELECT | LVIS_SELECTED;
	m_listThread.ModifyStyleEx(0, dwStyle);


	dwStyle = m_listThread.GetStyle();

	SetRedraw(FALSE);
	RECT rcList;
	m_listThread.GetClientRect(&rcList);

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
	m_listThread.InsertColumn(ListColumn_ThreadName, _T("Name"), LVCFMT_LEFT, columnWidth, ListColumn_ThreadName);
	m_listThread.InsertColumn(ListColumn_Pid, _T("Pid"), LVCFMT_LEFT, columnWidth, ListColumn_Pid);


	//////先删除同名进程
	//PROCESSENTRY32 proc;
	//THREADENTRY32 thread;
	//HANDLE hthSnapshot = NULL;
	//BOOL theloop;
	DWORD dwCurrentProcessId = GetCurrentProcessId();//当前进程id
	CHXSpyManage::GetInstance()->Init();
	MapProcess mapProcess = CHXSpyManage::GetInstance()->GetProcess();
	HXSpyProcessAdd add(dwCurrentProcessId, &m_listThread, 0);
	std::for_each(mapProcess.begin(), mapProcess.end(), add);
	SetRedraw(TRUE);
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

	
	//CHXMessageShowDlg dlg;
	//dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnLVColumnClick(int, LPNMHDR pnmh, BOOL&)
{
	return LRESULT();
}

LRESULT CMainDlg::OnInitMessageHook(WORD, WORD wID, HWND, BOOL&)
{
	DWORD nProcessId = -1;
#ifndef _HX_DEBUG_
	LVITEM lvi = {};
	BSTR szStr = nullptr;
	m_listThread.GetSelectedItem(&lvi);
	if (lvi.iItem == -1)
	{
		return S_OK;
	}
	m_listThread.GetItemText(lvi.iItem, ListColumn_Pid, szStr);
	nProcessId = _ttoi(szStr);
#else
	HWND pWnd = FindWindow(NULL, TEXT("Last MessageBox info"));
	if (pWnd)
	{
		DWORD dwThread = GetWindowThreadProcessId(pWnd, &nProcessId);
	}

#endif // !_HX_DEBUG_
	if (-1 == nProcessId)
	{
		return S_FALSE;
	}
	CHXMessageShowDlg dlg(nProcessId);
	dlg.DoModal();

	//LPLVITEMW lp = new LVITEMW();
	//m_listThread.GetSelectedItem();
	//m_listThread.getitem
	//CHXMessageShowDlg dlg;
	//dlg.DoModal();
	return LRESULT();
}

LRESULT CMainDlg::OnNMRClick(int, LPNMHDR pnmh, BOOL&)
{
	POINT pos = { 0, 0 };
	::GetCursorPos(&pos);
	POINT ptClient = pos;
	if (pnmh->hwndFrom != NULL)
		::ScreenToClient(pnmh->hwndFrom, &ptClient);

	if (pnmh->hwndFrom == m_listThread.m_hWnd)
	{
		CMenu menu;
		menu.LoadMenu(IDR_MENU1);
		CMenu SubMenu(menu.GetSubMenu(0));
		SubMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this->m_hWnd);
		SubMenu.DestroyMenu();
		menu.DestroyMenu();

		//LVHITTESTINFO lvhti = { 0 };
		//lvhti.pt = ptClient;
		//m_listThread.HitTest(&lvhti);
		//if ((lvhti.flags & LVHT_ONITEMLABEL) != 0)
		//{
		//	LVITEM lvi = { 0 };
		//	lvi.mask = LVIF_PARAM;
		//	lvi.iItem = lvhti.iItem;
		//	if (m_listThread.GetItem(&lvi) != FALSE)
		//	{
		//		CMenu menu;
		//		menu.LoadMenu(IDR_MENU1);
		//		CMenuHandle menuPopup = menu.GetSubMenu(0);
		//		m_CmdBar.TrackPopupMenu(menuPopup, TPM_RIGHTBUTTON | TPM_VERTICAL, pt.x, pt.y);
		//	}
		//}
	}

	return 0L;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}



HXSpyProcessAdd::HXSpyProcessAdd(DWORD dwCurrentID, CListViewCtrl* list, int nCurrentListItem)
{
	m_dwCurrentID = dwCurrentID;
	m_nCurrentListItem = nCurrentListItem;
	m_ListCurrent = list;
}

void HXSpyProcessAdd::operator()(std::pair<DWORD, PROCESSENTRY32> iter)
{
	if (!m_ListCurrent)
	{
		ATLASSERT(0);
		return;
	}
	if (iter.first != m_dwCurrentID)
	{
		m_ListCurrent->InsertItem(m_nCurrentListItem, iter.second.szExeFile);
		CString strPid;
		strPid.Format(_T("%d"), iter.second.th32ProcessID);
		m_ListCurrent->SetItemText(m_nCurrentListItem, ListColumn_Pid, strPid);
		m_nCurrentListItem++;
	}
}

