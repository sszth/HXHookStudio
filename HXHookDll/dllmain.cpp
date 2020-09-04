// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
DWORD oldProtect;
BYTE  JmpBtye[5];
BYTE  OldByte[5];
void* OpenProcessaddr;
bool H1_OpenProcess();
void UnHook();
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_THREAD_ATTACH:
    case DLL_PROCESS_ATTACH:
        H1_OpenProcess();
        break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        UnHook();
        break;
    }
    return TRUE;
}

int MyOpenProcess(int x)
{
    return  x * 10;
}

void* F1_OpenProcess()
{
    //寻找到OpenProcess的地址
    void* addr = 0;
    //加载kernel32.dll
    HMODULE hModule = LoadLibrary(TEXT("HXAdd.dll"));
    if (!hModule)
    {
        return nullptr;
    }
    //获取OpenProcess的地址
    addr = (void*)GetProcAddress(hModule, "HXAddFunc");
    return addr;
}


void* F2_OpenProcess()
{
    return (void*)OpenProcess;
}


bool H1_OpenProcess()
{
    //1.开始寻找地址
    void* addr = F1_OpenProcess();
    OpenProcessaddr = addr;
    //判断是否寻找成功
    if (addr == 0)
    {
        MessageBox(NULL, TEXT("寻找地址失败"), NULL, 0);
        return false;
    }
    //2.进行Hook

    /*
    一般代码段是不可写的,我们需要把其改为可读可写.
    */
    VirtualProtect((void*)addr, 5, PAGE_EXECUTE_READWRITE, &oldProtect);

    //修改前面的5个字节为jmp 跳转到我们的代码.
    //内联Hook 跳转偏移计算方式:跳转偏移=目标地址-指令地址-5
    //jmp 的OpCode 为:0xE9

    JmpBtye[0] = 0xE9;
    *(DWORD*)&JmpBtye[1] = (DWORD)((long long)MyOpenProcess - (long long)addr - 5);
    //保存原先字节
    memcpy(OldByte, (void*)addr, 5);
    //替换原先字节
    memcpy((void*)addr, JmpBtye, 5);
    return true;
}

void UnHook()
{
    //恢复原先字节
    memcpy((void*)OpenProcessaddr, OldByte, 5);
    //恢复属性
    DWORD p;
    VirtualProtect((void*)OpenProcessaddr, 5, oldProtect, &p);
}
