#pragma once
//���ڴ����ʱ����



#ifndef BFFUNC_H
#define BFFUNC_H
#define _CRT_SECURE_NO_WARNINGS
#include <cstring>
#include <string>
#include "BFDetail.h"
#include "BFException.h"

extern bool ReserveZero;			//�Ƿ���С������0
extern bool ScinotationShow;		//�Ƿ��Կ�ѧ��������ʾ����
extern bool ConfirmWontLossAccuracy;//ȷ�����ض�С������Чλ(�ر�֮�������ֵʱС��λ̫��,���ʡ�Զ���Ĳ���;�������,����׳��쳣)
extern bool ConfirmWontLossHighBit;	//ȷ������ʧ�����ĸ�λ(����������ʱ),���Ϊtrue,�������ʱ���׳��쳣,���Ϊfalse,����������������λ
extern bool AutoExpand;				//���ڴ治��ʱ�Զ���չ�ռ�
extern size_t ScinotationLen;		//�ÿ�ѧ��������ʾʱ����Чλ��

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

