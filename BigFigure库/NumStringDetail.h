#pragma once
//���ڴ洢�ַ��������ַ�������Ϣ

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
	byte RadixMinus : 1;	//���������Ƿ��и���
	byte ExpMinus : 1;		//ָ�������Ƿ��и���
	byte IntBeZero : 1;		//���������Ƿ�Ϊ0,�����Ƿ���Ҫ�ڸ�������Ѱ����Чλ
	byte Mode : 4;			//��ʾ�����ֵ�����
	size_t IntLen;			//�������ֵĳ���
	size_t IntStart_p;		//��������ʼ�±�
	size_t FloatLen;		//���������ֵĳ���
	size_t FloatStart_p;	//����������ʼ�±�
	size_t ExpLen;			//ָ�����ֵĳ���
	size_t ExpStart_p;		//ָ������ʼ�±�
	char* NumString;		//������ַ�����Ϣ
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