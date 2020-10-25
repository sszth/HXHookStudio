#include "stdafx.h"
#include <Windows.h>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")	
#include "HXWindowsSocketCommon.h"


class HXWindowsSocket : public IHXSocket
{
public:
	HXWindowsSocket() :m_socket(INVALID_SOCKET) {}

	int WinErrorToHXError(int nWinError)
	{
		int nRet = HX_FALSE;
		switch (nWinError)
		{
		case WSAENOBUFS:
			nRet = HX_FALSE_NOBUFFSIZE;
		default:
			break;
		}
		return nRet;
	}
	int WinErrorToHXError()
	{
		int nWinError = WSAGetLastError();
		int nRet = HX_FALSE;
		switch (nWinError)
		{
		case WSAENOBUFS:
			nRet = HX_FALSE_NOBUFFSIZE;
		default:
			break;
		}
		return nRet;
	}
public:
	virtual int Socket(HXSOCKET_AF af, HXSOCKET_TYPE type, HXSOCKET_PROTOCOL protocol) override
	{
		m_socket = ::socket(HXSOCKET_AF_TO_WINAF(af), HXSOCKET_TYPE_TO_WINTYPE(type), HXSOCKET_PROTOCOL_TO_WINPROTOCOL(protocol));
		if (INVALID_SOCKET == m_socket)
		{
			return WinErrorToHXError();
		}
		return HX_OK;
	}
	virtual int Send(IN char* pByte, IN int nByte, IN int nFlags) override
	{
		if (SOCKET_ERROR == ::send(m_socket, pByte, nByte, nFlags))
		{
			// TODO:WSAENOBUFS需特殊处理
			return WinErrorToHXError();
		}
		else
		{
			return HX_OK;
		}
	}

	// 异常
	// WSANOTINITIALISED	A successful WSAStartup call must occur before using this function.
	// WSAENETDOWN			The network subsystem has failed. 
	// WSAEACCES			The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST socket option to enable use of the broadcast address. 
	// WSAEINTR				A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall. 
	// WSAEINPROGRESS		A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. 
	// WSAEFAULT			The buf parameter is not completely contained in a valid part of the user address space.
	virtual int Connect(IN const HXSocketAddr &hxAddr) override
	{
		SOCKADDR_IN winAddr = {};
		if (!HXSocketAddrToWinSocketAddr(hxAddr, winAddr))
		{
			return WinErrorToHXError();
		}

		if (SOCKET_ERROR != ::connect(m_socket, (SOCKADDR*)&winAddr, sizeof(winAddr)))
		{
			return HX_OK;
		}
		else
		{
			::closesocket(m_socket);
			return WinErrorToHXError();
		}
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

	bool HXSocketAddrToWinSocketAddr(IN const HXSocketAddr& hxAddr, OUT SOCKADDR_IN& winAddr)
	{
		winAddr.sin_family = HXSOCKET_AF_TO_WINAF(hxAddr.m_af);
		std::string strIP;
		if (false == HXCommonAlgorithm::WStringToString(hxAddr.m_strIP, strIP))
		{
			return false;
		}
		winAddr.sin_addr.s_addr = inet_addr(strIP.c_str());
		winAddr.sin_port = hxAddr.m_shortPort;
		return true;
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

