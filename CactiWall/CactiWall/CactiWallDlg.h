
// CactiWallDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "ctlcode.h"

// CCactiWallDlg �Ի���
class CCactiWallDlg : public CDialog
{
// ����
public:
    CCactiWallDlg(CWnd* pParent = NULL);    // ��׼���캯��

//�ع�
    ~CCactiWallDlg();

// �Ի�������
    enum { IDD = IDD_CACTIWALL_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��


// ʵ��
protected:
    HICON m_hIcon;

    // ���ɵ���Ϣӳ�亯��
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

    //�Զ������

    bool    m_bProcessMonitorOn;
    bool    m_bIpMonitorOn;
    bool    m_bDnsMonitorOn;
    bool    m_bMonitorOn;
    HKEY    m_hKey;
    HANDLE  m_hDevice;
    //�ؼ�����

    CButton m_ButtonStartProcessMon;
    CButton m_ButtonStopProcessMon;
    CButton m_ButtonStartIpMon;
    CButton m_ButtonStopIpMon;
    CButton m_ButtonStartDnsMon;
    CButton m_ButtonStopDnsMon;
    CButton m_ButtonStartMon;
    CButton m_ButtonStopMon;
public:
    afx_msg void OnBnClickedButtonStartMon();
    afx_msg void OnBnClickedButtonStopMon();
    afx_msg void OnBnClickedButtonStartProcessMon();
    afx_msg void OnBnClickedButtonStopProcessMon();
    afx_msg void OnBnClickedButtonStartIpMon();
    afx_msg void OnBnClickedButtonStopIpMon();
    afx_msg void OnBnClickedButtonStartDnsMon();
    afx_msg void OnBnClickedButtonStopDnsMon();
    afx_msg void OnBnClickedButtonProcessruleSetting();
    afx_msg void OnBnClickedButtonIpruleSetting();
    afx_msg void OnBnClickedButtonDnsruleSetting();
};
