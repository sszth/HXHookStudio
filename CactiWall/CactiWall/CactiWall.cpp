
// CactiWall.cpp : 定义应用程序的类行为。
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


// CCactiWallApp 构造

CCactiWallApp::CCactiWallApp()
    :m_hDevice(NULL)
    ,m_hKey(NULL)
{
    // TODO: 在此处添加构造代码，
    // 将所有重要的初始化放置在 InitInstance 中
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
        AfxMessageBox(_T("打开设备失败，程序即将退出！"),MB_OK | MB_ICONSTOP );
        exit(0);
    }
#endif

    status = RegCreateKey(HKEY_LOCAL_MACHINE,_T("Software\\lzcj\\CactiWall"),&m_hKey );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("打开注册表失败，程序即将退出！"),MB_OK | MB_ICONSTOP );
        exit(0);
    }
    crc32_init();

#if LOADDEVICE
    ASSERT( m_hDevice != NULL && m_hKey != NULL );
#endif
}

//柝构函数
CCactiWallApp::~CCactiWallApp()
{
    CloseHandle( m_hDevice );
    m_hDevice = NULL;
    CloseHandle( m_hKey );
    m_hKey = NULL;
}

// 唯一的一个 CCactiWallApp 对象

CCactiWallApp theApp;


// CCactiWallApp 初始化

BOOL CCactiWallApp::InitInstance()
{
    // 如果一个运行在 Windows XP 上的应用程序清单指定要
    // 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
    //则需要 InitCommonControlsEx()。否则，将无法创建窗口。
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // 将它设置为包括所有要在应用程序中使用的
    // 公共控件类。
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinAppEx::InitInstance();

    AfxEnableControlContainer();

    // 标准初始化
    // 如果未使用这些功能并希望减小
    // 最终可执行文件的大小，则应移除下列
    // 不需要的特定初始化例程
    // 更改用于存储设置的注册表项
    // TODO: 应适当修改该字符串，
    // 例如修改为公司或组织名
    SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

    CCactiWallDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        // TODO: 在此放置处理何时用
        //  “确定”来关闭对话框的代码
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: 在此放置处理何时用
        //  “取消”来关闭对话框的代码
    }

    // 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
    //  而不是启动应用程序的消息泵。
    return FALSE;
}
