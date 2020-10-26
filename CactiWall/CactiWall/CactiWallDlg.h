
// CactiWallDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "ctlcode.h"

// CCactiWallDlg 对话框
class CCactiWallDlg : public CDialog
{
// 构造
public:
    CCactiWallDlg(CWnd* pParent = NULL);    // 标准构造函数

//柝构
    ~CCactiWallDlg();

// 对话框数据
    enum { IDD = IDD_CACTIWALL_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


// 实现
protected:
    HICON m_hIcon;

    // 生成的消息映射函数
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

    //自定义变量

    bool    m_bProcessMonitorOn;
    bool    m_bIpMonitorOn;
    bool    m_bDnsMonitorOn;
    bool    m_bMonitorOn;
    HKEY    m_hKey;
    HANDLE  m_hDevice;
    //控件变量

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
