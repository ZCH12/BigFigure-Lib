#pragma once

//存储数据的结构体
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
};