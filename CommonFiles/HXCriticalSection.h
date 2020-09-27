#pragma once

#include <Windows.h>
class CHXCriticalSection
{
public:
	CHXCriticalSection() throw()
	{
		memset(&m_sec, 0, sizeof(CRITICAL_SECTION));
	}
	~CHXCriticalSection()
	{}
	void Lock() throw()
	{
		EnterCriticalSection(&m_sec);
	}
	void Unlock() throw()
	{
		LeaveCriticalSection(&m_sec);
	}
	HRESULT Init() throw()
	{
#ifdef HX_NO_DEBUG_INFO
		if (!InitializeCriticalSectionEx(&m_sec, 4000, CRITICAL_SECTION_NO_DEBUG_INFO))
#else
		if (!InitializeCriticalSectionEx(&m_sec, 4000, 0))
#endif // HX_NO_DEBUG_INFO
		{
			return E_FAIL;
		}
		return S_OK;
	}

	HRESULT Try() throw()
	{
		if (!TryEnterCriticalSection(&m_sec))
		{
			return S_FALSE;
		}
		return S_OK;
	}

	void Term() throw()
	{
		DeleteCriticalSection(&m_sec);
	}

private:
	CRITICAL_SECTION m_sec;
};

class CHXAutoCriticalSection : public CHXCriticalSection
{
public:
	CHXAutoCriticalSection()
	{
		HRESULT hr = CHXCriticalSection::Init();
		if (FAILED(hr))
		{
			throw hr;
		}
	}
	~CHXAutoCriticalSection() throw()
	{
		CHXCriticalSection::Term();
	}
private:
	HRESULT Init() = delete;
	void Term() = delete;
};
