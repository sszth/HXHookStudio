#include "stdafx.h"
#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")	
#include "HXWindowsSocketCommon.h"

class HXWindowsSocket : public IHXSocket
{
public:

	virtual bool Socket()
	{
		m_socket = ::socket();
	}
private:
	SOCKET m_socket;
};

HXWindowsSocketCommon::~HXWindowsSocketCommon()
{
	WSACleanup();
}

int HXWindowsSocketCommon::Initialize()
{
	int nRet = 0;
	std::call_once(m_flag, [&nRet]() {
		WORD wVersion = MAKEWORD(2, 2);
		WSAData wsaData = { 0 };
		if (::WSAStartup(wVersion, &wsaData) != 0)
		{
			nRet = -1;
		}
		if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
		{
			nRet = 1;
		}
	});
	return nRet;
}

IHXSocket_Ptr HXWindowsSocketCommon::GetSocket()
{
	IHXSocket_Ptr ptr = std::make_shared<HXWindowsSocket>();
	::socket
	return ptr;
}

