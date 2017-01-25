#pragma once


#ifndef BigFigure_H
#define BigFigure_H
#include "BigFiugre.h"

/*
BigFigure的临时对象
*/
class _BigFigure
{
private:
	struct BFDetail *Detail;

	//friend int BFCmp_abs(const BigFigure &OperandA, const BigFigure &OperandB, int minus);	//比较两个数的绝对值大小
	char* _toString(size_t &length, bool UseScinotation, bool ReserveZero);


public:
	_BigFigure(size_t IntSize, size_t FloatSize);
	_BigFigure(const _BigFigure & Base);
	_BigFigure(const BigFigure& Base);
	~_BigFigure();
	//各种重载函数

	//operator BigFigure();
	friend _BigFigure& operator+(_BigFigure &OperandA, const BigFigure &OperandB);
	friend class BigFigure;
};

#endif