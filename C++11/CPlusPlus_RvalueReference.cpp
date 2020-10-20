#include <iostream>
#include "CPlusPlus_RvalueReference.h"

namespace CPlusPlus_RvalueReference
{
	CPlusPlus_RvalueReference_A::CPlusPlus_RvalueReference_A()
	{
		std::wcout << L"A()\n";
	}

	CPlusPlus_RvalueReference_A& CPlusPlus_RvalueReference_A::operator=(const CPlusPlus_RvalueReference_A& objectSrc)
	{
		std::wcout << L"A operator=()\n";
		return *this;
	}

	CPlusPlus_RvalueReference::CPlusPlus_RvalueReference_A::CPlusPlus_RvalueReference_A(const CPlusPlus_RvalueReference_A& other)
	{
		std::wcout << L"A(const A&)\n";
	}

	CPlusPlus_RvalueReference_B::CPlusPlus_RvalueReference_B()
	{
		std::wcout << L"B()\n";
	}

	CPlusPlus_RvalueReference_B::CPlusPlus_RvalueReference_B(const CPlusPlus_RvalueReference_B& other)
	{
		std::wcout << L"B(const B&)\n";
	}

	CPlusPlus_RvalueReference_B& CPlusPlus_RvalueReference_B::operator=(const CPlusPlus_RvalueReference_B& objectSrc)
	{
		std::wcout << L"B operator=()\n";
		return *this;
	}

	CPlusPlus_RvalueReference_B::CPlusPlus_RvalueReference_B(const CPlusPlus_RvalueReference_B&& other)
	{
		std::wcout << L"B(const B&&)\n";
	}

	CPlusPlus_RvalueReference_B& CPlusPlus_RvalueReference_B::operator=(const CPlusPlus_RvalueReference_B&& objectSrc)
	{
		std::wcout << L"B operator=(&&)\n";
		return *this;
	}
	CPlusPlus_RvalueReference_A GetA()
	{
		return CPlusPlus_RvalueReference_A();
	}
	CPlusPlus_RvalueReference_B GetB()
	{
		return CPlusPlus_RvalueReference_B();
	}

	void CPlusPlus_RvalueReference_Test()
	{
		std::wcout << L"C++ 11 _RvalueReference Test:\n";
		std::wcout << L"A:\n";
		CPlusPlus_RvalueReference_A a1 = GetA();
		std::wcout << L"A&:\n";
		const CPlusPlus_RvalueReference_A& a2 = GetA();
		std::wcout << L"B:\n";
		CPlusPlus_RvalueReference_B b1 = GetB();
		std::wcout << L"B&:\n";
		const CPlusPlus_RvalueReference_B& b2 = GetB();
		std::wcout << L"B&&:\n";
		CPlusPlus_RvalueReference_B&& b3 = GetB();
	}

}