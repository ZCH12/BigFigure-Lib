#pragma once
//用于存放临时函数



#ifndef BFFUNC_H
#define BFFUNC_H
#define _CRT_SECURE_NO_WARNINGS
#include <cstring>
#include <string>
#include "BFDetail.h"
#include "BFException.h"

extern bool ReserveZero;			//是否保留小数点后的0
extern bool ScinotationShow;		//是否以科学计数法显示数字
extern bool ConfirmWontLossAccuracy;//确保不截断小数的有效位(关闭之后如果赋值时小数位太多,则会省略多出的部分;如果开启,则会抛出异常)
extern bool ConfirmWontLossHighBit;	//确保不丢失整数的高位(如果发生溢出时),如果为true,则发生溢出时会抛出异常,如果为false,则如果溢出则舍弃高位
extern bool AutoExpand;				//在内存不足时自动拓展空间
extern size_t ScinotationLen;		//用科学计数法表示时的有效位数

void core_IntAdd(BFDetail* result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB, int carry);
void core_IntSub(BFDetail* result, const char* OperandA, size_t LengthA, const char* OperandB, size_t LengthB, int borrow);

int core_FloatAdd(BFDetail* result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB);
int core_FloatSub(BFDetail* result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB);

void core_FloatCopy(BFDetail* result, const char * OperandA, size_t LengthA);

BFDetail * core_BFAdd(BFDetail * result, const BFDetail * OperandA, const BFDetail * OperandB);
BFDetail * core_BFSub(BFDetail*result, const BFDetail * OperandA, const BFDetail * OperandB);


BFDetail* core_Expand(BFDetail* OperandDetail, size_t IntSize, size_t FloatSize);
char* core_toString(BFDetail * OperandDetail, size_t &length, bool UseScinotation, bool ReserveZero);
void core_CopyDetail(BFDetail* Dest, const BFDetail * Source);

int core_BFCmp(const BFDetail *OperandA, const BFDetail *OperandB);
int core_BFCmp_abs(const BFDetail*OperandA, const BFDetail *OperandB);
int core_BFCmp_abs(const BFDetail*OperandA, const BFDetail *OperandB, int minus);


#endif // !BFFUNC_H

