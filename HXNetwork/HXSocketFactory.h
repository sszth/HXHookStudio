#pragma once
#include "IHXSocket.h"
class HXSocketFactory : public HXSingleton<HXSocketFactory>
{
	HXDECLARE_DATA(HXSocketFactoryData);
public:
	HXSocketFactory();
public:
	int Initialize();
	IHXSocket_Ptr Produce(HXSocketType type);
};

