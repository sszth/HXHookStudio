// ProcessRuleAddDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CactiWall.h"
#include "ProcessRuleAddDlg.h"
#include "ProcessLogDlg.h"

// CProcessRuleAddDlg �Ի���

IMPLEMENT_DYNAMIC(CProcessRuleAddDlg, CDialog)

CProcessRuleAddDlg::CProcessRuleAddDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessRuleAddDlg::IDD, pParent)
    , m_ProcessPath(_T(""))
    ,m_DlgTitle( _T("��ӳ������"))
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


// CProcessRuleAddDlg ��Ϣ�������

void CProcessRuleAddDlg::OnBnClickedButtonBrowse()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog FileDialog(TRUE,NULL,NULL,4|2,_T("Ӧ�ó���|*.exe|�����ļ�|*.*|"));
	FileDialog.m_ofn.lpstrTitle = _T("��ӳ���");
    if( IDOK == FileDialog.DoModal()){
	    m_ProcessPath = FileDialog.GetPathName();
        UpdateData( false );
    }
}

void CProcessRuleAddDlg::OnBnClickedButtonBrowselogfile()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CProcessLogDlg  dlg;
    if( IDOK == dlg.DoModal())
    {
        m_ProcessPath = dlg.m_ProcessPath;
        UpdateData( false );
    }
    
}

void CProcessRuleAddDlg::OnBnClickedOk()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    UpdateData( TRUE );
    m_bAllowed = ((CButton*)GetDlgItem( IDC_RADIO_ALLOW ))->GetCheck();

    OnOK();
}
