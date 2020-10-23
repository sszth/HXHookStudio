#pragma once
//地址族。
enum HXSOCKET_AF
{
	AF_UNSPEC = 0,	//未定义
	AF_INET	= 1,	//IPV4

};

class IHXSocket
{
	HXNOCOPY(IHXSocket);
public:
	IHXSocket();
	~IHXSocket();

	virtual bool Socket() = 0;
};
HXDECLARE_PTR(IHXSocket);

