
// CactiWall.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
    #error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"        // ������


// CCactiWallApp:
// �йش����ʵ�֣������ CactiWall.cpp
//

class CCactiWallApp : public CWinAppEx
{
public:
    CCactiWallApp();
    ~CCactiWallApp();

// ��д
    public:
    virtual BOOL InitInstance();

// ʵ��

    DECLARE_MESSAGE_MAP()

//ȫ�ֱ�������
public:
    HANDLE  m_hDevice;
    HKEY    m_hKey;
};

extern CCactiWallApp theApp;

