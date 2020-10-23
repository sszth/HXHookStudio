#pragma once
class HXWindowsSocketCommon : public HXSingleton<HXWindowsSocketCommon>
{
public:
	HXWindowsSocketCommon() {};
	~HXWindowsSocketCommon();
public:
	int Initialize();
	IHXSocket_Ptr GetSocket();
private:
	std::once_flag m_flag;
};

