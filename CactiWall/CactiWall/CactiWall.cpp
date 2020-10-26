
// CactiWall.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "CactiWall.h"
#include "CactiWallDlg.h"
#include "crc32.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCactiWallApp

BEGIN_MESSAGE_MAP(CCactiWallApp, CWinAppEx)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCactiWallApp ����

CCactiWallApp::CCactiWallApp()
    :m_hDevice(NULL)
    ,m_hKey(NULL)
{
    // TODO: �ڴ˴���ӹ�����룬
    // ��������Ҫ�ĳ�ʼ�������� InitInstance ��
    LSTATUS status = ERROR_SUCCESS;
#if LOADDEVICE
    m_hDevice = CreateFile( _T("\\\\.\\Wall_Device"),
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
    if( m_hDevice == INVALID_HANDLE_VALUE)
    {
        AfxMessageBox(_T("���豸ʧ�ܣ����򼴽��˳���"),MB_OK | MB_ICONSTOP );
        exit(0);
    }
#endif

    status = RegCreateKey(HKEY_LOCAL_MACHINE,_T("Software\\lzcj\\CactiWall"),&m_hKey );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("��ע���ʧ�ܣ����򼴽��˳���"),MB_OK | MB_ICONSTOP );
        exit(0);
    }
    crc32_init();

#if LOADDEVICE
    ASSERT( m_hDevice != NULL && m_hKey != NULL );
#endif
}

//�ع�����
CCactiWallApp::~CCactiWallApp()
{
    CloseHandle( m_hDevice );
    m_hDevice = NULL;
    CloseHandle( m_hKey );
    m_hKey = NULL;
}

// Ψһ��һ�� CCactiWallApp ����

CCactiWallApp theApp;


// CCactiWallApp ��ʼ��

BOOL CCactiWallApp::InitInstance()
{
    // ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
    // ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
    //����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
    // �����ؼ��ࡣ
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinAppEx::InitInstance();

    AfxEnableControlContainer();

    // ��׼��ʼ��
    // ���δʹ����Щ���ܲ�ϣ����С
    // ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
    // ����Ҫ���ض���ʼ������
    // �������ڴ洢���õ�ע�����
    // TODO: Ӧ�ʵ��޸ĸ��ַ�����
    // �����޸�Ϊ��˾����֯��
    SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

    CCactiWallDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: �ڴ˷��ô����ʱ��
        //  ��ȷ�������رնԻ���Ĵ���
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: �ڴ˷��ô����ʱ��
        //  ��ȡ�������رնԻ���Ĵ���
    }

    // ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
    //  ����������Ӧ�ó������Ϣ�á�
    return FALSE;
}
