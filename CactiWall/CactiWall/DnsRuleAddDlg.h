#pragma once


// CDnsRuleAddDlg �Ի���

class CDnsRuleAddDlg : public CDialog
{
	DECLARE_DYNAMIC(CDnsRuleAddDlg)

public:
	CDnsRuleAddDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDnsRuleAddDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_DNSRULE_ADD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

public:
    BOOL m_bNewRule;
    CString m_DnsPath;
    BOOL m_bAllowed;
    CString m_DlgTitle;
    afx_msg void OnBnClickedOk();
};
