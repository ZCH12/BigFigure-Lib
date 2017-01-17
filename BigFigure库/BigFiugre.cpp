#define _DLL_API_ 
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
	char *temp;			//新的内存空间

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
		if (IntSize >= Detail->LenInt)
			strncpy(temp + IntSize - Detail->LenInt, Detail->pSInt, Detail->LenInt);//小数部分安全得进行复制
		else
		{
			//整数部分将会被截断
			if (ConfirmWontLossHighBit)
			{
				delete[] temp;
				throw BFException(ERR_NUMBERTOOBIG, "修改大小之后对象中原来存放的值会被截断", EXCEPTION_DETAIL);
			}
			else
				strncpy(temp, Detail->pSInt + Detail->LenInt - IntSize, IntSize);//进行安全截断
		}
		if (FloatSize >= Detail->LenFloat)
			strncpy(temp + IntSize + 1, Detail->pSFloat, Detail->LenFloat + 1);//小数部分安全得进行复制

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
				strncpy(temp + IntSize + 1, Detail->pSFloat, FloatSize);
				temp[AllocatedMem - 1] = 0;		//写入结束符
			}
		}
		//运行到这里如果没有发生错误,则可以安全得提交数据
		delete[] Detail->DataHead;
		Detail->DataHead = temp;
		Detail->pSRP = temp + IntSize;
		if (Detail->LenInt > IntSize)
			Detail->LenInt = IntSize;
		if (Detail->LenFloat > FloatSize)
			Detail->LenFloat = FloatSize;
		Detail->pSInt = Detail->pSRP - Detail->LenInt;
		Detail->pSFloat = Detail->pSRP + 1;
		Detail->AllocInt = IntSize;
		Detail->AllocFloat = FloatSize;

	}
	catch (std::bad_alloc)
	{
		throw BFException(ERR_MEMORYALLOCATEDEXCEPTION, "对象内存分配错误(String)", EXCEPTION_DETAIL);
	}

	return *this;
}

/*
将当前对象的大小调整为调整为根Source一致(如果大则调小,如果小则调大)
*/
BigFigure & BigFigure::Expand(const BigFigure & Source)
{
	return Expand(Source.Detail->AllocInt, Source.Detail->AllocFloat);
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
				skip = (int)index_p;															//初始化跳过的为
				while (*(SourceStr) == '0')SourceStr++, index_p--;						//跳过数字前边的0
				skip = ~(skip - (int)index_p + 1) + 1;										//取得跳过的有效位的位数再取相反数
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
		catch (BFException e)
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
				throw e;
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
				temp = expon - (int)Detail->AllocInt;							//忽略的位数
				strcpy(Detail->pSInt, tempStr + temp);
				memset(Detail->pSInt + size - temp - 1, '0', Detail->AllocInt - (size - temp));
			}
			if (Detail->AllocFloat)
			{
				if (Detail->LenFloat <= Detail->AllocFloat)
				{
					temp = (int)(size - Detail->LenInt);
					if (temp > 0)
						strncpy(Detail->pSFloat, tempStr + Detail->LenInt, temp);	//写入小数位
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
			index_p = ~expon;//~expon+1-1				//计算前置输出的0的个数
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

//将一个对象的值复制到当前对象中,两个对象相互独立
BigFigure & BigFigure::CopyDetail(const BigFigure & Source)
{
	this->Detail->Minus = Source.Detail->Minus;
	this->Detail->Legal = Source.Detail->Legal;

	try
	{
		if (this->Detail->AllocInt < Source.Detail->LenInt)
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "整数部分内存不足", EXCEPTION_DETAIL);
		}
		if (this->Detail->AllocFloat < Source.Detail->LenFloat)
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
				Expand(this->Detail->AllocInt > Source.Detail->LenFloat ? this->Detail->AllocInt : Source.Detail->LenFloat,
					this->Detail->AllocFloat > Source.Detail->LenFloat ? this->Detail->AllocFloat : Source.Detail->LenFloat);
				break;

			}
		default:
			throw BFException(e);
		}
	}


	if (this->Detail->AllocInt >= Source.Detail->LenInt)
	{
		//空间足够复制,进行复制
		this->Detail->pSInt = this->Detail->pSRP - Source.Detail->LenInt;	//找到写入位置
		this->Detail->LenInt = Source.Detail->LenInt;
		strcpy(this->Detail->pSInt, Source.Detail->pSInt);
	}
	else
	{
		//截断性复制
		this->Detail->pSInt = this->Detail->DataHead;
		strncpy(this->Detail->pSInt, Source.Detail->pSInt + Source.Detail->LenInt - this->Detail->AllocInt, this->Detail->AllocInt);
		while (this->Detail->pSInt[0] == '0')this->Detail->pSInt++;			//去除整数前面的0
		this->Detail->LenInt = this->Detail->pSRP - this->Detail->pSInt;	//计算整数的长度
	}
	if (Source.Detail->LenFloat)
	{
		if (this->Detail->AllocFloat >= Source.Detail->LenFloat)
		{
			if (Source.Detail->pSFloat[0] != 0)
			{
				this->Detail->LenFloat = Source.Detail->LenFloat;
				strcpy(this->Detail->pSFloat, Source.Detail->pSFloat);
			}
		}
		else
		{
			this->Detail->LenFloat = this->Detail->AllocFloat;
			strncpy(this->Detail->pSFloat, Source.Detail->pSFloat, this->Detail->AllocFloat);
		}
	}
	else
	{
		//源为空
		if (!this->Detail->AllocFloat)
		{
			this->Detail->LenFloat = 0;
			this->Detail->pSFloat[0] = 0;
		}
	}

	return *this;
}

//将BF转化为数字进行输出(核心)
//此函数不要自己调用它,而应该调用它的封装版本
char* BigFigure::_toString(size_t &length, bool UseScinotation, bool ReserveZero)
{
	char *tempString = new char[Detail->LenInt + Detail->LenFloat + 7];	//新建一块足够存放数据的缓冲区;							//保存缓冲区地址
	int skip;											//被跳过的位数省略的位数用于最终计算指数
	//std::string RetVal;								//返回的字符串的长度
	size_t index_p = 0;									//写入位置标记
	size_t r_index;										//从元数据中读取数据的下标
	size_t sign = ScinotationLen;						//记录有效数字的个数(仅在科学计数法时使用)

	if (!Detail->Legal)
	{
		//非法的数字输出Nan
		strcpy(tempString, "NaN");
		length = 3;
		return tempString;
	}
	else
	{
		if (Detail->Minus)								//输出负号
		{
			tempString[index_p++] = '-';				//写入负号
			sign++;										//使负号不加入有效位
		}

		if (UseScinotation)
		{
			//使用科学计数法输出
			if (Detail->pSInt[0] == '0' && Detail->LenInt == 1)
			{
				//整数部分数据为0
				r_index = 0;
				skip = 0;

				while (Detail->pSFloat[r_index] == '0' && r_index < Detail->AllocFloat) r_index++;	//找到有效位

				if (Detail->pSFloat[r_index] != 0)
				{
					skip = -(int)r_index - 1;								//计算省略的位数
					tempString[index_p++] = Detail->pSFloat[r_index++];		//输出第一位
					if (Detail->pSFloat[r_index] != 0)
					{
						//后面还有有效位,继续输出
						tempString[index_p++] = '.';						//输出小数点
						sign++;												//使小数点不加入有效位
						while (Detail->pSFloat[r_index] != 0 && r_index < Detail->AllocFloat&&index_p < sign)	//输出剩余有效位
							tempString[index_p++] = Detail->pSFloat[r_index++];
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
				skip = (int)Detail->LenInt - 1;
				tempString[index_p++] = Detail->pSInt[0];							//把第一位输入
				if (Detail->pSInt[index_p] != 0)
				{
					tempString[index_p++] = '.';
					size_t temp = ScinotationLen;
					if (temp > Detail->LenInt)
						temp = Detail->LenInt;
					strncpy(tempString + index_p, Detail->pSInt + 1, temp - 1);
					index_p += temp - 1;
				}
				else if (Detail->AllocFloat &&Detail->pSFloat[0] != 0)
					tempString[index_p++] = '.';							//小数点后有数字

				if (Detail->AllocFloat &&Detail->pSFloat[0] != 0)
				{
					if (Detail->LenInt < ScinotationLen)
					{
						//整数的有效位不够,继续拿小数位
						size_t temp = ScinotationLen - Detail->LenInt;
						if (temp > Detail->LenFloat)
							temp = Detail->LenFloat;
						strncpy(tempString + index_p, Detail->pSFloat, temp);
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
			strncpy(tempString + index_p, Detail->pSInt, Detail->LenInt);

			index_p += Detail->LenInt;
			if (Detail->LenFloat)
			{
				r_index = 0;
				tempString[index_p++] = '.';
				while (Detail->pSFloat[r_index] != 0)
					tempString[index_p++] = Detail->pSFloat[r_index++];

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

std::string BigFigure::toString()
{
	size_t a;
	char *temp = _toString(a, ScinotationShow, ReserveZero);
	std::string ReturnVal = std::string(temp, a);
	delete[] temp;
	return ReturnVal;
}
std::string BigFigure::toString(bool UseScinotation, bool ReserveZero)
{
	size_t a;
	char *temp = _toString(a, UseScinotation, ReserveZero);
	std::string ReturnVal = std::string(temp, a);
	delete[] temp;
	return ReturnVal;
}

BFString BigFigure::toBFString()
{
	size_t a;
	char *temp = _toString(a, ScinotationShow, ReserveZero);
	BFString ReturnVal(temp, a);
	delete[] temp;
	return ReturnVal;
}
BFString BigFigure::toBFString(bool UseScinotation, bool ReserveZero)
{
	size_t a;
	char *temp = _toString(a, UseScinotation, ReserveZero);
	BFString ReturnVal(temp, a);
	delete[] temp;
	return ReturnVal;
}


char* BigFigure::toCString(char *result)
{
	size_t a;
	strcpy(result, _toString(a, ScinotationShow, ReserveZero));
	return result;
}
char* BigFigure::toCString(char *result, bool UseScinotation, bool ReserveZero)
{
	size_t a;
	strcpy(result, _toString(a, UseScinotation, ReserveZero));
	return result;
}
