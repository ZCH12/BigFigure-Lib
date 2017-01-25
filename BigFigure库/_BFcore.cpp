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
	bool minusA = OperandA.Detail->Minus, minusB = OperandB.Detail->Minus;
	if (!(minusA || minusB))
	{
		//正正相加
		core_IntAdd(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = false;
	}
	else if (minusA && !minusB)
	{
		//负正相加
		if (BFCmp_abs(OperandA, OperandB, 1) > 0)
		{
			//-5+3=-2
			core_IntSub(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatSub(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
			result.Detail->Minus = true;
		}
		else
		{
			//-2+5=3
			core_IntSub(result.Detail, OperandB.Detail->pSInt, OperandB.Detail->LenInt, OperandA.Detail->pSInt, OperandA.Detail->LenInt, core_FloatSub(result.Detail, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat));
			result.Detail->Minus = false;
		}
	}
	else if (!minusA&&minusB)
	{
		//正负相加
		if (BFCmp_abs(OperandA, OperandB, 1) >= 0)
		{
			//5-3=2
			core_IntSub(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatSub(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
			result.Detail->Minus = false;
		}
		else
		{
			//2-5=-3
			core_IntSub(result.Detail, OperandB.Detail->pSInt, OperandB.Detail->LenInt, OperandA.Detail->pSInt, OperandA.Detail->LenInt, core_FloatSub(result.Detail, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat));
			result.Detail->Minus = true;
		}
	}
	else {
		//负负相加
		core_IntAdd(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = true;
	}
	return result;
}

BigFigure & BFSub(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB)
{
	bool minusA = OperandA.Detail->Minus, minusB = OperandB.Detail->Minus;

	if (!(minusA || minusB))
	{
		//正正相减
		if (BFCmp_abs(OperandA, OperandB, 1) >= 0)
		{
			//5-1=4
			core_IntSub(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatSub(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
			result.Detail->Minus = false;
		}
		else
		{
			//5-6=-1
			core_IntSub(result.Detail, OperandB.Detail->pSInt, OperandB.Detail->LenInt, OperandA.Detail->pSInt, OperandA.Detail->LenInt, core_FloatSub(result.Detail, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat));
			result.Detail->Minus = true;
		}
	}
	else if (minusA && !minusB)
	{
		//负正相减
		//-5-9=-14
		core_IntAdd(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = true;
	}
	else if (!minusA&&minusB)
	{
		//正负相加
		//9--5=14
		core_IntAdd(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = false;
	}
	else {
		//负负相加
		if (BFCmp_abs(OperandA, OperandB, 1) >= 0)
		{
			//-5--1=-4
			core_IntSub(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatSub(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
			result.Detail->Minus = true;
		}
		else
		{
			//-5--6=1
			core_IntSub(result.Detail, OperandB.Detail->pSInt, OperandB.Detail->LenInt, OperandA.Detail->pSInt, OperandA.Detail->LenInt, core_FloatSub(result.Detail, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat));
			result.Detail->Minus = false;
		}
	}
	return result;
}

//接口函数




//比较两个数的大小
/*
返回值表示两个数的大小情况
0,	OperandA==OperandB
1,	OperandA>OperandB
-1,	OperandA<OperandB
2,	OperandA>>OperandB
-2,	OperandA<<OperandB
*/
int BFCmp(const BigFigure &OperandA, const BigFigure &OperandB)
{
	int minus = 1;				//当为负数时,返回的结果与正数相反,此变量控制的就是这个因数

	//判断负号
	if (OperandA.Detail->Minus)
	{
		if (OperandB.Detail->Minus)
			minus = -1;//A,B同负
		else
			return -2;//A负B正
	}
	else
	{
		if (OperandB.Detail->Minus)
			return 2;//A正B负
			//else      //A正B正   minus默认为1,不需要改动
	}
	return BFCmp_abs(OperandA, OperandB, minus);

}
int BFCmp_abs(const BigFigure &OperandA, const BigFigure &OperandB)
{
	return BFCmp_abs(OperandA, OperandB, 1);
}
/*
比较两个数的绝对值大小
*/
int BFCmp_abs(const BigFigure &OperandA, const BigFigure &OperandB, int minus)
{
	//判断位
	if (OperandA.Detail->LenInt > OperandB.Detail->LenInt)
		return 2 * minus;
	else if (OperandA.Detail->LenInt < OperandB.Detail->LenInt)
		return -2 * minus;
	else
	{
		//如果两个数整数部分长度相等,则进行比较各个位
		int temp = strcmp(OperandA.Detail->pSInt, OperandB.Detail->pSInt);
		if (temp)
			return temp*minus;		//比较出结果,返回
		else
		{
			//整数部分完全相等,继续进行比较小数部分
			char *StringAH = OperandA.Detail->pSFloat,
				*StringBH = OperandB.Detail->pSFloat;

			while (*StringAH && *StringAH == *StringBH)StringAH++, StringBH++;		//找到不相等项,并且确保他们都不为'\0'
			if (*StringBH && *StringAH > *StringBH)
				return minus;
			else if (*StringAH && *StringAH < *StringBH)
				return -minus;
			else
			{
				//找到不相等的项(可能存在相等但小数末尾的0太多)
				temp = 0;
				if (*StringAH == '0')
				{
					while (*StringAH == '0'&&*StringAH != 0)StringAH++;
					if (*StringAH != 0 && *StringAH != '0')
						temp = 1;
				}
				if (*StringBH == '0')
				{
					while (*StringBH == '0'&&*StringBH != 0)StringBH++;
					if (*StringBH != 0 && *StringBH != '0')
						temp = -1;
				}
				return temp*minus;
			}
		}
	}
}

