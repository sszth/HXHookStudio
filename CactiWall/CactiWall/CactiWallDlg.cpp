
// CactiWallDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CactiWall.h"
#include "CactiWallDlg.h"
#include "ProcessSettingDlg.h"
#include "IpSettingDlg.h"
#include "DnsSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCactiWallDlg 对话框




CCactiWallDlg::CCactiWallDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CCactiWallDlg::IDD, pParent)
    , m_bProcessMonitorOn(false)
    , m_bIpMonitorOn(false)
    , m_bDnsMonitorOn(false)
    , m_bMonitorOn(false)
    , m_hDevice(NULL)
    , m_hKey( NULL )
{
    LSTATUS  status = ERROR_SUCCESS;
    DWORD    value = 0;
    DWORD    cbSize = sizeof( DWORD );

    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    status = RegCreateKey( (static_cast<CCactiWallApp*>(AfxGetApp()))->m_hKey,
        _T("globalrules"),
        &m_hKey );
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( _T("打开注册表失败，程序即将退出！"),MB_OK | MB_ICONSTOP );
        exit(0);
    }

#if LOADDEVICE
    //设备句柄在程序类中打开，不需要在本类中关闭！
    m_hDevice = ((CCactiWallApp*)AfxGetApp())->m_hDevice;
    ASSERT( m_hKey != NULL && m_hDevice != NULL);
#else
    ASSERT( m_hKey != NULL );
#endif

    cbSize = sizeof(DWORD);
    status = ::RegQueryValueEx( m_hKey,TEXT("MonitorEnable"),NULL,NULL,(LPBYTE)&value,&cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("读取配置信息失败，程序即将退出！"),MB_OK | MB_ICONSTOP);
        exit(0);
    }
    if( value != 0 )m_bMonitorOn = true;
    else m_bMonitorOn = false;

    if( m_bMonitorOn )
    {
        cbSize = sizeof(DWORD);
        status = ::RegQueryValueEx( m_hKey,TEXT("ProcessMonitorEnable"),NULL,NULL,(LPBYTE)&value,&cbSize);
        if( status != ERROR_SUCCESS )
        {
            AfxMessageBox( TEXT("读取配置信息失败，程序即将退出！"),MB_OK | MB_ICONSTOP);
            exit(0);
        }
        if( value != 0 )m_bProcessMonitorOn = true;
        else m_bProcessMonitorOn = false;

        cbSize = sizeof(DWORD);
        status = ::RegQueryValueEx( m_hKey,TEXT("IpMonitorEnable"),NULL,NULL,(LPBYTE)&value,&cbSize);
        if( status != ERROR_SUCCESS )
        {
            AfxMessageBox( TEXT("读取配置信息失败，程序即将退出！"),MB_OK | MB_ICONSTOP);
            exit(0);
        }
        if( value != 0 )m_bIpMonitorOn = true;
        else m_bIpMonitorOn = false;

        cbSize = sizeof(DWORD);
        status = ::RegQueryValueEx( m_hKey,TEXT("DnsMonitorEnable"),NULL,NULL,(LPBYTE)&value,&cbSize);
        if( status != ERROR_SUCCESS )
        {
            AfxMessageBox( TEXT("读取配置信息失败，程序即将退出！"),MB_OK | MB_ICONSTOP);
            exit(0);
        }
        if( value != 0 )m_bDnsMonitorOn = true;
        else m_bDnsMonitorOn = false;
    }

}

CCactiWallDlg::~CCactiWallDlg()
{
    //柝构函数
    CloseHandle( m_hKey );
    m_hKey = NULL;
#if LOADDEVICE
    m_hDevice = NULL;
#endif

}

void CCactiWallDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_START_PROCESS_MON, m_ButtonStartProcessMon);
    DDX_Control(pDX, IDC_BUTTON_STOP_PROCESS_MON, m_ButtonStopProcessMon);
    DDX_Control(pDX, IDC_BUTTON_START_IP_MON, m_ButtonStartIpMon);
    DDX_Control(pDX, IDC_BUTTON_STOP_IP_MON, m_ButtonStopIpMon);
    DDX_Control(pDX, IDC_BUTTON_START_DNS_MON, m_ButtonStartDnsMon);
    DDX_Control(pDX, IDC_BUTTON_STOP_DNS_MON, m_ButtonStopDnsMon);
    DDX_Control(pDX, IDC_BUTTON_START_MON, m_ButtonStartMon);
    DDX_Control(pDX, IDC_BUTTON_STOP_MON, m_ButtonStopMon);
}

BEGIN_MESSAGE_MAP(CCactiWallDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_START_MON, &CCactiWallDlg::OnBnClickedButtonStartMon)
    ON_BN_CLICKED(IDC_BUTTON_STOP_MON, &CCactiWallDlg::OnBnClickedButtonStopMon)
    ON_BN_CLICKED(IDC_BUTTON_START_PROCESS_MON, &CCactiWallDlg::OnBnClickedButtonStartProcessMon)
    ON_BN_CLICKED(IDC_BUTTON_STOP_PROCESS_MON, &CCactiWallDlg::OnBnClickedButtonStopProcessMon)
    ON_BN_CLICKED(IDC_BUTTON_START_IP_MON, &CCactiWallDlg::OnBnClickedButtonStartIpMon)
    ON_BN_CLICKED(IDC_BUTTON_STOP_IP_MON, &CCactiWallDlg::OnBnClickedButtonStopIpMon)
    ON_BN_CLICKED(IDC_BUTTON_START_DNS_MON, &CCactiWallDlg::OnBnClickedButtonStartDnsMon)
    ON_BN_CLICKED(IDC_BUTTON_STOP_DNS_MON, &CCactiWallDlg::OnBnClickedButtonStopDnsMon)
    ON_BN_CLICKED(IDC_BUTTON_PROCESSRULE_SETTING, &CCactiWallDlg::OnBnClickedButtonProcessruleSetting)
    ON_BN_CLICKED(IDC_BUTTON_IPRULE_SETTING, &CCactiWallDlg::OnBnClickedButtonIpruleSetting)
    ON_BN_CLICKED(IDC_BUTTON_DNSRULE_SETTING, &CCactiWallDlg::OnBnClickedButtonDnsruleSetting)
END_MESSAGE_MAP()


// CCactiWallDlg 消息处理程序

BOOL CCactiWallDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);            // 设置大图标
    SetIcon(m_hIcon, FALSE);        // 设置小图标

    // TODO: 在此添加额外的初始化代码

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCactiWallDlg::OnPaint()
{

    m_ButtonStartMon.EnableWindow( !m_bMonitorOn );
    m_ButtonStopMon.EnableWindow( m_bMonitorOn );
    if( m_bMonitorOn )
    {
        m_ButtonStartProcessMon.EnableWindow( !m_bProcessMonitorOn );
        m_ButtonStopProcessMon.EnableWindow( m_bProcessMonitorOn );
        m_ButtonStartIpMon.EnableWindow( !m_bIpMonitorOn );
        m_ButtonStopIpMon.EnableWindow( m_bIpMonitorOn );
        m_ButtonStartDnsMon.EnableWindow( !m_bDnsMonitorOn );
        m_ButtonStopDnsMon.EnableWindow( m_bDnsMonitorOn );
    }
    else
    {
        m_ButtonStartProcessMon.EnableWindow( false );
        m_ButtonStopProcessMon.EnableWindow( false );
        m_ButtonStartIpMon.EnableWindow( false );
        m_ButtonStopIpMon.EnableWindow( false );
        m_ButtonStartDnsMon.EnableWindow( false );
        m_ButtonStopDnsMon.EnableWindow( false );

    }

    if (IsIconic())
    {
        CPaintDC dc(this); // 用于绘制的设备上下文

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCactiWallDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CCactiWallDlg::OnBnClickedButtonStartMon()
{
    // TODO: 在此添加控件通知处理程序代码
    LSTATUS  status = ERROR_SUCCESS;
    DWORD    value = 0;
    DWORD    cbSize = sizeof( DWORD );
    DWORD    retLength = 0;
    BOOL     bOk = TRUE;

#if LOADDEVICE
    bOk = DeviceIoControl(m_hDevice,IOCTL_MONITOR_ON,NULL,0,NULL,0,&retLength,NULL);
    if( !bOk )
    {
        AfxMessageBox(_T("控制码发送失败！"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif

    cbSize = sizeof(DWORD);
    value = 1;
    status = ::RegSetValueEx( m_hKey,TEXT("MonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("写入配置信息失败"),MB_OK );
    }

    m_bMonitorOn = true;

    //从注册表中读取数据
    cbSize = sizeof(DWORD);
    status = ::RegQueryValueEx( m_hKey,TEXT("ProcessMonitorEnable"),NULL,NULL,(LPBYTE)&value,&cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("读取配置信息失败，程序即将退出！"),MB_OK | MB_ICONSTOP);
        exit(0);
    }
    if( value != 0 )m_bProcessMonitorOn = true;
    else m_bProcessMonitorOn = false;

    cbSize = sizeof(DWORD);
    status = ::RegQueryValueEx( m_hKey,TEXT("IpMonitorEnable"),NULL,NULL,(LPBYTE)&value,&cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("读取配置信息失败，程序即将退出！"),MB_OK | MB_ICONSTOP);
        exit(0);
    }
    if( value != 0 )m_bIpMonitorOn = true;
    else m_bIpMonitorOn = false;

    cbSize = sizeof(DWORD);
    status = ::RegQueryValueEx( m_hKey,TEXT("DnsMonitorEnable"),NULL,NULL,(LPBYTE)&value,&cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("读取配置信息失败，程序即将退出！"),MB_OK | MB_ICONSTOP);
        exit(0);
    }
    if( value != 0 )m_bDnsMonitorOn = true;
    else m_bDnsMonitorOn = false;

    OnPaint();
}

void CCactiWallDlg::OnBnClickedButtonStopMon()
{
    // TODO: 在此添加控件通知处理程序代码
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      value = 0,retLength = 0;
    DWORD      cbSize = sizeof( DWORD );
    BOOL       bOk = TRUE;

#if LOADDEVICE
    bOk = DeviceIoControl(m_hDevice,IOCTL_MONITOR_OFF,NULL,0,NULL,0,&retLength,NULL);
    if( !bOk )
    {
        AfxMessageBox(_T("控制码发送失败！"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif
    //发送控制命令
    //如果不成功，直接返回

    cbSize = sizeof(DWORD);
    value = 0;
    status = ::RegSetValueEx( m_hKey,TEXT("MonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("写入配置信息失败"),MB_OK );
    }

    m_bMonitorOn = false;

    OnPaint();

}

void CCactiWallDlg::OnBnClickedButtonStartProcessMon()
{
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      value = 0,retLength = 0;
    DWORD      cbSize = sizeof( DWORD );
    BOOL       bOk = TRUE;

#if LOADDEVICE
    bOk = DeviceIoControl(m_hDevice,IOCTL_LOAD_PROCESS_CONFIG,NULL,0,NULL,0,&retLength,NULL);
    if( !bOk )
    {
        AfxMessageBox(_T("控制码发送失败！"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif
    ASSERT( m_bMonitorOn );

    //发送控制命令
    //如果不成功，直接返回

    m_bProcessMonitorOn = true;
    cbSize = sizeof(DWORD);
    value = 1;
    status = ::RegSetValueEx( m_hKey,TEXT("ProcessMonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("写入配置信息失败"),MB_OK );
    }
    OnPaint();
}

void CCactiWallDlg::OnBnClickedButtonStopProcessMon()
{
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      value = 0,retLength = 0;
    DWORD      cbSize = sizeof( DWORD );
    BOOL       bOk = TRUE;

#if LOADDEVICE
    bOk = DeviceIoControl(m_hDevice,IOCTL_UNLOAD_PROCESS_CONFIG,NULL,0,NULL,0,&retLength,NULL);
    if( !bOk )
    {
        AfxMessageBox(_T("控制码发送失败！"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif
    // TODO: 在此添加控件通知处理程序代码
    ASSERT( m_bMonitorOn );

    //发送控制命令
    //如果不成功，直接返回

    m_bProcessMonitorOn = false;
    cbSize = sizeof(DWORD);
    value = 0;
    status = ::RegSetValueEx( m_hKey,TEXT("ProcessMonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("写入配置信息失败"),MB_OK );
    }

    OnPaint();

}

void CCactiWallDlg::OnBnClickedButtonStartIpMon()
{
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      value = 0,retLength = 0;
    DWORD      cbSize = sizeof( DWORD );
    BOOL       bOk = TRUE;

#if LOADDEVICE
    bOk = DeviceIoControl(m_hDevice,IOCTL_LOAD_IP_CONFIG,NULL,0,NULL,0,&retLength,NULL);
    if( !bOk )
    {
        AfxMessageBox(_T("控制码发送失败！"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif
    // TODO: 在此添加控件通知处理程序代码
    ASSERT( m_bMonitorOn );

    //发送控制命令
    //如果不成功，直接返回

    m_bIpMonitorOn = true;
    cbSize = sizeof(DWORD);
    value = 1;
    status = ::RegSetValueEx( m_hKey,TEXT("IpMonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("写入配置信息失败"),MB_OK );
    }
    OnPaint();

}

void CCactiWallDlg::OnBnClickedButtonStopIpMon()
{
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      value = 0,retLength = 0;
    DWORD      cbSize = sizeof( DWORD );
    BOOL       bOk = TRUE;

    //发送控制命令
    //如果不成功，直接返回
#if LOADDEVICE
    bOk = DeviceIoControl(m_hDevice,IOCTL_UNLOAD_IP_CONFIG,NULL,0,NULL,0,&retLength,NULL);
    if( !bOk )
    {
        AfxMessageBox(_T("控制码发送失败！"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif
    // TODO: 在此添加控件通知处理程序代码
    ASSERT( m_bMonitorOn );

    m_bIpMonitorOn = false;
    cbSize = sizeof(DWORD);
    value = 0;
    status = ::RegSetValueEx( m_hKey,TEXT("IpMonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("写入配置信息失败"),MB_OK );
    }
    OnPaint();
}

void CCactiWallDlg::OnBnClickedButtonStartDnsMon()
{
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      value = 0,retLength = 0;
    DWORD      cbSize = sizeof( DWORD );
    BOOL       bOk = TRUE;

#if LOADDEVICE
    bOk = DeviceIoControl(m_hDevice,IOCTL_LOAD_DNS_CONFIG,NULL,0,NULL,0,&retLength,NULL);
    if( !bOk )
    {
        AfxMessageBox(_T("控制码发送失败！"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif
    // TODO: 在此添加控件通知处理程序代码
    ASSERT( m_bMonitorOn );

    //发送控制命令
    //如果不成功，直接返回

    m_bDnsMonitorOn = true;
    cbSize = sizeof(DWORD);
    value = 1;
    status = ::RegSetValueEx( m_hKey,TEXT("DnsMonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("写入配置信息失败"),MB_OK );
    }
    OnPaint();

}

void CCactiWallDlg::OnBnClickedButtonStopDnsMon()
{
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      value = 0,retLength = 0;
    DWORD      cbSize = sizeof( DWORD );
    BOOL       bOk = TRUE;

    //发送控制命令
    //如果不成功，直接返回
#if LOADDEVICE
    bOk = DeviceIoControl(m_hDevice,IOCTL_UNLOAD_DNS_CONFIG,NULL,0,NULL,0,&retLength,NULL);
    if( !bOk )
    {
        AfxMessageBox(_T("控制码发送失败！"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif
    // TODO: 在此添加控件通知处理程序代码
    ASSERT( m_bMonitorOn );

    m_bDnsMonitorOn = false;
    cbSize = sizeof(DWORD);
    value = 0;
    status = ::RegSetValueEx( m_hKey,TEXT("DnsMonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("写入配置信息失败"),MB_OK );
    }
    OnPaint();
}

void CCactiWallDlg::OnBnClickedButtonProcessruleSetting()
{
    // TODO: 在此添加控件通知处理程序代码
    CProcessSettingDlg  dlg;

    if( IDOK != dlg.DoModal())
        return;

    if( m_bMonitorOn && this->m_bProcessMonitorOn )
    {
#if LOADDEVICE
        BOOL    bOk = TRUE;
        DWORD   retLength = 0;

        bOk = DeviceIoControl(m_hDevice,IOCTL_UNLOAD_PROCESS_CONFIG,NULL,0,NULL,0,&retLength,NULL);
        if( !bOk )
        {
            AfxMessageBox(_T("控制码发送失败！"),MB_OK | MB_ICONSTOP );
            return;
        }
        bOk = DeviceIoControl(m_hDevice,IOCTL_LOAD_PROCESS_CONFIG,NULL,0,NULL,0,&retLength,NULL);
        if( !bOk )
        {
            AfxMessageBox(_T("控制码发送失败！"),MB_OK | MB_ICONSTOP );
            return;
        }
#endif
    }
}

void CCactiWallDlg::OnBnClickedButtonIpruleSetting()
{
    // TODO: 在此添加控件通知处理程序代码
    CIpSettingDlg  dlg;

    if( IDOK != dlg.DoModal())
        return;

    if( m_bMonitorOn && this->m_bIpMonitorOn )
    {
#if LOADDEVICE
        BOOL    bOk = TRUE;
        DWORD   retLength = 0;

        bOk = DeviceIoControl(m_hDevice,IOCTL_UNLOAD_IP_CONFIG,NULL,0,NULL,0,&retLength,NULL);
        if( !bOk )
        {
            AfxMessageBox(_T("控制码发送失败！"),MB_OK | MB_ICONSTOP );
            return;
        }
        bOk = DeviceIoControl(m_hDevice,IOCTL_LOAD_IP_CONFIG,NULL,0,NULL,0,&retLength,NULL);
        if( !bOk )
        {
            AfxMessageBox(_T("控制码发送失败！"),MB_OK | MB_ICONSTOP );
            return;
        }
#endif
    }
}

void CCactiWallDlg::OnBnClickedButtonDnsruleSetting()
{
    // TODO: 在此添加控件通知处理程序代码
    CDnsSettingDlg  dlg;

    if( IDOK != dlg.DoModal())
        return;

    if( m_bMonitorOn && this->m_bDnsMonitorOn )
    {
#if LOADDEVICE
        BOOL    bOk = TRUE;
        DWORD   retLength = 0;

        bOk = DeviceIoControl(m_hDevice,IOCTL_UNLOAD_DNS_CONFIG,NULL,0,NULL,0,&retLength,NULL);
        if( !bOk )
        {
            AfxMessageBox(_T("控制码发送失败！"),MB_OK | MB_ICONSTOP );
            return;
        }
        bOk = DeviceIoControl(m_hDevice,IOCTL_LOAD_DNS_CONFIG,NULL,0,NULL,0,&retLength,NULL);
        if( !bOk )
        {
            AfxMessageBox(_T("控制码发送失败！"),MB_OK | MB_ICONSTOP );
            return;
        }
#endif
    }
}
