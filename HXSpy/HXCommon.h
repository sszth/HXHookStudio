#pragma once
#include <memory>
#include <unordered_map>
#include <TlHelp32.h>         //声明快照函数的头文件
#include <mutex>

template<class TYPE>
class CSingletonObj
{
public:
	CSingletonObj() {};
	~CSingletonObj() {};

	static TYPE* GetInstance()
	{
		try
		{
			if (NULL == m_pInstance)
			{
				m_pInstance = new TYPE;
			}
		}
		catch (...)
		{

		}
		return m_pInstance;
	}

	static void Release()
	{
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}
private:
	static TYPE* m_pInstance;
};

template<class TYPE>
TYPE* CSingletonObj<TYPE>::m_pInstance = NULL;

#define DECLARE_SINGLETON(Obj) \
	friend class CSingletonObj<Obj>; \
public: \
	static Obj *GetInstance() \
{ \
	return CSingletonObj<Obj>::GetInstance(); \
} \
	static void Release() \
{ \
	CSingletonObj<Obj>::Release();  \
} \
private:\
	Obj() = default;\
	~Obj() = default;\
    Obj(const Obj&) = delete;\
    Obj& operator=(const Obj&) = delete;
	



typedef std::unordered_map<DWORD, PROCESSENTRY32> MapProcess;
typedef std::unordered_map<DWORD, THREADENTRY32> MapThread;
typedef std::unordered_map<DWORD, HEAPLIST32> MapHeapList;
typedef std::unordered_map<DWORD, MODULEENTRY32> MapModuleEntry;
