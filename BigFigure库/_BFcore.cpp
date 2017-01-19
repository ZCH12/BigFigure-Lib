#define _DLL_API_ 
#include  "BigFiugre.h"

#define CONST_OVER9 106		//该值等于'0'+'9'+1
#define INT_MAXLEN 32		//该值用于模板,表示的是最大的整数类型的长度,如果一个对象与一个整数进行操作时,整数部分长度小于这个长度,则将对象重新进行分配

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
	int buffer;										//计算时的缓冲区
	int offsetA = 0, offsetB = 0;


	if (OperandA.Detail->LenInt >= result.Detail->AllocInt)
	{
		//内存不足以存放,准备报错
		if (AutoExpand)
		{
			//对空间进行拓展
			size_t temp1 = OperandA.Detail->LenInt >= OperandB.Detail->LenInt ? OperandA.Detail->LenInt : OperandB.Detail->LenInt;
			result.Expand((temp1 > result.Detail->AllocInt ? temp1 : result.Detail->AllocInt) + 1,
				result.Detail->AllocFloat);
		}
		else
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "操作数A的值太大,无法存储到result中", EXCEPTION_DETAIL);
			else
			{
				//进行截断(截去高位),注意,截断之后的数字跟之前不同
				offsetA = (int)(OperandA.Detail->LenInt - result.Detail->AllocInt);
			}
		}
	}
	if (OperandB.Detail->LenInt >= result.Detail->AllocInt)
	{
		if (AutoExpand)
		{
			//对空间进行拓展
			result.Expand((OperandB.Detail->LenInt >= result.Detail->AllocInt ? OperandB.Detail->LenInt : result.Detail->AllocInt) + 1,
				result.Detail->AllocFloat);
		}
		else
		{
			//内存不足以存放,准备报错
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "操作数B的值太大,无法存储到result中", EXCEPTION_DETAIL);
			else
			{
				//进行截断(截去高位),注意,截断之后的数字跟之前不同
				offsetB = (int)(OperandB.Detail->LenInt - result.Detail->AllocInt);
			}
		}

	}

	char *StringAH = OperandA.Detail->pSInt + offsetA,	//字符串A的首指针
		*StringAT = OperandA.Detail->pSRP - 1,			//字符串的尾指针,也是写入位置
		*StringBH = OperandB.Detail->pSInt + offsetB,	//字符串B的首指针
		*StringBT = OperandB.Detail->pSRP - 1,			//字符串B的尾指针,也是写入位置
		*StringRH = result.Detail->DataHead,
		*StringRT = result.Detail->pSRP - 1;


	while (StringRH <= StringRT&&StringAH <= StringAT&&StringBH <= StringBT)
	{
		buffer = (int)*StringAT + *StringBT + carry;
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
		*(StringRT--) = (char)buffer;
		StringAT--;
		StringBT--;
	}

	//当存在进位时,并且A还有余下位时,要加上进位,到没有进位后就是纯粹的复制
	while (carry&&StringAH <= StringAT&&StringRH <= StringRT)
	{
		buffer = *StringAT + carry;
		if (buffer > '9')
		{
			//大于9,将要进行进位
			buffer -= 10;
			carry = 1;
		}
		else
			carry = 0;//数字没有大于9,不需要进位
		*StringRT = (char)buffer;
		StringRT--;
		StringAT--;
	}
	//当存在进位时,并且B还有余下位时,要加上进位,到没有进位后就是纯粹的复制
	while (carry&&StringBH <= StringBT&&StringRH <= StringRT)
	{
		buffer = *StringBT + carry;
		if (buffer > '9')
		{
			//大于9,将要进行进位
			buffer -= 10;
			carry = 1;
		}
		else
			carry = 0;//数字没有大于9,不需要进位
		*(StringRT--) = (char)buffer;
		StringBT--;
	}
	//复制剩下的,A或B多出的并且是没有进位的位(在这个地方时,可以确保不会出现越界的情况)
	while (StringAH <= StringAT&&StringRH <= StringRT)
		*(StringRT--) = *(StringAT--);
	while (StringBH <= StringBT&&StringRH <= StringRT)
		*(StringRT--) = *(StringBT--);

	if (carry&&StringRH <= StringRT)
	{
		*StringRT = '1';
	}
	else
		StringRT++;																//回到数字最前面的第一个有效位

	result.Detail->Legal = true;
	result.Detail->LenInt = result.Detail->pSRP - StringRT;
	result.Detail->pSInt = StringRT;

	if (!ConfirmWontLossHighBit)
	{
		while (*result.Detail->pSInt == '0')result.Detail->pSInt++;				//去除因进位溢出导致的'0'开头
		result.Detail->LenInt = result.Detail->pSRP - result.Detail->pSInt;		//重新计算长度
		if (result.Detail->LenInt == 0)											//如果整数部分长度为0,则赋予"0"
		{
			result.Detail->LenInt = 1;
			result.Detail->pSInt = result.Detail->pSRP - 1;
			result.Detail->pSInt[0] = '0';
		}
	}
	return result;
}
//BF加基本数字的模板,只能用于整数加整数,如果是浮点数,请先转换为BF然后调用core_IntAdd进行计算,此函数只进行整数部分的处理,如果是一个小数加上此基本数据,还需要调用core_FloatCopy来复制小数部分到result
/*
@param result 存储结果的对象
@param OperandA 操作数A
@param OperandB 操作数B(基本数据类型)
@param carry 是否进位(该值只允许为1或0)
@return result
*/
template <class T>BigFigure& core_IntAdd_Basis(BigFigure & result, const BigFigure & OperandA, T OperandB, int carry)
{
	int buffer = 0;											//计算时的缓冲区

	if (OperandA.Detail->LenInt >= result.Detail->AllocInt || (size_t)(buffer = (int)_CountBits<T>(OperandB)) >= result.Detail->AllocInt)
	{
		//一定不够内存
		if (AutoExpand)
		{
			if (buffer)
				result.Expand((result.Detail->AllocInt > (size_t)buffer ? result.Detail->AllocInt : (size_t)buffer) + 2
					, result.Detail->AllocFloat);
			else
			{
				size_t temp = (result.Detail->AllocInt > OperandA.Detail->LenInt ? result.Detail->AllocInt : OperandA.Detail->LenInt) + 2;
				result.Expand(temp > 32 ? temp : 32, result.Detail->AllocFloat);
			}
		}
		else {
			if (ConfirmWontLossHighBit)
			{
				throw BFException(ERR_MAYACCURACYLOSS, "结果无法保存到result中", EXCEPTION_DETAIL);
			}
		}
	}
	char *StringRH = result.Detail->DataHead,
		*StringRT = result.Detail->pSRP - 1,
		*StringAH = OperandA.Detail->pSInt,
		*StringAT = OperandA.Detail->pSRP - 1;

	while (StringRH <= StringRT&&StringAH <= StringAT)
	{
		buffer = *(StringAT--) + OperandB % 10 + carry;
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
		*(StringRT--) = (char)buffer;
	}

	if (OperandB != 0)
	{
		while (carry&&StringRH <= StringRT&&OperandB != 0)
		{
			buffer = OperandB % 10 + carry;
			OperandB /= 10;
			if (buffer > 9)
			{
				buffer += 38;
				carry = 1;
			}
			else
			{
				buffer += '0';
				carry = 0;
			}
			*(StringRT--) = (char)buffer;
		}
		while (StringRH <= StringRT&&OperandB != 0)
		{
			*(StringRT--) = OperandB % 10 + '0';
			OperandB /= 10;
		}
	}
	if (carry&&StringRH <= StringRT)
		*(StringRT) = '1';
	else
		StringRT++;


	if (OperandB)
	{
		if (ConfirmWontLossHighBit)
		{
			result.Detail->Legal = false;
			throw BFException(ERR_NUMBERTOOBIG, "数字太大无法存储", EXCEPTION_DETAIL);
		}
	}

	result.Detail->LenInt = result.Detail->pSRP - StringRT;
	result.Detail->pSInt = StringRT;
	result.Detail->Legal = true;
	return result;
}

//复制小数部分的数字,此函数一般是调用整数部分处理之后再使用
/*
@param result 存储结果的对象
@param OperandA 操作数A
@param OperandB 操作数B(基本数据类型)
@param carry 是否进位(该值只允许为1或0)
@return result
*/
BigFigure& core_FloatCopy(BigFigure &result, const BigFigure &OperandA)
{
	if (result.Detail->AllocFloat < OperandA.Detail->LenFloat)						//进行容量判断
	{
		//内存不足
		if (AutoExpand)
		{
			result.Expand(result.Detail->AllocInt, OperandA.Detail->LenFloat);		//进行拓展内存
		}
		else
		{
			if (ConfirmWontLossAccuracy)
			{
				result.Detail->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "小数部分内存不足,可能丢失精度", EXCEPTION_DETAIL);
			}
		}
	}
	strncpy(result.Detail->pSFloat, OperandA.Detail->pSFloat, result.Detail->AllocFloat);
	if (result.Detail->AllocFloat > OperandA.Detail->LenFloat)		//取得新的小数位的长度
		result.Detail->LenFloat = OperandA.Detail->LenFloat;
	else 
		result.Detail->LenFloat = result.Detail->AllocFloat;
	result.Detail->pSFloat[result.Detail->LenFloat] = 0;			//写入字符串结束符
	return result;
}

void test(BigFigure & result, const BigFigure & OperandA, long long OperandB, int carry)
{
	core_IntAdd_Basis<long long>(result, OperandA, OperandB, 0);
	core_FloatCopy(result, OperandA);
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



