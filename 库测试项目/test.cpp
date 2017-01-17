#include "..\BigFigure¿â\BigFiugre.h"

#pragma comment(lib,"..\\Debug\\BigFigure¿â.lib")

#include <ctime>
#include <iostream>

int main()
{
	BF a(10, 10), b(10, 12), c(100, 100);
	a = "4560";
	b = "456";

	//ºÄÊ±5747
	clock_t t2 = clock();
	for (int i = 0; i < 100000000; i++)
	{
		core_IntAdd(c, a, b, 0);
	}
	std::cout << clock() - t2 << std::endl;

	std::cout << c << std::endl;
	system("pause");
}