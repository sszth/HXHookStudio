#pragma once


class IHXSocket
{
	HXNOCOPY(IHXSocket);
public:
	IHXSocket();
	~IHXSocket();

	virtual int Socket(IN HXSOCKET_AF af, IN HXSOCKET_TYPE type, IN HXSOCKET_PROTOCOL protocol) = 0;
	virtual int Connect(IN const HXSocketAddr& hxAddr) = 0;

	//************************************
	// Method:    Send
	// FullName:  HXWindowsSocket::Send
	// Access:    virtual public 
	// Returns:   int
	// Qualifier: must do HX_FALSE_NOBUFFSIZE
	// Parameter: IN char * pByte
	// Parameter: IN int nByte
	// Parameter: IN int nFlags
	// author:	  SSZTH
	//************************************
	virtual int Send(IN char* pByte, IN int nByte, IN int nFlags) = 0;
};
HXDECLARE_PTR(IHXSocket);
