#include "BigFiugre.h"

bool ReserveZero = true;				//保留小数后的0
bool ScinotationShow = false;			//不以科学计数法显示数字
bool ConfirmWontLossAccuracy = true;	//确保不截断小数的有效位(关闭之后如果赋值时小数位太多,则会省略多出的部分;如果开启,则会抛出异常)
bool ConfirmWontLossHighBit = true;		//确保不丢失整数的高位(如果发生溢出时),如果为true,则发生溢出时会抛出异常,如果为false,则如果溢出则舍弃高位
bool AutoExpand = true;					//在内存不足时自动拓展空间
size_t ScinotationLen = 5;				//科学计数法时有效位数为5位


/*
构造一个BF对象
@param IntSize 整数部分分配的内存
@param FloatSize 小数部分分配的内存
*/
BigFigure::BigFigure(size_t IntSize, size_t FloatSize)

{
	int AllocatedMem;
	if (IntSize == 0)
		throw BFException(ERR_ILLEGALPARAM, "整数部分分配的内存不能为0", EXCEPTION_DETAIL);
	AllocatedMem = (int)IntSize + 1;
	if (FloatSize > 0)
		AllocatedMem += (int)FloatSize + 1;
	try {
		Detail = new BFDetail;
		Detail->AllocFloat = FloatSize;
		Detail->AllocInt = IntSize;
		Detail->ReferCount = 1;							//初始化引用计数
		Detail->LenInt = 1;								//初始化的长度为0,为了保存'0'
		Detail->LenFloat = 0;							//小数部分长度为0,因为没有值
		Detail->Minus = false;							//为无负号
		Detail->Legal = true;							//标记为合法数字
		try {
			Detail->DataHead = new char[AllocatedMem];	//分配内存
			Detail->pSRP = Detail->DataHead + IntSize;	//计算小数点所在的位置
			*Detail->pSRP = 0;							//将小数点所在的位标记为0,它同时也是整数部分输出时的结束符
			Detail->pSInt = Detail->pSRP - 1;			//计算"0"的起始写入位置
			Detail->pSInt[0] = '0';						//写入数字

			if (FloatSize > 0) {
				Detail->pSFloat = Detail->pSRP + 1;		//计算小数位的首地址
				Detail->pSFloat[0] = 0;					//小数点后默认没有数字(初始化为空字符串)
			}

		}
		catch (const std::bad_alloc)
		{
			delete Detail;
			throw BFException(ERR_MEMORYALLOCATEDEXCEPTION, "对象内存分配错误(String)", EXCEPTION_DETAIL);
		}
	}
	catch (const std::bad_alloc)
	{
		throw BFException(ERR_MEMORYALLOCATEDEXCEPTION, "对象内存分配错误(Detial)", EXCEPTION_DETAIL);
	}
	catch (const BFException e)
	{
		throw e;
	}
}

/*
拷贝一个BF
注意,调用此拷贝构造函数得到的是两个共用内存的对象,两个对象本质上是同一个对象
*/
BigFigure::BigFigure(const BigFigure & Base)
{
	this->Detail = Base.Detail;
	this->Detail->ReferCount++;
}

/*
拷贝一个BF
注意,调用此拷贝构造函数得到的是两个共用内存的对象,两个对象本质上是同一个对象

BigFigure::BigFigure(const _BigFigure & Base)
{
	this->Detail = Base.Detail;
	this->Detail->ReferCount++;
}
*/
/*
析构函数
用于释放对象
*/
BigFigure::~BigFigure()
{
	Detail->ReferCount--;
	if (!Detail->ReferCount)
	{
		//当引用计数为0时才释放Detail所占的内存
		delete[] Detail->DataHead;
		delete Detail;
	}
	return;
}



/*
重新分配对象内存(保留数据),
如果缩小内存则可能导致数据丢失
@param IntSize 新的整数部分分配的内存
@param FloatSize 新的小数部分分配的内存
*/
BigFigure& BigFigure::Expand(size_t IntSize, size_t FloatSize)
{
	core_Expand(Detail, IntSize, FloatSize);
	return *this;
}

/*
将当前对象的大小调整为调整为根Source一致(如果大则调小,如果小则调大)
*/
BigFigure & BigFigure::Expand(const BigFigure & Source)
{
	core_Expand(Detail, Source.Detail->AllocInt, Source.Detail->AllocFloat);
	return *this;
}

BigFigure& BigFigure::toBF(NumStringDetail &NumStringDetail)
{
	int temp;
	char *tempStr;
	const char *SourceStr;
	size_t index_p, size;
	int skip;
	int expon;							//科学计数法的指数部分

	Detail->Minus = NumStringDetail.RadixMinus ? true : false;

	switch (NumStringDetail.Mode)
	{
	case 1://整数
	case 2://小数
		if (Detail->AllocInt < NumStringDetail.IntLen)
		{
			//内存空间不足
			if (AutoExpand)
			{
				//自动进行拓展
				Expand(NumStringDetail.IntLen, NumStringDetail.FloatLen > Detail->AllocFloat ? NumStringDetail.FloatLen : Detail->AllocFloat);
			}
			else {
				//如果发生溢出,则判断是否要截断
				if (ConfirmWontLossHighBit)
				{
					Detail->Legal = false;
					throw BFException(ERR_NUMBERTOOBIG, "对于目标对象,传入的数据太大,该对象无法容纳", EXCEPTION_DETAIL);
				}
			}
		}

		if (Detail->AllocInt >= NumStringDetail.IntLen)
		{
			//正常情况下

			if (NumStringDetail.IntBeZero)
			{
				//整数部分为0,则写入"0"
				Detail->pSInt = Detail->pSRP - 1;
				Detail->pSInt[0] = '0';
				Detail->LenInt = 1;
			}
			else
			{
				//整数部分不为0,则拷贝整数部分进入对象的字符串中
				Detail->pSInt = Detail->pSRP - NumStringDetail.IntLen;
				Detail->LenInt = NumStringDetail.IntLen;
				strncpy(Detail->pSInt, NumStringDetail.NumString + NumStringDetail.IntStart_p, NumStringDetail.IntLen);
			}
		}
		else
		{
			//截断型复制(整数部分)
			Detail->pSInt = Detail->DataHead;
			Detail->LenInt = Detail->AllocInt;
			temp = (int)(NumStringDetail.IntLen - Detail->AllocInt);	//计算要进行舍弃的位数
			strncpy(Detail->pSInt, NumStringDetail.NumString + NumStringDetail.IntStart_p + temp, Detail->AllocInt);
		}

		//判断数字的类型,如果是整数,则这个函数就是出口
		if (NumStringDetail.Mode == 1)
		{
			Detail->Legal = true;
			if (Detail->AllocFloat)
			{
				Detail->pSFloat[0] = 0;		//如果有分配小数位内存,则将小数设为0
				Detail->LenFloat = 0;
			}
			return *this;
		}

		//以下为小数部分的处理

		if (NumStringDetail.FloatLen > Detail->AllocFloat || !Detail->AllocFloat)
		{
			if (AutoExpand)
			{
				Expand(Detail->AllocInt, NumStringDetail.FloatLen);
			}
			else
			{
				if (ConfirmWontLossAccuracy)
				{
					Detail->Legal = false;
					throw BFException(ERR_MAYACCURACYLOSS, "写入的有效位数多于目标对象的容量,可能丢失数据", EXCEPTION_DETAIL);
				}
			}
		}

		if (Detail->AllocFloat)
		{
			//如果当前对象有小数位,则可能可以进行写入
			if (NumStringDetail.FloatLen <= Detail->AllocFloat)
			{
				//如果内存足够存放,则进行复制
				Detail->LenFloat = NumStringDetail.FloatLen;
				strncpy(Detail->pSFloat, NumStringDetail.NumString + NumStringDetail.FloatStart_p, NumStringDetail.FloatLen);
				Detail->pSFloat[Detail->LenFloat] = 0;
			}
			else
			{
				//截断式复制
				Detail->LenFloat = Detail->AllocFloat;
				strncpy(Detail->pSFloat, NumStringDetail.NumString + NumStringDetail.FloatStart_p, NumStringDetail.FloatLen);
				Detail->pSFloat[Detail->LenFloat] = 0;
			}
		}
		Detail->Legal = true;
		return *this;
	case 3://科学计数法表示的以整数为底数的数字
	case 4://科学计数法表示的以小数为底数的数字
		tempStr = new char[NumStringDetail.IntLen + NumStringDetail.FloatLen + 1];		//临时存放有效位的字符串
		SourceStr = NumStringDetail.NumString + NumStringDetail.FloatStart_p;			//存放小数位的首指针
		index_p = 0;

		//取出有效位
		if (NumStringDetail.Mode == 3)
		{
			//当底为整数时
			strncpy(tempStr, NumStringDetail.NumString + NumStringDetail.IntStart_p, NumStringDetail.IntLen);
			expon = atoi(NumStringDetail.NumString + NumStringDetail.ExpStart_p);		//取得指数
			index_p = NumStringDetail.IntLen - 1;										//取得tempStr的结尾的下标

			if (NumStringDetail.ExpMinus)								//在这里,index_p的值是跳过的位数
				expon = ~expon + 1 + (int)index_p;							//指数为负数,取相反数再加上被跳过的数的个数,得到新的指数
			else
				expon += (int)index_p;										//指数为正数,直接加上被跳过的数的个数,得到新的指数

			while (index_p != 0 && tempStr[index_p] == '0')index_p--;	//去除末尾的0
			tempStr[++index_p] = 0;										//写入结束符,
			size = index_p;
		}
		else
		{
			//当底为小数时
			if (!NumStringDetail.IntBeZero)
			{
				//整数部分不为0
				strncpy(tempStr, NumStringDetail.NumString + NumStringDetail.IntStart_p, NumStringDetail.IntLen);	//复制整数部分的有效数字
				index_p = NumStringDetail.IntLen;
				skip = (int)index_p - 1;
				strncpy(tempStr + index_p, SourceStr, NumStringDetail.FloatLen);		//复制小数部分的有效数字
				index_p += NumStringDetail.FloatLen;									//将小数位的长度加上
				index_p--;																//回到上一个写入位置
				while (index_p != 0 && tempStr[index_p] == '0')index_p--;				//去除末尾的'0'
				tempStr[++index_p] = 0;													//写入结束符,确认截断
			}
			else {
				//整数部分为0
				index_p = NumStringDetail.FloatLen;
				skip = (int)index_p;													//初始化跳过的为
				while (*(SourceStr) == '0')SourceStr++, index_p--;						//跳过数字前边的0
				skip = ~(skip - (int)index_p + 1) + 1;									//取得跳过的有效位的位数再取相反数
				strncpy(tempStr, SourceStr, index_p);									//复制有效数字
				while (index_p != 0 && tempStr[index_p] == '0')index_p--;				//去除末尾的0
				tempStr[index_p++] = 0;
			}
			size = index_p;
			expon = atoi(NumStringDetail.NumString + NumStringDetail.ExpStart_p);
			if (NumStringDetail.ExpMinus)
				expon = ~expon + 1 + skip;		//指数为负数,再加上被跳过的位数(skip可能正也可能负,取决于小数点向前偏移还是向后偏移)
			else
				expon += skip;					//指数为正数
		}

		//开始输入到对象中
		{
			//计算写入后的长度
			int temp = expon + 1;
			Detail->LenInt = temp > 0 ? temp : 1;
			temp = (int)size - temp;
			Detail->LenFloat = temp > 0 ? temp : 0;
		}

		try
		{
			if (Detail->AllocInt < Detail->LenInt)
			{
				if (ConfirmWontLossHighBit)
					throw BFException(ERR_NUMBERTOOBIG, "传入的值太大,无法存储,请进行拓展", EXCEPTION_DETAIL);
			}
			if (Detail->AllocFloat < Detail->LenFloat)
			{
				if (ConfirmWontLossAccuracy)
					throw BFException(ERR_MAYACCURACYLOSS, "传入的值小数位过多,将丢失精度", EXCEPTION_DETAIL);
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
					Expand(Detail->AllocInt > Detail->LenInt ? Detail->AllocInt : Detail->LenInt,
						Detail->AllocFloat > Detail->LenFloat ? Detail->AllocFloat : Detail->LenFloat);
					break;
				}
			default:
				Detail->Legal = false;
				throw BFException(e);
			}
		}

		if (expon > 0)				//指数大于0的处理方案
		{
			if (Detail->LenInt <= Detail->AllocInt)
			{
				//小数位一定为0
				Detail->pSInt = Detail->pSRP - Detail->LenInt;				//计算整数部分写入位置
				strncpy(Detail->pSInt, tempStr, Detail->LenInt);			//复制有效位
				temp = (int)(Detail->LenInt - size);
				if (temp > 0)
					memset(Detail->pSInt + size, '0', temp);				//空位用0填充
			}
			else
			{
				//进行截断性复制
				Detail->pSInt = Detail->DataHead;
				Detail->LenInt = Detail->AllocInt;
				temp = expon - (int)Detail->AllocInt;						//忽略的位数
				strcpy(Detail->pSInt, tempStr + temp);
				memset(Detail->pSInt + size - temp - 1, '0', Detail->AllocInt - (size - temp));
			}
			if (Detail->AllocFloat)
			{
				if (Detail->LenFloat <= Detail->AllocFloat)
				{
					temp = (int)(size - Detail->LenInt);
					if (temp > 0)
						strncpy(Detail->pSFloat, tempStr + Detail->LenInt, temp);			//写入小数位
					else
						temp = 0;
					Detail->pSFloat[temp] = 0;
				}
				else
				{
					Detail->LenFloat = Detail->AllocFloat;
					strncpy(Detail->pSFloat, tempStr + Detail->LenInt, Detail->AllocFloat);	//截断性复制
					Detail->pSFloat[Detail->LenFloat] = 0;									//写入结束符
				}
			}
		}
		else if (expon < 0)				//指数小于0时的处理方案
		{
			//指数小于0,则整数部分一定为0
			Detail->pSInt = Detail->pSRP - 1;
			Detail->pSInt[0] = '0';
			Detail->LenInt = 1;
			index_p = ~expon;//~expon+1-1													//计算前置输出的0的个数
			memset(Detail->pSFloat, '0', index_p);

			if (Detail->AllocFloat)
			{
				if (Detail->LenFloat <= Detail->AllocFloat)
				{
					//内存足够,直接复制
					strncpy(Detail->pSFloat + index_p, tempStr, size);						//写入有效位
					Detail->pSFloat[index_p + size] = 0;
				}
				else
				{
					//内存不足,进行截断性复制
					strncpy(Detail->pSFloat + index_p, tempStr, Detail->AllocFloat - index_p);
					Detail->LenFloat = Detail->AllocFloat;
					Detail->pSFloat[Detail->AllocFloat] = 0;
				}
			}
		}
		else
		{
			//指数等于0的处理方案
			Detail->pSInt = Detail->pSRP - 1;
			Detail->pSInt[0] = tempStr[0];
			Detail->LenInt = 1;
			Detail->LenFloat = size - 1;

			if (Detail->LenFloat <= Detail->AllocFloat)
			{
				//精确度大于有效位数,能够完全存下
				strcpy(Detail->pSFloat, tempStr + 1);
			}
			else
			{
				Detail->LenFloat = Detail->AllocFloat;
				strncpy(Detail->pSFloat, tempStr + 1, Detail->LenFloat);
			}
		}

		Detail->Legal = true;
		delete[] tempStr;
		return *this;
	case 0:
	default:
		Detail->Legal = false;
		throw BFException(ERR_ILLEGALNUMBER, "不是一个合法的数字", EXCEPTION_DETAIL);
		break;
	}
	return *this;
}

std::string BigFigure::toString()
{
	size_t a;
	char *temp = core_toString(Detail, a, ScinotationShow, ReserveZero);
	std::string ReturnVal = std::string(temp, a);
	delete[] temp;
	return ReturnVal;
}
std::string BigFigure::toString(bool UseScinotation, bool ReserveZero)
{
	size_t a;
	char *temp = core_toString(Detail, a, UseScinotation, ReserveZero);
	std::string ReturnVal = std::string(temp, a);
	delete[] temp;
	return ReturnVal;
}

BFString BigFigure::toBFString()
{
	size_t a;
	char *temp = core_toString(Detail, a, ScinotationShow, ReserveZero);
	BFString ReturnVal(temp, a);
	delete[] temp;
	return ReturnVal;
}
BFString BigFigure::toBFString(bool UseScinotation, bool ReserveZero)
{
	size_t a;
	char *temp = core_toString(Detail, a, UseScinotation, ReserveZero);
	BFString ReturnVal(temp, a);
	delete[] temp;
	return ReturnVal;
}

char* BigFigure::toCString(char *result)
{
	size_t a;
	char *temp = core_toString(Detail, a, ScinotationShow, ReserveZero);
	strcpy(result, temp);
	delete temp;
	return result;
}
char* BigFigure::toCString(char *result, bool UseScinotation, bool ReserveZero)
{
	size_t a;
	char *temp = core_toString(Detail, a, UseScinotation, ReserveZero);
	strcpy(result, temp);
	delete temp;
	return result;
}
