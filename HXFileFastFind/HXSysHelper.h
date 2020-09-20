#pragma once
// 定义一些与Windows操作系统相关的函数  方便直接使用
#include <Windows.h>
extern const int g_nWorkThreadMax;
// 获得默认线程大小 
DWORD HXGetDefaultWorkerThreadCout();

// 获得默认buffer大小
DWORD GetDefaultBufferSize();

// 获得CPU处理器个数
DWORD HXGetSysNumberOfProcessors();


// 获取页面大小
DWORD HXGetSysPageSize();