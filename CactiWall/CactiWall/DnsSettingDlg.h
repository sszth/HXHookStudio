#pragma once
#include "afxcmn.h"
#include "ClrListCtrl.h"

#ifdef _WIN32_WCE
#error "Windows CE 不支持 CDHtmlDialog。"
#endif 

// CDnsSettingDlg 对话框

class CDnsSettingDlg : public CDialog
{
	DECLARE_DYNCREATE(CDnsSettingDlg)

public:
	CDnsSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDnsSettingDlg();
// 重写

// 对话框数据
	enum { IDD = IDD_DIALOG_DNS_SETTING};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CClrListCtrl m_RuleList;

//自定义成员函数
    void UpdateRuleList();
    bool CDnsSettingDlg::RegAddDnsRule( LPTSTR dnsPath,BOOL bAllow );
    bool CDnsSettingDlg::RegDeleteDnsRule( DWORD crcDnsPath );

    afx_msg void OnBnClickedRadioAllow();
    afx_msg void OnNMClickRuleList(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedButtonAddrule();
    afx_msg void OnBnClickedButtonEditrule();
    afx_msg void OnBnClickedButtonDelrule();
};
