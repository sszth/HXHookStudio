// DnsSettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CactiWall.h"
#include "DnsSettingDlg.h"
#include "DnsRuleAddDlg.h"
#include "crc32.h"

// CDnsSettingDlg 对话框

IMPLEMENT_DYNCREATE(CDnsSettingDlg, CDialog)

CDnsSettingDlg::CDnsSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDnsSettingDlg::IDD, pParent)
{

}

CDnsSettingDlg::~CDnsSettingDlg()
{
}

void CDnsSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RULE_LIST, m_RuleList);
}

BOOL CDnsSettingDlg::OnInitDialog()
{
    CButton *pBtn = (CButton*)GetDlgItem( IDC_BUTTON_EDITRULE );

    CDialog::OnInitDialog();

    m_RuleList.InsertColumn(0,_T("DNS名称"),0,450);
    m_RuleList.InsertColumn(1,_T("解析控制"),0,150);
    UpdateRuleList();
    pBtn->EnableWindow( FALSE );
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDnsSettingDlg::UpdateRuleList()
{
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      cbSize = 0;
    HKEY       hKey = NULL;
    TCHAR      buffer[256]={0};
    DWORD      i;
    BOOL       bOtherAccess = 0;

    status = RegCreateKey( (static_cast<CCactiWallApp*>(AfxGetApp()))->m_hKey,
        _T("dnsrules"),
        &hKey );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("获取DNS规则失败(001)"),MB_OK | MB_ICONWARNING );
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
        AfxMessageBox(_T("读取DNS信息失败（otheraccess)"));
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
            AfxMessageBox(_T("获取DNS规则失败(002)"),MB_OK | MB_ICONWARNING );
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
            AfxMessageBox(_T("获取DNS规则失败(004)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );
            break;
        }

        m_RuleList.InsertItem( i,buffer );
        if( rule == 1 )
           m_RuleList.SetItemText( i,1,_T("允许"));
        else
           m_RuleList.SetItemText( i,1,_T("禁止"));

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

bool CDnsSettingDlg::RegAddDnsRule( LPTSTR dnsPath,BOOL bAllow )
{
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      cbSize = 0;
    HKEY       hKey = NULL,hRuleKey = NULL;
    WCHAR      buffer[256]={0};
    size_t     len = 0,len2=0;
    DWORD      i = 0;
    DWORD      crcDnsPath = 0;

    status = RegCreateKey( (static_cast<CCactiWallApp*>(AfxGetApp()))->m_hKey,
        _T("dnsrules"),
        &hKey );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("获取DNS规则失败(001)"),MB_OK | MB_ICONWARNING );
        return false;
    }

    len = _tcslen( dnsPath );
    if( len > 255 )
        return false;//缓冲区举出检查
   
    crcDnsPath = crc32_encode( (char*)dnsPath,len * sizeof( TCHAR ) );
    RtlZeroMemory( buffer,sizeof( buffer ));
    _stprintf_s( (LPTSTR)buffer,255,_T("%x"),crcDnsPath );
    
    status = RegOpenKey( hKey,(LPTSTR)buffer,&hRuleKey);
    if( status == ERROR_SUCCESS )
    {
        CloseHandle( hKey );
        return false;
    }

    status = RegCreateKey( hKey,
        (LPTSTR)buffer,
        &hRuleKey );
    if( status != ERROR_SUCCESS)
    {
        CloseHandle( hKey );hKey = NULL;
        AfxMessageBox(_T("获取DNS规则失败(002)"),MB_OK | MB_ICONWARNING );
        return false;
    }
    
    status = RegSetValueEx( hRuleKey,
                      _T("name"),
                      0,
                      REG_SZ,
                      (BYTE *)dnsPath,
                      len * sizeof( TCHAR ) );
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
        AfxMessageBox(_T("写入DNS规则失败(004)"),MB_OK | MB_ICONWARNING );
        CloseHandle( hRuleKey );hRuleKey = NULL;
        CloseHandle( hKey );hKey = NULL;
        return false;
    }

    CloseHandle( hRuleKey );hRuleKey = NULL;
    CloseHandle( hKey );hKey = NULL;
    return true;
}

bool CDnsSettingDlg::RegDeleteDnsRule( DWORD crcDnsPath )
{
    LSTATUS status = ERROR_SUCCESS;
    HKEY    hKey = NULL;
    TCHAR   buffer[10];

    //删除旧规则
    status = RegCreateKey( (static_cast<CCactiWallApp*>(AfxGetApp()))->m_hKey,
        _T("dnsrules"),
        &hKey );
    if( status != ERROR_SUCCESS)
    {
        return false;
    }
        
    _stprintf_s( buffer,10,_T("%x"),
        crcDnsPath );
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

BEGIN_MESSAGE_MAP(CDnsSettingDlg, CDialog)
    ON_BN_CLICKED(IDC_RADIO_ALLOW, &CDnsSettingDlg::OnBnClickedRadioAllow)
    ON_BN_CLICKED(IDC_RADIO_DENY, &CDnsSettingDlg::OnBnClickedRadioAllow)
    ON_NOTIFY(NM_CLICK, IDC_RULE_LIST, &CDnsSettingDlg::OnNMClickRuleList)
    ON_BN_CLICKED(IDOK, &CDnsSettingDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDC_BUTTON_ADDRULE, &CDnsSettingDlg::OnBnClickedButtonAddrule)
    ON_BN_CLICKED(IDC_BUTTON_EDITRULE, &CDnsSettingDlg::OnBnClickedButtonEditrule)
    ON_BN_CLICKED(IDC_BUTTON_DELRULE, &CDnsSettingDlg::OnBnClickedButtonDelrule)
END_MESSAGE_MAP()


// CDnsSettingDlg 消息处理程序

extern void ShowError(DWORD code);

void CDnsSettingDlg::OnBnClickedRadioAllow()
{
    // TODO: 在此添加控件通知处理程序代码
    CButton *pBtn = NULL;
    HKEY    hKey = NULL;
    BOOL    bChecked = FALSE;
    LSTATUS status = ERROR_SUCCESS;
    
    status = RegCreateKey( (static_cast<CCactiWallApp*>(AfxGetApp()))->m_hKey,
        _T("dnsrules"),
        &hKey );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("获取DNS规则失败(005)"),MB_OK | MB_ICONWARNING );
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

void CDnsSettingDlg::OnNMClickRuleList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
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

void CDnsSettingDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    OnOK();
}

void CDnsSettingDlg::OnBnClickedButtonAddrule()
{
    // TODO: 在此添加控件通知处理程序代码
    CDnsRuleAddDlg  dlg;
    dlg.m_bNewRule = TRUE;
    if( IDOK == dlg.DoModal())
    {
        //添加新规则
        if( false == RegAddDnsRule( dlg.m_DnsPath.GetBuffer(),
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

void CDnsSettingDlg::OnBnClickedButtonEditrule()
{
    // TODO: 在此添加控件通知处理程序代码
    CDnsRuleAddDlg  dlg;
    dlg.m_DnsPath = m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),0 );
    dlg.m_DlgTitle = _T("编辑DNS规则");
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
        if( !RegDeleteDnsRule( m_RuleList.GetItemData(m_RuleList.GetSelectionMark())))
        {
           AfxMessageBox(_T("更新规则失败！"));
           return;
        }
        //添加新规则
        RegAddDnsRule( dlg.m_DnsPath.GetBuffer(),
                          dlg.m_bAllowed );
        UpdateRuleList();
    }

}

void CDnsSettingDlg::OnBnClickedButtonDelrule()
{
    // TODO: 在此添加控件通知处理程序代码
    if( !RegDeleteDnsRule( m_RuleList.GetItemData(m_RuleList.GetSelectionMark())))
        AfxMessageBox(_T("删除规则失败！"));
    else
    {
        UpdateRuleList();
    }
}
