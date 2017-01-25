#include "BigFiugre.h"


/*
此源文件用于存放BigFigure相关的重载函数
*/

BigFigure& BigFigure::operator=(const BigFigure &Source)
{
	core_CopyDetail(this->Detail, Source.Detail);
	return *this;
}
BigFigure& BigFigure::operator=(const _BigFigure &Source)
{
	core_CopyDetail(this->Detail, Source.Detail);
	return *this;
}

BigFigure& BigFigure::operator=(const char* Source)
{
	this->toBF(NumStringDetail(Source));
	return *this;
}
BigFigure& BigFigure::operator=(const double Source)
{
	this->toBF(NumStringDetail(Source));
	return *this;
}
BigFigure& BigFigure::operator=(const __int64 Source)
{
	this->toBF(NumStringDetail(Source));
	return *this;
}
BigFigure& BigFigure::operator=(const long Source)
{
	this->toBF(NumStringDetail(Source));
	return *this;
}
BigFigure& BigFigure::operator=(const int Source)
{
	this->toBF(NumStringDetail(Source));
	return *this;
}


std::ostream& operator<<(std::ostream &os, BigFigure &Source)
{
	os << Source.toBFString().c_str();
	return os;
}


_BigFigure operator+(const BigFigure &OperandA, const BigFigure &OperandB)
{
	_BigFigure Return((OperandA.Detail->AllocInt > OperandB.Detail->AllocInt ? OperandA.Detail->AllocInt : OperandB.Detail->AllocInt) + 1,
		OperandA.Detail->AllocFloat > OperandB.Detail->AllocFloat ? OperandA.Detail->AllocFloat : OperandB.Detail->AllocFloat);
	core_BFAdd(Return.Detail, OperandA.Detail, OperandB.Detail);
	return Return;
}

_BigFigure operator-(const BigFigure &OperandA, const BigFigure &OperandB)
{
	_BigFigure Return((OperandA.Detail->AllocInt > OperandB.Detail->AllocInt ? OperandA.Detail->AllocInt : OperandB.Detail->AllocInt) + 1,
		OperandA.Detail->AllocFloat > OperandB.Detail->AllocFloat ? OperandA.Detail->AllocFloat : OperandB.Detail->AllocFloat);
	core_BFSub(Return.Detail, OperandA.Detail, OperandB.Detail);
	return Return;
}

_BigFigure& operator+(_BigFigure &OperandA, const BigFigure &OperandB)
{
	core_BFAdd(OperandA.Detail, OperandA.Detail, OperandB.Detail);
	return OperandA;
}
_BigFigure& operator-(_BigFigure &OperandA, const BigFigure &OperandB)
{
	core_BFSub(OperandA.Detail, OperandA.Detail, OperandB.Detail);
	return OperandA;
}


_BigFigure& operator+(_BigFigure &OperandA, const _BigFigure &OperandB)
{
	core_BFAdd(OperandA.Detail, OperandA.Detail, OperandB.Detail);
	return OperandA;
}
_BigFigure& operator-(_BigFigure &OperandA, const _BigFigure &OperandB)
{
	core_BFSub(OperandA.Detail, OperandA.Detail, OperandB.Detail);
	return OperandA;
}