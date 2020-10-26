#pragma once
class HXSystemInfo : public HXSingleton<HXSystemInfo>
{
public:
	HXSystemInfo():m_nCpuCore(-1){};
	~HXSystemInfo();

	int GetCpuCore();
private:
	int m_nCpuCore;
};

