#pragma once


#ifndef BIGFIGURE_H
#define BIGFIGURE_H

#define _CRT_SECURE_NO_WARNINGS
#include "BFException.h"
#include "NumStringDetail.h"
#include "BFFunc.h"
#include "BFString.h"



//编译开关

//全局变量
extern bool ReserveZero;			//是否保留小数点后的0
extern bool ScinotationShow;		//是否以科学计数法显示数字
extern bool ConfirmWontLossAccuracy;//确保不截断小数的有效位(关闭之后如果赋值时小数位太多,则会省略多出的部分;如果开启,则会抛出异常)
extern bool ConfirmWontLossHighBit;	//确保不丢失整数的高位(如果发生溢出时),如果为true,则发生溢出时会抛出异常,如果为false,则如果溢出则舍弃高位
extern bool AutoExpand;				//在内存不足时自动拓展空间
extern size_t ScinotationLen;		//用科学计数法表示时的有效位数

class BigFigure
{
private:
	struct BFDetail
	{
		size_t ReferCount;      //引用计数,用于判断何时销毁
		size_t AllocInt;		//整数部分的已分配内存长度
		size_t AllocFloat;      //浮点数部分的最大精确度
		size_t LenInt;			//整数部分数字的有效位数(实际长度)
		size_t LenFloat;		//小数部分数字的有效位数(实际长度)
		bool Minus;             //表示是否为负数,如果为负数,则该值为1
		bool Legal;				//表示是否为非法数字,如果是非法数字,则此值为true
		char *pSInt;			//可输出的整数部分的字符串的首地址
		char *pSRP;				//整数部分的尾地址(用于快速计算写入位置)(处于小数点所在的位置)
		char *pSFloat;			//可输出的浮点数部分的字符串的首地址
		char *DataHead;			//保存申请的字符串空间的首指针
	} *Detail;


	//friend int BFCmp_abs(const BigFigure &OperandA, const BigFigure &OperandB, int minus);	//比较两个数的绝对值大小
	char* BigFigure::_toString(size_t &length, bool UseScinotation, bool ReserveZero);
public:
	//构造器与析构器
	//BigFigure();
	BigFigure(size_t IntSize, size_t FloatSize);
	BigFigure(const BigFigure& Base);
	~BigFigure();

	BigFigure& Expand(size_t IntSize, size_t FloatSize);
	BigFigure& Expand(const BigFigure &Source);

	BigFigure& toBF(NumStringDetail &NumStringDetail);

	BigFigure & CopyDetail(const BigFigure & Source);

	
	//三种函数用于输出BF,其中运行效率由低到高,推荐使用第二种,第三种效率最高,但需要假设缓冲区的大小
	std::string toString();
	std::string toString(bool UseScinotation, bool ReserveZero);
	BFString toBFString();
	BFString toBFString(bool UseScinotation, bool ReserveZero);
	char* toCString(char *result);
	char* toCString(char *result, bool UseScinotation, bool ReserveZero);
	

	//运算函数
	friend BigFigure& core_IntAdd(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB, int carry);
	template <class T> friend BigFigure& core_IntAdd_Basis(BigFigure & result, const BigFigure & OperandA, T OperandB, int carry);
	/*
	friend int core_FloatAdd(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB);
	friend BigFigure& core_IntSub(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB, int borrow);
	friend int core_FloatSub(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB);

	//friend void core_IntAdd(BigFigure & result, const BigFigure & OperandA, double OperandB);
	friend BigFigure& core_IntAdd(BigFigure & result, const BigFigure & OperandA, __int64 OperandB);
	friend BigFigure& core_IntAdd(BigFigure & result, const BigFigure & OperandA, long OperandB);
	friend BigFigure& core_IntAdd(BigFigure & result, const BigFigure & OperandA, int OperandB);
	//friend void core_FloatAdd(BigFigure & result, const BigFigure & OperandA, double OperandB);

	friend BigFigure& BFAdd(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB);
	friend BigFigure& BFAdd(BigFigure & result, BigFigure & OperandA, double OperandB);
	friend BigFigure& BFAdd(BigFigure & result, BigFigure & OperandA, const __int64 OperandB);
	friend BigFigure& BFSub(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB);
	friend BigFigure& BFSub(BigFigure & result, BigFigure & OperandA, double OperandB);

	//void core_IntAdd(BigFigure &result, BigFigure &OperandA, int OperandB);
	//void core_FloatAdd();
	//void core_FloatAdd();

	//基础函数
	BigFigure& toBF(NumStringDetail &NumStringDetail);						//将其他数据类型写入到当前对象中
	std::string toString();													//将当前对象输出为字符串
	std::string toString(bool UseScinotation, bool ReserveZero);			//将当前对象输出为字符串(通过mode可选择输出模式(正常显示|科学计数法))
	BigFigure& CopyDetail(const BigFigure &Source);							//复制数字的值
	void printDetail();														//打印数字的详细信息

																			//友元函数
	friend int BFCmp(const BigFigure &OperandA, const BigFigure &OperandB);		//比较两个数字的大小
	friend int BFCmp_abs(const BigFigure &OperandA, const BigFigure &OperandB);	//比较两个数的绝对值大小
	template <typename T> friend int BFCmp_abs(const BigFigure &OperandA, T OperandB);

	//重载函数
	*/
	BigFigure& operator=(const BigFigure &Source);
	BigFigure& operator=(const char* Source);
	BigFigure& operator=(const double Source);
	BigFigure& operator=(const __int64 Source);
	BigFigure& operator=(const long Source);
	BigFigure& operator=(const int Source);
	/*
	friend BigFigure operator+(const BigFigure &OperandA, const BigFigure &OperandB);
	friend BigFigure operator+(const BigFigure &OperandA, const double OperandB);


	friend BigFigure operator-(const BigFigure & OperandA, const BigFigure & OperandB);
	friend BigFigure operator-(const BigFigure & OperandA, const double OperandB);

	friend std::ostream& operator<<(std::ostream& out, BigFigure& Source);
	friend bool operator<(const BigFigure& OperandA, const BigFigure&OperandB);
	friend bool operator>(const BigFigure& OperandA, const BigFigure&OperandB);
	friend bool operator==(const BigFigure& OperandA, const BigFigure&OperandB);
	friend bool operator<=(const BigFigure& OperandA, const BigFigure&OperandB);
	friend bool operator>=(const BigFigure& OperandA, const BigFigure&OperandB);
	*/
};

typedef BigFigure BF;

#endif