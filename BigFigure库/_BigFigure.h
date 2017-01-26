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


	//运算函数
	friend _BigFigure & BFAdd(_BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB);
	friend _BigFigure & BFSub(_BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB);



	//各种重载函数

	//三种函数用于输出BF,其中运行效率由低到高,推荐使用第二种,第三种效率最高,但需要假设缓冲区的大小
	std::string toString();
	std::string toString(bool UseScinotation, bool ReserveZero);
	BFString toBFString();
	BFString toBFString(bool UseScinotation, bool ReserveZero);
	char* toCString(char *result);
	char* toCString(char *result, bool UseScinotation, bool ReserveZero);


	//operator BigFigure();
	friend _BigFigure operator+(const BigFigure &OperandA, const BigFigure &OperandB);
	friend _BigFigure operator-(const BigFigure &OperandA, const BigFigure &OperandB);
	friend _BigFigure& operator+(_BigFigure &OperandA, const BigFigure &OperandB);
	friend _BigFigure& operator-(_BigFigure &OperandA, const BigFigure &OperandB);
	friend _BigFigure& operator+(_BigFigure &OperandA, const _BigFigure &OperandB);
	friend _BigFigure& operator-(_BigFigure &OperandA, const _BigFigure &OperandB);

	friend _BigFigure operator+(const BigFigure &OperandA, const double OperandB);
	friend _BigFigure operator-(const BigFigure &OperandA, const double OperandB);
	friend _BigFigure operator+(const _BigFigure &OperandA, const double OperandB);
	friend _BigFigure operator-(const _BigFigure &OperandA, const double OperandB);
	friend class BigFigure;
};

#endif