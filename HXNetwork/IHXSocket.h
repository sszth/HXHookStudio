#pragma once
// Ŀǰֻ֧��tcp
// ��ַ�塣
enum HXSOCKET_AF
{
	HXSOCKET_AF_UNSPEC	= 0,	//δ����
	HXSOCKET_AF_INET	= 1,	//IPV4
	//HXSOCKET_AF_IPX		= 2,	//
	//AF_APPLETALK
	//AF_NETBIOS
	//AF_INET6
	//AF_IRDA
	//AF_BTH
};
// ����
enum HXSOCKET_TYPE
{
	HXSOCKET_STREAM = 0,//tcp
	//SOCK_DGRAM
	//SOCK_RAW
	//SOCK_RDM
	//SOCK_SEQPACKET
};
// Э��
enum HXSOCKET_PROTOCOL
{
	PROTOCOL_ICMP = 0,
	PROTOCOL_IGMP = 1,
	PROTOCOL_RFCOMM = 2,
	PROTOCOL_TCP = 3,
};
class IHXSocket
{
	HXNOCOPY(IHXSocket);
public:
	IHXSocket();
	~IHXSocket();

	virtual bool Socket(HXSOCKET_AF af, HXSOCKET_TYPE type, HXSOCKET_PROTOCOL protocol) = 0;
	virtual bool Connect() = 0;
};
HXDECLARE_PTR(IHXSocket);

