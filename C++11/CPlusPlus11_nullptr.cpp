#include "CPlusPlus11_nullptr.h"
#include <iostream>
int CPlusPlus11_nullptr::Add(int nIndex)
{
	std::wcout << L"Add int\n";
	return 0;
}

int CPlusPlus11_nullptr::Add(int* pIndex)
{
	std::wcout << L"Add ptr\n";
	return 1;
}

void CPlusPlus11_nullptr::Test()
{
	std::wcout << L"C++11 nullptr Test:\n";
	std::wcout << L"Add NULL:\n";
	Add(NULL);
	std::wcout << L"Add nullptr:\n";
	Add(nullptr);
}
