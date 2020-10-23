#pragma once
class HXNetworkManagerData;

class HXNetworkManager
{
public:
	HXNetworkManager();
	~HXNetworkManager();

	unsigned int Initialize();
private:
	std::shared_ptr<HXNetworkManagerData>	m_pData;
};

