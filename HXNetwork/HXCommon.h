#pragma once
#include <memory>
#include <thread>
#include <mutex>


#ifndef HXDECLARE_CLASS_INTERNAL_PTR(T)
#define HXDECLARE_CLASS_INTERNAL_PTR(T)			\
	typedef std::shared_ptr<T>	Ptr;

#endif // !HXDECLARE_CLASS_INTERNAL_PTR

#ifndef HXNOCOPY(T)

#define HXNOCOPY(T)			\
	private:				\
	T(const T&) = delete;	\
	T& operator=(const T&) = delete;

#endif // !HXNOCOPY

#ifndef HXDECLARE_PTR(T)
#define HXDECLARE_PTR(T)			\
	typedef std::shared_ptr<T>	T##_Ptr;

#endif // !HXDECLARE


#ifndef HXDECLARE_DATA(T)
#define HXDECLARE_DATA(T)\
	class T;\
	HXDECLARE_PTR(T);\
private:\
	T##_Ptr	m_pData;

#endif // !HXDECLARE_DATA


#define HXHASMEMBER(member)	\
	template<typename T, typename... Args>struct HXHasMember_##member { \
private:\
	template<typename U> static auto Check(int)->decltype(std::declval<u>().member(std::declval<Args>()...),std::true_type());\
	template<typename U> static auto Check(...)->decltype(std::false_type());\
public:\
	static const bool value = std::is_same<decltype(Check<T>(0)), std::true_type>::value;};


template<typename T>
class HXLazySingleton
{
	HXNOCOPY(HXLazySingleton);
public:
	
	static std::shared_ptr<T> Get()
	{
		static std::shared_ptr<T>	m_p;
		static std::mutex	m_m;
		if (nullptr == m_p)
		{
			std::lock_guard<std::mutex> lock(m_m);
			if (nullptr == m_p)
			{
				m_p = std::make_shared<T>();
			}
			//TODO:加个类中是否有函数判断  如果有执行初始化函数
		}
		return m_p;
	}
protected:
	HXLazySingleton() {}
	~HXLazySingleton() {}
};



template<typename T>
class HXSingleton : public HXLazySingleton<T>
{

};


enum HXSocketType
{
	HXSocketUnknow = 0,
	HXSocketTcp = 1,
	HXSocketUdp = 2,
};