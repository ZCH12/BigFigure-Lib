#define _DLL_API_ 
#include "BigFiugre.h"


/*
此源文件用于存放BigFigure相关的重载函数
*/

BigFigure& BigFigure::operator=(const BigFigure &Source)
{
	return CopyDetail(Source);
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