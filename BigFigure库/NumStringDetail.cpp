#define _DLL_API_ 
#include "NumStringDetail.h"

inline void NumStringDetail::NumCheck(const char * NumString)
{
	const char *tempString = NumString;

	size_t StringLen = strlen(NumString);
	if (StringLen == 0)
		throw BFException(ERR_ILLEGALNUMBER, "字符串不能为空", EXCEPTION_DETAIL);

	if (this->NumString&&this->NumString != NumString)
	{
		this->NumString = new char[StringLen + 1];
		strcpy(this->NumString, NumString);
	}

	bool HasNumPre = false;			//记录在指定符号前方是否有数字
	bool HasPoint = false;			//记录是否有小数点
	bool Scinotation = false;		//记录是否以科学计数法进行标记
	bool IntBeZero = true;			//记录整数部分是否为0,用于决定浮点的下标是否等找到有效位再设置


	//开始遍历
	for (; *tempString != 0; tempString++)
	{
		//开始进行判断
		if (*tempString > '0'&&*tempString <= '9')
		{
			if (!HasNumPre)
			{
				if (Scinotation)
					this->ExpStart_p = tempString - NumString;
				else
					if (HasPoint)
						this->FloatStart_p = tempString - NumString;
					else
						if (IntBeZero)
						{
							//整数部分,设置第一个有效数字的字符串的指针
							this->IntStart_p = tempString - NumString;		//整数为0时,发现第一个有效的位,记载整数开始的位置
							IntBeZero = false;
						}
				HasNumPre = true;
			}
			continue;
		}
		else if (*tempString == '0')
		{

			if (!HasNumPre&&HasPoint && !Scinotation)
			{
				this->FloatStart_p = tempString - NumString;
				HasNumPre = true;
			}
			continue;
		}
		else if (*tempString == '-')
		{
			if (HasNumPre || !(HasNumPre || Scinotation) && HasPoint)
			{
				//将负号掺杂在数字中间,数字无效
				throw BFException(ERR_ILLEGALNUMBER, "数字中间不能掺杂负号", EXCEPTION_DETAIL);
			}
			if (Scinotation)
			{
				if (this->ExpMinus)
				{
					//在指数部分已有负号时(存在两个负号),数字无效
					throw BFException(ERR_ILLEGALNUMBER, "不能存在多个负号", EXCEPTION_DETAIL);
				}
				else
					this->ExpMinus = 1;
			}
			else
			{
				//处理底数部分
				if (this->RadixMinus)
				{
					//在底数部分已有负号时(存在两个负号),数字无效
					throw BFException(ERR_ILLEGALNUMBER, "不能存在多个负号", EXCEPTION_DETAIL);
				}
				else
					this->RadixMinus = 1;
			}
			continue;
		}
		else if (*tempString == '.')
		{
			if (Scinotation) {
				//指数部分有小数点,数字无效
				throw BFException(ERR_ILLEGALNUMBER, "指数部分只能为整数", EXCEPTION_DETAIL);
			}
			else {
				if (HasPoint)
				{
					//发现两个小数点,数字无效
					throw BFException(ERR_ILLEGALNUMBER, "一个数字只能有一个小数点", EXCEPTION_DETAIL);
				}
				else
				{
					//保存整数的长度

					if (HasNumPre)
						this->IntLen = tempString - NumString - this->IntStart_p;		//计算整数部分的长度
					else
					{
						IntBeZero = true;
						this->IntLen = 0;
					}
					HasPoint = true;
					HasNumPre = false;
				}
			}
			continue;
		}
		else if (*tempString == 'E' || *tempString == 'e')
		{
			if (Scinotation)
			{
				//一个数字出现两个E,数字错误
				throw BFException(ERR_ILLEGALNUMBER, "一个数字智能有一个指数", EXCEPTION_DETAIL);
			}
			else
			{
				if (HasPoint)
				{
					//底数为小数
					if (HasNumPre)
						this->FloatLen = tempString - NumString - this->FloatStart_p;	//计算小数部分长度,如果小数部分没有数字,则关闭标记
					else
						HasPoint = false;
				}
				else
				{
					//在底数为整数
					//保存整数的长度
					this->IntLen = tempString - NumString - this->IntStart_p;
				}
				Scinotation = true;
				HasNumPre = false;
			}
			continue;
		}
		else if (*tempString == '+')
		{
			if (HasNumPre || (!HasNumPre&&HasPoint && !Scinotation))
			{
				//将正号掺杂在数字中间,数字无效
				throw BFException(ERR_ILLEGALNUMBER, "数字中间不能掺杂加号", EXCEPTION_DETAIL);
			}
			continue;
		}
		else {
			//发现其他字符,数字非法
			throw BFException(ERR_ILLEGALNUMBER, "数字字符串不能掺杂其他无关字符", EXCEPTION_DETAIL);
		}
	}

	//结束for
	if (HasPoint)
	{
		const char *temp = NumString + this->FloatStart_p;
		while (this->FloatLen&& temp[this->FloatLen] == '0')	//去除小数点后的'0'
			this->FloatLen--;
	}

	this->IntBeZero = IntBeZero;
	if (Scinotation)			//判断是否为科学计数法表示的数
	{
		//科学计数法表示的数
		if (HasNumPre)			//判断指数位是否有数字
		{
			this->Mode = 2;		//初始化为2,Mode将为3,4
			this->ExpLen = tempString - NumString - this->ExpStart_p;	//计算指数位的长度
		}
		else
		{
			this->Mode = 0;		//初始化为0,Mode将为1,2
		}
	}
	else
	{
		//用标准表示法表示的数字
		this->Mode = 0;		//初始化为0,Mode将为1,2
		if (HasPoint)		//计算还未计算的长度值
			this->FloatLen = tempString - NumString - this->FloatStart_p;
		else
			this->IntLen = tempString - NumString - this->IntStart_p;
	}

	if (HasPoint)					//判断是否为小数
	{
		//小数,但进一步判断小数位的长度,如果小数位为0,则还是小数
		if (this->FloatLen)			//如果小数位有数字,则数字是小数
		{
			this->Mode += 2;
		}
		else
		{
			this->Mode += 1;
		}
	}
	else
	{
		//整数
		this->Mode += 1;
	}
	if ((!this->FloatLen&&IntBeZero) || !(HasNumPre || HasPoint))
	{
		//数字为0,两种情况:
		//1.整数为0
		//2.小数为0
		this->IntStart_p = 0;
		this->IntLen = 1;
		this->RadixMinus = false;
		strcpy(this->NumString, "0");
	}

	return;
}

NumStringDetail::NumStringDetail(const char * NumString) :
	RadixMinus(0), ExpMinus(0), IntBeZero(0), Mode(0), IntLen(0), FloatLen(0), ExpLen(0), IntStart_p(0), FloatStart_p(0), ExpStart_p(0)
{
	NumCheck(NumString);
}

NumStringDetail::~NumStringDetail()
{
	delete[] NumString;
}


NumStringDetail::NumStringDetail(double Num) :
	RadixMinus(0), ExpMinus(0), IntBeZero(0), Mode(0), IntLen(0), FloatLen(0), ExpLen(0), IntStart_p(0), FloatStart_p(0), ExpStart_p(0)
{
	this->NumString = new char[32];
	_gcvt(Num, 16, this->NumString);
	NumCheck(this->NumString);
}
NumStringDetail::NumStringDetail(__int64 Num) :
	RadixMinus(0), ExpMinus(0), IntBeZero(0), Mode(0), IntLen(0), FloatLen(0), ExpLen(0), IntStart_p(0), FloatStart_p(0), ExpStart_p(0)
{
	this->NumString = new char[32];
	_i64toa(Num, this->NumString, 10);
	NumCheck(this->NumString);

}
NumStringDetail::NumStringDetail(long Num) :
	RadixMinus(0), ExpMinus(0), IntBeZero(0), Mode(0), IntLen(0), FloatLen(0), ExpLen(0), IntStart_p(0), FloatStart_p(0), ExpStart_p(0)
{
	this->NumString = new char[32];
	ltoa(Num, this->NumString, 10);
	NumCheck(this->NumString);
}
NumStringDetail::NumStringDetail(int Num) :
	RadixMinus(0), ExpMinus(0), IntBeZero(0), Mode(0), IntLen(0), FloatLen(0), ExpLen(0), IntStart_p(0), FloatStart_p(0), ExpStart_p(0)
{
	this->NumString = new char[16];
	itoa(Num, this->NumString, 10);
	NumCheck(this->NumString);
}
