// ProcessSettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include<winternl.h>
#include "CactiWall.h"
#include "ProcessSettingDlg.h"
#include "crc32.h"
#include "ProcessRuleAddDlg.h"

// CProcessSettingDlg 对话框

IMPLEMENT_DYNAMIC(CProcessSettingDlg, CDialog)

CProcessSettingDlg::CProcessSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessSettingDlg::IDD, pParent)
{

}

CProcessSettingDlg::~CProcessSettingDlg()
{
}

void CProcessSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RULE_LIST, m_RuleList);
}

BOOL CProcessSettingDlg::OnInitDialog()
{
    CButton *pBtn = (CButton*)GetDlgItem( IDC_BUTTON_EDITRULE );

    CDialog::OnInitDialog();

    m_RuleList.InsertColumn(0,_T("程序名称"),0,150);
    m_RuleList.InsertColumn(1,_T("状态"),0,50);
    m_RuleList.InsertColumn(2,_T("全路径"),0,430);
    UpdateRuleList();
    pBtn->EnableWindow( FALSE );

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CProcessSettingDlg::UpdateRuleList()
{
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      cbSize = 0;
    HKEY       hKey = NULL;
    TCHAR      buffer[256]={0};
    DWORD      i;
    int        j;
    BOOL       bOtherAccess = 0;

    status = RegCreateKey( (static_cast<CCactiWallApp*>(AfxGetApp()))->m_hKey,
        _T("processrules"),
        &hKey );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("获取进程规则失败(001)"),MB_OK | MB_ICONWARNING );
        return;
    }

    cbSize = sizeof( buffer );
    status = RegQueryValueEx( hKey,
                          _T("other_access"),
                            NULL,
                            NULL,
                            (LPBYTE)buffer,
                            &cbSize );
    if( status != ERROR_SUCCESS )
    {
        bOtherAccess = TRUE;
        AfxMessageBox(_T("读取配置信息失败（otheraccess)"));
    }
    ASSERT( cbSize == sizeof(DWORD));
    if( *(PDWORD)buffer == 0 )
        bOtherAccess = FALSE;
    else
        bOtherAccess = TRUE;

    m_RuleList.DeleteAllItems();

    for( i = 0,cbSize = sizeof( buffer);
        ERROR_SUCCESS == RegEnumKey( hKey,i,buffer,cbSize / sizeof(TCHAR));
       i++)
    {
        HKEY   hRuleKey = NULL;
        DWORD  crcName = 0;
        DWORD  rule = 0;

        _stscanf_s( buffer,_T("%x"),&crcName );

        status = RegOpenKey( hKey,buffer,&hRuleKey );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("获取进程规则失败(002)"),MB_OK | MB_ICONWARNING );
            break;
        }
        
        cbSize = sizeof( buffer);
        status = RegQueryValueEx( hRuleKey,
                              _T("name"),
                                NULL,
                                NULL,
                                (LPBYTE)buffer,
                                &cbSize );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("获取进程规则失败(003)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );
            break;
        }
        
        cbSize = sizeof( DWORD);
        status = RegQueryValueEx( hRuleKey,
                              _T("rule"),
                                NULL,
                                NULL,
                                (LPBYTE)&rule,
                                &cbSize );
        ASSERT(cbSize==sizeof(DWORD));
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("获取进程规则失败(004)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );
            break;
        }

        for( j = _tcslen( buffer);j>=0 && buffer[j]!=_T('\\');j--);
        j++;

        m_RuleList.InsertItem( i,buffer+j );
        if( rule == 1 )
           m_RuleList.SetItemText( i,1,_T("允许"));
        else
           m_RuleList.SetItemText( i,1,_T("禁止"));

        m_RuleList.SetItemText( i,2,buffer );
        m_RuleList.SetItemData( i,crcName );

        CloseHandle(hRuleKey );
        cbSize = sizeof( buffer);
    }
    
    CButton *pBtn = (CButton *)GetDlgItem( IDC_RADIO_ALLOW );
    pBtn->SetCheck( bOtherAccess );
    pBtn = (CButton *)GetDlgItem( IDC_RADIO_DENY );
    pBtn->SetCheck( !bOtherAccess );
    
    pBtn = (CButton *)GetDlgItem( IDC_BUTTON_EDITRULE );
    pBtn->EnableWindow( FALSE );

    pBtn = (CButton *)GetDlgItem( IDC_BUTTON_DELRULE );
    pBtn->EnableWindow( FALSE );

    if( hKey != NULL)
        CloseHandle( hKey );
}



bool CProcessSettingDlg::RegAddProcessRule( LPTSTR processPath,BOOL bAllow )
{
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      cbSize = 0;
    HKEY       hKey = NULL,hRuleKey = NULL;
    WCHAR      buffer[256]={0};
    size_t     len = 0,len2=0;
    DWORD      i = 0;
    DWORD      crcProcessPath = 0;

    status = RegCreateKey( (static_cast<CCactiWallApp*>(AfxGetApp()))->m_hKey,
        _T("processrules"),
        &hKey );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("获取进程规则失败(001)"),MB_OK | MB_ICONWARNING );
        return false;
    }

    len = _tcslen( processPath );
    if( len > 255 )
        return false;//缓冲区举出检查
   

#ifndef UNICODE
//#error you must clear here carefully!

    UNICODE_STRING  uniStr = {0};
    ANSI_STRING     ansiStr = {0};
    HMODULE        hNtDll = NULL;
    NTSTATUS ( NTAPI *pRtlAnsiStringToUnicodeString)(PUNICODE_STRING DestinationString,
                                            PCANSI_STRING SourceString,
                                            BOOLEAN AllocateDestinationString)
                                            = NULL;

    hNtDll = GetModuleHandle( _T("ntdll.dll") );
    ASSERT( hNtDll != NULL );
    pRtlAnsiStringToUnicodeString = (NTSTATUS (__stdcall *)(PUNICODE_STRING,PCANSI_STRING,BOOLEAN))
        GetProcAddress( hNtDll,"RtlAnsiStringToUnicodeString");
    ASSERT( pRtlAnsiStringToUnicodeString != NULL);
    uniStr.Buffer = buffer;
    uniStr.MaximumLength = sizeof(buffer);
    ansiStr.Buffer = processPath;
    ansiStr.MaximumLength = ansiStr.Length
        = strlen( processPath );
    pRtlAnsiStringToUnicodeString( &uniStr,&ansiStr,FALSE );
    len = uniStr.Length;
#else
    for( i = 0;i < len;i++)
        buffer[i] = processPath[i];
    len = len * sizeof( WCHAR );
#endif

    if( buffer[1] == _T(':'))
    {
        //转换为小写
        for( i = 0;i < len / sizeof( WCHAR );i++)
        {
            if( buffer[i] >= L'A' && buffer[i] <= L'Z' )
                buffer[i] = buffer[i] - L'A' + L'a';
        }
    }

    crcProcessPath = crc32_encode( (char*)buffer,len );
    RtlZeroMemory( buffer,sizeof( buffer ));
    _stprintf_s( (LPTSTR)buffer,255,_T("%x"),crcProcessPath );
    
    status = RegOpenKey( hKey,(LPTSTR)buffer,&hRuleKey);
    if( status == ERROR_SUCCESS )
    {
        CloseHandle( hKey );
        CloseHandle( hRuleKey );
        return false;
    }

    status = RegCreateKey( hKey,
        (LPTSTR)buffer,
        &hRuleKey );
    if( status != ERROR_SUCCESS)
    {
        CloseHandle( hKey );hKey = NULL;
        AfxMessageBox(_T("获取进程规则失败(002)"),MB_OK | MB_ICONWARNING );
        return false;
    }
    
    status = RegSetValueEx( hRuleKey,
                      _T("name"),
                      0,
                      REG_SZ,
                      (BYTE *)processPath,
                      len );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("写入进程规则失败(003)"),MB_OK | MB_ICONWARNING );
        CloseHandle( hRuleKey );hRuleKey = NULL;
        CloseHandle( hKey );hKey = NULL;
        return false;
    }

    status = RegSetValueEx( hRuleKey,
                      _T("rule"),
                      0,
                      REG_DWORD,
                      (BYTE *)&bAllow,
                      sizeof( BOOL) );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("写入进程规则失败(004)"),MB_OK | MB_ICONWARNING );
        CloseHandle( hRuleKey );hRuleKey = NULL;
        CloseHandle( hKey );hKey = NULL;
        return false;
    }

    CloseHandle( hRuleKey );hRuleKey = NULL;
    CloseHandle( hKey );hKey = NULL;
    return true;
}

bool CProcessSettingDlg::RegDeleteProcessRule( DWORD crcProcessPath )
{
    LSTATUS status = ERROR_SUCCESS;
    HKEY    hKey = NULL;
    TCHAR   buffer[10];

    //删除旧规则
    status = RegCreateKey( (static_cast<CCactiWallApp*>(AfxGetApp()))->m_hKey,
        _T("processrules"),
        &hKey );
    if( status != ERROR_SUCCESS)
    {
        return false;
    }
        
    _stprintf_s( buffer,10,_T("%x"),
        crcProcessPath );
    status = RegDeleteKey( hKey,buffer );
    if( status != ERROR_SUCCESS )
    {
        CloseHandle( hKey );hKey = NULL;
        return false;
    }
    
    CloseHandle( hKey );
    hKey = NULL;
    return true;

}
BEGIN_MESSAGE_MAP(CProcessSettingDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CProcessSettingDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CProcessSettingDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_RADIO_ALLOW, &CProcessSettingDlg::OnBnClickedRadioAllow)
    ON_BN_CLICKED(IDC_RADIO_DENY, &CProcessSettingDlg::OnBnClickedRadioAllow)
    ON_BN_CLICKED(IDC_BUTTON_ADDRULE, &CProcessSettingDlg::OnBnClickedButtonAddrule)
    ON_BN_CLICKED(IDC_BUTTON_EDITRULE, &CProcessSettingDlg::OnBnClickedButtonEditrule)
    ON_NOTIFY(LVN_ITEMACTIVATE, IDC_RULE_LIST, &CProcessSettingDlg::OnLvnItemActivateRuleList)
    ON_NOTIFY(NM_CLICK, IDC_RULE_LIST, &CProcessSettingDlg::OnNMClickRuleList)
    ON_BN_CLICKED(IDC_BUTTON_DELRULE, &CProcessSettingDlg::OnBnClickedButtonDelrule)
END_MESSAGE_MAP()


// CProcessSettingDlg 消息处理程序

void CProcessSettingDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    OnOK();

}

void CProcessSettingDlg::OnBnClickedCancel()
{
    // TODO: 在此添加控件通知处理程序代码
    OnCancel();
}

void ShowError(DWORD code)
/*
功能描述：用MessageBox显示GetLastError的错误代码描述信息
*/
{
	LPVOID                 lpMsgBuf;

	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
	// Process any inserts in lpMsgBuf.
	// ...
	// Display the string.
	MessageBox( NULL, (LPCTSTR)lpMsgBuf,_T("Error"), MB_OK | MB_ICONINFORMATION );
	// Free the buffer.
	LocalFree( lpMsgBuf );
}

void CProcessSettingDlg::OnBnClickedRadioAllow()
{
    // TODO: 在此添加控件通知处理程序代码
    CButton *pBtn = NULL;
    HKEY    hKey = NULL;
    BOOL    bChecked = FALSE;
    LSTATUS status = ERROR_SUCCESS;
    
    status = RegCreateKey( (static_cast<CCactiWallApp*>(AfxGetApp()))->m_hKey,
        _T("processrules"),
        &hKey );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("获取进程规则失败(005)"),MB_OK | MB_ICONWARNING );
        return;
    }

    pBtn = (CButton *)GetDlgItem( IDC_RADIO_ALLOW );
    bChecked = pBtn->GetCheck();
    status = RegSetValueEx( hKey,
                          _T("other_access"),
                          0,
                          REG_DWORD,
                          (BYTE*)&bChecked,
                          sizeof(BOOL));
    if( status != ERROR_SUCCESS)
    {
        ShowError( status );
    }

}

void CProcessSettingDlg::OnBnClickedButtonAddrule()
{
    // TODO: 在此添加控件通知处理程序代码
    CProcessRuleAddDlg  dlg;
    dlg.m_bNewRule = TRUE;
    if( IDOK == dlg.DoModal())
    {
        //添加新规则
        if( false == RegAddProcessRule( dlg.m_ProcessPath.GetBuffer(),
                          dlg.m_bAllowed ))
        {
            AfxMessageBox(_T("该程序已存在！"));
            return;
        }
        UpdateRuleList();
    }
    else
    {
    }
}

void CProcessSettingDlg::OnBnClickedButtonEditrule()
{
    // TODO: 在此添加控件通知处理程序代码
    CProcessRuleAddDlg  dlg;
    dlg.m_ProcessPath = m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),2 );
    dlg.m_DlgTitle = _T("编辑程序规则");
    dlg.m_bNewRule = FALSE;
    if( m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),1 )
          == _T("允许") )
    {
        dlg.m_bAllowed = TRUE;
    }
    else
    {
        dlg.m_bAllowed = FALSE;
    }

    if( IDOK == dlg.DoModal())
    {
        LSTATUS status = ERROR_SUCCESS;
        HKEY   hKey = NULL;
        TCHAR  buffer[10];

        //删除旧规则
        status = RegCreateKey( (static_cast<CCactiWallApp*>(AfxGetApp()))->m_hKey,
            _T("processrules"),
            &hKey );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("获取进程规则失败(001)"),MB_OK | MB_ICONWARNING );
            return;
        }
        
        _stprintf_s( buffer,10,_T("%x"),
            m_RuleList.GetItemData(m_RuleList.GetSelectionMark()));
        status = RegDeleteKey( hKey,buffer );
        if( status != ERROR_SUCCESS )
        {
            AfxMessageBox(_T("更新失败002"));
            CloseHandle( hKey );hKey = NULL;
            return;
        }

        CloseHandle( hKey );
        hKey = NULL;
        
        //添加新规则
        RegAddProcessRule( dlg.m_ProcessPath.GetBuffer(),
                          dlg.m_bAllowed );
        UpdateRuleList();
    }

}

void CProcessSettingDlg::OnLvnItemActivateRuleList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    OnBnClickedButtonEditrule();
    *pResult = 0;
}

void CProcessSettingDlg::OnNMClickRuleList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    CButton *pBtn_edit = (CButton*)GetDlgItem( IDC_BUTTON_EDITRULE );
    CButton *pBtn_delete = (CButton*)GetDlgItem( IDC_BUTTON_DELRULE);
    // TODO: 在此添加控件通知处理程序代码
    if( pNMItemActivate->iItem >= 0 )
    {
        pBtn_edit->EnableWindow( TRUE );
        pBtn_delete->EnableWindow( TRUE );
    }
    else
    {
        pBtn_edit->EnableWindow( FALSE );
        pBtn_delete->EnableWindow( FALSE );
    }
    *pResult = 0;
}

void CProcessSettingDlg::OnBnClickedButtonDelrule()
{
    // TODO: 在此添加控件通知处理程序代码
    if( !RegDeleteProcessRule( m_RuleList.GetItemData(m_RuleList.GetSelectionMark())))
        AfxMessageBox(_T("删除规则失败！"));
    else
    {
        UpdateRuleList();
    }

}
