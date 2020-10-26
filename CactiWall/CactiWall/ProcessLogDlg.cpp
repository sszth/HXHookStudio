// ProcessLogDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CactiWall.h"
#include "ProcessLogDlg.h"
#include <winternl.h>

// CProcessLogDlg �Ի���

IMPLEMENT_DYNAMIC(CProcessLogDlg, CDialog)

CProcessLogDlg::CProcessLogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProcessLogDlg::IDD, pParent)
    ,m_ProcessPath(_T(""))
{

}

CProcessLogDlg::~CProcessLogDlg()
{
}

void CProcessLogDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_PROCESSLOG, m_ProcessLogList);
}

BOOL CProcessLogDlg::OnInitDialog()
{
    CString logFile = _T("");
    HKEY    hKey = NULL;
    LSTATUS status = ERROR_SUCCESS;
    WCHAR   buffer[256] = {0};
    DWORD   cbSize = 0;
    FILE    *fp = NULL;
    int     i = 0;

    CDialog::OnInitDialog();
    
    
    m_ProcessLogList.InsertColumn(0,_T("ʱ��"),0,80);
    m_ProcessLogList.InsertColumn(1,_T("ʱ��"),0,60);
    m_ProcessLogList.InsertColumn(2,_T("ȫ·��"),0,480);

    status = RegCreateKeyW( (static_cast<CCactiWallApp*>(AfxGetApp()))->m_hKey,
        L"globalrules",
        &hKey );
    if( status != ERROR_SUCCESS )
    {
        AfxMessageBox( _T("��ȡ��־�ļ�ʧ��(001)"),MB_OK );
        return TRUE;
    }

    cbSize = sizeof( buffer );
    status = RegQueryValueExW( hKey,L"ProcessLogFile",NULL,NULL,(BYTE*)buffer,&cbSize);
    if( status != ERROR_SUCCESS )
    {
        CloseHandle( hKey );
        AfxMessageBox( _T("��ȡ��־�ļ�ʧ��(002)"),MB_OK );
        return TRUE;
    }
    CloseHandle( hKey );

    //buffer�ĸ�ʽΪ����\??\c:\windows\system........."
    if( 0 != _wfopen_s( &fp, buffer + 4,L"r,ccs=UTF-16LE"))
    {
        AfxMessageBox(_T("����־�ļ�ʧ�ܣ�"));
        return TRUE;
    }
    

    for( i = 0; NULL != fgetws( buffer,256,fp );i++)
    {
        int yy=0,mon=0,dd=0,hh=0,mm=0,ss=0;
        TCHAR  s[12];
        int j;

        //�����β��'\r'�ַ�
        j = wcslen( buffer );
        buffer[j-1] = L'\0';
        j--;

#ifndef UNICODE
        UNICODE_STRING  uniStr = {0};
        ANSI_STRING     ansiStr = {0};
        HMODULE        hNtDll = NULL;
        NTSTATUS ( NTAPI *pRtlUnicodeStringToAnsiString)(PANSI_STRING SourceString,
                                                PCUNICODE_STRING DestinationString,
                                                BOOLEAN AllocateDestinationString) 
                  = NULL;

        hNtDll = GetModuleHandle( _T("ntdll.dll") );
        ASSERT( hNtDll != NULL );
        pRtlUnicodeStringToAnsiString = (NTSTATUS (__stdcall *)(PANSI_STRING,PCUNICODE_STRING,BOOLEAN))
            GetProcAddress( hNtDll,"RtlUnicodeStringToAnsiString");
        ASSERT( pRtlUnicodeStringToAnsiString != NULL);
        ansiStr.Buffer = (char *)buffer;
        ansiStr.MaximumLength = sizeof(buffer);
        uniStr.Buffer = buffer;;
        uniStr.MaximumLength = sizeof( buffer );
        uniStr.Length = j * sizeof( WCHAR );
        pRtlUnicodeStringToAnsiString( &ansiStr,&uniStr,FALSE );
        ((char*)buffer)[ansiStr.Length] = '\0';
#endif
    
        _stscanf_s( (TCHAR *)buffer,_T("[%d-%d-%d-%d-%d-%d]"),&yy,&mon,&dd,&hh,&mm,&ss );
        _stprintf_s( s,12,_T("%d-%d-%d"),yy,mon,dd );
        m_ProcessLogList.InsertItem( 0,s );
        _stprintf_s( s,12,_T("%d:%d:%d"),hh,mm,ss );
        m_ProcessLogList.SetItemText( 0,1,s );
        
        for( j = 0;((TCHAR *)buffer)[j] != _T(']');j++);
        j++;
        m_ProcessLogList.SetItemText( 0,2,(TCHAR*)buffer + j);

        m_ProcessLogList.SetItemData( 0,i );
    }

    fclose( fp );
    fp = NULL; 

    return TRUE;
}

BEGIN_MESSAGE_MAP(CProcessLogDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CProcessLogDlg::OnBnClickedOk)
    ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LIST_PROCESSLOG, &CProcessLogDlg::OnLvnItemActivateListProcesslog)
END_MESSAGE_MAP()


// CProcessLogDlg ��Ϣ�������

void CProcessLogDlg::OnBnClickedOk()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    if( m_ProcessLogList.GetSelectionMark() < 0 )
    {
        AfxMessageBox(_T("δѡ����Ŀ��"));
        return;
    }
    m_ProcessPath = m_ProcessLogList.GetItemText(
        m_ProcessLogList.GetSelectionMark(),
        2);
    OnOK();
}

void CProcessLogDlg::OnLvnItemActivateListProcesslog(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: �ڴ���ӿؼ�֪ͨ����������

    OnBnClickedOk();

    *pResult = 0;
}
