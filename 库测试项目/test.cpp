#include "..\BigFigure¿â\BigFiugre.h"

#pragma comment(lib,"..\\Debug\\BigFigure¿â.lib")

#include <iostream>

int main()
{
	BF a(10, 10);
	a = "456";
	std::cout << a.toBFString() << std::endl;
	system("pause");
}