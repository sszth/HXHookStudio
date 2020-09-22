#include "stdafx.h"
#include "HXFileFastFind.h"
#include "HXThreadPool.h"

void HXStartFileFind(std::wstring strDir)
{
	CString strD = strDir.data();
	HXThreadPool::Initstance()->Start(strD);
}

void HXShutDownFileFind(int dwMaxWait)
{
	HXThreadPool::Initstance()->ShutDown(dwMaxWait);
}
