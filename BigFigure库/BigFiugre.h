#pragma once


#ifndef BIGFIGURE_H
#define BIGFIGURE_H

#define _CRT_SECURE_NO_WARNINGS
#include "BFException.h"
#include "NumStringDetail.h"
#include "BFFunc.h"
#include "BFString.h"



//���뿪��

//ȫ�ֱ���
extern bool ReserveZero;			//�Ƿ���С������0
extern bool ScinotationShow;		//�Ƿ��Կ�ѧ��������ʾ����
extern bool ConfirmWontLossAccuracy;//ȷ�����ض�С������Чλ(�ر�֮�������ֵʱС��λ̫��,���ʡ�Զ���Ĳ���;�������,����׳��쳣)
extern bool ConfirmWontLossHighBit;	//ȷ������ʧ�����ĸ�λ(����������ʱ),���Ϊtrue,�������ʱ���׳��쳣,���Ϊfalse,����������������λ
extern bool AutoExpand;				//���ڴ治��ʱ�Զ���չ�ռ�
extern size_t ScinotationLen;		//�ÿ�ѧ��������ʾʱ����Чλ��

class BigFigure
{
private:
	struct BFDetail
	{
		size_t ReferCount;      //���ü���,�����жϺ�ʱ����
		size_t AllocInt;		//�������ֵ��ѷ����ڴ泤��
		size_t AllocFloat;      //���������ֵ����ȷ��
		size_t LenInt;			//�����������ֵ���Чλ��(ʵ�ʳ���)
		size_t LenFloat;		//С���������ֵ���Чλ��(ʵ�ʳ���)
		bool Minus;             //��ʾ�Ƿ�Ϊ����,���Ϊ����,���ֵΪ1
		bool Legal;				//��ʾ�Ƿ�Ϊ�Ƿ�����,����ǷǷ�����,���ֵΪtrue
		char *pSInt;			//��������������ֵ��ַ������׵�ַ
		char *pSRP;				//�������ֵ�β��ַ(���ڿ��ټ���д��λ��)(����С�������ڵ�λ��)
		char *pSFloat;			//������ĸ��������ֵ��ַ������׵�ַ
		char *DataHead;			//����������ַ����ռ����ָ��
	} *Detail;


	//friend int BFCmp_abs(const BigFigure &OperandA, const BigFigure &OperandB, int minus);	//�Ƚ��������ľ���ֵ��С
	char* BigFigure::_toString(size_t &length, bool UseScinotation, bool ReserveZero);
public:
	//��������������
	//BigFigure();
	BigFigure(size_t IntSize, size_t FloatSize);
	BigFigure(const BigFigure& Base);
	~BigFigure();

	BigFigure& Expand(size_t IntSize, size_t FloatSize);
	BigFigure& Expand(const BigFigure &Source);

	BigFigure& toBF(NumStringDetail &NumStringDetail);

	BigFigure & CopyDetail(const BigFigure & Source);

	
	//���ֺ����������BF,��������Ч���ɵ͵���,�Ƽ�ʹ�õڶ���,������Ч�����,����Ҫ���軺�����Ĵ�С
	std::string toString();
	std::string toString(bool UseScinotation, bool ReserveZero);
	BFString toBFString();
	BFString toBFString(bool UseScinotation, bool ReserveZero);
	char* toCString(char *result);
	char* toCString(char *result, bool UseScinotation, bool ReserveZero);
	

	//���㺯��
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

	//��������
	BigFigure& toBF(NumStringDetail &NumStringDetail);						//��������������д�뵽��ǰ������
	std::string toString();													//����ǰ�������Ϊ�ַ���
	std::string toString(bool UseScinotation, bool ReserveZero);			//����ǰ�������Ϊ�ַ���(ͨ��mode��ѡ�����ģʽ(������ʾ|��ѧ������))
	BigFigure& CopyDetail(const BigFigure &Source);							//�������ֵ�ֵ
	void printDetail();														//��ӡ���ֵ���ϸ��Ϣ

																			//��Ԫ����
	friend int BFCmp(const BigFigure &OperandA, const BigFigure &OperandB);		//�Ƚ��������ֵĴ�С
	friend int BFCmp_abs(const BigFigure &OperandA, const BigFigure &OperandB);	//�Ƚ��������ľ���ֵ��С
	template <typename T> friend int BFCmp_abs(const BigFigure &OperandA, T OperandB);

	//���غ���
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