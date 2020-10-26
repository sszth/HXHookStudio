#pragma once
#include "afxcmn.h"
#include "ClrListCtrl.h"

#ifdef _WIN32_WCE
#error "Windows CE ��֧�� CDHtmlDialog��"
#endif 

// CDnsSettingDlg �Ի���

class CDnsSettingDlg : public CDialog
{
	DECLARE_DYNCREATE(CDnsSettingDlg)

public:
	CDnsSettingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDnsSettingDlg();
// ��д

// �Ի�������
	enum { IDD = IDD_DIALOG_DNS_SETTING};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CClrListCtrl m_RuleList;

//�Զ����Ա����
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
