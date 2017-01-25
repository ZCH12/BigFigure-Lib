#pragma once


#ifndef BigFigure_H
#define BigFigure_H
#include "BigFiugre.h"

/*
BigFigure����ʱ����
*/
class _BigFigure
{
private:
	struct BFDetail *Detail;

	//friend int BFCmp_abs(const BigFigure &OperandA, const BigFigure &OperandB, int minus);	//�Ƚ��������ľ���ֵ��С
	char* _toString(size_t &length, bool UseScinotation, bool ReserveZero);


public:
	_BigFigure(size_t IntSize, size_t FloatSize);
	_BigFigure(const _BigFigure & Base);
	_BigFigure(const BigFigure& Base);
	~_BigFigure();
	//�������غ���

	//operator BigFigure();
	friend _BigFigure& operator+(_BigFigure &OperandA, const BigFigure &OperandB);
	friend class BigFigure;
};

#endif