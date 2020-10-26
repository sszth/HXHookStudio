#pragma once


// CProcessRuleAddDlg 对话框

class CProcessRuleAddDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcessRuleAddDlg)

public:
	CProcessRuleAddDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProcessRuleAddDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PROCESSRULE_ADD };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonBrowse();
    afx_msg void OnBnClickedButtonBrowselogfile();
    afx_msg void OnBnClickedOk();

//自定义变量
public:

    CString m_ProcessPath;
    BOOL    m_bAllowed;
    CString m_DlgTitle;
    BOOL    m_bNewRule;
};
