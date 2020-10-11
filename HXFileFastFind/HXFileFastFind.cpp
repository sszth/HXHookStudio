// HXFileFastFind.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
//#include <atlstr.h>
//#include <afxtempl.h>
#include <list>
#include <vector>
#include <thread>
#include <afxwin.h>
#include <Windows.h>
#include <atlutil.h>
#include "HXThreadPool.h"


int main(int argc, char* argv[])
{
    CString strDir = _T("G:\\20_WindowsSystem\\01——简单TCP_UDP\\*");
    //CString strDir = _T("G:\\*");
    HXThreadPool::Initstance()->Start(strDir);
    HXThreadPool::Initstance()->ShutDown(INFINITE);

    return 0;
}