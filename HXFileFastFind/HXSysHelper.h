#pragma once
// ����һЩ��Windows����ϵͳ��صĺ���  ����ֱ��ʹ��
#include <Windows.h>
extern const int g_nWorkThreadMax;
// ���Ĭ���̴߳�С 
DWORD HXGetDefaultWorkerThreadCout();

// ���Ĭ��buffer��С
DWORD GetDefaultBufferSize();

// ���CPU����������
DWORD HXGetSysNumberOfProcessors();


// ��ȡҳ���С
DWORD HXGetSysPageSize();