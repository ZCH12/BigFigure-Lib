#include "..\BigFigure��\BigFiugre.h"

#pragma comment(lib,"..\\Debug\\BigFigure��.lib")

#include <ctime>
#include <iostream>

int main()
{
	BF a(10, 10), b(10, 12), c(100, 10);
	a = "4560";
	b = "123456789";
	//c = "12640";
	/*
	core_IntAdd(c, c, b, 0);
	std::cout << c << std::endl;
	
	core_IntAdd(c, c, b, 0);
	std::cout << c << std::endl;

	core_IntAdd(c, c, b, 0);
	*/
	//��ʱ5747
	
	clock_t t2 = clock();
	for (int i = 0; i < 100000000; i++)
	{
		core_IntAdd(c, b, c, 0);
		//std::cout << c << std::endl;
	}
	
	std::cout << clock() - t2 << std::endl;
	
	std::cout << c << std::endl;
	system("pause");
}