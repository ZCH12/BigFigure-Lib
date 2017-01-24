#include "..\BigFigure¿â\BigFiugre.h"


#if _M_X64
#pragma comment(lib,"..\\x64\\Debug\\BigFigure¿â.lib")
#else
#pragma comment(lib,"..\\Debug\\BigFigure¿â.lib")
#endif

#include <ctime>
#include <iostream>
#include <climits>

void testTime()
{
	clock_t t1 = clock();
	/********************************************************/

	BF a(100, 100), b(100, 120), c(3, 100);

	a = "10000000000000000000000000000000000000000000000000000000000000000000000";
	b = "978916123132123515315641231561212315351321513132313265564561451515515";
	for (int i = 0; i < 1000000; i++)
		//core_IntSub(c, a, b, 0);
		a = 123456789123456789;
	/********************************************************/
	std::cout << clock() - t1 << std::endl;
}





int main()
{
	BF a(100, 100), b(100, 120), c(3, 100);
	a = "10000000000000.123";
	b = "123456789.9";

	clock_t t1 = clock();
	for (int i = 0; i < 10000000; i++)
		BFAdd(c, a, b);
	std::cout << clock() - t1<<"ms" << std::endl;
	std::cout << c << std::endl;




	//std::cout << core_IntSub(c, a, b, 0) << std::endl;
	//testTime();
	//std::cout << c << std::endl;
	system("pause");
}