y,(LPTSTR)buffer,&hRuleKey);
    if( status == ERROR_SUCCESS )
    {
        CloseHandle( hKey );
        CloseHandle( hRuleKey );
        return false;
    }

    status = RegCreateKey( hKey,
        (LPTSTR)buffer,
        &hRuleKey );
    if( status != ERROR_SUCCESS)
    {
        CloseHandle( hKey );hKey = NULL;
        AfxMessageBox(_T("��ȡIP����ʧ��(002)"),MB_OK | MB_ICONWARNING );
        return false;
    }
    
    status = RegSetValueEx( hRuleKey,
                      _T("name"),
                      0,
                      REG_SZ,
                      (BYTE *)ruleName,
                      _tcslen( ruleName )*sizeof(TCHAR) );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("д����̹���ʧ��(003)"),MB_OK | MB_ICONWARNING );
        CloseHandle( hRuleKey );hRuleKey = NULL;
        CloseHandle( hKey );hKey = NULL;
        return false;
    }

    status = RegSetValueEx( hRuleKey,
                      _T("rule"),
                      0,
                      REG_DWORD,
                      (BYTE *)&rule->rule.u32,
                      sizeof( UINT32) );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("д��IP����ʧ��(004)"),MB_OK | MB_ICONWARNING );
        CloseHandle( hRuleKey );hRuleKey = NULL;
        CloseHandle( hKey );hKey = NULL;
        return false;
    }

    //д��ԴIP
    if( rule->rule.Bits.LocalAddrType == 1/*UniqueAddr*/)
    {
        status = RegSetValueEx( hRuleKey,
                          _T("LocalAddr"),
                          0,
                          REG_DWORD,
                          (BYTE *)&rule->LocalAddr,
                          sizeof( UINT32) );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("д��IP����ʧ��(005)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );hRuleKey = NULL;
            CloseHandle( hKey );hKey = NULL;
            return false;
        }
    }
    else if( rule->rule.Bits.LocalAddrType == 2/*RangeAddr*/)
    {
        status = RegSetValueEx( hRuleKey,
                          _T("LocalAddr"),
                          0,
                          REG_DWORD,
                          (BYTE *)&rule->LocalAddr,
                          sizeof( UINT32) );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("д��IP����ʧ��(006)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );hRuleKey = NULL;
            CloseHandle( hKey );hKey = NULL;
            return false;
        }

        status = RegSetValueEx( hRuleKey,
                          _T("LocalAddr2"),
                          0,
                          REG_DWORD,
                          (BYTE *)&rule->LocalAddr2,
                          sizeof( UINT32) );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("д��IP����ʧ��(007)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );hRuleKey = NULL;
            CloseHandle( hKey );hKey = NULL;
            return false;
        }
    }

    //д��Ŀ��IP
    if( rule->rule.Bits.RemoteAddrType == 1/*UniqueAddr*/)
    {
        status = RegSetValueEx( hRuleKey,
                          _T("RemoteAddr"),
                          0,
                          REG_DWORD,
                          (BYTE *)&rule->RemoteAddr,
                          sizeof( UINT32) );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("д��IP����ʧ��(008)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );hRuleKey = NULL;
            CloseHandle( hKey );hKey = NULL;
            return false;
        }
    }
    else if( rule->rule.Bits.RemoteAddrType == 2/*RangeAddr*/)
    {
        status = RegSetValueEx( hRuleKey,
                          _T("RemoteAddr"),
                          0,
                          REG_DWORD,
                          (BYTE *)&rule->RemoteAddr,
                          sizeof( UINT32) );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("д��IP����ʧ��(009)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );hRuleKey = NULL;
            CloseHandle( hKey );hKey = NULL;
            return false;
        }

        status = RegSetValueEx( hRuleKey,
                          _T("RemoteAddr2"),
                          0,
                          REG_DWORD,
                          (BYTE *)&rule->RemoteAddr2,
                          sizeof( UINT32) );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("д��IP����ʧ��(010)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );hRuleKey = NULL;
            CloseHandle( hKey );hKey = NULL;
            return false;
        }
    }

    //д��Դ�˿�
    if( rule->rule.Bits.LocalPortType == 1/*UniqueAddr*/)
    {
        status = RegSetValueEx( hRuleKey,
                          _T("LocalPort"),
                          0,
                          REG_DWORD,
                          (BYTE *)&rule->LocalPort,
                          sizeof( UINT32) );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("д��IP����ʧ��(011)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );hRuleKey = NULL;
            CloseHandle( hKey );hKey = NULL;
            return false;
        }
    }
    else if( rule->rule.Bits.LocalPortType == 2/*RangeAddr*/)
    {
        status = RegSetValueEx( hRuleKey,
                          _T("LocalPort"),
                          0,
                          REG_DWORD,
                          (BYTE *)&rule->LocalPort,
                          sizeof( UINT32) );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("д��IP����ʧ��(012)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );hRuleKey = NULL;
            CloseHandle( hKey );hKey = NULL;
            return false;
        }

        status = RegSetValueEx( hRuleKey,
                          _T("LocalPort2"),
                          0,
                          REG_DWORD,
                          (BYTE *)&rule->LocalPort2,
                          sizeof( UINT32) );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("д��IP����ʧ��(013)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );hRuleKey = NULL;
            CloseHandle( hKey );hKey = NULL;
            return false;
        }
    }

    //д��Ŀ��˿�
    if( rule->rule.Bits.RemotePortType == 1/*UniqueAddr*/)
    {
        status = RegSetValueEx( hRuleKey,
                          _T("RemotePort"),
                          0,
                          REG_DWORD,
                          (BYTE *)&rule->RemotePort,
                          sizeof( UINT32) );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("д��IP����ʧ��(014)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );hRuleKey = NULL;
            CloseHandle( hKey );hKey = NULL;
            return false;
        }
    }
    else if( rule->rule.Bits.RemotePortType == 2/*RangeAddr*/)
    {
        status = RegSetValueEx( hRuleKey,
                          _T("RemotePort"),
                          0,
                          REG_DWORD,
                          (BYTE *)&rule->RemotePort,
                          sizeof( UINT32) );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("д��IP����ʧ��(015)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );hRuleKey = NULL;
            CloseHandle( hKey );hKey = NULL;
            return false;
        }

        status = RegSetValueEx( hRuleKey,
                          _T("RemotePort2"),
                          0,
                          REG_DWORD,
                          (BYTE *)&rule->RemotePort2,
                          sizeof( UINT32) );
        if( status != ERROR_SUCCESS)
        {
            AfxMessageBox(_T("д��IP����ʧ��(016)"),MB_OK | MB_ICONWARNING );
            CloseHandle( hRuleKey );hRuleKey = NULL;
            CloseHandle( hKey );hKey = NULL;
            return false;
        }
    }

    CloseHandle( hRuleKey );hRuleKey = NULL;
    CloseHandle( hKey );hKey = NULL;
    return true;
}

bool CIpSettingDlg::RegDeleteIpRule(DWORD crcRuleName )
{
    LSTATUS    status = ERROR_SUCCESS;
    HKEY       hKey = NULL,hRuleKey = NULL;
    TCHAR      buffer[256]={0};


    status = RegCreateKey( (static_cast<CCactiWallApp*>(AfxGetApp()))->m_hKey,
        _T("iprules"),
        &hKey );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("��ȡIP����ʧ��(001)"),MB_OK | MB_ICONWARNING );
        return false;
    }

    
    RtlZeroMemory( buffer,sizeof( buffer ));
    _stprintf_s( (LPTSTR)buffer,255,_T("%x"),crcRuleName );
    
    status = RegDeleteKey( hKey,buffer );
    if( status != ERROR_SUCCESS )
    {
        CloseHandle( hKey );
        hKey = NULL;
        return false;

    }

    CloseHandle( hKey );hKey = NULL;
    return true;
}

BEGIN_MESSAGE_MAP(CIpSettingDlg, CDialog)
    ON_NOTIFY(LVN_ITEMACTIVATE, IDC_RULE_LIST, &CIpSettingDlg::OnLvnItemActivateRuleList)
    ON_BN_CLICKED(IDC_BUTTON_ADDRULE, &CIpSettingDlg::OnBnClickedButtonAddrule)
    ON_BN_CLICKED(IDC_BUTTON_EDITRULE, &CIpSettingDlg::OnBnClickedButtonEditrule)
    ON_BN_CLICKED(IDC_BUTTON_DELRULE, &CIpSettingDlg::OnBnClickedButtonDelrule)
    ON_BN_CLICKED(IDC_RADIO_ALLOW, &CIpSettingDlg::OnBnClickedRadioAllow)
    ON_BN_CLICKED(IDC_RADIO_DENY, &CIpSettingDlg::OnBnClickedRadioAllow)
    ON_BN_CLICKED(IDOK, &CIpSettingDlg::OnBnClickedOk)
    ON_NOTIFY(NM_CLICK, IDC_RULE_LIST, &CIpSettingDlg::OnNMClickRuleList)
END_MESSAGE_MAP()


// CIpSettingDlg ��Ϣ�������

void CIpSettingDlg::OnLvnItemActivateRuleList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    OnBnClickedButtonEditrule();

    *pResult = 0;
}


void CIpSettingDlg::OnBnClickedButtonDelrule()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    if( !RegDeleteIpRule( m_RuleList.GetItemData(m_RuleList.GetSelectionMark())))
        AfxMessageBox(_T("ɾ������ʧ�ܣ�"));
    else
    {
        UpdateRuleList();
    }
}

extern void ShowError(DWORD code);

void CIpSettingDlg::OnBnClickedRadioAllow()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CButton *pBtn = NULL;
    HKEY    hKey = NULL;
    BOOL    bChecked = FALSE;
    LSTATUS status = ERROR_SUCCESS;
    
    status = RegCreateKey( (static_cast<CCactiWallApp*>(AfxGetApp()))->m_hKey,
        _T("iprules"),
        &hKey );
    if( status != ERROR_SUCCESS)
    {
        AfxMessageBox(_T("��ȡ���̹���ʧ��(005)"),MB_OK | MB_ICONWARNING );
        return;
    }

    pBtn = (CButton *)GetDlgItem( IDC_RADIO_ALLOW );
    bChecked = pBtn->GetCheck();
    status = RegSetValueEx( hKey,
                          _T("other_access"),
                          0,
                          REG_DWORD,
                          (BYTE*)&bChecked,
                          sizeof(BOOL));
    if( status != ERROR_SUCCESS)
    {
        ShowError( status );
    }
}


void CIpSettingDlg::OnNMClickRuleList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CButton *pBtn_edit = (CButton*)GetDlgItem( IDC_BUTTON_EDITRULE );
    CButton *pBtn_delete = (CButton*)GetDlgItem( IDC_BUTTON_DELRULE);
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    if( pNMItemActivate->iItem >= 0 )
    {
        pBtn_edit->EnableWindow( TRUE );
        pBtn_delete->EnableWindow( TRUE );
    }
    else
    {
        pBtn_edit->EnableWindow( FALSE );
        pBtn_delete->EnableWindow( FALSE );
    }
    *pResult = 0;
}

void CIpSettingDlg::OnBnClickedOk()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    OnOK();
}

void CIpSettingDlg::OnBnClickedButtonAddrule()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    IP_RULES_ELEM   rule = {0};
    CIpRuleAddDlg   dlg;
    dlg.m_Title = _T("���IP����");
    if( IDOK == dlg.DoModal())
    {
        //����¹���
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
            AfxMessageBox(_T("���IP�������!"));

    }
    else
    {
    }
}

void CIpSettingDlg::OnBnClickedButtonEditrule()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CIpRuleAddDlg   dlg;
    CString        s = _T("");
    DWORD          a1=0,a2=0,a3=0,a4=0;//����ת��IP��ַ�Ͷ˿�
    DWORD          a5=0,a6=0,a7=0,a8=0; 
    dlg.m_Title = _T("�༭IP����");

    dlg.m_RuleName = m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),0 );

    if( m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),1 )==_T("����"))
        dlg.m_bAllow = TRUE;
    else
        dlg.m_bAllow = FALSE;

    if( m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),2 )==_T("����"))
        dlg.m_Direction = 0;
    else if( m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),2 )==_T("����"))
        dlg.m_Direction = 1;
    else if( m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),2 )==_T("����"))
        dlg.m_Direction = 2;
    
    s = m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),3 );
    if( s == _T("�����ַ") )
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
    if( s == _T("�����ַ") )
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
        dlg.m_ProtocolType = IPPROTO_ICMP;
    else if( s == _T("IGMP"))
        dlg.m_ProtocolType = IPPROTO_IGMP;
    else if( s == _T("RDP"))
        dlg.m_ProtocolType = IPPROTO_RDP;
    else if( s == _T("AH"))
        dlg.m_ProtocolType = IPPROTO_AH;
    else
    {
        _stscanf_s( s.GetBuffer(),_T("%d"),&a1 );
        dlg.m_ProtocolType = (BYTE)a1;
    }
    
    if( dlg.m_ProtocolType == IPPROTO_TCP ||
        dlg.m_ProtocolType == IPPROTO_UDP )
    {
        //Դ�˿�
        s = m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),6 );
        if( s == _T("--"))
            dlg.m_LocalPortType = 0;
        else if( _tcsstr( s.GetBuffer(),_T("-")) == NULL )
        {
            dlg.m_LocalPortType = 1;
            _stscanf_s( s.GetBuffer(),_T("%d"),&a1 );
            dlg.m_LocalPort1 = a1;
        }
        else
        {
            dlg.m_LocalPortType = 2;
            _stscanf_s( s.GetBuffer(),_T("%d-%d"),&a1,&a2 );
            dlg.m_LocalPort1 = a1;
            dlg.m_LocalPort2 = a2;
        }

        //Ŀ��˿�
        s = m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),7 );
        if( s == _T("--"))
            dlg.m_RemotePortType = 0;
        else if( _tcsstr( s.GetBuffer(),_T("-")) == NULL )
        {
            dlg.m_RemotePortType = 1;
            _stscanf_s( s.GetBuffer(),_T("%d"),&a1 );
            dlg.m_RemotePort1 = a1;
        }
        else
        {
            dlg.m_RemotePortType = 2;
            _stscanf_s( s.GetBuffer(),_T("%d-%d"),&a1,&a2 );
            dlg.m_RemotePort1 = a1;
            dlg.m_RemotePort2 = a2;
        }
    }//end if tcp or udp

    if( dlg.m_ProtocolType == IPPROTO_ICMP )
    {
        s = m_RuleList.GetItemText( m_RuleList.GetSelectionMark(),8 );
        _stscanf_s( s.GetBuffer(),_T("%d,%d"),&a1,&a2 );
        
        //�б���ʾ-1��������Э�飬��Ӧ����Ϊ31
        if( a1 == -1 )a1 = 31;
        if( a2 == -1 )a2 = 31;
        dlg.m_IcmpType = (BYTE)a1;
        dlg.m_IcmpCode = (BYTE)a2;
    }

    if( IDOK == dlg.DoModal())
    {
        IP_RULES_ELEM rule = {0};

        //ɾ���ɹ���crcName���б������л�ȡ
        RegDeleteIpRule( m_RuleList.GetItemData(m_RuleList.GetSelectionMark()));
        
        //��ʼ��һ��IP_RULES_ELEM�ṹ������¹���
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
            AfxMessageBox(_T("���IP�������!"));
    }
    else
    {
    }
}