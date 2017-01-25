#include "_BigFigure.h"

/*
构造一个BF对象
@param IntSize 整数部分分配的内存
@param FloatSize 小数部分分配的内存
*/
_BigFigure::_BigFigure(size_t IntSize, size_t FloatSize)

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
_BigFigure::_BigFigure(const _BigFigure & Base)
{
	this->Detail = Base.Detail;
	this->Detail->ReferCount++;
}

_BigFigure::_BigFigure(const BigFigure & Base)
{
	this->Detail = Base.Detail;
	this->Detail->ReferCount++;
}

/*
析构函数
用于释放对象
*/
_BigFigure::~_BigFigure()
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
_BigFigure::operator BigFigure()
{
	return (BigFigure)*this;
}*/