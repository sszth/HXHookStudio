// DnsRuleAddDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CactiWall.h"
#include "DnsRuleAddDlg.h"


// CDnsRuleAddDlg �Ի���

IMPLEMENT_DYNAMIC(CDnsRuleAddDlg, CDialog)

CDnsRuleAddDlg::CDnsRuleAddDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDnsRuleAddDlg::IDD, pParent)
    , m_DnsPath(_T(""))
    , m_DlgTitle(_T("���DNS����"))
{

}

CDnsRuleAddDlg::~CDnsRuleAddDlg()
{
}

void CDnsRuleAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_DNS_PATH, m_DnsPath);
}

BOOL CDnsRuleAddDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetWindowText( m_DlgTitle.GetBuffer());
    if( !m_bNewRule )
    {
        ((CButton *)GetDlgItem( IDC_RADIO_ALLOW ))->SetCheck( m_bAllowed);
        ((CButton *)GetDlgItem( IDC_RADIO_DENY ))->SetCheck( !m_bAllowed);

    }
    ((CEdit *)GetDlgItem( IDC_EDIT_DNS_PATH ))->SetFocus();
    return TRUE;
}

BEGIN_MESSAGE_MAP(CDnsRuleAddDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CDnsRuleAddDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDnsRuleAddDlg ��Ϣ�������


void CDnsRuleAddDlg::OnBnClickedOk()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    UpdateData( TRUE );
    if( m_DnsPath == _T("") )
    {
        AfxMessageBox(_T("���˹ؼ��ʲ���Ϊ�գ�"));
        return;
    }
    m_bAllowed = ((CButton*)GetDlgItem( IDC_RADIO_ALLOW ))->GetCheck();
    
    OnOK();
}
