#pragma once
#include "ClrListCtrl.h"


// CProcessLogDlg 对话框

class CProcessLogDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcessLogDlg)

public:
	CProcessLogDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProcessLogDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PROCESSLOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
    CClrListCtrl m_ProcessLogList;
    afx_msg void OnBnClickedOk();

//自定义变量
    CString m_ProcessPath;
    afx_msg void OnLvnItemActivateListProcesslog(NMHDR *pNMHDR, LRESULT *pResult);
};
