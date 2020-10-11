#pragma once
#include <Windows.h>

// TODO:后续需优化 HANDLE有的当NULL为无效值，有的当INVALID_HANDLE_VALUE为无效值
// ATL中对此封装并不完善 可参考boost
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
			// 为了防止不阻塞其它窗口接受消息
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