#pragma once
#include <Windows.h>

// TODO:�������Ż� HANDLE�еĵ�NULLΪ��Чֵ���еĵ�INVALID_HANDLE_VALUEΪ��Чֵ
// ATL�жԴ˷�װ�������� �ɲο�boost
void CHXWaitForObject(HANDLE hHandle)
{
	if (INVALID_HANDLE_VALUE == hHandle)
	{
		return;
	}

	MSG msg;
	int nTaskThreads = 1;
	while (nTaskThreads>0)
	{
		int nRet = (int)MsgWaitForMultipleObjects(nTaskThreads, &hHandle, FALSE, INFINITE, QS_ALLINPUT);
		if (nRet == WAIT_OBJECT_0 + nTaskThreads)
		{
			// Ϊ�˷�ֹ�������������ڽ�����Ϣ
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE ))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else if (nRet >= WAIT_OBJECT_0 && nRet < (int)(WAIT_OBJECT_0+nTaskThreads))
		{
			hHandle = NULL;
			nTaskThreads--;
		}
		else
		{
			nRet = GetLastError();
			break;
		}
	}
}