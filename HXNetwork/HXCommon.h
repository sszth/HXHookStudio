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

// 目前只支持tcp
// 地址族。
enum HXSOCKET_AF
{
	HXSOCKET_AF_UNSPEC = 0,	//未定义
	HXSOCKET_AF_INET = 1,	//IPV4
	//HXSOCKET_AF_IPX		= 2,	//
	//AF_APPLETALK
	//AF_NETBIOS
	//AF_INET6
	//AF_IRDA
	//AF_BTH
};
// 类型
enum HXSOCKET_TYPE
{
	HXSOCKET_STREAM = 0,//tcp
	//SOCK_DGRAM
	//SOCK_RAW
	//SOCK_RDM
	//SOCK_SEQPACKET
};
// 协议
enum HXSOCKET_PROTOCOL
{
	PROTOCOL_ICMP = 0,
	PROTOCOL_IGMP = 1,
	PROTOCOL_RFCOMM = 2,
	PROTOCOL_TCP = 3,
};

struct HXSocketAddr
{
	HXSOCKET_AF m_af;
	std::wstring m_strIP;
	unsigned short m_shortPort;

	HXSocketAddr() :m_af(HXSOCKET_AF_UNSPEC), m_shortPort(HXDEFAULT_PORT) {}
};


#include <cwchar>
class HXCommonAlgorithm
{
public:
	static bool WStringToString(IN const std::wstring& str1, OUT std::string& str2)
	{
		str2.clear();
		for (wchar_t wc : str1)
		{
			std::mbstate_t state{};
			std::string mb(MB_CUR_MAX, '\0');
			if (-1 == std::wcrtomb(&mb[0], wc, &state))
			{
				return false;
			}
			str2.append(mb);
		}
		return true;
	}
	static bool StringToWstring()
	{}

private:

};
