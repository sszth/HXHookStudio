// HXInjectionTaskManage.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <windows.h>
#include <iostream>
//获取进程句柄
HANDLE GetThePidOfTargetProcess(HWND hwnd)
{
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	HANDLE hProcee = ::OpenProcess(PROCESS_ALL_ACCESS | PROCESS_CREATE_THREAD, 0, pid);
	return hProcee;
}
//提升权限
void Up()
{
	HANDLE hToken;
	LUID luid;
	TOKEN_PRIVILEGES tp;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
}

//进程注入

BOOL DoInjection(char *DllPath, HANDLE hProcess)
{
	DWORD BufSize = strlen(DllPath) + 1;
	LPVOID AllocAddr = VirtualAllocEx(hProcess, NULL, BufSize, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, AllocAddr, DllPath, BufSize, NULL);
	PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryA");

	HANDLE hRemoteThread;
	hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, pfnStartAddr, AllocAddr, 0, NULL);
	if (hRemoteThread)
	{
		MessageBox(NULL, TEXT("注入成功"), TEXT("提示"), MB_OK);
		return true;
	}
	else
	{
		MessageBox(NULL, TEXT("注入失败"), TEXT("提示"), MB_OK);
		return false;
	}
}
int main()
{
	//这里填写窗口标题
	HWND hwnd = FindWindowExA(NULL, NULL, NULL, "任务管理器");
	Up();
	HANDLE hP = GetThePidOfTargetProcess(hwnd);
	//开始注入
	//这里填写Dll路径
	DoInjection("E:\\studio\\VS2017\\F2H1.MessageBox\\x64\\Release\\F2H1.MessageBox.dll", hP);
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
