#pragma once
namespace CPlusPlus_RvalueReference
{
	class CPlusPlus_RvalueReference_A
	{
	public:
		CPlusPlus_RvalueReference_A();
		CPlusPlus_RvalueReference_A(const CPlusPlus_RvalueReference_A& other);
		CPlusPlus_RvalueReference_A& operator=(const CPlusPlus_RvalueReference_A& objectSrc);
	};

	class CPlusPlus_RvalueReference_B
	{
	public:
		CPlusPlus_RvalueReference_B();
		CPlusPlus_RvalueReference_B(const CPlusPlus_RvalueReference_B& other);
		CPlusPlus_RvalueReference_B& operator=(const CPlusPlus_RvalueReference_B& objectSrc);
		CPlusPlus_RvalueReference_B(const CPlusPlus_RvalueReference_B&& other);
		CPlusPlus_RvalueReference_B& operator=(const CPlusPlus_RvalueReference_B&& objectSrc);
	};

	extern void CPlusPlus_RvalueReference_Test();
}

