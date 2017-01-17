#define _DLL_API_ 
#include  "BigFiugre.h"

#define CONST_OVER9 106		//该值等于'0'+'6'+1

//此文件用于存放所有算法核心函数
//都是友元函数或友元模板

/*
该函数是BF整数加法核心,没有正负判断,纯粹地按位相加
@param result 存储结果的对象
@param OperandA 操作数A
@param OperandB 操作数B
@param carry 是否进位(该值只允许为1或0)
@return result
*/
BigFigure& core_IntAdd(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB, int carry)
{
	//判断内存是否足够
	int buffer;									//计算时的缓冲区
	int index_r = (int)result.Detail->AllocInt - 1,
		index_A = (int)OperandA.Detail->LenInt - 1,
		index_B = (int)OperandB.Detail->LenInt - 1;	//正在处理的位的下标
	char *String1 = OperandA.Detail->pSInt,		//保存字符串的首指针
		*String2 = OperandB.Detail->pSInt;

	if (index_A > index_r)
	{
		//内存不足以存放,准备报错
		if (ConfirmWontLossHighBit)
			throw BFException(ERR_NUMBERTOOBIG, "操作数A的值太大,无法存储到result中", EXCEPTION_DETAIL);
		else
		{
			//进行截断(截去高位),注意,截断之后的数字跟之前不同
			String1 += OperandA.Detail->LenInt - result.Detail->AllocInt;
			index_A = index_r;
		}
	}
	if (index_B > index_r)
	{
		//内存不足以存放,准备报错
		if (ConfirmWontLossHighBit)
			throw BFException(ERR_NUMBERTOOBIG, "操作数B的值太大,无法存储到result中", EXCEPTION_DETAIL);
		else
		{
			//进行截断(截去高位),注意,截断之后的数字跟之前不同
			String2 += OperandB.Detail->LenInt - result.Detail->AllocInt;
			index_B = index_r;
		}
	}

	while (index_A >= 0 && index_B >= 0)
	{
		buffer = (int)String1[index_A] + String2[index_B] + carry;
		if (buffer >= CONST_OVER9)
		{
			//大于9,将要进行进位
			buffer -= 58;//58='9'+1
			carry = 1;
		}
		else
		{
			//数字没有大于9,不需要进位
			buffer -= '0';
			carry = 0;
		}
		result.Detail->DataHead[index_r--] = (char)buffer;
		index_A--;
		index_B--;
	}

	//当存在进位时,并且A还有余下位时,要加上进位,到没有进位后就是纯粹的复制
	while (carry&&index_A >= 0)
	{
		buffer = String1[index_A] + carry;
		if (buffer >= '9')
		{
			//大于9,将要进行进位
			buffer -= 10;
			carry = 1;
		}
		else
		{
			//数字没有大于9,不需要进位
			carry = 0;
		}
		result.Detail->DataHead[index_r--] = (char)buffer;
		index_A--;
	}
	//当存在进位时,并且B还有余下位时,要加上进位,到没有进位后就是纯粹的复制
	while (carry&&index_B >= 0)
	{
		buffer = String2[index_B] + carry;
		if (buffer >= '9')
		{
			//大于9,将要进行进位
			buffer -= 10;
			carry = 1;
		}
		else
		{
			//数字没有大于9,不需要进位
			carry = 0;
		}
		result.Detail->DataHead[index_r--] = (char)buffer;
		index_B--;
	}
	//复制剩下的,A或B多出的并且是没有进位的位(在这个地方时,可以确保不会出现越界的情况)
	while (index_A >= 0)
		result.Detail->DataHead[index_r--] = String1[index_A--];
	while (index_B >= 0)
		result.Detail->DataHead[index_r--] = String2[index_B--];

	if (carry)
	{
		if (index_r >= 0)
			result.Detail->DataHead[index_r--] = '1';
	}
	result.Detail->Legal = true;
	result.Detail->LenInt = result.Detail->AllocInt - index_r - 1;
	result.Detail->pSInt = result.Detail->DataHead + index_r + 1;
	if (!ConfirmWontLossHighBit)
	{
		while (*result.Detail->pSInt == '0')result.Detail->pSInt++;
		result.Detail->LenInt = result.Detail->pSRP - result.Detail->pSInt;
		if (result.Detail->LenInt == 0)
		{
			result.Detail->LenInt = 1;
			result.Detail->pSInt = result.Detail->pSRP - 1;
			result.Detail->pSInt[0] = '0';
		}
	}
	return result;


}
template <class T>BigFigure& core_IntAdd_Basis(BigFigure & result, const BigFigure & OperandA, T OperandB, int carry)
{
	int index_p = result.Detail->IntAllocatedLen - 1, index_r = OperandA.Detail->LenInt - 1;
	char *SourceString = OperandA.Detail->pSInt, *String3 = result.Detail->StringHead;
	int buffer;
	for (; index_r >= 0 && OperandB != 0; index_r--)
	{
		buffer = SourceString[index_r];
		buffer += OperandB % 10 + carry;
		OperandB /= 10;
		if (buffer > '9')
		{
			buffer -= 10;
			carry = 1;
		}
		else
		{
			carry = 0;
		}
		String3[index_p--] = (char)buffer;
	}

	if (OperandB != 0)
	{
		if (ConfirmWontLossHighBit)
		{
			result.Detail->Illage = false;
			throw BFException(ERR_NUMBERTOOBIG, "数字太大无法存储");
		}
	}
	else {
		while (carry&&index_p >= 0 && index_r >= 0)
		{
			buffer = SourceString[index_r] + carry;
			if (buffer > '9')
			{
				buffer -= 10;
				carry = 1;
			}
			else
			{
				carry = 0;
			}
			String3[index_p] = (char)buffer;
			index_p--, index_r--;
		}
		if (carry)
		{
			String3[index_p--] = '1';
			carry = false;
		}

	}
	while (index_p >= 0 && index_r >= 0)
		String3[index_p--] = SourceString[index_r--];

	result.Detail->LenInt = result.Detail->IntAllocatedLen - index_p - 1;
	result.Detail->pSInt = String3 + (index_p == -1 ? 0 : index_p) + 1;
	result.Detail->Illage = false;
	return result;
}
/*
int core_FloatAdd(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB)
{
	int index_A, index_B = strlen(OperandB.Detail->NumFloat);
	char *String1 = OperandA.Detail->NumFloat, *String2 = OperandB.Detail->NumFloat, *String3 = result.Detail->NumFloat;
	int buffer;
	int carry = 0;

	if (OperandA.Detail->Accuracy)
		index_A = strlen(OperandA.Detail->NumFloat);
	else index_A = 0;

	if (OperandB.Detail->Accuracy)
		index_B = strlen(OperandB.Detail->NumFloat);
	else index_B = 0;


	if (index_A < index_B)
	{
		//如果A的小数位比B的短
		String3[index_B--] = 0;
		if ((int)result.Detail->Accuracy >= index_A)
		{
			while (index_B >= index_A)
			{
				String3[index_B] = String2[index_B];
				index_B--;
			}
		}
		else
		{
			if (ConfirmWontLossAccuracy)
			{
				//报错
				result.Detail->Illage = true;
				throw BFException(ERR_MAYACCURACYLOSS, "目标对象太小,无法存储足够的小数位,可能丢失精度");
			}
			else {
				//截断小数位,继续运行
				index_B = result.Detail->Accuracy;
				while (index_B >= index_A)
				{
					String3[index_B] = String2[index_B];
					index_B--;
				}
			}
		}
		index_A = index_B;							//将A,B同步,为下面只使用index_A做准备
	}
	else if (index_A > index_B)
	{
		//如果A的小数位比B的长
		String3[index_A--] = 0;
		if ((int)result.Detail->Accuracy >= index_B)
		{
			while (index_A >= index_B)
			{
				String3[index_A] = String1[index_A];
				index_A--;
			}
		}
		else
		{
			if (ConfirmWontLossAccuracy)
			{
				//报错
				result.Detail->Illage = true;
				throw BFException(ERR_MAYACCURACYLOSS, "目标对象太小,无法存储足够的小数位,可能丢失精度");
			}
			else {
				//截断小数位,继续运行
				index_A = result.Detail->Accuracy;
				while (index_A <= index_B)
				{
					String3[index_A] = String1[index_A];
					index_A--;
				}
			}
		}
	}
	else
	{
		String3[index_A--] = 0;
	}
	//从这里开始,两个字符串共用一个游标
	for (; index_A >= 0; index_A--)
	{
		buffer = (int)String1[index_A] + String2[index_A] + carry;
		if (buffer >= CONST_OVER9)
		{
			buffer -= 58;
			carry = 1;
		}
		else
		{
			buffer -= '0';
			carry = 0;
		}
		String3[index_A] = (char)buffer;
	}

	return carry;
}



*/



