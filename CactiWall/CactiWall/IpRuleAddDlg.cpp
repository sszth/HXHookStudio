// IpRuleAddDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CactiWall.h"
#include "IpRuleAddDlg.h"


// CIpRuleAddDlg 对话框

IMPLEMENT_DYNAMIC(CIpRuleAddDlg, CDialog)

CIpRuleAddDlg::CIpRuleAddDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIpRuleAddDlg::IDD, pParent)
    , m_LocalAddr1(0)
    , m_LocalAddr2(0)
    , m_RemoteAddr1(0)
    , m_RemoteAddr2(0)
    , m_LocalPort1(0)
    , m_LocalPort2(0)
    , m_RemotePort1(0)
    , m_RemotePort2(0)
    , m_IcmpType(0x1f)
    , m_IcmpCode(0x1f)
    , m_RuleName(_T(""))
    , m_bAllow(FALSE)
    , m_Direction(0)
    , m_LocalAddrType(0)
    , m_RemoteAddrType(0)
    , m_ProtocolType(0)
    , m_Title(_T("添加IP规则"))
{

}

CIpRuleAddDlg::~CIpRuleAddDlg()
{
}

void CIpRuleAddDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_IPAddress(pDX, IDC_IPADDRESS_LOCALADDR1, m_LocalAddr1);
    DDX_IPAddress(pDX, IDC_IPADDRESS_LOCALADDR2, m_LocalAddr2);
    DDX_IPAddress(pDX, IDC_IPADDRESS_REMOTEADDR1, m_RemoteAddr1);
    DDX_IPAddress(pDX, IDC_IPADDRESS_REMOTEADDR2, m_RemoteAddr2);
    DDX_Text(pDX, IDC_EDIT_LOCALPORT1, m_LocalPort1);
    DDV_MinMaxUInt(pDX, m_LocalPort1, 0, 65535);
    DDX_Text(pDX, IDC_EDIT_LOCALPORT2, m_LocalPort2);
    DDV_MinMaxUInt(pDX, m_LocalPort2, 0, 65535);
    DDX_Text(pDX, IDC_EDIT_REMOTEPORT1, m_RemotePort1);
    DDV_MinMaxUInt(pDX, m_RemotePort1, 0, 65535);
    DDX_Text(pDX, IDC_EDIT_REMOTEPORT2, m_RemotePort2);
    DDV_MinMaxUInt(pDX, m_RemotePort2, 0, 65535);
    DDX_Text(pDX, IDC_EDIT_ICMPCODE, m_IcmpCode);
    DDV_MinMaxByte(pDX, m_IcmpCode, 0, 255);
    DDX_Text(pDX, IDC_EDIT_RULENAME, m_RuleName);
    DDX_Control(pDX, IDC_COMBO_PROTOCOLTYPE, m_ComboProtoType);
    DDX_Control(pDX, IDC_COMBO_ICMPTYPE, m_ComboIcmpType);
}

BOOL CIpRuleAddDlg::OnInitDialog()
{

    int i = 0;

    CDialog::OnInitDialog();
    
    SetWindowText( m_Title.GetBuffer() );
    if( m_bAllow )
    {
        ((CButton *)GetDlgItem( IDC_RADIO_ALLOW))->SetCheck( TRUE );
        ((CButton *)GetDlgItem( IDC_RADIO_DENY))->SetCheck( FALSE );
    }
    else
    {
        ((CButton *)GetDlgItem( IDC_RADIO_ALLOW))->SetCheck( FALSE );
        ((CButton *)GetDlgItem( IDC_RADIO_DENY))->SetCheck( TRUE );
    }
    
    for( i = IDC_RADIO_ANYDIRECTION;i <= IDC_RADIO_DOWNDIRECTION;i++)
    {
        if( i - IDC_RADIO_ANYDIRECTION == m_Direction )
           ((CButton *)GetDlgItem( i ))->SetCheck( TRUE );
        else
           ((CButton *)GetDlgItem( i ))->SetCheck( FALSE );
    }

    for( i = IDC_RADIO_LOCAL_ANYIP;i <= IDC_RADIO_LOCAL_RANGEIP;i++)
    {
        if( i - IDC_RADIO_LOCAL_ANYIP == m_LocalAddrType )
           ((CButton *)GetDlgItem( i ))->SetCheck( TRUE );
        else
           ((CButton *)GetDlgItem( i ))->SetCheck( FALSE );
    }
    if( m_LocalAddrType == 0 )
        OnBnClickedRadioLocalAnyip();
    else if( m_LocalAddrType == 1 )
        OnBnClickedRadioLocalUniqueip();
    else if( m_LocalAddrType == 2 )
        OnBnClickedRadioLocalRangeip();

    for( i = IDC_RADIO_REMOTE_ANYIP;i <= IDC_RADIO_REMOTE_RANGEIP;i++)
    {
        if( i - IDC_RADIO_REMOTE_ANYIP == m_RemoteAddrType )
           ((CButton *)GetDlgItem( i ))->SetCheck( TRUE );
        else
           ((CButton *)GetDlgItem( i ))->SetCheck( FALSE );
    }
    if( m_RemoteAddrType == 0 )
        OnBnClickedRadioRemoteAnyip();
    else if( m_RemoteAddrType == 1 )
        OnBnClickedRadioRemoteUniqueip();
    else if( m_RemoteAddrType == 2 )
        OnBnClickedRadioRemoteRangeip();
    
    for( i = 0;i < 7;i++)
    {
        m_ComboProtoType.InsertString(i,protocolType[i].name );
        m_ComboProtoType.SetItemData( i,protocolType[i].protocolType );
        if( protocolType[i].protocolType == m_ProtocolType )
           m_ComboProtoType.SetCurSel( i );
    }
    if( -1 == m_ComboProtoType.GetCurSel())
    {
        SetDlgItemInt( IDC_COMBO_PROTOCOLTYPE,m_ProtocolType );
    }

    for( i = 0;i < 16;i++ )
    {
        m_ComboIcmpType.InsertString(i,icmpType[i].name );
        m_ComboIcmpType.SetItemData( i,icmpType[i].icmpType );
        if( icmpType[i].icmpType == m_IcmpType )
           m_ComboIcmpType.SetCurSel( i );

    }
    if( -1 == m_ComboIcmpType.GetCurSel())
    {
        SetDlgItemInt( IDC_COMBO_ICMPTYPE,m_IcmpType );
    }

    OnCbnSelchangeComboProtocoltype();
    
    return TRUE;
}

BEGIN_MESSAGE_MAP(CIpRuleAddDlg, CDialog)
    ON_BN_CLICKED(IDC_RADIO_ALLOW, &CIpRuleAddDlg::OnBnClickedRadioAllow)
    ON_BN_CLICKED(IDC_RADIO_DENY, &CIpRuleAddDlg::OnBnClickedRadioAllow)
    ON_BN_CLICKED(IDC_RADIO_ANYDIRECTION, &CIpRuleAddDlg::OnBnClickedRadioAnydirection)
    ON_BN_CLICKED(IDC_RADIO_UPDIRECTION, &CIpRuleAddDlg::OnBnClickedRadioUpdirection)
    ON_BN_CLICKED(IDC_RADIO_DOWNDIRECTION, &CIpRuleAddDlg::OnBnClickedRadioDowndirection)
    ON_BN_CLICKED(IDC_RADIO_LOCAL_ANYIP, &CIpRuleAddDlg::OnBnClickedRadioLocalAnyip)
    ON_BN_CLICKED(IDC_RADIO_LOCAL_UNIQUEIP, &CIpRuleAddDlg::OnBnClickedRadioLocalUniqueip)
    ON_BN_CLICKED(IDC_RADIO_LOCAL_RANGEIP, &CIpRuleAddDlg::OnBnClickedRadioLocalRangeip)
    ON_BN_CLICKED(IDC_RADIO_REMOTE_ANYIP, &CIpRuleAddDlg::OnBnClickedRadioRemoteAnyip)
    ON_BN_CLICKED(IDC_RADIO_REMOTE_UNIQUEIP, &CIpRuleAddDlg::OnBnClickedRadioRemoteUniqueip)
    ON_BN_CLICKED(IDC_RADIO_REMOTE_RANGEIP, &CIpRuleAddDlg::OnBnClickedRadioRemoteRangeip)
    ON_CBN_SELCHANGE(IDC_COMBO_PROTOCOLTYPE, &CIpRuleAddDlg::OnCbnSelchangeComboProtocoltype)
    ON_BN_CLICKED(IDC_RADIO_LOCAL_ANYPORT, &CIpRuleAddDlg::OnBnClickedRadioLocalAnyport)
    ON_BN_CLICKED(IDC_RADIO_LOCAL_UNIQUEPORT, &CIpRuleAddDlg::OnBnClickedRadioLocalUniqueport)
    ON_BN_CLICKED(IDC_RADIO_LOCAL_RANGEPORT, &CIpRuleAddDlg::OnBnClickedRadioLocalRangeport)
    ON_BN_CLICKED(IDC_RADIO_REMOTE_ANYPORT, &CIpRuleAddDlg::OnBnClickedRadioRemoteAnyport)
    ON_BN_CLICKED(IDC_RADIO_REMOTE_UNIQUEPORT, &CIpRuleAddDlg::OnBnClickedRadioRemoteUniqueport)
    ON_BN_CLICKED(IDC_RADIO_REMOTE_RANGEPORT, &CIpRuleAddDlg::OnBnClickedRadioRemoteRangeport)
    ON_BN_CLICKED(IDOK, &CIpRuleAddDlg::OnBnClickedOk)
    ON_CBN_SELCHANGE(IDC_COMBO_ICMPTYPE, &CIpRuleAddDlg::OnCbnSelchangeComboIcmptype)
END_MESSAGE_MAP()


// CIpRuleAddDlg 消息处理程序

void CIpRuleAddDlg::OnBnClickedRadioAllow()
{
    // TODO: 在此添加控件通知处理程序代码
    m_bAllow = ((CButton *)GetDlgItem( IDC_RADIO_ALLOW))->GetCheck();
}

void CIpRuleAddDlg::OnBnClickedRadioAnydirection()
{
    // TODO: 在此添加控件通知处理程序代码
    m_Direction = 0;

}

void CIpRuleAddDlg::OnBnClickedRadioUpdirection()
{
    // TODO: 在此添加控件通知处理程序代码
        m_Direction = 1;
}

void CIpRuleAddDlg::OnBnClickedRadioDowndirection()
{
    // TODO: 在此添加控件通知处理程序代码
    m_Direction = 2;
}

void CIpRuleAddDlg::OnBnClickedRadioLocalAnyip()
{
    // TODO: 在此添加控件通知处理程序代码
    m_LocalAddrType = 0;
    GetDlgItem( IDC_IPADDRESS_LOCALADDR1)->ShowWindow( SW_HIDE );
    GetDlgItem( IDC_IPADDRESS_LOCALADDR2)->ShowWindow( SW_HIDE );
    GetDlgItem( IDC_STATIC1)->ShowWindow( SW_HIDE );

}

void CIpRuleAddDlg::OnBnClickedRadioLocalUniqueip()
{
    // TODO: 在此添加控件通知处理程序代码
    m_LocalAddrType = 1;
    GetDlgItem( IDC_IPADDRESS_LOCALADDR1)->ShowWindow( SW_SHOW );
    GetDlgItem( IDC_IPADDRESS_LOCALADDR2)->ShowWindow( SW_HIDE );
    GetDlgItem( IDC_STATIC1)->ShowWindow( SW_HIDE );

}

void CIpRuleAddDlg::OnBnClickedRadioLocalRangeip()
{
    // TODO: 在此添加控件通知处理程序代码
    m_LocalAddrType = 2;
    GetDlgItem( IDC_IPADDRESS_LOCALADDR1)->ShowWindow( SW_SHOW );
    GetDlgItem( IDC_IPADDRESS_LOCALADDR2)->ShowWindow( SW_SHOW );
    GetDlgItem( IDC_STATIC1)->ShowWindow( SW_SHOW );

}

void CIpRuleAddDlg::OnBnClickedRadioRemoteAnyip()
{
    // TODO: 在此添加控件通知处理程序代码
    m_RemoteAddrType = 0;
    GetDlgItem( IDC_IPADDRESS_REMOTEADDR1)->ShowWindow( SW_HIDE );
    GetDlgItem( IDC_IPADDRESS_REMOTEADDR2)->ShowWindow( SW_HIDE );
    GetDlgItem( IDC_STATIC2)->ShowWindow( SW_HIDE );

}

void CIpRuleAddDlg::OnBnClickedRadioRemoteUniqueip()
{
    // TODO: 在此添加控件通知处理程序代码
    m_RemoteAddrType = 1;
    GetDlgItem( IDC_IPADDRESS_REMOTEADDR1)->ShowWindow( SW_SHOW );
    GetDlgItem( IDC_IPADDRESS_REMOTEADDR2)->ShowWindow( SW_HIDE );
    GetDlgItem( IDC_STATIC2)->ShowWindow( SW_HIDE );

}

void CIpRuleAddDlg::OnBnClickedRadioRemoteRangeip()
{
    // TODO: 在此添加控件通知处理程序代码
    m_RemoteAddrType = 2;
    GetDlgItem( IDC_IPADDRESS_REMOTEADDR1)->ShowWindow( SW_SHOW );
    GetDlgItem( IDC_IPADDRESS_REMOTEADDR2)->ShowWindow( SW_SHOW );
    GetDlgItem( IDC_STATIC2)->ShowWindow( SW_SHOW );

}

void CIpRuleAddDlg::OnCbnSelchangeComboProtocoltype()
{
    // TODO: 在此添加控件通知处理程序代码
    m_ProtocolType = (BYTE)m_ComboProtoType.GetItemData(m_ComboProtoType.GetCurSel());
    if( m_ProtocolType == IPPROTO_TCP || m_ProtocolType == IPPROTO_UDP )
    {
        GetDlgItem( IDC_RADIO_LOCAL_ANYPORT )->ShowWindow(SW_SHOW );
        GetDlgItem( IDC_RADIO_LOCAL_UNIQUEPORT )->ShowWindow(SW_SHOW );
        GetDlgItem( IDC_RADIO_LOCAL_RANGEPORT )->ShowWindow(SW_SHOW );
        GetDlgItem( IDC_RADIO_REMOTE_ANYPORT )->ShowWindow(SW_SHOW );
        GetDlgItem( IDC_RADIO_REMOTE_UNIQUEPORT )->ShowWindow(SW_SHOW );
        GetDlgItem( IDC_RADIO_REMOTE_RANGEPORT )->ShowWindow(SW_SHOW );

        GetDlgItem( IDC_COMBO_ICMPTYPE)->ShowWindow( SW_HIDE );
        GetDlgItem( IDC_EDIT_ICMPCODE)->ShowWindow( SW_HIDE );

        if( m_LocalPortType == 0 )
        {
           ((CButton *)GetDlgItem( IDC_RADIO_LOCAL_ANYPORT ))->SetCheck(true);
            GetDlgItem( IDC_EDIT_LOCALPORT1 )->ShowWindow(SW_HIDE );
            GetDlgItem( IDC_EDIT_LOCALPORT2 )->ShowWindow(SW_HIDE );
            GetDlgItem( IDC_STATIC3 )->ShowWindow(SW_HIDE );
        }
        else if( m_LocalPortType == 1 )
        {
            ((CButton *)GetDlgItem( IDC_RADIO_LOCAL_UNIQUEPORT ))->SetCheck(true);
            GetDlgItem( IDC_EDIT_LOCALPORT1 )->ShowWindow(SW_SHOW );
            GetDlgItem( IDC_EDIT_LOCALPORT2 )->ShowWindow(SW_HIDE );
            GetDlgItem( IDC_STATIC3 )->ShowWindow(SW_HIDE );

        }
        else if( m_LocalPortType == 2 )
        {
            ((CButton *)GetDlgItem( IDC_RADIO_LOCAL_RANGEPORT ))->SetCheck(true);
            GetDlgItem( IDC_EDIT_LOCALPORT1 )->ShowWindow(SW_SHOW );
            GetDlgItem( IDC_EDIT_LOCALPORT2 )->ShowWindow(SW_SHOW );
            GetDlgItem( IDC_STATIC3 )->ShowWindow(SW_SHOW );
        }


        if( m_RemotePortType == 0 )
        {
           ((CButton *)GetDlgItem( IDC_RADIO_REMOTE_ANYPORT ))->SetCheck(true);
            GetDlgItem( IDC_EDIT_REMOTEPORT1 )->ShowWindow(SW_HIDE );
            GetDlgItem( IDC_EDIT_REMOTEPORT2 )->ShowWindow(SW_HIDE );
            GetDlgItem( IDC_STATIC4 )->ShowWindow(SW_HIDE );
        }
        else if( m_RemotePortType == 1 )
        {
            ((CButton *)GetDlgItem( IDC_RADIO_REMOTE_UNIQUEPORT ))->SetCheck(true);
            GetDlgItem( IDC_EDIT_REMOTEPORT1 )->ShowWindow(SW_SHOW );
            GetDlgItem( IDC_EDIT_REMOTEPORT2 )->ShowWindow(SW_HIDE );
            GetDlgItem( IDC_STATIC4 )->ShowWindow(SW_HIDE );

        }
        else if( m_RemotePortType == 2 )
        {
            ((CButton *)GetDlgItem( IDC_RADIO_REMOTE_RANGEPORT ))->SetCheck(true);
            GetDlgItem( IDC_EDIT_REMOTEPORT1 )->ShowWindow(SW_SHOW );
            GetDlgItem( IDC_EDIT_REMOTEPORT2 )->ShowWindow(SW_SHOW );
            GetDlgItem( IDC_STATIC4 )->ShowWindow(SW_SHOW );
        }

    }
    else if( m_ProtocolType == IPPROTO_ICMP )
    {
        GetDlgItem( IDC_RADIO_LOCAL_ANYPORT )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_RADIO_LOCAL_UNIQUEPORT )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_RADIO_LOCAL_RANGEPORT )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_EDIT_LOCALPORT1 )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_EDIT_LOCALPORT2 )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_STATIC3 )->ShowWindow(SW_HIDE );

        GetDlgItem( IDC_RADIO_REMOTE_ANYPORT )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_RADIO_REMOTE_UNIQUEPORT )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_RADIO_REMOTE_RANGEPORT )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_EDIT_REMOTEPORT1 )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_EDIT_REMOTEPORT2 )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_STATIC4 )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_COMBO_ICMPTYPE)->ShowWindow( SW_SHOW );
        GetDlgItem( IDC_EDIT_ICMPCODE)->ShowWindow( SW_SHOW );
    }
    else
    {
        GetDlgItem( IDC_RADIO_LOCAL_ANYPORT )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_RADIO_LOCAL_UNIQUEPORT )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_RADIO_LOCAL_RANGEPORT )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_EDIT_LOCALPORT1 )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_EDIT_LOCALPORT2 )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_STATIC3 )->ShowWindow(SW_HIDE );

        GetDlgItem( IDC_RADIO_REMOTE_ANYPORT )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_RADIO_REMOTE_UNIQUEPORT )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_RADIO_REMOTE_RANGEPORT )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_EDIT_REMOTEPORT1 )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_EDIT_REMOTEPORT2 )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_STATIC4 )->ShowWindow(SW_HIDE );
        GetDlgItem( IDC_COMBO_ICMPTYPE)->ShowWindow( SW_HIDE );
        GetDlgItem( IDC_EDIT_ICMPCODE)->ShowWindow( SW_HIDE );
    }
}

void CIpRuleAddDlg::OnBnClickedRadioLocalAnyport()
{
    // TODO: 在此添加控件通知处理程序代码
    m_LocalPortType = 0;
    GetDlgItem( IDC_EDIT_LOCALPORT1 )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_EDIT_LOCALPORT2 )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_STATIC3 )->ShowWindow(SW_HIDE );
}

void CIpRuleAddDlg::OnBnClickedRadioLocalUniqueport()
{
    // TODO: 在此添加控件通知处理程序代码
    m_LocalPortType = 1;
    GetDlgItem( IDC_EDIT_LOCALPORT1 )->ShowWindow(SW_SHOW );
    GetDlgItem( IDC_EDIT_LOCALPORT2 )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_STATIC3 )->ShowWindow(SW_HIDE );

}

void CIpRuleAddDlg::OnBnClickedRadioLocalRangeport()
{
    // TODO: 在此添加控件通知处理程序代码
    m_LocalPortType = 2;
    GetDlgItem( IDC_EDIT_LOCALPORT1 )->ShowWindow(SW_SHOW );
    GetDlgItem( IDC_EDIT_LOCALPORT2 )->ShowWindow(SW_SHOW );
    GetDlgItem( IDC_STATIC3 )->ShowWindow(SW_SHOW );

}

void CIpRuleAddDlg::OnBnClickedRadioRemoteAnyport()
{
    m_RemotePortType = 0;
    GetDlgItem( IDC_EDIT_REMOTEPORT1 )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_EDIT_REMOTEPORT2 )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_STATIC4 )->ShowWindow(SW_HIDE );
}

void CIpRuleAddDlg::OnBnClickedRadioRemoteUniqueport()
{
    // TODO: 在此添加控件通知处理程序代码
    m_RemotePortType = 1;
    GetDlgItem( IDC_EDIT_REMOTEPORT1 )->ShowWindow(SW_SHOW );
    GetDlgItem( IDC_EDIT_REMOTEPORT2 )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_STATIC4 )->ShowWindow(SW_HIDE );

}

void CIpRuleAddDlg::OnBnClickedRadioRemoteRangeport()
{
    m_RemotePortType = 2;
    GetDlgItem( IDC_EDIT_REMOTEPORT1 )->ShowWindow(SW_SHOW );
    GetDlgItem( IDC_EDIT_REMOTEPORT2 )->ShowWindow(SW_SHOW );
    GetDlgItem( IDC_STATIC4 )->ShowWindow(SW_SHOW );
}

void CIpRuleAddDlg::OnBnClickedOk()
{
    // TODO: 在此添加控件通知处理程序代码
    UINT i;

    UpdateData( true );
    if( this->m_RuleName == _T(""))
    {
        AfxMessageBox(_T("规则名不能为空！"));
        return;
    }
    i = GetDlgItemInt( IDC_COMBO_PROTOCOLTYPE);
    if( i != 0 )
        m_ProtocolType = i;

    OnOK();
}

void CIpRuleAddDlg::OnCbnSelchangeComboIcmptype()
{
    // TODO: 在此添加控件通知处理程序代码
    m_IcmpType = (BYTE)m_ComboIcmpType.GetItemData(m_ComboIcmpType.GetCurSel());

}
