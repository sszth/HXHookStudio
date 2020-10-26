#pragma once


// CDnsRuleAddDlg 对话框

class CDnsRuleAddDlg : public CDialog
{
	DECLARE_DYNAMIC(CDnsRuleAddDlg)

public:
	CDnsRuleAddDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDnsRuleAddDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_DNSRULE_ADD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

public:
    BOOL m_bNewRule;
    CString m_DnsPath;
    BOOL m_bAllowed;
    CString m_DlgTitle;
    afx_msg void OnBnClickedOk();
};
