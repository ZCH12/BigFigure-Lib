#define _DLL_API_ 
#include "BFException.h"

#include <cstdio>

BFException::BFException(int ErrVal, const char* _File, int _Line)
{
	this->File = _File;
	this->Line = _Line;
	this->ErrVal = ErrVal;
}

BFException::BFException(int ErrVal,const char* detail, const char* _File, int _Line)
{
	this->File = _File;
	this->Line = _Line;
	this->ErrVal = ErrVal;
	this->message = detail;
}


BFException::~BFException()
{
}

void BFException::GetMessage()
{
	printf("�ļ�:%s,\n������:%d\nԭ��:%s\n�������:%d\n", File, Line, message, ErrVal);
}

int BFException::GetID()
{
	return ErrVal;
}
