// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

// 客户端.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <iostream>
using namespace std;

int main()
{
    HANDLE hPipe = NULL;
    HANDLE hEvent = NULL;
    DWORD  dwReadLen = 0;
    DWORD  dwWriteLen = 0;
    TCHAR senbuf[] = L"This is client!";
    char rebuf[100];

    //1. 连接命名管道
    if (!WaitNamedPipe(L"\\\\.\\pipe\\Communication", NMPWAIT_WAIT_FOREVER))
    {
        cout << "当前没有可利用的命名管道实例！" << endl;
        system("pause");
        return -1;
    }

    //2. 打开命名管道
    hPipe = CreateFile(L"\\\\.\\pipe\\Communication", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hPipe)
    {
        cout << "打开命名管道失败！" << endl;
        hPipe = NULL;
        system("pause");
        return -1;
    }

    //3. 读写管道数据
    //3.1 写入数据
    if (!WriteFile(hPipe, senbuf, (lstrlenW(senbuf) + 1)*2, &dwWriteLen, NULL))
    {
        cout << "写入数据失败！" << endl;
        system("pause");
        return -1;
    }

    //3.2 读取数据
    if (!ReadFile(hPipe, rebuf, 100, &dwReadLen, NULL))
    {
        cout << "读取数据失败！" << endl;
        system("pause");
        return -1;
    }
    cout << rebuf << endl;

    system("pause");
    return 0;
}
// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
