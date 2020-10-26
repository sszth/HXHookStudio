
// CactiWall.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
    #error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"        // 主符号


// CCactiWallApp:
// 有关此类的实现，请参阅 CactiWall.cpp
//

class CCactiWallApp : public CWinAppEx
{
public:
    CCactiWallApp();
    ~CCactiWallApp();

// 重写
    public:
    virtual BOOL InitInstance();

// 实现

    DECLARE_MESSAGE_MAP()

//全局变量定义
public:
    HANDLE  m_hDevice;
    HKEY    m_hKey;
};

extern CCactiWallApp theApp;

