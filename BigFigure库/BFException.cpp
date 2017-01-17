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
	printf("文件:%s,\n错误行:%d\n原因:%s\n错误代码:%d\n", File, Line, message, ErrVal);
}

int BFException::GetID()
{
	return ErrVal;
}
