#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ClrListCtrl.h"


#define PROCESS_RULE_FLAG_ALLOW_ACCESS  ((UINT32)(1<<0))

// CProcessSettingDlg 对话框

class CProcessSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcessSettingDlg)

public:
	CProcessSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProcessSettingDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PROCESS_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
    void UpdateRuleList();
    bool RegAddProcessRule( LPTSTR processPath,BOOL bAllow );
    bool RegDeleteProcessRule( DWORD crcProcessPath );

	DECLARE_MESSAGE_MAP()

public:
    CClrListCtrl m_RuleList;

public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedRadioAllow();
    afx_msg void OnBnClickedButtonAddrule();
    afx_msg void OnBnClickedButtonEditrule();
    afx_msg void OnLvnItemActivateRuleList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMClickRuleList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonDelrule();
};
