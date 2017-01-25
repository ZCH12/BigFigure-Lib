#pragma once


#ifndef BigFigure_H
#define BigFigure_H
#include "BigFiugre.h"

/*
临时BigFigure对象
*/
class BigFigure;
class _BigFigure : public BigFigure
{
public:
	_BigFigure(size_t IntSize, size_t FloatSize);
	~_BigFigure();
	//各种重载函数
};

#endif