#include "stdafx.h"
#include "HXWindowsSocketCommon.h"
#include "HXNetworkManager.h"

class HXNetworkManagerData
{
public:
	HXNetworkManagerData();
	~HXNetworkManagerData();

	int Initialize();

private:
	std::shared_ptr<HXWindowsSocketCommon>	m_pSocket;
	
};

HXNetworkManagerData::HXNetworkManagerData()
{
}

HXNetworkManagerData::~HXNetworkManagerData()
{
}

int HXNetworkManagerData::Initialize()
{
	m_pSocket = std::make_shared<HXWindowsSocketCommon>();
	return 0;
}


HXNetworkManager::HXNetworkManager()
{
}


HXNetworkManager::~HXNetworkManager()
{
}

unsigned int HXNetworkManager::Initialize()
{
	m_pData = std::make_shared<HXNetworkManagerData>();
	return 0;
}
