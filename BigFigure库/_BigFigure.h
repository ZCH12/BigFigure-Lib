#pragma once


#ifndef BigFigure_H
#define BigFigure_H
#include "BigFiugre.h"

/*
��ʱBigFigure����
*/
class BigFigure;
class _BigFigure : public BigFigure
{
public:
	_BigFigure(size_t IntSize, size_t FloatSize);
	~_BigFigure();
	//�������غ���
};

#endif