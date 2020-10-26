#include "stdafx.h"
#include "HXSystemInfo.h"

HXSystemInfo::~HXSystemInfo()
{
}

int HXSystemInfo::GetCpuCore()
{
	if (-1 == m_nCpuCore)
	{
		int nCpuCore = m_nCpuCore;
		static std::once_flag flag;
		std::call_once(flag, [&nCpuCore]() {
			nCpuCore = std::thread::hardware_concurrency();
		});
		m_nCpuCore = nCpuCore;
	}

	return m_nCpuCore;
}
