#include  "BigFiugre.h"
//此文件用于存放所有算法核心函数
//都是友元函数或友元模板


template <typename T> size_t _CountBits(T Num)
{
	const long long Flag[] = {
		0,
		0,
		9,
		99,
		999,
		9999,
		99999,
		999999,
		9999999,
		99999999,
		999999999,
		9999999999,
		99999999999,
		999999999999,
		9999999999999,
		99999999999999,
		999999999999999,
		9999999999999999,
		99999999999999999,
		9223372036854775807i64
	};
	int size = 0;
	if (typeid(Num) == typeid(short))
		size = 5;
	else if (typeid(Num) == typeid(int))
		size = 10;
	else if (typeid(Num) == typeid(long))
#ifdef _M_IX86
		size = 10;
#else
		size = 19;
#endif
	else if (typeid(Num) == typeid(__int64))
		size = 19;
	else
		size = 19;
	while (Flag[size] > Num&&size >= 0)size--;
	return size;
}

BigFigure & BFAdd(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB)
{
	core_BFAdd(result.Detail, OperandA.Detail, OperandB.Detail);
	return result;
}

BigFigure & BFSub(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB)
{
	core_BFSub(result.Detail, OperandA.Detail, OperandB.Detail);
	return result;
}


//接口函数




