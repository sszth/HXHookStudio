#pragma once
#include "ClrListCtrl.h"


// CProcessLogDlg �Ի���

class CProcessLogDlg : public CDialog
{
	DECLARE_DYNAMIC(CProcessLogDlg)

public:
	CProcessLogDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProcessLogDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_PROCESSLOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
    CClrListCtrl m_ProcessLogList;
    afx_msg void OnBnClickedOk();

//�Զ������
    CString m_ProcessPath;
    afx_msg void OnLvnItemActivateListProcesslog(NMHDR *pNMHDR, LRESULT *pResult);
};
