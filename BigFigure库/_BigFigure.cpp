#include "_BigFigure.h"

/*
����һ��BF����
@param IntSize �������ַ�����ڴ�
@param FloatSize С�����ַ�����ڴ�
*/
_BigFigure::_BigFigure(size_t IntSize, size_t FloatSize)

{
	int AllocatedMem;
	if (IntSize == 0)
		throw BFException(ERR_ILLEGALPARAM, "�������ַ�����ڴ治��Ϊ0", EXCEPTION_DETAIL);
	AllocatedMem = (int)IntSize + 1;
	if (FloatSize > 0)
		AllocatedMem += (int)FloatSize + 1;
	try {
		Detail = new BFDetail;
		Detail->AllocFloat = FloatSize;
		Detail->AllocInt = IntSize;
		Detail->ReferCount = 1;							//��ʼ�����ü���
		Detail->LenInt = 1;								//��ʼ���ĳ���Ϊ0,Ϊ�˱���'0'
		Detail->LenFloat = 0;							//С�����ֳ���Ϊ0,��Ϊû��ֵ
		Detail->Minus = false;							//Ϊ�޸���
		Detail->Legal = true;							//���Ϊ�Ϸ�����
		try {
			Detail->DataHead = new char[AllocatedMem];	//�����ڴ�
			Detail->pSRP = Detail->DataHead + IntSize;	//����С�������ڵ�λ��
			*Detail->pSRP = 0;							//��С�������ڵ�λ���Ϊ0,��ͬʱҲ�������������ʱ�Ľ�����
			Detail->pSInt = Detail->pSRP - 1;			//����"0"����ʼд��λ��
			Detail->pSInt[0] = '0';						//д������

			if (FloatSize > 0) {
				Detail->pSFloat = Detail->pSRP + 1;		//����С��λ���׵�ַ
				Detail->pSFloat[0] = 0;					//С�����Ĭ��û������(��ʼ��Ϊ���ַ���)
			}

		}
		catch (const std::bad_alloc)
		{
			delete Detail;
			throw BFException(ERR_MEMORYALLOCATEDEXCEPTION, "�����ڴ�������(String)", EXCEPTION_DETAIL);
		}
	}
	catch (const std::bad_alloc)
	{
		throw BFException(ERR_MEMORYALLOCATEDEXCEPTION, "�����ڴ�������(Detial)", EXCEPTION_DETAIL);
	}
	catch (const BFException e)
	{
		throw e;
	}
}

/*
����һ��BF
ע��,���ô˿������캯���õ��������������ڴ�Ķ���,��������������ͬһ������
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
��������
�����ͷŶ���
*/
_BigFigure::~_BigFigure()
{
	Detail->ReferCount--;
	if (!Detail->ReferCount)
	{
		//�����ü���Ϊ0ʱ���ͷ�Detail��ռ���ڴ�
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