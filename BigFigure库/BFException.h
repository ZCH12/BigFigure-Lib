#pragma once

#ifndef BFEXCEPTION_H
#define BFEXCEPTION_H



//错误值的定义
#define ERR_UNKNOWEXCEPTION				0	//未知的错误
#define ERR_MEMORYALLOCATEDEXCEPTION	1	//内存分配错误
#define ERR_NULLPOINTEXCEPTION			2	//空指针
#define ERR_ILLEGALNUMBER				3	//非法的数字
#define ERR_DIVISORCANNOTBEZERO			4	//除数不能为0
#define ERR_ILLEGALPARAM				5	//参数有误
#define ERR_NUMBERTOOBIG				6	//源数字太大,无法装入目标对象中
#define ERR_MAYACCURACYLOSS				7	//可能丢失精度(小数点后的位被舍弃,并且ConfirmWontLossAccuracy==true)时才抛出

//#define BFException(ErrVal) BFException(ErrVal,__FILE__,  __LINE__)
#define EXCEPTION_DETAIL __FILE__,  __LINE__
class BFException
{
private:
	int ErrVal;
	const char *File;
	int Line;
	const char* message;
public:
	BFException(int ErrVal, const char* _File, int _Line);
	BFException(int ErrVal, const char* detail, const char* _File, int _Line);
	~BFException();
	void GetMessage();       //输出错误信息
	int GetID();
};

#endif