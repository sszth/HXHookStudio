#pragma once


// CProcessRuleAddDlg �Ի���

class CProcessRuleAddDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcessRuleAddDlg)

public:
	CProcessRuleAddDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProcessRuleAddDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_PROCESSRULE_ADD };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonBrowse();
    afx_msg void OnBnClickedButtonBrowselogfile();
    afx_msg void OnBnClickedOk();

//�Զ������
public:

    CString m_ProcessPath;
    BOOL    m_bAllowed;
    CString m_DlgTitle;
    BOOL    m_bNewRule;
};
