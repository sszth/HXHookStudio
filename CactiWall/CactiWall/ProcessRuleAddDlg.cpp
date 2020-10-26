// ProcessRuleAddDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CactiWall.h"
#include "ProcessRuleAddDlg.h"
#include "ProcessLogDlg.h"

// CProcessRuleAddDlg 对话框

IMPLEMENT_DYNAMIC(CProcessRuleAddDlg, CDialog)

CProcessRuleAddDlg::CProcessRuleAddDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessRuleAddDlg::IDD, pParent)
    , m_ProcessPath(_T(""))
    ,m_DlgTitle( _T("添加程序规则"))
{

}

CProcessRuleAddDlg::~CProcessRuleAddDlg()
{
}

void CProcessRuleAddDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_PROCESS_PATH, m_ProcessPath);
}

BOOL CProcessRuleAddDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    SetWindowText( m_DlgTitle.GetBuffer());
    if( !m_bNewRule )
    {
        ((CButton *)GetDlgItem( IDC_RADIO_ALLOW ))->SetCheck( m_bAllowed);
        ((CButton *)GetDlgItem( IDC_RADIO_DENY ))->SetCheck( !m_bAllowed);

    }
    ((CEdit *)GetDlgItem( IDC_EDIT_PROCESS_PATH ))->SetFocus();

    return TRUE;
}

BEGIN_MESSAGE_MAP(CProcessRuleAddDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CProcessRuleAddDlg::OnBnClickedButtonBrowse)
    ON_BN_CLICKED(IDC_BUTTON_BROWSELOGFILE, &CProcessRuleAddDlg::OnBnClickedButtonBrowselogfile)
    ON_BN_CLICKED(IDOK, &CProcessRuleAddDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CProcessRuleAddDlg 消息处理程序

void CProcessRuleAddDlg::OnBnClickedButtonBrowse()
{
    // TODO: 在此添加控件通知处理程序代码
	CFileDialog FileDialog(TRUE,NULL,NULL,4|2,_T("应用程序|*.exe|所有文件|*.*|"));
	FileDialog.m_ofn.lpstrTitle = _T("添加程序");
    if( IDOK == FileDialog.DoModal()){
	    m_ProcessPath = FileDialog.GetPathName();
        UpdateData( false );
    }
}

void CProcessRuleAddDlg::OnBnClickedButtonBrowselogfile()
{
    // TODO: 在此添加控件通知处理程序代码
    CProcessLogDlg  dlg;
    if( IDOK == dlg.DoModal())
    {
        m_ProcessPath = dlg.m_ProcessPath;
        UpdateData( false );
    }
    
}

void CProcessRuleAddDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    UpdateData( TRUE );
    m_bAllowed = ((CButton*)GetDlgItem( IDC_RADIO_ALLOW ))->GetCheck();

    OnOK();
}
