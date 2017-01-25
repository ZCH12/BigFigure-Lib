#include "BFFunc.h"


#define CONST_OVER9 106		//该值等于'0'+'9'+1
#define INT_MAXLEN 32		//该值用于模板,表示的是最大的整数类型的长度,如果一个对象与一个整数进行操作时,整数部分长度小于这个长度,则将对象重新进行分配

//用于缓冲结果的临时缓冲区
static char* ExtraBufferHead = NULL;
static size_t ExtraBufferSize = 0;
static bool UseExtra = false;


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
void core_IntAdd(BFDetail* result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB, int carry)
{
	//判断内存是否足够
	int buffer;										//计算时的缓冲区
	int offsetA = 0, offsetB = 0;


	if (LengthA >= result->AllocInt)
	{
		//内存不足以存放,准备报错
		if (AutoExpand)
		{
			//对空间进行拓展
			size_t temp1 = LengthA >= LengthB ? LengthA : LengthB;
			core_Expand(result, (temp1 > result->AllocInt ? temp1 : result->AllocInt) + 1,
				result->AllocFloat);
		}
		else
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "操作数A的值太大,无法存储到result中", EXCEPTION_DETAIL);
			else
			{
				//进行截断(截去高位),注意,截断之后的数字跟之前不同
				offsetA = (int)(LengthA - result->AllocInt);
			}
		}
	}
	if (LengthB >= result->AllocInt)
	{
		if (AutoExpand)
		{
			//对空间进行拓展
			core_Expand(result, (LengthB >= result->AllocInt ? LengthB : result->AllocInt) + 1,
				result->AllocFloat);
		}
		else
		{
			//内存不足以存放,准备报错
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "操作数B的值太大,无法存储到result中", EXCEPTION_DETAIL);
			else
			{
				//进行截断(截去高位),注意,截断之后的数字跟之前不同
				offsetB = (int)(LengthB - result->AllocInt);
			}
		}

	}

	const char *StringAH = OperandA + offsetA,				//字符串A的首指针
		*StringAT = OperandA + LengthA - 1,			//字符串的尾指针,也是写入位置
		*StringBH = OperandB + offsetB,				//字符串B的首指针
		*StringBT = OperandB + LengthB - 1;			//字符串B的尾指针,也是写入位置
	char *StringRH = result->DataHead,
		*StringRT = result->pSRP - 1;


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

	result->Legal = true;
	result->LenInt = result->pSRP - StringRT;
	result->pSInt = StringRT;

	if (!ConfirmWontLossHighBit)
	{
		while (*result->pSInt == '0')result->pSInt++;				//去除因进位溢出导致的'0'开头
		result->LenInt = result->pSRP - result->pSInt;		//重新计算长度
		if (result->LenInt == 0)											//如果整数部分长度为0,则赋予"0"
		{
			result->LenInt = 1;
			result->pSInt = result->pSRP - 1;
			result->pSInt[0] = '0';
		}
	}
	return;
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
void core_IntSub(BFDetail* result, const char* OperandA, size_t LengthA, const char* OperandB, size_t LengthB, int borrow)
{
	//判断内存是否足够
	int buffer;										//计算时的缓冲区
	int offsetA = 0, offsetB = 0;
	size_t ExtraSize;

	if (LengthA >= result->AllocInt)
	{
		//内存不足以存放,准备报错
		if (!AutoExpand)
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "操作数A的值太大,无法存储到result中", EXCEPTION_DETAIL);
			else
			{
				//进行截断(截去高位),注意,截断之后的数字跟之前不同
				offsetA = (int)(LengthA - result->AllocInt);
			}
		}
	}
	if (LengthB >= result->AllocInt)
	{
		if (!AutoExpand)
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "操作数B的值太大,无法存储到result中", EXCEPTION_DETAIL);
			else
			{
				//进行截断(截去高位),注意,截断之后的数字跟之前不同
				offsetB = (int)(LengthB - result->AllocInt);
			}
		}
	}

	const char *StringAH = OperandA + offsetA,				//字符串A的首指针
		*StringAT = OperandA + LengthA - 1,			//字符串的尾指针,也是写入位置
		*StringBH = OperandB + offsetB,				//字符串B的首指针
		*StringBT = OperandB + LengthB - 1;			//字符串B的尾指针,也是写入位置
	char *StringRH = result->DataHead,
		*StringRT = result->pSRP - 1,
		*StringEH, *StringET;								//额外的结果缓冲区的指针
	if (AutoExpand)
	{
		if (LengthA > result->AllocInt || LengthB > result->AllocInt)
		{
			ExtraSize = (LengthA > LengthB ? LengthA : LengthB) - result->AllocInt;
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
			result->LenInt = result->pSRP - StringRT;
			result->pSInt = StringRT;
		}
		else
		{
			int temp = (int)(StringEH + ExtraSize - StringET) - 1;
			result->LenInt = result->pSRP - StringRT;
			result->pSInt = StringRT;
			core_Expand(result, result->AllocInt + temp, result->AllocFloat);

			result->pSInt -= temp;
			strncpy(result->pSInt, StringET, temp);
			result->LenInt += temp;
		}
	}
	else
	{
		while (*StringRT == '0')		//去除结果前面多余的0
			StringRT++;
		if (*StringRT == 0)				//防止结果为空,使得最小值为0
			StringRT--;
		result->LenInt = result->pSRP - StringRT;
		result->pSInt = StringRT;
	}

	result->Legal = true;
	return;
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
int core_FloatAdd(BFDetail* result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB)
{
	if (LengthA > result->AllocFloat || LengthB > result->AllocFloat)
	{
		//内存不足,需要进行重新分配内存
		if (AutoExpand)
		{
			//进行自动拓展
			core_Expand(result, result->AllocInt, LengthA > LengthB ? LengthA : LengthB);
		}
		else if (ConfirmWontLossAccuracy)
		{
			if (LengthA > result->AllocFloat)
			{
				result->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "操作数A的小数位太多,无法全部保存到result中", EXCEPTION_DETAIL);
			}
			else
			{
				result->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "操作数B的小数位太多,无法全部保存到result中", EXCEPTION_DETAIL);
			}
		}
	}


	char *StringRH = result->pSFloat,
		*StringRT;
	const char 	*StringAH = OperandA,
		*StringAT,
		*StringBH = OperandB,
		*StringBT,
		*StringTemp;


	{
		size_t temp = LengthA > LengthB ? LengthA : LengthB;							//取得最长的小数
		temp = temp < result->AllocFloat ? temp : result->AllocFloat;		//取得一个最大的下标
		StringRT = StringRH + temp;														//计算小数的最后的写入位置
		*StringRT-- = 0;
		result->LenFloat = temp;
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
	result->Legal = true;
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
int core_FloatSub(BFDetail* result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB)
{
	if (LengthA > result->AllocFloat || LengthB > result->AllocFloat)
	{
		//内存不足,需要进行重新分配内存
		if (AutoExpand)
		{
			//进行自动拓展
			core_Expand(result, result->AllocInt, LengthA > LengthB ? LengthA : LengthB);
		}
		else if (ConfirmWontLossAccuracy)
		{
			if (LengthA > result->AllocFloat)
			{
				result->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "操作数A的小数位太多,无法全部保存到result中", EXCEPTION_DETAIL);
			}
			else
			{
				result->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "操作数B的小数位太多,无法全部保存到result中", EXCEPTION_DETAIL);
			}
		}
	}

	char	*StringRH = result->pSFloat,
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
		temp = temp < result->AllocFloat ? temp : result->AllocFloat;							//取得一个最大的下标
		StringRT = StringRH + temp;																			//计算小数的最后的写入位置
		*StringRT-- = 0;
		result->LenFloat = temp;
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
	result->Legal = true;
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
void core_FloatCopy(BFDetail* result, const char * OperandA, size_t LengthA)
{
	if (result->AllocFloat < LengthA)							//进行容量判断
	{
		//内存不足
		if (AutoExpand)
		{
			core_Expand(result, result->AllocInt, LengthA);		//进行拓展内存
		}
		else
		{
			if (ConfirmWontLossAccuracy)
			{
				result->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "小数部分内存不足,可能丢失精度", EXCEPTION_DETAIL);
			}
		}
	}
	strncpy(result->pSFloat, OperandA, result->AllocFloat);
	if (result->AllocFloat > LengthA)		//取得新的小数位的长度
		result->LenFloat = LengthA;
	else
		result->LenFloat = result->AllocFloat;
	result->pSFloat[result->LenFloat] = 0;			//写入字符串结束符
	return;
}




BFDetail * core_BFAdd(BFDetail * result, const BFDetail * OperandA, const BFDetail * OperandB)
{
	bool minusA = OperandA->Minus, minusB = OperandB->Minus;
	if (!(minusA || minusB))
	{
		//正正相加
		core_IntAdd(result, OperandA->pSInt, OperandA->LenInt, OperandB->pSInt, OperandB->LenInt, core_FloatAdd(result, OperandA->pSFloat, OperandA->LenFloat, OperandB->pSFloat, OperandB->LenFloat));
		result->Minus = false;
	}
	else if (minusA && !minusB)
	{
		//负正相加
		if (core_BFCmp_abs(OperandA, OperandB, 1) > 0)
		{
			//-5+3=-2
			core_IntSub(result, OperandA->pSInt, OperandA->LenInt, OperandB->pSInt, OperandB->LenInt, core_FloatSub(result, OperandA->pSFloat, OperandA->LenFloat, OperandB->pSFloat, OperandB->LenFloat));
			result->Minus = true;
		}
		else
		{
			//-2+5=3
			core_IntSub(result, OperandB->pSInt, OperandB->LenInt, OperandA->pSInt, OperandA->LenInt, core_FloatSub(result, OperandB->pSFloat, OperandB->LenFloat, OperandA->pSFloat, OperandA->LenFloat));
			result->Minus = false;
		}
	}
	else if (!minusA&&minusB)
	{
		//正负相加
		if (core_BFCmp_abs(OperandA, OperandB, 1) >= 0)
		{
			//5-3=2
			core_IntSub(result, OperandA->pSInt, OperandA->LenInt, OperandB->pSInt, OperandB->LenInt, core_FloatSub(result, OperandA->pSFloat, OperandA->LenFloat, OperandB->pSFloat, OperandB->LenFloat));
			result->Minus = false;
		}
		else
		{
			//2-5=-3
			core_IntSub(result, OperandB->pSInt, OperandB->LenInt, OperandA->pSInt, OperandA->LenInt, core_FloatSub(result, OperandB->pSFloat, OperandB->LenFloat, OperandA->pSFloat, OperandA->LenFloat));
			result->Minus = true;
		}
	}
	else {
		//负负相加
		core_IntAdd(result, OperandA->pSInt, OperandA->LenInt, OperandB->pSInt, OperandB->LenInt, core_FloatAdd(result, OperandA->pSFloat, OperandA->LenFloat, OperandB->pSFloat, OperandB->LenFloat));
		result->Minus = true;
	}
	return result;
}

BFDetail * core_BFSub(BFDetail*result, const BFDetail * OperandA, const BFDetail * OperandB)
{
	bool minusA = OperandA->Minus, minusB = OperandB->Minus;

	if (!(minusA || minusB))
	{
		//正正相减
		if (core_BFCmp_abs(OperandA, OperandB, 1) >= 0)
		{
			//5-1=4
			core_IntSub(result, OperandA->pSInt, OperandA->LenInt, OperandB->pSInt, OperandB->LenInt, core_FloatSub(result, OperandA->pSFloat, OperandA->LenFloat, OperandB->pSFloat, OperandB->LenFloat));
			result->Minus = false;
		}
		else
		{
			//5-6=-1
			core_IntSub(result, OperandB->pSInt, OperandB->LenInt, OperandA->pSInt, OperandA->LenInt, core_FloatSub(result, OperandB->pSFloat, OperandB->LenFloat, OperandA->pSFloat, OperandA->LenFloat));
			result->Minus = true;
		}
	}
	else if (minusA && !minusB)
	{
		//负正相减
		//-5-9=-14
		core_IntAdd(result, OperandA->pSInt, OperandA->LenInt, OperandB->pSInt, OperandB->LenInt, core_FloatAdd(result, OperandA->pSFloat, OperandA->LenFloat, OperandB->pSFloat, OperandB->LenFloat));
		result->Minus = true;
	}
	else if (!minusA&&minusB)
	{
		//正负相加
		//9--5=14
		core_IntAdd(result, OperandA->pSInt, OperandA->LenInt, OperandB->pSInt, OperandB->LenInt, core_FloatAdd(result, OperandA->pSFloat, OperandA->LenFloat, OperandB->pSFloat, OperandB->LenFloat));
		result->Minus = false;
	}
	else {
		//负负相加
		if (core_BFCmp_abs(OperandA, OperandB, 1) >= 0)
		{
			//-5--1=-4
			core_IntSub(result, OperandA->pSInt, OperandA->LenInt, OperandB->pSInt, OperandB->LenInt, core_FloatSub(result, OperandA->pSFloat, OperandA->LenFloat, OperandB->pSFloat, OperandB->LenFloat));
			result->Minus = true;
		}
		else
		{
			//-5--6=1
			core_IntSub(result, OperandB->pSInt, OperandB->LenInt, OperandA->pSInt, OperandA->LenInt, core_FloatSub(result, OperandB->pSFloat, OperandB->LenFloat, OperandA->pSFloat, OperandA->LenFloat));
			result->Minus = false;
		}
	}
	return result;
}


BFDetail* core_Expand(BFDetail* OperandDetail, size_t IntSize, size_t FloatSize)
{
	char *temp;								//新的内存空间

	int AllocatedMem;
	if (IntSize == 0)
		throw BFException(ERR_ILLEGALPARAM, "整数部分分配的内存不能为0", EXCEPTION_DETAIL);

	AllocatedMem = (int)IntSize + 1;
	if (FloatSize > 0)
		AllocatedMem += (int)FloatSize + 1;

	try
	{
		temp = new char[AllocatedMem];
		temp[IntSize] = 0;				//写入中间的'\0'
		if (IntSize >= OperandDetail->LenInt)
			strncpy(temp + IntSize - OperandDetail->LenInt, OperandDetail->pSInt, OperandDetail->LenInt);//整数部分安全得进行复制
		else
		{
			//整数部分将会被截断
			if (ConfirmWontLossHighBit)
			{
				delete[] temp;
				throw BFException(ERR_NUMBERTOOBIG, "修改大小之后对象中原来存放的值会被截断", EXCEPTION_DETAIL);
			}
			else
				strncpy(temp, OperandDetail->pSInt + OperandDetail->LenInt - IntSize, IntSize);//进行安全截断
		}
		if (FloatSize >= OperandDetail->LenFloat)
		{
			if (OperandDetail->LenFloat != 0)
				strncpy(temp + IntSize + 1, OperandDetail->pSFloat, OperandDetail->LenFloat + 1);//小数部分安全得进行复制
			temp[IntSize + 1 + OperandDetail->LenFloat] = 0;
		}
		else
		{
			//小数部分将会被截断
			if (ConfirmWontLossAccuracy)
			{
				delete[] temp;
				throw BFException(ERR_MAYACCURACYLOSS, "修改后的对象无法装入原来的小数位,将会丢失精度", EXCEPTION_DETAIL);
			}
			else
			{
				strncpy(temp + IntSize + 1, OperandDetail->pSFloat, FloatSize);
				temp[AllocatedMem - 1] = 0;		//写入结束符
			}
		}
		//运行到这里如果没有发生错误,则可以安全得提交数据
		delete[]  OperandDetail->DataHead;
		 OperandDetail->DataHead = temp;
		 OperandDetail->pSRP = temp + IntSize;
		if ( OperandDetail->LenInt > IntSize)			//为被截断的数字重新计算长度
			 OperandDetail->LenInt = IntSize;
		if ( OperandDetail->LenFloat > FloatSize)
			 OperandDetail->LenFloat = FloatSize;
		 OperandDetail->pSInt = OperandDetail->pSRP - OperandDetail->LenInt;
		 OperandDetail->pSFloat = OperandDetail->pSRP + 1;
		 OperandDetail->AllocInt = IntSize;
		 OperandDetail->AllocFloat = FloatSize;

	}
	catch (std::bad_alloc)
	{
		OperandDetail->Legal = false;
		throw BFException(ERR_MEMORYALLOCATEDEXCEPTION, "对象内存分配错误(String)", EXCEPTION_DETAIL);
	}
	return OperandDetail;
}

//将BF转化为数字进行输出(核心)
//此函数不要自己调用它,而应该调用它的封装版本
char* core_toString(BFDetail * OperandDetail,size_t &length, bool UseScinotation, bool ReserveZero)
{
	char *tempString = new char[OperandDetail->LenInt + OperandDetail->LenFloat + 7];	//新建一块足够存放数据的缓冲区;							//保存缓冲区地址
	int skip;											//被跳过的位数省略的位数用于最终计算指数
														//std::string RetVal;								//返回的字符串的长度
	size_t index_p = 0;									//写入位置标记
	size_t r_index;										//从元数据中读取数据的下标
	size_t sign = ScinotationLen;						//记录有效数字的个数(仅在科学计数法时使用)

	if (!OperandDetail->Legal)
	{
		//非法的数字输出Nan
		strcpy(tempString, "NaN");
		length = 3;
		return tempString;
	}
	else
	{
		if (OperandDetail->Minus)								//输出负号
		{
			tempString[index_p++] = '-';				//写入负号
			sign++;										//使负号不加入有效位
		}

		if (UseScinotation)
		{
			//使用科学计数法输出
			if (OperandDetail->LenInt == 1 && OperandDetail->pSInt[0] == '0')
			{
				//整数部分数据为0
				r_index = 0;
				skip = 0;

				while (OperandDetail->pSFloat[r_index] == '0' && r_index < OperandDetail->AllocFloat) r_index++;	//找到有效位

				if (OperandDetail->pSFloat[r_index] != 0)
				{
					skip = -(int)r_index - 1;								//计算省略的位数
					tempString[index_p++] = OperandDetail->pSFloat[r_index++];		//输出第一位
					if (OperandDetail->pSFloat[r_index] != 0)
					{
						//后面还有有效位,继续输出
						tempString[index_p++] = '.';						//输出小数点
						sign++;												//使小数点不加入有效位
						while (OperandDetail->pSFloat[r_index] != 0 && r_index < OperandDetail->AllocFloat&&index_p < sign)	//输出剩余有效位
							tempString[index_p++] = OperandDetail->pSFloat[r_index++];
						if (!ReserveZero)
						{
							index_p--;										//定位到上一个写入的位置
							while (tempString[index_p] == '0')index_p--;	//将有效数字尾部的'0'去除
							if (tempString[index_p] == '.') index_p--;		//如果遇到小数点,则也将小数点去掉
							tempString[++index_p] = 0;						//写入'\0'
						}
					}
				}
				else
				{
					//该值为0,写入0
					tempString[index_p++] = '0';
					tempString[index_p++] = 0;
				}
			}
			else
			{
				//整数部分数据不为0
				skip = (int)OperandDetail->LenInt - 1;
				tempString[index_p++] = OperandDetail->pSInt[0];							//把第一位输入
				if (OperandDetail->pSInt[index_p] != 0)
				{
					tempString[index_p++] = '.';
					size_t temp = ScinotationLen;
					if (temp > OperandDetail->LenInt)
						temp = OperandDetail->LenInt;
					strncpy(tempString + index_p, OperandDetail->pSInt + 1, temp - 1);
					index_p += temp - 1;
				}
				else if (OperandDetail->AllocFloat &&OperandDetail->pSFloat[0] != 0)
					tempString[index_p++] = '.';							//小数点后有数字

				if (OperandDetail->AllocFloat &&OperandDetail->pSFloat[0] != 0)
				{
					if (OperandDetail->LenInt < ScinotationLen)
					{
						//整数的有效位不够,继续拿小数位
						size_t temp = ScinotationLen - OperandDetail->LenInt;
						if (temp > OperandDetail->LenFloat)
							temp = OperandDetail->LenFloat;
						strncpy(tempString + index_p, OperandDetail->pSFloat, temp);
						index_p += temp;
					}
				}
				if (!ReserveZero)
				{
					index_p--;
					while (tempString[index_p] == '0')index_p--;	//将有效数字尾部的'0'去除
					if (tempString[index_p] == '.')index_p--;
					tempString[++index_p] = 0;						//写入'\0'
				}
			}

			if (skip) //输出指数
			{
				tempString[index_p++] = 'E';
				sprintf(tempString + index_p, "%d", skip);
			}
			while (tempString[index_p] != 0)index_p++;
			length = index_p;
		}
		else
		{
			//正常数字输出
			strncpy(tempString + index_p, OperandDetail->pSInt, OperandDetail->LenInt);

			index_p += OperandDetail->LenInt;
			if (OperandDetail->LenFloat)
			{
				r_index = 0;
				tempString[index_p++] = '.';
				while (OperandDetail->pSFloat[r_index] != 0)
					tempString[index_p++] = OperandDetail->pSFloat[r_index++];

				if (!ReserveZero)									//去除0
				{
					index_p--;										//指向上一个已写的位
					while (tempString[index_p] == '0') index_p--;	//删除为0的位
					if (tempString[index_p] == '.')index_p--;		//如果小数点被删光,则再删除小数点
					index_p++;										//指向下一个可写的位
				}
			}
			tempString[index_p] = 0;								//写入字符串结束符
			length = index_p;
		}
	}

	return tempString;
}


//将一个对象的值复制到当前对象中,两个对象相互独立
void core_CopyDetail(BFDetail* Dest, const BFDetail * Source)
{
	Dest->Minus = Source->Minus;
	Dest->Legal = Source->Legal;

	try
	{
		if (Dest->AllocInt < Source->LenInt)
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "整数部分内存不足", EXCEPTION_DETAIL);
		}
		if (Dest->AllocFloat < Source->LenFloat)
		{
			if (ConfirmWontLossAccuracy)
				throw BFException(ERR_MAYACCURACYLOSS, "小数部分内存不足", EXCEPTION_DETAIL);
		}
	}
	catch (BFException &e)
	{
		switch (e.GetID())
		{
		case ERR_NUMBERTOOBIG:
		case ERR_MAYACCURACYLOSS:
			if (AutoExpand)
			{
				core_Expand(Dest, Dest->AllocInt > Source->LenFloat ? Dest->AllocInt : Source->LenFloat,
					Dest->AllocFloat > Source->LenFloat ? Dest->AllocFloat : Source->LenFloat);
				break;

			}
		default:
			throw BFException(e);
		}
	}


	if (Dest->AllocInt >= Source->LenInt)
	{
		//空间足够复制,进行复制
		Dest->pSInt = Dest->pSRP - Source->LenInt;			//找到写入位置
		Dest->LenInt = Source->LenInt;
		strcpy(Dest->pSInt, Source->pSInt);
	}
	else
	{
		//截断性复制
		Dest->pSInt = Dest->DataHead;
		strncpy(Dest->pSInt, Source->pSInt + Source->LenInt - Dest->AllocInt, Dest->AllocInt);
		while (Dest->pSInt[0] == '0')Dest->pSInt++;					//去除整数前面的0
		Dest->LenInt = Dest->pSRP - Dest->pSInt;			//计算整数的长度
	}
	if (Source->LenFloat)
	{
		if (Dest->AllocFloat >= Source->LenFloat)
		{
			if (Source->pSFloat[0] != 0)
			{
				Dest->LenFloat = Source->LenFloat;
				strcpy(Dest->pSFloat, Source->pSFloat);
			}
		}
		else
		{
			Dest->LenFloat = Dest->AllocFloat;
			strncpy(Dest->pSFloat, Source->pSFloat, Dest->AllocFloat);
		}
	}
	else
	{
		//源为空
		if (!Dest->AllocFloat)
		{
			Dest->LenFloat = 0;
			Dest->pSFloat[0] = 0;
		}
	}

	return;
}



//比较两个数的大小
/*
返回值表示两个数的大小情况
0,	OperandA==OperandB
1,	OperandA>OperandB
-1,	OperandA<OperandB
2,	OperandA>>OperandB
-2,	OperandA<<OperandB
*/
int core_BFCmp(const BFDetail *OperandA, const BFDetail *OperandB)
{
	int minus = 1;				//当为负数时,返回的结果与正数相反,此变量控制的就是这个因数

								//判断负号
	if (OperandA->Minus)
	{
		if (OperandB->Minus)
			minus = -1;//A,B同负
		else
			return -2;//A负B正
	}
	else
	{
		if (OperandB->Minus)
			return 2;//A正B负
					 //else      //A正B正   minus默认为1,不需要改动
	}
	return core_BFCmp_abs(OperandA, OperandB, minus);

}
int core_BFCmp_abs(const BFDetail*OperandA, const BFDetail *OperandB)
{
	return core_BFCmp_abs(OperandA, OperandB, 1);
}
/*
比较两个数的绝对值大小
*/
int core_BFCmp_abs(const BFDetail*OperandA, const BFDetail *OperandB, int minus)
{
	//判断位
	if (OperandA->LenInt > OperandB->LenInt)
		return 2 * minus;
	else if (OperandA->LenInt < OperandB->LenInt)
		return -2 * minus;
	else
	{
		//如果两个数整数部分长度相等,则进行比较各个位
		int temp = strcmp(OperandA->pSInt, OperandB->pSInt);
		if (temp)
			return temp*minus;		//比较出结果,返回
		else
		{
			//整数部分完全相等,继续进行比较小数部分
			char *StringAH = OperandA->pSFloat,
				*StringBH = OperandB->pSFloat;

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

