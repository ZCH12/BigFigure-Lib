#define _DLL_API_ 
#include  "BigFiugre.h"

#define CONST_OVER9 106		//该值等于'0'+'9'+1
#define INT_MAXLEN 32		//该值用于模板,表示的是最大的整数类型的长度,如果一个对象与一个整数进行操作时,整数部分长度小于这个长度,则将对象重新进行分配

//此文件用于存放所有算法核心函数
//都是友元函数或友元模板

//用于缓冲结果的临时缓冲区
static char* ExtraBufferHead = NULL;
static size_t ExtraBufferSize = 0;
static bool UseExtra = false;

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
@param LengthA 操作数A的长度
@param OperandB 操作数B
@param LengthB 操作数B的长度
@param carry 是否进位(该值只允许为1或0)
@return result
*/
BigFigure& core_IntAdd(BigFigure & result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB, int carry)
{
	//判断内存是否足够
	int buffer;										//计算时的缓冲区
	int offsetA = 0, offsetB = 0;


	if (LengthA >= result.Detail->AllocInt)
	{
		//内存不足以存放,准备报错
		if (AutoExpand)
		{
			//对空间进行拓展
			size_t temp1 = LengthA >= LengthB ? LengthA : LengthB;
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
				offsetA = (int)(LengthA - result.Detail->AllocInt);
			}
		}
	}
	if (LengthB >= result.Detail->AllocInt)
	{
		if (AutoExpand)
		{
			//对空间进行拓展
			result.Expand((LengthB >= result.Detail->AllocInt ? LengthB : result.Detail->AllocInt) + 1,
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
				offsetB = (int)(LengthB - result.Detail->AllocInt);
			}
		}

	}

	const char *StringAH = OperandA + offsetA,				//字符串A的首指针
		*StringAT = OperandA + LengthA - 1,			//字符串的尾指针,也是写入位置
		*StringBH = OperandB + offsetB,				//字符串B的首指针
		*StringBT = OperandB + LengthB - 1;			//字符串B的尾指针,也是写入位置
	char *StringRH = result.Detail->DataHead,
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
/*
该函数是BF整数减法法核心, 没有正负判断, 纯粹地按位相减
@param result 存储结果的对象
@param OperandA 操作数A
@param LengthA 操作数A的长度
@param OperandB 操作数B
@param LengthB 操作数B的长度
@param carry 是否进位(该值只允许为1或0)
@return result
*/
BigFigure& core_IntSub(BigFigure & result, const char* OperandA, size_t LengthA, const char* OperandB, size_t LengthB, int borrow)
{
	//判断内存是否足够
	int buffer;										//计算时的缓冲区
	int offsetA = 0, offsetB = 0;
	int ExtraSize;

	if (LengthA >= result.Detail->AllocInt)
	{
		//内存不足以存放,准备报错
		if (!AutoExpand)
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "操作数A的值太大,无法存储到result中", EXCEPTION_DETAIL);
			else
			{
				//进行截断(截去高位),注意,截断之后的数字跟之前不同
				offsetA = (int)(LengthA - result.Detail->AllocInt);
			}
		}
	}
	if (LengthB >= result.Detail->AllocInt)
	{
		if (!AutoExpand)
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "操作数B的值太大,无法存储到result中", EXCEPTION_DETAIL);
			else
			{
				//进行截断(截去高位),注意,截断之后的数字跟之前不同
				offsetB = (int)(LengthB - result.Detail->AllocInt);
			}
		}
	}

	const char *StringAH = OperandA + offsetA,				//字符串A的首指针
		*StringAT = OperandA + LengthA - 1,			//字符串的尾指针,也是写入位置
		*StringBH = OperandB + offsetB,				//字符串B的首指针
		*StringBT = OperandB + LengthB - 1;			//字符串B的尾指针,也是写入位置
	char *StringRH = result.Detail->DataHead,
		*StringRT = result.Detail->pSRP - 1,
		*StringEH, *StringET;								//额外的结果缓冲区的指针
	if (AutoExpand)
	{
		if (LengthA > result.Detail->AllocInt || LengthB > result.Detail->AllocInt)
		{
			ExtraSize = (LengthA > LengthB ? LengthA : LengthB) - result.Detail->AllocInt;
			if (ExtraSize <= (int)ExtraBufferSize)
			{
				ExtraSize = (int)ExtraBufferSize;
			}
			else {
				if (ExtraBufferHead != NULL)
					delete[] ExtraBufferHead;
				ExtraBufferHead = new char[ExtraSize];
				ExtraBufferSize = ExtraSize;
			}
			StringET = ExtraBufferHead + ExtraSize - 1;
			StringEH = ExtraBufferHead;
			*(StringET--) = 0;
			UseExtra = true;
		}
	}
	else UseExtra = false;

	while (StringRH <= StringRT&&StringAH <= StringAT&&StringBH <= StringBT)
	{
		buffer = (int)*(StringAT--) - (int)*(StringBT--) - borrow;
		if (buffer < 0)
		{
			buffer += 58;
			borrow = 1;
		}
		else
		{
			buffer += '0';
			borrow = 0;
		}
		*(StringRT--) = (char)buffer;
	}
	if (UseExtra)
		while (StringEH <= StringET&&StringAH <= StringAT&&StringBH <= StringBT)
		{
			buffer = (int)*(StringAT--) - (int)*(StringBT--) - borrow;
			if (buffer < 0)
			{
				buffer += 58;
				borrow = 1;
			}
			else
			{
				buffer += '0';
				borrow = 0;
			}
			*(StringET--) = (char)buffer;
		}
	while (borrow&&StringRH <= StringRT&&StringAH <= StringAT)
	{
		buffer = (int)*(StringAT--) - borrow;
		if (buffer < '0')
		{
			buffer += 10;
			borrow = 1;
		}
		else {
			borrow = 0;
		}
		*(StringRT--) = (char)buffer;
	}
	if (UseExtra)
		while (borrow&&StringEH <= StringET&&StringAH <= StringAT)
		{
			buffer = (int)*(StringAT--) - borrow;
			if (buffer < '0')
			{
				buffer += 10;
				borrow = 1;
			}
			else {
				borrow = 0;
			}
			*(StringET--) = (char)buffer;
		}

	while (StringRH <= StringRT&&StringAH <= StringAT)
		*(StringRT--) = *(StringAT--);
	if (UseExtra)
		while (StringEH <= StringET&&StringAH <= StringAT)
			*(StringET--) = *(StringAT--);

	StringRT++;										//回到上一个已写入数据的地方
	if (UseExtra)
	{
		StringET++;
		while (*StringET == '0')					//去除结果前面多余的0
			StringET++;
		if (!(*StringET))
		{
			while (*StringRT == '0')				//去除结果前面多余的0
				StringRT++;
			if (*StringRT == 0)						//防止结果为空,使得最小值为0
				StringRT--;
			result.Detail->LenInt = result.Detail->pSRP - StringRT;
			result.Detail->pSInt = StringRT;
		}
		else
		{
			int temp = StringEH + ExtraSize - StringET - 1;
			result.Detail->LenInt = result.Detail->pSRP - StringRT;
			result.Detail->pSInt = StringRT;
			result.Expand(result.Detail->AllocInt + temp, result.Detail->AllocFloat);

			result.Detail->pSInt -= temp;
			strncpy(result.Detail->pSInt, StringET, temp);
			result.Detail->LenInt += temp;
		}
	}
	else
	{
		while (*StringRT == '0')		//去除结果前面多余的0
			StringRT++;
		result.Detail->LenInt = result.Detail->pSRP - StringRT;
		result.Detail->pSInt = StringRT;
	}

	result.Detail->Legal = true;
	return result;
}

/*
该函数是BF小数加法核心,没有正负判断,纯粹地按位相加
@param result 存储结果的对象
@param OperandA 操作数A
@param LengthA 操作数A的长度
@param OperandB 操作数B
@param LengthB 操作数B的长度
@return result
*/
int core_FloatAdd(BigFigure & result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB)
{
	if (LengthA > result.Detail->AllocFloat || LengthB > result.Detail->AllocFloat)
	{
		//内存不足,需要进行重新分配内存
		if (AutoExpand)
		{
			//进行自动拓展
			result.Expand(result.Detail->AllocInt, LengthA > LengthB ? LengthA : LengthB);
		}
		else if (ConfirmWontLossAccuracy)
		{
			if (LengthA > result.Detail->AllocFloat)
			{
				result.Detail->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "操作数A的小数位太多,无法全部保存到result中", EXCEPTION_DETAIL);
			}
			else
			{
				result.Detail->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "操作数B的小数位太多,无法全部保存到result中", EXCEPTION_DETAIL);
			}
		}
	}


	char *StringRH = result.Detail->pSFloat,
		*StringRT;
	const char 	*StringAH = OperandA,
		*StringAT,
		*StringBH = OperandB,
		*StringBT,
		*StringTemp;


	{
		size_t temp = LengthA > LengthB ? LengthA : LengthB;							//取得最长的小数
		temp = temp < result.Detail->AllocFloat ? temp : result.Detail->AllocFloat;		//取得一个最大的下标
		StringRT = StringRH + temp;														//计算小数的最后的写入位置
		*StringRT-- = 0;
		result.Detail->LenFloat = temp;
		StringAT = StringAH + (temp < LengthA ? temp : LengthA) - 1;					//计算A的最后一个读取位置
		StringBT = StringBH + (temp < LengthB ? temp : LengthB) - 1;					//计算B的最后一个读取位置
	}

	if (LengthA > LengthB)						//拷贝多出的位
	{
		//A比较长
		StringTemp = StringAT - (LengthA - LengthB) + 1;
		while (StringTemp <= StringAT&&StringRH <= StringRT)
			*StringRT-- = *StringAT--;
	}
	else if (LengthA < LengthB)
	{
		//B比较长
		StringTemp = StringBT - (LengthB - LengthA) + 1;
		while (StringTemp <= StringBT&&StringRH <= StringRT)
			*StringRT-- = *StringBT--;
	}

	int carry = 0;
	int buffer;
	//开始拷贝有重合位的部分

	while (StringRH <= StringRT&&StringAH <= StringAT&&StringBH <= StringBT)
	{
		buffer = *(StringBT--) + *(StringAT--) + carry;
		if (buffer >= CONST_OVER9)
		{
			buffer -= 58;
			carry = 1;
		}
		else {
			buffer -= '0';
			carry = 0;
		}
		*(StringRT--) = (char)buffer;
	}
	result.Detail->Legal = true;
	return (int)carry;
}
/*
该函数是BF小数减法核心,没有正负判断,纯粹地按位相减
@param result 存储结果的对象
@param OperandA 操作数A
@param LengthA 操作数A的长度
@param OperandB 操作数B
@param LengthB 操作数B的长度
@return result
*/
int core_FloatSub(BigFigure & result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB)
{
	if (LengthA > result.Detail->AllocFloat || LengthB > result.Detail->AllocFloat)
	{
		//内存不足,需要进行重新分配内存
		if (AutoExpand)
		{
			//进行自动拓展
			result.Expand(result.Detail->AllocInt, LengthA > LengthB ? LengthA : LengthB);
		}
		else if (ConfirmWontLossAccuracy)
		{
			if (LengthA > result.Detail->AllocFloat)
			{
				result.Detail->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "操作数A的小数位太多,无法全部保存到result中", EXCEPTION_DETAIL);
			}
			else
			{
				result.Detail->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "操作数B的小数位太多,无法全部保存到result中", EXCEPTION_DETAIL);
			}
		}
	}

	char	*StringRH = result.Detail->pSFloat,
		*StringRT;
	const char	*StringAH = OperandA,
		*StringAT,
		*StringBH = OperandB,
		*StringBT,
		*StringTemp;
	int buffer;
	int borrow = 0;

	{
		//找到公共的读取位置
		size_t temp = LengthA > LengthB ? LengthA : LengthB;		//取得最长的小数
		temp = temp < result.Detail->AllocFloat ? temp : result.Detail->AllocFloat;							//取得一个最大的下标
		StringRT = StringRH + temp;																			//计算小数的最后的写入位置
		*StringRT-- = 0;
		result.Detail->LenFloat = temp;
		StringAT = StringAH + (temp < LengthA ? temp : LengthA) - 1;	//计算A的最后一个读取位置
		StringBT = StringBH + (temp < LengthB ? temp : LengthB) - 1;	//计算B的最后一个读取位置
	}


	if (LengthA > LengthB)
	{
		StringTemp = StringAT - (LengthA - LengthB) + 1;
		while (StringTemp <= StringAT&&StringRH <= StringRT)
			*StringRT-- = *StringAT--;
	}
	else if (LengthA < LengthB)
	{
		StringTemp = StringBT - (LengthB - LengthA) + 1;
		while (StringTemp <= StringBT&&StringRH <= StringRT)
		{
			*(StringRT--) = CONST_OVER9 - *(StringBT--) - borrow;
			borrow = 1;
		}
	}

	while (StringRH <= StringRT&&StringAH <= StringAT&&StringBH <= StringBT)
	{
		buffer = *(StringAT--) - *(StringBT--) - borrow;
		if (buffer < 0)
		{
			buffer += 58;
			borrow = 1;
		}
		else {
			buffer += '0';
			borrow = 0;
		}
		*(StringRT--) = (char)buffer;
	}
	result.Detail->Legal = true;
	return (int)borrow;
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

BigFigure & BFAdd(BigFigure & result, const BigFigure OperandA, const BigFigure & OperandB)
{
	bool minusA = OperandA.Detail->Minus, minusB = OperandB.Detail->Minus;

	if (!(minusA || minusB))
	{
		//正正相加
		core_IntAdd(result, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = false;
	}
	else if (minusA && !minusB)
	{
		//负正相加
		if (BFCmp_abs(OperandA, OperandB, 1) > 0)
		{
			//-5+3=-2
			core_IntSub(result, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatSub(result, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
			result.Detail->Minus = true;
		}
		else
		{
			//-2+5=3
			core_IntSub(result, OperandB.Detail->pSInt, OperandB.Detail->LenInt, OperandA.Detail->pSInt, OperandA.Detail->LenInt, core_FloatSub(result, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat));
			result.Detail->Minus = false;
		}
	}
	else if (!minusA&&minusB)
	{
		//正负相加
		if (BFCmp_abs(OperandA, OperandB, 1) >= 0)
		{
			//5-3=2
			core_IntSub(result, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatSub(result, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
			result.Detail->Minus = false;
		}
		else
		{
			//2-5=-3
			core_IntSub(result, OperandB.Detail->pSInt, OperandB.Detail->LenInt, OperandA.Detail->pSInt, OperandA.Detail->LenInt, core_FloatSub(result, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat));
			result.Detail->Minus = true;
		}
	}
	else {
		//负负相加
		core_IntAdd(result, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = true;
	}
	return result;
}

BigFigure & BFSub(BigFigure & result, const BigFigure OperandA, const BigFigure & OperandB)
{
	bool minusA = OperandA.Detail->Minus, minusB = OperandB.Detail->Minus;

	if (!(minusA || minusB))
	{
		//正正相减
		if (BFCmp_abs(OperandA, OperandB, 1) >= 0)
		{
			//5-1=4
			core_IntSub(result, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatSub(result, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
			result.Detail->Minus = false;
		}
		else
		{
			//5-6=-1
			core_IntSub(result, OperandB.Detail->pSInt, OperandB.Detail->LenInt, OperandA.Detail->pSInt, OperandA.Detail->LenInt, core_FloatSub(result, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat));
			result.Detail->Minus = true;
		}
	}
	else if (minusA && !minusB)
	{
		//负正相减
		//-5-9=-14
		core_IntAdd(result, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = true;
	}
	else if (!minusA&&minusB)
	{
		//正负相加
		//9--5=14
		core_IntAdd(result, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = false;
	}
	else {
		//负负相加
		if (BFCmp_abs(OperandA, OperandB, 1) >= 0)
		{
			//-5--1=-4
			core_IntSub(result, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatSub(result, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
			result.Detail->Minus = true;
		}
		else
		{
			//-5--6=1
			core_IntSub(result, OperandB.Detail->pSInt, OperandB.Detail->LenInt, OperandA.Detail->pSInt, OperandA.Detail->LenInt, core_FloatSub(result, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat));
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
					 //minus默认为1,不需要改动
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
	{
		return 2 * minus;
	}
	else if (OperandA.Detail->LenInt < OperandB.Detail->LenInt)
	{
		return -2 * minus;
	}
	else
	{
		//如果两个数整数部分长度相等,则进行比较各个位
		int temp = strcmp(OperandA.Detail->pSInt, OperandB.Detail->pSInt);
		if (temp)
		{
			//比较出结果,返回
			return temp*minus;
		}
		else {
			//整数部分完全相等,继续进行比较小数部分
			int index_p = 0;
			while (OperandA.Detail->pSFloat[index_p] && OperandA.Detail->pSFloat[index_p] == OperandB.Detail->pSFloat[index_p])index_p++;
			if (OperandB.Detail->pSFloat[index_p] && OperandA.Detail->pSFloat[index_p] > OperandB.Detail->pSFloat[index_p])
			{
				//if (OperandB.Detail->NumFloat[index_p] >= '0')
				return minus;
			}
			else if (OperandA.Detail->pSFloat[index_p] && OperandA.Detail->pSFloat[index_p] < OperandB.Detail->pSFloat[index_p])
			{
				return -minus;
			}
			else
			{
				temp = 0;
				int index_p2 = index_p;
				if (OperandA.Detail->pSFloat[index_p2] == '0')
				{
					index_p2++;
					while (OperandA.Detail->pSFloat[index_p2] == '0'&&OperandA.Detail->pSFloat[index_p2] != 0)index_p2++;
					if (OperandA.Detail->pSFloat[index_p2] != 0 && OperandA.Detail->pSFloat[index_p2] != '0')
						temp = 1;
				}
				index_p2 = index_p;
				if (OperandB.Detail->pSFloat[index_p2] == '0')
				{
					index_p2++;
					while (OperandB.Detail->pSFloat[index_p2] == '0'&&OperandB.Detail->pSFloat[index_p2] != 0)index_p2++;
					if (OperandB.Detail->pSFloat[index_p2] != 0 && OperandB.Detail->pSFloat[index_p2] != '0')
						temp = -1;
				}
				return temp*minus;
			}
		}
	}
}

