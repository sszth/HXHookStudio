#pragma once
class CPlusPlus11_nullptr
{
public:
	static int Add(int nIndex);
	static int Add(int* pIndex);
	static void Test();
};
//
//#include <codecvt>
//
//
//namespace StrConvert {
//	// string的编码方式为utf8，则采用：
//	std::string wstring2utf8string(const std::wstring& str)
//	{
//		static std::wstring_convert<std::codecvt_utf8<wchar_t> > strCnv;
//		return strCnv.to_bytes(str);
//	}
//
//	std::wstring utf8string2wstring(const std::string& str)
//	{
//		static std::wstring_convert< std::codecvt_utf8<wchar_t> > strCnv;
//		return strCnv.from_bytes(str);
//	}
//
//	// string的编码方式为除utf8外的其它编码方式，可采用：
//	std::string wstring2string(const std::wstring& str, const std::string& locale)//locale = "chs"或"zh-cn"
//	{
//		typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;
//		static std::wstring_convert<F> strCnv(new F(locale));
//
//		return strCnv.to_bytes(str);
//	}
//
//	std::wstring string2wstring(const std::string& str, const std::string& locale)
//	{
//		typedef std::codecvt_byname<wchar_t, char, std::mbstate_t> F;
//		static std::wstring_convert<F> strCnv(new F(locale));
//
//		return strCnv.from_bytes(str);
//	}
//
//}

