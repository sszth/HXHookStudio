#pragma once
class HXWindowsSocketCommon : public HXSingleton<HXWindowsSocketCommon>
{
public:
	HXWindowsSocketCommon(){};
	~HXWindowsSocketCommon();
public:
	int Initialize();
	IHXSocket_Ptr GetSocket();

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
};
HXDECLARE_CLASS_INTERNAL_PTR(IHXClinet)


class HXWinTcpClient : public IHXClinet
{
public:
	HXWinTcpClient();
	~HXWinTcpClient();

	virtual int Initialize(const std::wstring &strIP = L"127.0.0.1", int nPort = 26666) override;
	virtual int Send(IN char* sz, IN int nSize, IN int nFlags = 0) override;
	virtual int Recv() override;
private:
	IHXSocket_Ptr	m_socket;
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

};