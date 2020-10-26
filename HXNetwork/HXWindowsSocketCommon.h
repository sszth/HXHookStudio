#pragma once
class HXWindowsSocketCommon : public HXSingleton<HXWindowsSocketCommon>
{
public:
	HXWindowsSocketCommon(){};
	~HXWindowsSocketCommon();
public:
	int Initialize();
	IHXSocket_Ptr GetSocket();

	static int WinErrorToHXError(int nWinError)
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
	static int WinErrorToHXError()
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
	static bool HXSocketAddrToWinSocketAddr(IN const HXSocketAddr& hxAddr, OUT SOCKADDR_IN& winAddr);


	static int HXSOCKET_AF_TO_WINAF(HXSOCKET_AF af);

	static int HXSOCKET_TYPE_TO_WINTYPE(HXSOCKET_TYPE type);

	static int HXSOCKET_PROTOCOL_TO_WINPROTOCOL(HXSOCKET_PROTOCOL protocol);
private:
private:
	static std::once_flag	m_flagInitializeCommon;
	static std::once_flag	m_flagSystemInfo;
	SYSTEM_INFO		m_systemInfo;
};
class IHXClinet
{
public:

	virtual int Initialize(const std::wstring &strIP = L"127.0.0.1", int nPort = 26666) = 0;
	virtual int Send(IN char* sz, IN int nSize, IN int nFlags = 0) = 0;
	virtual int Recv(OUT char* sz, IN int nSize, IN int nFlags) = 0;
};
HXDECLARE_CLASS_INTERNAL_PTR(IHXClinet)


class HXWinTcpClient : public IHXClinet
{
public:
	HXWinTcpClient();
	~HXWinTcpClient();

	virtual int Initialize(const std::wstring &strIP = L"127.0.0.1", int nPort = 26666) override;
	virtual int Send(IN char* sz, IN int nSize, IN int nFlags = 0) override;
	virtual int Recv(OUT char* sz, IN int nSize, IN int nFlags) override;
private:
	IHXSocket_Ptr	m_socketPtr;
	HXSocketAddr	m_socketAddr;
};

class IHXServer
{
public:
	virtual int Initialize(const std::wstring &strIP = L"127.0.0.1", int nPort = 26668) = 0;
	virtual int Listen() = 0;
	virtual int Send(IN char* sz, IN int nSize, IN int nFlags = 0) = 0;
private:
};
HXDECLARE_CLASS_INTERNAL_PTR(IHXServer)

class HXWinTcpServer : public IHXServer
{
public:
	HXWinTcpServer();
	~HXWinTcpServer();

	virtual int Initialize(const std::wstring &strIP = L"127.0.0.1", int nPort = 26666) override;
	virtual int Send(IN char* sz, IN int nSize, IN int nFlags = 0) override;
private:
	IHXSocket_Ptr	m_socketPtr;

};