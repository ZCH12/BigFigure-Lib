#include "..\BigFigure¿â\BigFiugre.h"


#if _M_X64
#pragma comment(lib,"..\\x64\\Debug\\BigFigure¿â.lib")
#else
#pragma comment(lib,"..\\Debug\\BigFigure¿â.lib")
#endif

#include <ctime>
#include <iostream>
#include <climits>



int main()
{
	BF a(10, 10), b(100, 12), c(100, 10);
	a = "123E100";
	b = "123456789.232323";
	void test(BigFigure & result, const BigFigure & OperandA, long long OperandB, int carry);
	test(c, b, 9, 0);
	//std::cout << a << std::endl;

	//c = "9999999909";
	/*
	core_IntAdd(c, a, b, 0);
	std::cout << c << std::endl;
	
	core_IntAdd(c, c, b, 0);
	std::cout << c << std::endl;

	core_IntAdd(c, c, b, 0);
	*/
	//ºÄÊ±5747
	/*
	clock_t t2 = clock();
	long long ssd = 1;
	for (int i = 0; i < 100000000; i++)
	{
		core_IntAdd(c, b, c, 0);
		//ssd += 4560000;
		std::cout << c << std::endl;
	}

	std::cout << clock() - t2 << std::endl;
	*/
	std::cout << c << std::endl;
	system("pause");
}