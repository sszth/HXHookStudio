    IP_RULES_ELEM   rule = {0};
    CIpRuleAddDlg   dlg;
    dlg.m_Title = _T("添加IP规则");
    if( IDOK == dlg.DoModal())
    {
        //添加新规则
        rule.crcRuleName = crc32_encode( (char *)dlg.m_RuleName.GetBuffer(),
                      dlg.m_RuleName.GetLength() * sizeof( TCHAR)); 
        rule.rule.Bits.Access = dlg.m_bAllow;
        rule.rule.Bits.Direction = dlg.m_Direction;
        rule.rule.Bits.IcmpCode = dlg.m_IcmpCode;
        rule.rule.Bits.IcmpType = dlg.m_IcmpType;
        rule.rule.Bits.LocalAddrType = dlg.m_LocalAddrType;
        rule.rule.Bits.LocalPortType = dlg.m_LocalPortType;
        rule.rule.Bits.ProtocolType = dlg.m_ProtocolType;
        rule.rule.Bits.RemoteAddrType = dlg.m_RemoteAddrType;
        rule.rule.Bits.RemotePortType = dlg.m_RemotePortType;
        rule.rule.Bits.Reserved = 0;
        rule.LocalAddr = htonl(dlg.m_LocalAddr1);
        rule.LocalAddr2 = htonl( dlg.m_LocalAddr2 );
        rule.LocalPort = htons( (USHORT)dlg.m_LocalPort1 );
        rule.LocalPort2 = htons( (USHORT)dlg.m_LocalPort2 );
        rule.RemoteAddr = htonl( dlg.m_RemoteAddr1 );
        rule.RemoteAddr2 = htonl( dlg.m_RemoteAddr2 );
        rule.RemotePort = htons( (USHORT)dlg.m_RemotePort1);
        rule.RemotePort2 = htons( (USHORT)dlg.m_RemotePort2);
        
        if(RegAddIpRule( &rule,dlg.m_RuleName.GetBuffer()))
           UpdateRuleList();
        else
            AfxMessageBox(_T("添加IP规则错误!"));

    }
    else
    {
    }
}

void CIpSettingDlg::OnBnClickedButtonEditrule()
{
    // TODO: 在此添加控件通知处理程序代码
    CIpRuleAddDlg   dlg;
    CString        s = _T("");
    DWORD          a1=0,a2=0,a3=0,a4=0;//用于转换IP地址和端口
    DWORD          a5=0,a6=0,a7=0,a8=0; 
    dlg.m_Title = _T("编辑IP规则");

    dlg.m_RuleName = m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),0 );

    if( m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),1 )==_T("允许"))
        dlg.m_bAllow = TRUE;
    else
        dlg.m_bAllow = FALSE;

    if( m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),2 )==_T("任意"))
        dlg.m_Direction = 0;
    else if( m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),2 )==_T("上行"))
        dlg.m_Direction = 1;
    else if( m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),2 )==_T("下行"))
        dlg.m_Direction = 2;
    
    s = m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),3 );
    if( s == _T("任意地址") )
        dlg.m_LocalAddrType = 0;
    else if( NULL == _tcsstr( s.GetBuffer(),_T("--")))
    {
        _stscanf_s( s.GetBuffer(),_T("%d.%d.%d.%d"),&a1,&a2,&a3,&a4 );
        dlg.m_LocalAddrType = 1;
        dlg.m_LocalAddr1 = a4 | (a3<<8) | (a2<<16) | ( a1<<24);
    }
    else
    {
        _stscanf_s( s.GetBuffer(),_T("%d.%d.%d.%d--%d.%d.%d.%d"),
            &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8 );
        dlg.m_LocalAddrType = 2;
        dlg.m_LocalAddr1 = a4 | (a3<<8) | (a2<<16) | ( a1<<24);
        dlg.m_LocalAddr2 = a8 | (a7<<8) | (a6<<16) | ( a5<<24);
    }

    s = m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),4 );
    if( s == _T("任意地址") )
        dlg.m_RemoteAddrType = 0;
    else if( NULL == _tcsstr( s.GetBuffer(),_T("--")))
    {
        _stscanf_s( s.GetBuffer(),_T("%d.%d.%d.%d"),&a1,&a2,&a3,&a4 );
        dlg.m_RemoteAddrType = 1;
        dlg.m_RemoteAddr1 = a4 | (a3<<8) | (a2<<16) | ( a1<<24);
    }
    else
    {
        _stscanf_s( s.GetBuffer(),_T("%d.%d.%d.%d--%d.%d.%d.%d"),
            &a1,&a2,&a3,&a4,&a5,&a6,&a7,&a8 );
        dlg.m_RemoteAddrType = 2;
        dlg.m_RemoteAddr1 = a4 | (a3<<8) | (a2<<16) | ( a1<<24);
        dlg.m_RemoteAddr2 = a8 | (a7<<8) | (a6<<16) | ( a5<<24);
    }

    s = m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),5 );
    if( s == _T("--") )
        dlg.m_ProtocolType = 0;
    else if( s == _T("TCP"))
        dlg.m_ProtocolType = IPPROTO_TCP;
    else if( s == _T("UDP"))
        dlg.m_ProtocolType = IPPROTO_UDP;
    else if( s == _T("ICMP"))
        dlg.m_ProtocolType = I