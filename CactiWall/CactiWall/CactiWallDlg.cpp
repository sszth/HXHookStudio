
// CactiWallDlg.cpp : ʵ���ļ�
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


// CCactiWallDlg �Ի���




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
        AfxMessageBox( _T("��ע���ʧ�ܣ����򼴽��˳���"),MB_OK | MB_ICONSTOP );
        exit(0);
    }

#if LOADDEVICE
    //�豸����ڳ������д򿪣�����Ҫ�ڱ����йرգ�
    m_hDevice = ((CCactiWallApp*)AfxGetApp())->m_hDevice;
    ASSERT( m_hKey != NULL && m_hDevice != NULL);
#else
    ASSERT( m_hKey != NULL );
#endif

    cbSize = sizeof(DWORD);
    status = ::RegQueryValueEx( m_hKey,TEXT("MonitorEnable"),NULL,NULL,(LPBYTE)&value,&cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("��ȡ������Ϣʧ�ܣ����򼴽��˳���"),MB_OK | MB_ICONSTOP);
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
            AfxMessageBox( TEXT("��ȡ������Ϣʧ�ܣ����򼴽��˳���"),MB_OK | MB_ICONSTOP);
            exit(0);
        }
        if( value != 0 )m_bProcessMonitorOn = true;
        else m_bProcessMonitorOn = false;

        cbSize = sizeof(DWORD);
        status = ::RegQueryValueEx( m_hKey,TEXT("IpMonitorEnable"),NULL,NULL,(LPBYTE)&value,&cbSize);
        if( status != ERROR_SUCCESS )
        {
            AfxMessageBox( TEXT("��ȡ������Ϣʧ�ܣ����򼴽��˳���"),MB_OK | MB_ICONSTOP);
            exit(0);
        }
        if( value != 0 )m_bIpMonitorOn = true;
        else m_bIpMonitorOn = false;

        cbSize = sizeof(DWORD);
        status = ::RegQueryValueEx( m_hKey,TEXT("DnsMonitorEnable"),NULL,NULL,(LPBYTE)&value,&cbSize);
        if( status != ERROR_SUCCESS )
        {
            AfxMessageBox( TEXT("��ȡ������Ϣʧ�ܣ����򼴽��˳���"),MB_OK | MB_ICONSTOP);
            exit(0);
        }
        if( value != 0 )m_bDnsMonitorOn = true;
        else m_bDnsMonitorOn = false;
    }

}

CCactiWallDlg::~CCactiWallDlg()
{
    //�ع�����
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


// CCactiWallDlg ��Ϣ�������

BOOL CCactiWallDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
    //  ִ�д˲���
    SetIcon(m_hIcon, TRUE);            // ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);        // ����Сͼ��

    // TODO: �ڴ���Ӷ���ĳ�ʼ������

    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

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
        CPaintDC dc(this); // ���ڻ��Ƶ��豸������

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // ʹͼ���ڹ����������о���
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // ����ͼ��
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCactiWallDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CCactiWallDlg::OnBnClickedButtonStartMon()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    LSTATUS  status = ERROR_SUCCESS;
    DWORD    value = 0;
    DWORD    cbSize = sizeof( DWORD );
    DWORD    retLength = 0;
    BOOL     bOk = TRUE;

#if LOADDEVICE
    bOk = DeviceIoControl(m_hDevice,IOCTL_MONITOR_ON,NULL,0,NULL,0,&retLength,NULL);
    if( !bOk )
    {
        AfxMessageBox(_T("�����뷢��ʧ�ܣ�"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif

    cbSize = sizeof(DWORD);
    value = 1;
    status = ::RegSetValueEx( m_hKey,TEXT("MonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("д��������Ϣʧ��"),MB_OK );
    }

    m_bMonitorOn = true;

    //��ע����ж�ȡ����
    cbSize = sizeof(DWORD);
    status = ::RegQueryValueEx( m_hKey,TEXT("ProcessMonitorEnable"),NULL,NULL,(LPBYTE)&value,&cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("��ȡ������Ϣʧ�ܣ����򼴽��˳���"),MB_OK | MB_ICONSTOP);
        exit(0);
    }
    if( value != 0 )m_bProcessMonitorOn = true;
    else m_bProcessMonitorOn = false;

    cbSize = sizeof(DWORD);
    status = ::RegQueryValueEx( m_hKey,TEXT("IpMonitorEnable"),NULL,NULL,(LPBYTE)&value,&cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("��ȡ������Ϣʧ�ܣ����򼴽��˳���"),MB_OK | MB_ICONSTOP);
        exit(0);
    }
    if( value != 0 )m_bIpMonitorOn = true;
    else m_bIpMonitorOn = false;

    cbSize = sizeof(DWORD);
    status = ::RegQueryValueEx( m_hKey,TEXT("DnsMonitorEnable"),NULL,NULL,(LPBYTE)&value,&cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("��ȡ������Ϣʧ�ܣ����򼴽��˳���"),MB_OK | MB_ICONSTOP);
        exit(0);
    }
    if( value != 0 )m_bDnsMonitorOn = true;
    else m_bDnsMonitorOn = false;

    OnPaint();
}

void CCactiWallDlg::OnBnClickedButtonStopMon()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      value = 0,retLength = 0;
    DWORD      cbSize = sizeof( DWORD );
    BOOL       bOk = TRUE;

#if LOADDEVICE
    bOk = DeviceIoControl(m_hDevice,IOCTL_MONITOR_OFF,NULL,0,NULL,0,&retLength,NULL);
    if( !bOk )
    {
        AfxMessageBox(_T("�����뷢��ʧ�ܣ�"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif
    //���Ϳ�������
    //������ɹ���ֱ�ӷ���

    cbSize = sizeof(DWORD);
    value = 0;
    status = ::RegSetValueEx( m_hKey,TEXT("MonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("д��������Ϣʧ��"),MB_OK );
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
        AfxMessageBox(_T("�����뷢��ʧ�ܣ�"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif
    ASSERT( m_bMonitorOn );

    //���Ϳ�������
    //������ɹ���ֱ�ӷ���

    m_bProcessMonitorOn = true;
    cbSize = sizeof(DWORD);
    value = 1;
    status = ::RegSetValueEx( m_hKey,TEXT("ProcessMonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("д��������Ϣʧ��"),MB_OK );
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
        AfxMessageBox(_T("�����뷢��ʧ�ܣ�"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    ASSERT( m_bMonitorOn );

    //���Ϳ�������
    //������ɹ���ֱ�ӷ���

    m_bProcessMonitorOn = false;
    cbSize = sizeof(DWORD);
    value = 0;
    status = ::RegSetValueEx( m_hKey,TEXT("ProcessMonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("д��������Ϣʧ��"),MB_OK );
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
        AfxMessageBox(_T("�����뷢��ʧ�ܣ�"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    ASSERT( m_bMonitorOn );

    //���Ϳ�������
    //������ɹ���ֱ�ӷ���

    m_bIpMonitorOn = true;
    cbSize = sizeof(DWORD);
    value = 1;
    status = ::RegSetValueEx( m_hKey,TEXT("IpMonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("д��������Ϣʧ��"),MB_OK );
    }
    OnPaint();

}

void CCactiWallDlg::OnBnClickedButtonStopIpMon()
{
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      value = 0,retLength = 0;
    DWORD      cbSize = sizeof( DWORD );
    BOOL       bOk = TRUE;

    //���Ϳ�������
    //������ɹ���ֱ�ӷ���
#if LOADDEVICE
    bOk = DeviceIoControl(m_hDevice,IOCTL_UNLOAD_IP_CONFIG,NULL,0,NULL,0,&retLength,NULL);
    if( !bOk )
    {
        AfxMessageBox(_T("�����뷢��ʧ�ܣ�"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    ASSERT( m_bMonitorOn );

    m_bIpMonitorOn = false;
    cbSize = sizeof(DWORD);
    value = 0;
    status = ::RegSetValueEx( m_hKey,TEXT("IpMonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("д��������Ϣʧ��"),MB_OK );
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
        AfxMessageBox(_T("�����뷢��ʧ�ܣ�"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    ASSERT( m_bMonitorOn );

    //���Ϳ�������
    //������ɹ���ֱ�ӷ���

    m_bDnsMonitorOn = true;
    cbSize = sizeof(DWORD);
    value = 1;
    status = ::RegSetValueEx( m_hKey,TEXT("DnsMonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("д��������Ϣʧ��"),MB_OK );
    }
    OnPaint();

}

void CCactiWallDlg::OnBnClickedButtonStopDnsMon()
{
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      value = 0,retLength = 0;
    DWORD      cbSize = sizeof( DWORD );
    BOOL       bOk = TRUE;

    //���Ϳ�������
    //������ɹ���ֱ�ӷ���
#if LOADDEVICE
    bOk = DeviceIoControl(m_hDevice,IOCTL_UNLOAD_DNS_CONFIG,NULL,0,NULL,0,&retLength,NULL);
    if( !bOk )
    {
        AfxMessageBox(_T("�����뷢��ʧ�ܣ�"),MB_OK | MB_ICONSTOP );
        return;
    }
#endif
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    ASSERT( m_bMonitorOn );

    m_bDnsMonitorOn = false;
    cbSize = sizeof(DWORD);
    value = 0;
    status = ::RegSetValueEx( m_hKey,TEXT("DnsMonitorEnable"),0,REG_DWORD,(PBYTE)&value,cbSize);
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( TEXT("д��������Ϣʧ��"),MB_OK );
    }
    OnPaint();
}

void CCactiWallDlg::OnBnClickedButtonProcessruleSetting()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
            AfxMessageBox(_T("�����뷢��ʧ�ܣ�"),MB_OK | MB_ICONSTOP );
            return;
        }
        bOk = DeviceIoControl(m_hDevice,IOCTL_LOAD_PROCESS_CONFIG,NULL,0,NULL,0,&retLength,NULL);
        if( !bOk )
        {
            AfxMessageBox(_T("�����뷢��ʧ�ܣ�"),MB_OK | MB_ICONSTOP );
            return;
        }
#endif
    }
}

void CCactiWallDlg::OnBnClickedButtonIpruleSetting()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
            AfxMessageBox(_T("�����뷢��ʧ�ܣ�"),MB_OK | MB_ICONSTOP );
            return;
        }
        bOk = DeviceIoControl(m_hDevice,IOCTL_LOAD_IP_CONFIG,NULL,0,NULL,0,&retLength,NULL);
        if( !bOk )
        {
            AfxMessageBox(_T("�����뷢��ʧ�ܣ�"),MB_OK | MB_ICONSTOP );
            return;
        }
#endif
    }
}

void CCactiWallDlg::OnBnClickedButtonDnsruleSetting()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
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
            AfxMessageBox(_T("�����뷢��ʧ�ܣ�"),MB_OK | MB_ICONSTOP );
            return;
        }
        bOk = DeviceIoControl(m_hDevice,IOCTL_LOAD_DNS_CONFIG,NULL,0,NULL,0,&retLength,NULL);
        if( !bOk )
        {
            AfxMessageBox(_T("�����뷢��ʧ�ܣ�"),MB_OK | MB_ICONSTOP );
            return;
        }
#endif
    }
}
