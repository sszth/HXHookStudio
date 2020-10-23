#include "stdafx.h"
#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")	
#include "HXWindowsSocketCommon.h"

class HXWindowsSocket : public IHXSocket
{
public:
	HXWindowsSocket() :m_socket(INVALID_SOCKET) {}
public:
	virtual bool Socket(HXSOCKET_AF af, HXSOCKET_TYPE type, HXSOCKET_PROTOCOL protocol) override
	{
		m_socket = ::socket(HXSOCKET_AF_TO_WINAF(af), HXSOCKET_TYPE_TO_WINTYPE(type), HXSOCKET_PROTOCOL_TO_WINPROTOCOL(protocol));
		if (INVALID_SOCKET == m_socket)
		{
			return false;
		}
		return true;
	}

	virtual bool Connect() override
	{
		::connect(m_socket,)
	}

	int HXSOCKET_AF_TO_WINAF(HXSOCKET_AF af)
	{
		int nWinAF = 0;
		switch (af)
		{
		case HXSOCKET_AF_UNSPEC:
			nWinAF = AF_UNSPEC;
			break;
		case HXSOCKET_AF_INET:
			nWinAF = AF_INET;
			break;
		default:
			break;
		}
		return nWinAF;
	}

	int HXSOCKET_TYPE_TO_WINTYPE(HXSOCKET_TYPE type)
	{
		int nWinType = 0;
		switch (type)
		{
		case HXSOCKET_STREAM:
			nWinType = SOCK_STREAM;
			break;
		default:
			break;
		}
		return nWinType;
	}

	int HXSOCKET_PROTOCOL_TO_WINPROTOCOL(HXSOCKET_PROTOCOL protocol)
	{
		int nWinProtocol = 0;
		switch (protocol)
		{
		case PROTOCOL_ICMP:
			nWinProtocol = IPPROTO_ICMP;
			break;
		case PROTOCOL_IGMP:
			nWinProtocol = IPPROTO_IGMP;
			break;
		case PROTOCOL_RFCOMM:
			//nWinProtocol = BTHPROTO_RFCOMM;
			break;
		case PROTOCOL_TCP:
			nWinProtocol = IPPROTO_TCP;
			break;
		default:
			break;
		}
		return nWinProtocol;
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

	return ptr;
}

