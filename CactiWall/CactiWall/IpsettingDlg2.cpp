// IpSettingDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CactiWall.h"
#include "IpSettingDlg.h"
#include "IpRuleAddDlg.h"
#include "crc32.h"

// CIpSettingDlg �Ի���

IMPLEMENT_DYNAMIC(CIpSettingDlg, CDialog)

CIpSettingDlg::CIpSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIpSettingDlg::IDD, pParent)
{

}

CIpSettingDlg::~CIpSettingDlg()
{
}

void CIpSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RULE_LIST, m_RuleList);
}

BOOL CIpSettingDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_RuleList.InsertColumn(0,_T("��������"),0,100);
    m_RuleList.InsertColumn(1,_T("����"),0,50);
    m_RuleList.InsertColumn(2,_T("����"),0,50);
    m_RuleList.InsertColumn(3,_T("ԴIP"),0,100);
    m_RuleList.InsertColumn(4,_T("Ŀ��IP"),0,100);
    m_RuleList.InsertColumn(5,_T("Э������"),0,70);
    m_RuleList.InsertColumn(6,_T("Դ�˿�"),0,70);
    m_RuleList.InsertColumn(7,_T("Ŀ��˿�"),0,70);
    m_RuleList.InsertColumn(8,_T("ICMP���ͺʹ���"),0,100);

    UpdateRuleList();
    return TRUE;
}

void CIpSettingDlg::UpdateRuleList()
{
    LSTATUS    status = ERROR_SUCCESS;
    DWORD      cbSize = 0;
    HKEY       hKey = NULL;
    TCHAR      buffer[256]={0};
    TCHAR      szRuleName[256] = {0};
    DWORD      i;
    BOOL       bOtherAccess = FALSE;

    status = RegCreateKey( (static_cast<CCactiWallApp*>(AfxGetApp()))->m_hKey,
        _T("iprules"),
        &hKey );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("��ȡIP����ʧ��(001)"),MB_OK | MB_ICONWARNING );
        return;
    }

    cbSize = sizeof( buffer );
    status = RegQueryValueEx( hKey,
                          _T("other_access"),
                            NULL,
                            NULL,
                            (LPBYTE)buffer,
                            &cbSize );
    if( status != ERROR_SUCCESS )
    {
        bOtherAccess = TRUE;
        AfxMessageBox(_T("��ȡ������Ϣʧ�ܣ�otheraccess)"));
    }
    ASSERT( cbSize == sizeof(DWORD));
    if( *(PDWORD)buffer == 0 )
        bOtherAccess = FALSE;
    else
        bOtherAccess = TRUE;

    m_RuleList.DeleteAllItems();

    for( i = 0,cbSize = sizeof( buffer);
        ERROR_SUCCESS == RegEnumKey( hKey,i,buffer,cbSize / sizeof(TCHAR));
       i++)
    {
        HKEY   hRuleKey = NULL;
        DWORD  crcName = 0;
        DWORD  rule = 0;
        IP_RULES_ELEM   rule_elem = {0};

        _stscanf_s( buffer,_T("%x"),&crcName );
        rule_elem.crcRuleName = crcName;

        status = RegOpenKey( hKey,buffer,&hRuleKey );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("��ȡIP����ʧ��(002)"),MB_OK | MB_ICONWARNING );
            break;
        }
        
        cbSize = sizeof( buffer);
        status = RegQueryValueEx( hRuleKey,
                              _T("name"),
                                NULL,
                                NULL,
                                (LPBYTE)buffer,
                                &cbSize );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("��ȡ���̹���ʧ��(003)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );
            break;
        }
        _tcscpy_s( szRuleName,255,buffer );

        cbSize = sizeof( DWORD);
        status = RegQueryValueEx( hRuleKey,
                              _T("rule"),
                                NULL,
                                NULL,
                                (LPBYTE)&rule,
                                &cbSize );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("��ȡ���̹���ʧ��(004)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );
            break;
        }
        ASSERT(cbSize==sizeof(DWORD));
        rule_elem.rule.u32 = rule;

        //��ȡԴIP
        if( rule_elem.rule.Bits.LocalAddrType == 1/*UniqueAddr*/ )
        {
            cbSize = sizeof( buffer);
            status = RegQueryValueEx( hRuleKey,
                                  _T("LocalAddr"),
                                    NULL,
                                    NULL,
                                    (LPBYTE)buffer,
                                    &cbSize );
            if( status != ERROR_SUCCESS )
            {
                AfxMessageBox(_T("��ȡ������Ϣʧ�ܣ�LocalAddr"));
                CloseHandle( hRuleKey );
                break;
            }
            ASSERT( cbSize == sizeof(DWORD));
            rule_elem.LocalAddr = *(UINT32 *)buffer;
            
        }
        else if( rule_elem.rule.Bits.LocalAddrType == 2/*RangeAddr*/ )
        {
            cbSize = sizeof( buffer);
            status = RegQueryValueEx( hRuleKey,
                                  _T("LocalAddr"),
                                    NULL,
                                    NULL,
                                    (LPBYTE)buffer,
                                    &cbSize );
            if( status != ERROR_SUCCESS )
            {
                AfxMessageBox(_T("��ȡ������Ϣʧ�ܣ�LocalAddr)"));
                CloseHandle( hRuleKey );
                break;
            }
            ASSERT( cbSize == sizeof(DWORD));
            rule_elem.LocalAddr = *(UINT32 *)buffer;

            cbSize = sizeof( buffer);
            status = RegQueryValueEx( hRuleKey,
                                  _T("LocalAddr2"),
                                    NULL,
                                    NULL,
                                    (LPBYTE)buffer,
                                    &cbSize );
            if( status != ERROR_SUCCESS )
            {
                AfxMessageBox(_T("��ȡ������Ϣʧ�ܣ�LocalAddr2)"));
                CloseHandle( hRuleKey );
                break;
            }
            ASSERT( cbSize == sizeof(DWORD));
            rule_elem.LocalAddr2 = *(UINT32 *)buffer;
        }
        
        //��ȡĿ��IP
        if( rule_elem.rule.Bits.RemoteAddrType == 1/*UniqueAddr*/ )
        {
            cbSize = sizeof( buffer);
            status = RegQueryValueEx( hRuleKey,
                                  _T("RemoteAddr"),
                                    NULL,
                                    NULL,
                                    (LPBYTE)buffer,
                                    &cbSize );
            if( status != ERROR_SUCCESS )
            {
                AfxMessageBox(_T("��ȡ������Ϣʧ�ܣ�RemoteAddr)"));
                CloseHandle( hRuleKey );
                break;
            }
            ASSERT( cbSize == sizeof(DWORD));
            rule_elem.RemoteAddr = *(UINT32 *)buffer;
        }
        else if( rule_elem.rule.Bits.RemoteAddrType == 2/*RangeAddr*/ )
        {
            cbSize = sizeof( buffer);
            status = RegQueryValueEx( hRuleKey,
                                  _T("RemoteAddr"),
                                    NULL,
                                    NULL,
                                    (LPBYTE)buffer,
                                    &cbSize );
            if( status != ERROR_SUCCESS )
            {
                AfxMessageBox(_T("��ȡ������Ϣʧ�ܣ�RemoteAddr)"));
                CloseHandle( hRuleKey );
                break;
            }
            ASSERT( cbSize == sizeof(DWORD));
            rule_elem.RemoteAddr = *(UINT32 *)buffer;

            cbSize = sizeof( buffer);
            status = RegQueryValueEx( hRuleKey,
                                  _T("RemoteAddr2"),
                                    NULL,
                                    NULL,
                                    (LPBYTE)buffer,
                                    &cbSize );
            if( status != ERROR_SUCCESS )
            {
                AfxMessageBox(_T("��ȡ������Ϣʧ�ܣ�RemoteAddr2)"));
                CloseHandle( hRuleKey );
                break;
            }
            ASSERT( cbSize == sizeof(DWORD));
            rule_elem.RemoteAddr2 = *(UINT32 *)buffer;
