#include "BFFunc.h"


#define CONST_OVER9 106		//��ֵ����'0'+'9'+1
#define INT_MAXLEN 32		//��ֵ����ģ��,��ʾ���������������͵ĳ���,���һ��������һ���������в���ʱ,�������ֳ���С���������,�򽫶������½��з���

//���ڻ���������ʱ������
static char* ExtraBufferHead = NULL;
static size_t ExtraBufferSize = 0;
static bool UseExtra = false;


/*
�ú�����BF�����ӷ�����,û�������ж�,����ذ�λ���
@param result �洢����Ķ���
@param OperandA ������A
@param LengthA ������A�ĳ���
@param OperandB ������B
@param LengthB ������B�ĳ���
@param carry �Ƿ��λ(��ֵֻ����Ϊ1��0)
@return result
*/
void core_IntAdd(BFDetail* result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB, int carry)
{
	//�ж��ڴ��Ƿ��㹻
	int buffer;										//����ʱ�Ļ�����
	int offsetA = 0, offsetB = 0;


	if (LengthA >= result->AllocInt)
	{
		//�ڴ治���Դ��,׼������
		if (AutoExpand)
		{
			//�Կռ������չ
			size_t temp1 = LengthA >= LengthB ? LengthA : LengthB;
			Expand(result,(temp1 > result->AllocInt ? temp1 : result->AllocInt) + 1,
				result->AllocFloat);
		}
		else
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "������A��ֵ̫��,�޷��洢��result��", EXCEPTION_DETAIL);
			else
			{
				//���нض�(��ȥ��λ),ע��,�ض�֮������ָ�֮ǰ��ͬ
				offsetA = (int)(LengthA - result->AllocInt);
			}
		}
	}
	if (LengthB >= result->AllocInt)
	{
		if (AutoExpand)
		{
			//�Կռ������չ
			Expand(result, (LengthB >= result->AllocInt ? LengthB : result->AllocInt) + 1,
				result->AllocFloat);
		}
		else
		{
			//�ڴ治���Դ��,׼������
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "������B��ֵ̫��,�޷��洢��result��", EXCEPTION_DETAIL);
			else
			{
				//���нض�(��ȥ��λ),ע��,�ض�֮������ָ�֮ǰ��ͬ
				offsetB = (int)(LengthB - result->AllocInt);
			}
		}

	}

	const char *StringAH = OperandA + offsetA,				//�ַ���A����ָ��
		*StringAT = OperandA + LengthA - 1,			//�ַ�����βָ��,Ҳ��д��λ��
		*StringBH = OperandB + offsetB,				//�ַ���B����ָ��
		*StringBT = OperandB + LengthB - 1;			//�ַ���B��βָ��,Ҳ��д��λ��
	char *StringRH = result->DataHead,
		*StringRT = result->pSRP - 1;


	while (StringRH <= StringRT&&StringAH <= StringAT&&StringBH <= StringBT)
	{
		buffer = (int)*StringAT + *StringBT + carry;
		if (buffer >= CONST_OVER9)
		{
			//����9,��Ҫ���н�λ
			buffer -= 58;//58='9'+1
			carry = 1;
		}
		else
		{
			//����û�д���9,����Ҫ��λ
			buffer -= '0';
			carry = 0;
		}
		*(StringRT--) = (char)buffer;
		StringAT--;
		StringBT--;
	}

	//�����ڽ�λʱ,����A��������λʱ,Ҫ���Ͻ�λ,��û�н�λ����Ǵ���ĸ���
	while (carry&&StringAH <= StringAT&&StringRH <= StringRT)
	{
		buffer = *StringAT + carry;
		if (buffer > '9')
		{
			//����9,��Ҫ���н�λ
			buffer -= 10;
			carry = 1;
		}
		else
			carry = 0;//����û�д���9,����Ҫ��λ
		*StringRT = (char)buffer;
		StringRT--;
		StringAT--;
	}
	//�����ڽ�λʱ,����B��������λʱ,Ҫ���Ͻ�λ,��û�н�λ����Ǵ���ĸ���
	while (carry&&StringBH <= StringBT&&StringRH <= StringRT)
	{
		buffer = *StringBT + carry;
		if (buffer > '9')
		{
			//����9,��Ҫ���н�λ
			buffer -= 10;
			carry = 1;
		}
		else
			carry = 0;//����û�д���9,����Ҫ��λ
		*(StringRT--) = (char)buffer;
		StringBT--;
	}
	//����ʣ�µ�,A��B����Ĳ�����û�н�λ��λ(������ط�ʱ,����ȷ���������Խ������)
	while (StringAH <= StringAT&&StringRH <= StringRT)
		*(StringRT--) = *(StringAT--);
	while (StringBH <= StringBT&&StringRH <= StringRT)
		*(StringRT--) = *(StringBT--);

	if (carry&&StringRH <= StringRT)
	{
		*StringRT = '1';
	}
	else
		StringRT++;																//�ص�������ǰ��ĵ�һ����Чλ

	result->Legal = true;
	result->LenInt = result->pSRP - StringRT;
	result->pSInt = StringRT;

	if (!ConfirmWontLossHighBit)
	{
		while (*result->pSInt == '0')result->pSInt++;				//ȥ�����λ������µ�'0'��ͷ
		result->LenInt = result->pSRP - result->pSInt;		//���¼��㳤��
		if (result->LenInt == 0)											//����������ֳ���Ϊ0,����"0"
		{
			result->LenInt = 1;
			result->pSInt = result->pSRP - 1;
			result->pSInt[0] = '0';
		}
	}
	return;
}
/*
�ú�����BF��������������, û�������ж�, ����ذ�λ���
@param result �洢����Ķ���
@param OperandA ������A
@param LengthA ������A�ĳ���
@param OperandB ������B
@param LengthB ������B�ĳ���
@param carry �Ƿ��λ(��ֵֻ����Ϊ1��0)
@return result
*/
void core_IntSub(BFDetail* result, const char* OperandA, size_t LengthA, const char* OperandB, size_t LengthB, int borrow)
{
	//�ж��ڴ��Ƿ��㹻
	int buffer;										//����ʱ�Ļ�����
	int offsetA = 0, offsetB = 0;
	size_t ExtraSize;

	if (LengthA >= result->AllocInt)
	{
		//�ڴ治���Դ��,׼������
		if (!AutoExpand)
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "������A��ֵ̫��,�޷��洢��result��", EXCEPTION_DETAIL);
			else
			{
				//���нض�(��ȥ��λ),ע��,�ض�֮������ָ�֮ǰ��ͬ
				offsetA = (int)(LengthA - result->AllocInt);
			}
		}
	}
	if (LengthB >= result->AllocInt)
	{
		if (!AutoExpand)
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "������B��ֵ̫��,�޷��洢��result��", EXCEPTION_DETAIL);
			else
			{
				//���нض�(��ȥ��λ),ע��,�ض�֮������ָ�֮ǰ��ͬ
				offsetB = (int)(LengthB - result->AllocInt);
			}
		}
	}

	const char *StringAH = OperandA + offsetA,				//�ַ���A����ָ��
		*StringAT = OperandA + LengthA - 1,			//�ַ�����βָ��,Ҳ��д��λ��
		*StringBH = OperandB + offsetB,				//�ַ���B����ָ��
		*StringBT = OperandB + LengthB - 1;			//�ַ���B��βָ��,Ҳ��д��λ��
	char *StringRH = result->DataHead,
		*StringRT = result->pSRP - 1,
		*StringEH, *StringET;								//����Ľ����������ָ��
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

	StringRT++;										//�ص���һ����д�����ݵĵط�
	if (UseExtra)
	{
		StringET++;
		while (*StringET == '0')					//ȥ�����ǰ������0
			StringET++;
		if (!(*StringET))
		{
			while (*StringRT == '0')				//ȥ�����ǰ������0
				StringRT++;
			if (*StringRT == 0)						//��ֹ���Ϊ��,ʹ����СֵΪ0
				StringRT--;
			result->LenInt = result->pSRP - StringRT;
			result->pSInt = StringRT;
		}
		else
		{
			int temp = (int)(StringEH + ExtraSize - StringET) - 1;
			result->LenInt = result->pSRP - StringRT;
			result->pSInt = StringRT;
			Expand(result, result->AllocInt + temp, result->AllocFloat);

			result->pSInt -= temp;
			strncpy(result->pSInt, StringET, temp);
			result->LenInt += temp;
		}
	}
	else
	{
		while (*StringRT == '0')		//ȥ�����ǰ������0
			StringRT++;
		if (*StringRT == 0)				//��ֹ���Ϊ��,ʹ����СֵΪ0
			StringRT--;
		result->LenInt = result->pSRP - StringRT;
		result->pSInt = StringRT;
	}

	result->Legal = true;
	return;
}

/*
�ú�����BFС���ӷ�����,û�������ж�,����ذ�λ���
@param result �洢����Ķ���
@param OperandA ������A
@param LengthA ������A�ĳ���
@param OperandB ������B
@param LengthB ������B�ĳ���
@return result
*/
int core_FloatAdd(BFDetail* result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB)
{
	if (LengthA > result->AllocFloat || LengthB > result->AllocFloat)
	{
		//�ڴ治��,��Ҫ�������·����ڴ�
		if (AutoExpand)
		{
			//�����Զ���չ
			Expand(result,result->AllocInt, LengthA > LengthB ? LengthA : LengthB);
		}
		else if (ConfirmWontLossAccuracy)
		{
			if (LengthA > result->AllocFloat)
			{
				result->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "������A��С��λ̫��,�޷�ȫ�����浽result��", EXCEPTION_DETAIL);
			}
			else
			{
				result->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "������B��С��λ̫��,�޷�ȫ�����浽result��", EXCEPTION_DETAIL);
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
		size_t temp = LengthA > LengthB ? LengthA : LengthB;							//ȡ�����С��
		temp = temp < result->AllocFloat ? temp : result->AllocFloat;		//ȡ��һ�������±�
		StringRT = StringRH + temp;														//����С��������д��λ��
		*StringRT-- = 0;
		result->LenFloat = temp;
		StringAT = StringAH + (temp < LengthA ? temp : LengthA) - 1;					//����A�����һ����ȡλ��
		StringBT = StringBH + (temp < LengthB ? temp : LengthB) - 1;					//����B�����һ����ȡλ��
	}

	if (LengthA > LengthB)						//���������λ
	{
		//A�Ƚϳ�
		StringTemp = StringAT - (LengthA - LengthB) + 1;
		while (StringTemp <= StringAT&&StringRH <= StringRT)
			*StringRT-- = *StringAT--;
	}
	else if (LengthA < LengthB)
	{
		//B�Ƚϳ�
		StringTemp = StringBT - (LengthB - LengthA) + 1;
		while (StringTemp <= StringBT&&StringRH <= StringRT)
			*StringRT-- = *StringBT--;
	}

	int carry = 0;
	int buffer;
	//��ʼ�������غ�λ�Ĳ���

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
�ú�����BFС����������,û�������ж�,����ذ�λ���
@param result �洢����Ķ���
@param OperandA ������A
@param LengthA ������A�ĳ���
@param OperandB ������B
@param LengthB ������B�ĳ���
@return result
*/
int core_FloatSub(BFDetail* result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB)
{
	if (LengthA > result->AllocFloat || LengthB > result->AllocFloat)
	{
		//�ڴ治��,��Ҫ�������·����ڴ�
		if (AutoExpand)
		{
			//�����Զ���չ
			Expand(result, result->AllocInt, LengthA > LengthB ? LengthA : LengthB);
		}
		else if (ConfirmWontLossAccuracy)
		{
			if (LengthA > result->AllocFloat)
			{
				result->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "������A��С��λ̫��,�޷�ȫ�����浽result��", EXCEPTION_DETAIL);
			}
			else
			{
				result->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "������B��С��λ̫��,�޷�ȫ�����浽result��", EXCEPTION_DETAIL);
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
		//�ҵ������Ķ�ȡλ��
		size_t temp = LengthA > LengthB ? LengthA : LengthB;		//ȡ�����С��
		temp = temp < result->AllocFloat ? temp : result->AllocFloat;							//ȡ��һ�������±�
		StringRT = StringRH + temp;																			//����С��������д��λ��
		*StringRT-- = 0;
		result->LenFloat = temp;
		StringAT = StringAH + (temp < LengthA ? temp : LengthA) - 1;	//����A�����һ����ȡλ��
		StringBT = StringBH + (temp < LengthB ? temp : LengthB) - 1;	//����B�����һ����ȡλ��
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



//����С�����ֵ�����,�˺���һ���ǵ����������ִ���֮����ʹ��
/*
@param result �洢����Ķ���
@param OperandA ������A
@param OperandB ������B(������������)
@param carry �Ƿ��λ(��ֵֻ����Ϊ1��0)
@return result
*/
void core_FloatCopy(BFDetail* result, const char * OperandA, size_t LengthA)
{
	if (result->AllocFloat < LengthA)						//���������ж�
	{
		//�ڴ治��
		if (AutoExpand)
		{
			Expand(result, result->AllocInt, LengthA);		//������չ�ڴ�
		}
		else
		{
			if (ConfirmWontLossAccuracy)
			{
				result->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "С�������ڴ治��,���ܶ�ʧ����", EXCEPTION_DETAIL);
			}
		}
	}
	strncpy(result->pSFloat, OperandA, result->AllocFloat);
	if (result->AllocFloat > LengthA)		//ȡ���µ�С��λ�ĳ���
		result->LenFloat = LengthA;
	else
		result->LenFloat = result->AllocFloat;
	result->pSFloat[result->LenFloat] = 0;			//д���ַ���������
	return;
}


BFDetail* Expand(BFDetail* OperandDetail, size_t IntSize, size_t FloatSize)
{
	char *temp;								//�µ��ڴ�ռ�

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
		if (IntSize >= OperandDetail->LenInt)
			strncpy(temp + IntSize - OperandDetail->LenInt, OperandDetail->pSInt, OperandDetail->LenInt);//�������ְ�ȫ�ý��и���
		else
		{
			//�������ֽ��ᱻ�ض�
			if (ConfirmWontLossHighBit)
			{
				delete[] temp;
				throw BFException(ERR_NUMBERTOOBIG, "�޸Ĵ�С֮�������ԭ����ŵ�ֵ�ᱻ�ض�", EXCEPTION_DETAIL);
			}
			else
				strncpy(temp, OperandDetail->pSInt + OperandDetail->LenInt - IntSize, IntSize);//���а�ȫ�ض�
		}
		if (FloatSize >= OperandDetail->LenFloat)
		{
			if (OperandDetail->LenFloat != 0)
				strncpy(temp + IntSize + 1, OperandDetail->pSFloat, OperandDetail->LenFloat + 1);//С�����ְ�ȫ�ý��и���
			temp[IntSize + 1 + OperandDetail->LenFloat] = 0;
		}
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
				strncpy(temp + IntSize + 1, OperandDetail->pSFloat, FloatSize);
				temp[AllocatedMem - 1] = 0;		//д�������
			}
		}
		//���е��������û�з�������,����԰�ȫ���ύ����
		delete[] OperandDetail->DataHead;
		OperandDetail->DataHead = temp;
		OperandDetail->pSRP = temp + IntSize;
		if (OperandDetail->LenInt > IntSize)			//Ϊ���ضϵ��������¼��㳤��
			OperandDetail->LenInt = IntSize;
		if (OperandDetail->LenFloat > FloatSize)
			OperandDetail->LenFloat = FloatSize;
		OperandDetail->pSInt = OperandDetail->pSRP - OperandDetail->LenInt;
		OperandDetail->pSFloat = OperandDetail->pSRP + 1;
		OperandDetail->AllocInt = IntSize;
		OperandDetail->AllocFloat = FloatSize;

	}
	catch (std::bad_alloc)
	{
		OperandDetail->Legal = false;
		throw BFException(ERR_MEMORYALLOCATEDEXCEPTION, "�����ڴ�������(String)", EXCEPTION_DETAIL);
	}
}
