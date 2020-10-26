#include "stdafx.h"
#include <iostream>
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
		std::wclog << L"Socket HXSOCKET_AF(" << af << L") HXSOCKET_TYPE(" << type << L") HXSOCKET_PROTOCOL(" << protocol << L") Begin" << std::endl;
		m_socket = ::socket(HXSOCKET_AF_TO_WINAF(af), HXSOCKET_TYPE_TO_WINTYPE(type), HXSOCKET_PROTOCOL_TO_WINPROTOCOL(protocol));
		if (INVALID_SOCKET == m_socket)
		{
			return WinErrorToHXError();
		}
		std::wclog << L"Socket HXSOCKET_AF(" << af << L") HXSOCKET_TYPE(" << type << L") HXSOCKET_PROTOCOL(" << protocol << L") Success" << std::endl;
		return HX_OK;
	}
	virtual int Send(IN char* pByte, IN int nByte, IN int nFlags) override
	{
		std::wclog << L"Send Size(" << nByte << L") Flags(" << nFlags << L") Begin" << std::endl;
		if (SOCKET_ERROR == ::send(m_socket, pByte, nByte, nFlags))
		{
			// TODO:WSAENOBUFS需特殊处理
			return WinErrorToHXError();
		}
		std::wclog << L"Send Size(" << nByte << L") Flags(" << nFlags << L") Success" << std::endl;

		return HX_OK;
	}
	virtual int Recv(OUT char* szChar, OUT int nSize, IN int nFlags) override
	{
		std::wclog << L"Recv Size(" << nSize << L") Flags(" << nFlags << L") Begin" << std::endl;
		if (SOCKET_ERROR == ::recv(m_socket, szChar, nSize, nFlags))
		{
			return WinErrorToHXError();
		}
		std::wclog << L"Recv Size(" << nSize << L") Flags(" << nFlags << L") Success" << std::endl;

		return HX_OK;
	}
	virtual int Bind(IN const HXSocketAddr &hxAddr) override
	{
		std::wclog << L"Bind IP(" << hxAddr.m_strIP.c_str() << L") HXSOCKET_AF(" << hxAddr.m_af << L") Port(" << hxAddr.m_shortPort << L") Begin" << std::endl;
		m_hxsocketAddr = hxAddr;
		SOCKADDR_IN winAddr = {};
		if (!HXSocketAddrToWinSocketAddr(m_hxsocketAddr, winAddr))
		{
			return WinErrorToHXError();
		}

		if (SOCKET_ERROR == ::bind(m_socket, (SOCKADDR*)&winAddr, sizeof(winAddr))))
		{
			return WinErrorToHXError();
		}
		std::wclog << L"Bind IP(" << m_hxsocketAddr.m_strIP.c_str() << L") HXSOCKET_AF(" << m_hxsocketAddr.m_af << L") Port(" << m_hxsocketAddr.m_shortPort << L") Success" << std::endl;

		return HX_OK;
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
		std::wclog << L"Connect IP(" << hxAddr.m_strIP.c_str() << L") HXSOCKET_AF(" << hxAddr.m_af << L") Port(" << hxAddr.m_shortPort << L") Begin" << std::endl;
		SOCKADDR_IN winAddr = {};
		if (!HXSocketAddrToWinSocketAddr(hxAddr, winAddr))
		{
			return WinErrorToHXError();
		}

		if (SOCKET_ERROR == ::connect(m_socket, (SOCKADDR*)&winAddr, sizeof(winAddr)))
		{
			::closesocket(m_socket);
			return WinErrorToHXError();
		}
		std::wclog << L"Connect IP(" << hxAddr.m_strIP.c_str() << L") HXSOCKET_AF(" << hxAddr.m_af << L") Port(" << hxAddr.m_shortPort << L") Success" << std::endl;
		return HX_OK;
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
	HXSocketAddr m_hxsocketAddr;
};

HXWindowsSocketCommon::~HXWindowsSocketCommon()
{
	WSACleanup();
}

int HXWindowsSocketCommon::Initialize()
{
	int nRet = 0;
	std::call_once(m_flagInitializeCommon, [&nRet]() {
		WORD wVersion = MAKEWORD(2, 2);
		WSAData wsaData = { 0 };
		if (::WSAStartup(wVersion, &wsaData) != 0)
		{
			nRet = -1;
		}
		else if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) 
		{
			nRet = 1;
		}
		else
		{
			nRet = 0;
		}
	});
	return nRet;
}

IHXSocket_Ptr HXWindowsSocketCommon::GetSocket()
{
	IHXSocket_Ptr ptr = std::make_shared<HXWindowsSocket>();

	return ptr;
}

HXWinTcpClient::HXWinTcpClient()
{
}

HXWinTcpClient::~HXWinTcpClient()
{
}

int HXWinTcpClient::Initialize(const std::wstring & strIP, int nPort)
{
	m_socket =  HXWindowsSocketCommon::Get()->GetSocket();
	if (HX_OK == m_socket->Socket(HXSOCKET_AF_INET, HXSOCKET_TYPE_STREAM, HXSOCKET_PROTOCOL_TCP))
	{
		m_socketAddr.m_af = HXSOCKET_AF_INET;
		m_socketAddr.m_shortPort = nPort;
		m_socketAddr.m_strIP = strIP;
		if (HX_OK == m_socket->Connect(m_socketAddr))
		{
			return HX_OK;
		}
	}

	return HX_FALSE;
}

int HXWinTcpClient::Send(IN char * sz, IN int nSize, IN int nFlags)
{	
	m_socket->Send(sz, nSize, nFlags);
	return 0;
}

int HXWinTcpClient::Recv()
{
	/*::recv(m_);*/
	return 0;
}

