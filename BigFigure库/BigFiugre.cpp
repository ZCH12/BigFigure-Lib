#define _DLL_API_ 
#include "BigFiugre.h"

bool ReserveZero = true;				//����С�����0
bool ScinotationShow = false;			//���Կ�ѧ��������ʾ����
bool ConfirmWontLossAccuracy = true;	//ȷ�����ض�С������Чλ(�ر�֮�������ֵʱС��λ̫��,���ʡ�Զ���Ĳ���;�������,����׳��쳣)
bool ConfirmWontLossHighBit = true;		//ȷ������ʧ�����ĸ�λ(����������ʱ),���Ϊtrue,�������ʱ���׳��쳣,���Ϊfalse,����������������λ
bool AutoExpand = true;					//���ڴ治��ʱ�Զ���չ�ռ�
size_t ScinotationLen = 5;				//��ѧ������ʱ��Чλ��Ϊ5λ


/*
����һ��BF����
@param IntSize �������ַ�����ڴ�
@param FloatSize С�����ַ�����ڴ�
*/
BigFigure::BigFigure(size_t IntSize, size_t FloatSize)

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
BigFigure::BigFigure(const BigFigure & Base)
{
	this->Detail = Base.Detail;
	this->Detail->ReferCount++;
}

/*
��������
�����ͷŶ���
*/
BigFigure::~BigFigure()
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
���·�������ڴ�(��������),
�����С�ڴ�����ܵ������ݶ�ʧ
@param IntSize �µ��������ַ�����ڴ�
@param FloatSize �µ�С�����ַ�����ڴ�
*/
BigFigure& BigFigure::Expand(size_t IntSize, size_t FloatSize)
{
	char *temp;			//�µ��ڴ�ռ�

	int AllocatedMem;
	if (IntSize == 0)
		throw BFException(ERR_ILLEGALPARAM, "�������ַ�����ڴ治��Ϊ0", EXCEPTION_DETAIL);

	AllocatedMem = (int)IntSize + 1;
	if (FloatSize > 0)
		AllocatedMem += (int)FloatSize + 1;

	try
	{
		temp = new char[AllocatedMem];
		temp[IntSize] = 0;				//д���м��'\0'
		if (IntSize >= Detail->LenInt)
			strncpy(temp + IntSize - Detail->LenInt, Detail->pSInt, Detail->LenInt);//С�����ְ�ȫ�ý��и���
		else
		{
			//�������ֽ��ᱻ�ض�
			if (ConfirmWontLossHighBit)
			{
				delete[] temp;
				throw BFException(ERR_NUMBERTOOBIG, "�޸Ĵ�С֮�������ԭ����ŵ�ֵ�ᱻ�ض�", EXCEPTION_DETAIL);
			}
			else
				strncpy(temp, Detail->pSInt + Detail->LenInt - IntSize, IntSize);//���а�ȫ�ض�
		}
		if (FloatSize >= Detail->LenFloat)
			strncpy(temp + IntSize + 1, Detail->pSFloat, Detail->LenFloat + 1);//С�����ְ�ȫ�ý��и���

		else
		{
			//С�����ֽ��ᱻ�ض�
			if (ConfirmWontLossAccuracy)
			{
				delete[] temp;
				throw BFException(ERR_MAYACCURACYLOSS, "�޸ĺ�Ķ����޷�װ��ԭ����С��λ,���ᶪʧ����", EXCEPTION_DETAIL);
			}
			else
			{
				strncpy(temp + IntSize + 1, Detail->pSFloat, FloatSize);
				temp[AllocatedMem - 1] = 0;		//д�������
			}
		}
		//���е��������û�з�������,����԰�ȫ���ύ����
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
		throw BFException(ERR_MEMORYALLOCATEDEXCEPTION, "�����ڴ�������(String)", EXCEPTION_DETAIL);
	}

	return *this;
}

/*
����ǰ����Ĵ�С����Ϊ����Ϊ��Sourceһ��(��������С,���С�����)
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
	int expon;							//��ѧ��������ָ������

	Detail->Minus = NumStringDetail.RadixMinus ? true : false;

	switch (NumStringDetail.Mode)
	{
	case 1://����
	case 2://С��
		if (Detail->AllocInt < NumStringDetail.IntLen)
		{
			//�ڴ�ռ䲻��
			if (AutoExpand)
			{
				//�Զ�������չ
				Expand(NumStringDetail.IntLen, NumStringDetail.FloatLen > Detail->AllocFloat ? NumStringDetail.FloatLen : Detail->AllocFloat);
			}
			else {
				//����������,���ж��Ƿ�Ҫ�ض�
				if (ConfirmWontLossHighBit)
				{
					Detail->Legal = false;
					throw BFException(ERR_NUMBERTOOBIG, "����Ŀ�����,���������̫��,�ö����޷�����", EXCEPTION_DETAIL);
				}
			}
		}

		if (Detail->AllocInt >= NumStringDetail.IntLen)
		{
			//���������

			if (NumStringDetail.IntBeZero)
			{
				//��������Ϊ0,��д��"0"
				Detail->pSInt = Detail->pSRP - 1;
				Detail->pSInt[0] = '0';
				Detail->LenInt = 1;
			}
			else
			{
				//�������ֲ�Ϊ0,�򿽱��������ֽ��������ַ�����
				Detail->pSInt = Detail->pSRP - NumStringDetail.IntLen;
				Detail->LenInt = NumStringDetail.IntLen;
				strncpy(Detail->pSInt, NumStringDetail.NumString + NumStringDetail.IntStart_p, NumStringDetail.IntLen);
			}
		}
		else
		{
			//�ض��͸���(��������)
			Detail->pSInt = Detail->DataHead;
			Detail->LenInt = Detail->AllocInt;
			temp = (int)(NumStringDetail.IntLen - Detail->AllocInt);	//����Ҫ����������λ��
			strncpy(Detail->pSInt, NumStringDetail.NumString + NumStringDetail.IntStart_p + temp, Detail->AllocInt);
		}

		//�ж����ֵ�����,���������,������������ǳ���
		if (NumStringDetail.Mode == 1)
		{
			Detail->Legal = true;
			if (Detail->AllocFloat)
			{
				Detail->pSFloat[0] = 0;		//����з���С��λ�ڴ�,��С����Ϊ0
				Detail->LenFloat = 0;
			}
			return *this;
		}

		//����ΪС�����ֵĴ���

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
					throw BFException(ERR_MAYACCURACYLOSS, "д�����Чλ������Ŀ����������,���ܶ�ʧ����", EXCEPTION_DETAIL);
				}
			}
		}

		if (Detail->AllocFloat)
		{
			//�����ǰ������С��λ,����ܿ��Խ���д��
			if (NumStringDetail.FloatLen <= Detail->AllocFloat)
			{
				//����ڴ��㹻���,����и���
				Detail->LenFloat = NumStringDetail.FloatLen;
				strncpy(Detail->pSFloat, NumStringDetail.NumString + NumStringDetail.FloatStart_p, NumStringDetail.FloatLen);
				Detail->pSFloat[Detail->LenFloat] = 0;
			}
			else
			{
				//�ض�ʽ����
				Detail->LenFloat = Detail->AllocFloat;
				strncpy(Detail->pSFloat, NumStringDetail.NumString + NumStringDetail.FloatStart_p, NumStringDetail.FloatLen);
				Detail->pSFloat[Detail->LenFloat] = 0;
			}
		}
		Detail->Legal = true;
		return *this;
	case 3://��ѧ��������ʾ��������Ϊ����������
	case 4://��ѧ��������ʾ����С��Ϊ����������
		tempStr = new char[NumStringDetail.IntLen + NumStringDetail.FloatLen + 1];		//��ʱ�����Чλ���ַ���
		SourceStr = NumStringDetail.NumString + NumStringDetail.FloatStart_p;			//���С��λ����ָ��
		index_p = 0;

		//ȡ����Чλ
		if (NumStringDetail.Mode == 3)
		{
			//����Ϊ����ʱ
			strncpy(tempStr, NumStringDetail.NumString + NumStringDetail.IntStart_p, NumStringDetail.IntLen);
			expon = atoi(NumStringDetail.NumString + NumStringDetail.ExpStart_p);		//ȡ��ָ��
			index_p = NumStringDetail.IntLen - 1;										//ȡ��tempStr�Ľ�β���±�

			if (NumStringDetail.ExpMinus)								//������,index_p��ֵ��������λ��
				expon = ~expon + 1 + (int)index_p;							//ָ��Ϊ����,ȡ�෴���ټ��ϱ����������ĸ���,�õ��µ�ָ��
			else
				expon += (int)index_p;										//ָ��Ϊ����,ֱ�Ӽ��ϱ����������ĸ���,�õ��µ�ָ��

			while (index_p != 0 && tempStr[index_p] == '0')index_p--;	//ȥ��ĩβ��0
			tempStr[++index_p] = 0;										//д�������,
			size = index_p;
		}
		else
		{
			//����ΪС��ʱ
			if (!NumStringDetail.IntBeZero)
			{
				//�������ֲ�Ϊ0
				strncpy(tempStr, NumStringDetail.NumString + NumStringDetail.IntStart_p, NumStringDetail.IntLen);	//�����������ֵ���Ч����
				index_p = NumStringDetail.IntLen;
				skip = (int)index_p - 1;
				strncpy(tempStr + index_p, SourceStr, NumStringDetail.FloatLen);		//����С�����ֵ���Ч����
				index_p += NumStringDetail.FloatLen;									//��С��λ�ĳ��ȼ���
				index_p--;																//�ص���һ��д��λ��
				while (index_p != 0 && tempStr[index_p] == '0')index_p--;				//ȥ��ĩβ��'0'
				tempStr[++index_p] = 0;													//д�������,ȷ�Ͻض�
			}
			else {
				//��������Ϊ0
				index_p = NumStringDetail.FloatLen;
				skip = (int)index_p;															//��ʼ��������Ϊ
				while (*(SourceStr) == '0')SourceStr++, index_p--;						//��������ǰ�ߵ�0
				skip = ~(skip - (int)index_p + 1) + 1;										//ȡ����������Чλ��λ����ȡ�෴��
				strncpy(tempStr, SourceStr, index_p);									//������Ч����
				while (index_p != 0 && tempStr[index_p] == '0')index_p--;				//ȥ��ĩβ��0
				tempStr[index_p++] = 0;
			}
			size = index_p;
			expon = atoi(NumStringDetail.NumString + NumStringDetail.ExpStart_p);
			if (NumStringDetail.ExpMinus)
				expon = ~expon + 1 + skip;		//ָ��Ϊ����,�ټ��ϱ�������λ��(skip������Ҳ���ܸ�,ȡ����С������ǰƫ�ƻ������ƫ��)
			else
				expon += skip;					//ָ��Ϊ����
		}

		//��ʼ���뵽������
		{
			//����д���ĳ���
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
					throw BFException(ERR_NUMBERTOOBIG, "�����ֵ̫��,�޷��洢,�������չ", EXCEPTION_DETAIL);
			}
			if (Detail->AllocFloat < Detail->LenFloat)
			{
				if (ConfirmWontLossAccuracy)
					throw BFException(ERR_MAYACCURACYLOSS, "�����ֵС��λ����,����ʧ����", EXCEPTION_DETAIL);
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

		if (expon > 0)				//ָ������0�Ĵ�����
		{
			if (Detail->LenInt <= Detail->AllocInt)
			{
				//С��λһ��Ϊ0
				Detail->pSInt = Detail->pSRP - Detail->LenInt;				//������������д��λ��
				strncpy(Detail->pSInt, tempStr, Detail->LenInt);			//������Чλ
				temp = (int)(Detail->LenInt - size);
				if (temp > 0)
					memset(Detail->pSInt + size, '0', temp);				//��λ��0���
			}
			else
			{
				//���нض��Ը���
				Detail->pSInt = Detail->DataHead;
				Detail->LenInt = Detail->AllocInt;
				temp = expon - (int)Detail->AllocInt;							//���Ե�λ��
				strcpy(Detail->pSInt, tempStr + temp);
				memset(Detail->pSInt + size - temp - 1, '0', Detail->AllocInt - (size - temp));
			}
			if (Detail->AllocFloat)
			{
				if (Detail->LenFloat <= Detail->AllocFloat)
				{
					temp = (int)(size - Detail->LenInt);
					if (temp > 0)
						strncpy(Detail->pSFloat, tempStr + Detail->LenInt, temp);	//д��С��λ
					else
						temp = 0;
					Detail->pSFloat[temp] = 0;
				}
				else
				{
					Detail->LenFloat = Detail->AllocFloat;
					strncpy(Detail->pSFloat, tempStr + Detail->LenInt, Detail->AllocFloat);	//�ض��Ը���
					Detail->pSFloat[Detail->LenFloat] = 0;									//д�������
				}
			}
		}
		else if (expon < 0)				//ָ��С��0ʱ�Ĵ�����
		{
			//ָ��С��0,����������һ��Ϊ0
			Detail->pSInt = Detail->pSRP - 1;
			Detail->pSInt[0] = '0';
			Detail->LenInt = 1;
			index_p = ~expon;//~expon+1-1				//����ǰ�������0�ĸ���
			memset(Detail->pSFloat, '0', index_p);

			if (Detail->AllocFloat)
			{
				if (Detail->LenFloat <= Detail->AllocFloat)
				{
					//�ڴ��㹻,ֱ�Ӹ���
					strncpy(Detail->pSFloat + index_p, tempStr, size);						//д����Чλ
					Detail->pSFloat[index_p + size] = 0;
				}
				else
				{
					//�ڴ治��,���нض��Ը���
					strncpy(Detail->pSFloat + index_p, tempStr, Detail->AllocFloat - index_p);
					Detail->LenFloat = Detail->AllocFloat;
					Detail->pSFloat[Detail->AllocFloat] = 0;
				}
			}
		}
		else
		{
			//ָ������0�Ĵ�����
			Detail->pSInt = Detail->pSRP - 1;
			Detail->pSInt[0] = tempStr[0];
			Detail->LenInt = 1;
			Detail->LenFloat = size - 1;

			if (Detail->LenFloat <= Detail->AllocFloat)
			{
				//��ȷ�ȴ�����Чλ��,�ܹ���ȫ����
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
		throw BFException(ERR_ILLEGALNUMBER, "����һ���Ϸ�������", EXCEPTION_DETAIL);
		break;
	}
	return *this;
}

//��һ�������ֵ���Ƶ���ǰ������,���������໥����
BigFigure & BigFigure::CopyDetail(const BigFigure & Source)
{
	this->Detail->Minus = Source.Detail->Minus;
	this->Detail->Legal = Source.Detail->Legal;

	try
	{
		if (this->Detail->AllocInt < Source.Detail->LenInt)
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "���������ڴ治��", EXCEPTION_DETAIL);
		}
		if (this->Detail->AllocFloat < Source.Detail->LenFloat)
		{
			if (ConfirmWontLossAccuracy)
				throw BFException(ERR_MAYACCURACYLOSS, "С�������ڴ治��", EXCEPTION_DETAIL);
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
		//�ռ��㹻����,���и���
		this->Detail->pSInt = this->Detail->pSRP - Source.Detail->LenInt;	//�ҵ�д��λ��
		this->Detail->LenInt = Source.Detail->LenInt;
		strcpy(this->Detail->pSInt, Source.Detail->pSInt);
	}
	else
	{
		//�ض��Ը���
		this->Detail->pSInt = this->Detail->DataHead;
		strncpy(this->Detail->pSInt, Source.Detail->pSInt + Source.Detail->LenInt - this->Detail->AllocInt, this->Detail->AllocInt);
		while (this->Detail->pSInt[0] == '0')this->Detail->pSInt++;			//ȥ������ǰ���0
		this->Detail->LenInt = this->Detail->pSRP - this->Detail->pSInt;	//���������ĳ���
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
		//ԴΪ��
		if (!this->Detail->AllocFloat)
		{
			this->Detail->LenFloat = 0;
			this->Detail->pSFloat[0] = 0;
		}
	}

	return *this;
}

//��BFת��Ϊ���ֽ������(����)
//�˺�����Ҫ�Լ�������,��Ӧ�õ������ķ�װ�汾
char* BigFigure::_toString(size_t &length, bool UseScinotation, bool ReserveZero)
{
	char *tempString = new char[Detail->LenInt + Detail->LenFloat + 7];	//�½�һ���㹻������ݵĻ�����;							//���滺������ַ
	int skip;											//��������λ��ʡ�Ե�λ���������ռ���ָ��
	//std::string RetVal;								//���ص��ַ����ĳ���
	size_t index_p = 0;									//д��λ�ñ��
	size_t r_index;										//��Ԫ�����ж�ȡ���ݵ��±�
	size_t sign = ScinotationLen;						//��¼��Ч���ֵĸ���(���ڿ�ѧ������ʱʹ��)

	if (!Detail->Legal)
	{
		//�Ƿ����������Nan
		strcpy(tempString, "NaN");
		length = 3;
		return tempString;
	}
	else
	{
		if (Detail->Minus)								//�������
		{
			tempString[index_p++] = '-';				//д�븺��
			sign++;										//ʹ���Ų�������Чλ
		}

		if (UseScinotation)
		{
			//ʹ�ÿ�ѧ���������
			if (Detail->pSInt[0] == '0' && Detail->LenInt == 1)
			{
				//������������Ϊ0
				r_index = 0;
				skip = 0;

				while (Detail->pSFloat[r_index] == '0' && r_index < Detail->AllocFloat) r_index++;	//�ҵ���Чλ

				if (Detail->pSFloat[r_index] != 0)
				{
					skip = -(int)r_index - 1;								//����ʡ�Ե�λ��
					tempString[index_p++] = Detail->pSFloat[r_index++];		//�����һλ
					if (Detail->pSFloat[r_index] != 0)
					{
						//���滹����Чλ,�������
						tempString[index_p++] = '.';						//���С����
						sign++;												//ʹС���㲻������Чλ
						while (Detail->pSFloat[r_index] != 0 && r_index < Detail->AllocFloat&&index_p < sign)	//���ʣ����Чλ
							tempString[index_p++] = Detail->pSFloat[r_index++];
						if (!ReserveZero)
						{
							index_p--;										//��λ����һ��д���λ��
							while (tempString[index_p] == '0')index_p--;	//����Ч����β����'0'ȥ��
							if (tempString[index_p] == '.') index_p--;		//�������С����,��Ҳ��С����ȥ��
							tempString[++index_p] = 0;						//д��'\0'
						}
					}
				}
				else
				{
					//��ֵΪ0,д��0
					tempString[index_p++] = '0';
					tempString[index_p++] = 0;
				}
			}
			else
			{
				//�����������ݲ�Ϊ0
				skip = (int)Detail->LenInt - 1;
				tempString[index_p++] = Detail->pSInt[0];							//�ѵ�һλ����
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
					tempString[index_p++] = '.';							//С�����������

				if (Detail->AllocFloat &&Detail->pSFloat[0] != 0)
				{
					if (Detail->LenInt < ScinotationLen)
					{
						//��������Чλ����,������С��λ
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
					while (tempString[index_p] == '0')index_p--;	//����Ч����β����'0'ȥ��
					if (tempString[index_p] == '.')index_p--;
					tempString[++index_p] = 0;						//д��'\0'
				}
			}

			if (skip) //���ָ��
			{
				tempString[index_p++] = 'E';
				sprintf(tempString + index_p, "%d", skip);
			}
			while (tempString[index_p] != 0)index_p++;
			length = index_p;
		}
		else
		{
			//�����������
			strncpy(tempString + index_p, Detail->pSInt, Detail->LenInt);

			index_p += Detail->LenInt;
			if (Detail->LenFloat)
			{
				r_index = 0;
				tempString[index_p++] = '.';
				while (Detail->pSFloat[r_index] != 0)
					tempString[index_p++] = Detail->pSFloat[r_index++];

				if (!ReserveZero)									//ȥ��0
				{
					index_p--;										//ָ����һ����д��λ
					while (tempString[index_p] == '0') index_p--;	//ɾ��Ϊ0��λ
					if (tempString[index_p] == '.')index_p--;		//���С���㱻ɾ��,����ɾ��С����
					index_p++;										//ָ����һ����д��λ
				}
			}
			tempString[index_p] = 0;								//д���ַ���������
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
