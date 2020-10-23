#include "stdafx.h"
#include "HXWindowsSocketCommon.h"
#include "HXSocketFactory.h"


class HXSocketFactory::HXSocketFactoryData
{
public:
	
};


HXSocketFactory::HXSocketFactory()
{
	m_pData = std::make_shared<HXSocketFactoryData>();
}

int HXSocketFactory::Initialize()
{
	if (0 != HXWindowsSocketCommon::Get()->Initialize())
	{
		return -1;
	}

	
	return 0;
}

IHXSocket_Ptr HXSocketFactory::Produce(HXSocketType type)
{
	IHXSocket_Ptr socketPtr = nullptr;
	switch (type)
	{
	case HXSocketUnknow:
	case HXSocketTcp:
		break;
	case HXSocketUdp:
		break;
	default:
		break;
	}
	return socketPtr;
}
