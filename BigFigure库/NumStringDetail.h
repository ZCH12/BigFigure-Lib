#pragma once
//用于存储字符串数字字符串的信息

#ifndef NUMSTRINGDETAIL_H
#define NUMSTRINGDETAIL_H

#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <cstring>
#include "BFException.h"

class NumStringDetail
{
private:
	typedef char byte;
	byte RadixMinus : 1;	//底数部分是否有负号
	byte ExpMinus : 1;		//指数部分是否有负号
	byte IntBeZero : 1;		//整数部分是否为0,决定是否需要在浮点数中寻找有效位
	byte Mode : 4;			//表示的数字的类型
	size_t IntLen;			//整数部分的长度
	size_t IntStart_p;		//整数的起始下标
	size_t FloatLen;		//浮点数部分的长度
	size_t FloatStart_p;	//浮点数的起始下标
	size_t ExpLen;			//指数部分的长度
	size_t ExpStart_p;		//指数的起始下标
	char* NumString;		//保存的字符串信息
	void NumStringDetail::NumCheck(const char * NumString);
public:
	NumStringDetail(const char * NumString);
	NumStringDetail(double Num);
	NumStringDetail(__int64 Num);
	NumStringDetail(long Num);
	NumStringDetail(int Num);
	~NumStringDetail();
	//friend bool NumCheck(NumStringDetail &NumDetail);
	friend class BigFigure;
};
#endif