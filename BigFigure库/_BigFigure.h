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


	//���㺯��
	friend _BigFigure & BFAdd(_BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB);
	friend _BigFigure & BFSub(_BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB);



	//�������غ���

	//���ֺ����������BF,��������Ч���ɵ͵���,�Ƽ�ʹ�õڶ���,������Ч�����,����Ҫ���軺�����Ĵ�С
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