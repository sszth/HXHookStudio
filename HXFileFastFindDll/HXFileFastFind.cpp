// HXFileFastFind.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "stdafx.h"
#include "HXThreadPool.h"
#include "HXFileFastFind.h"


void HXFastFindInitialize()
{
	HXThreadPool::Initstance()->Initialize();
}

void HXFastFindStart(std::wstring strDir)
{
	CString strDirTmp = strDir.data();
	HXThreadPool::Initstance()->Start(strDirTmp);
}

void HXFastFindShutDown(DWORD dwMaxWait)
{
	HXThreadPool::Initstance()->ShutDown(dwMaxWait);
}

void HXFastFindEnd()
{
	HXThreadPool::Initstance()->FindEnd();
}
