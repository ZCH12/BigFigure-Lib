#include "..\BigFigure库\BigFiugre.h"


#if _DEBUG
#if _M_X64
#pragma comment(lib,"..\\x64\\Debug\\BigFigure库.lib")
#else
#pragma comment(lib,"..\\Debug\\BigFigure库.lib")
#endif
#else
#if _M_X64
#pragma comment(lib,"..\\x64\\Release\\BigFigure库.lib")
#else
#pragma comment(lib,"..\\Release\\BigFigure库.lib")
#endif
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
	for (int i = 0; i < 100000000; i++)
		//core_IntSub(c, a, b, 0);
		a = 123456789123456789;
	/********************************************************/
	std::cout << clock() - t1 << std::endl;
}

/*
2245ms
-0.123001
请按任意键继续. . .
2248ms
-0.123001
请按任意键继续. . .


*/



int main()
{
	BF a(100, 100), b(100, 120), c(3, 100);
	a = "-1.123";
	b = "-0.999999";


	/*
	clock_t t1 = clock();
	for (int i = 0; i < 100000000; i++)
		BFSub(c, a, b);
	std::cout << clock() - t1<<"ms" << std::endl;
	std::cout << c << std::endl;
*/
	c = a + b;
	c = c - b;


	//std::cout << core_IntSub(c, a, b, 0) << std::endl;
	//testTime();
	std::cout << c << std::endl;
	system("pause");
}