#define _DLL_API_ 
#include  "BigFiugre.h"

#define CONST_OVER9 106		//��ֵ����'0'+'9'+1
#define INT_MAXLEN 32		//��ֵ����ģ��,��ʾ���������������͵ĳ���,���һ��������һ���������в���ʱ,�������ֳ���С���������,�򽫶������½��з���

//���ļ����ڴ�������㷨���ĺ���
//������Ԫ��������Ԫģ��

//���ڻ���������ʱ������
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
�ú�����BF�����ӷ�����,û�������ж�,����ذ�λ���
@param result �洢����Ķ���
@param OperandA ������A
@param LengthA ������A�ĳ���
@param OperandB ������B
@param LengthB ������B�ĳ���
@param carry �Ƿ��λ(��ֵֻ����Ϊ1��0)
@return result
*/
BigFigure& core_IntAdd(BigFigure & result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB, int carry)
{
	//�ж��ڴ��Ƿ��㹻
	int buffer;										//����ʱ�Ļ�����
	int offsetA = 0, offsetB = 0;


	if (LengthA >= result.Detail->AllocInt)
	{
		//�ڴ治���Դ��,׼������
		if (AutoExpand)
		{
			//�Կռ������չ
			size_t temp1 = LengthA >= LengthB ? LengthA : LengthB;
			result.Expand((temp1 > result.Detail->AllocInt ? temp1 : result.Detail->AllocInt) + 1,
				result.Detail->AllocFloat);
		}
		else
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "������A��ֵ̫��,�޷��洢��result��", EXCEPTION_DETAIL);
			else
			{
				//���нض�(��ȥ��λ),ע��,�ض�֮������ָ�֮ǰ��ͬ
				offsetA = (int)(LengthA - result.Detail->AllocInt);
			}
		}
	}
	if (LengthB >= result.Detail->AllocInt)
	{
		if (AutoExpand)
		{
			//�Կռ������չ
			result.Expand((LengthB >= result.Detail->AllocInt ? LengthB : result.Detail->AllocInt) + 1,
				result.Detail->AllocFloat);
		}
		else
		{
			//�ڴ治���Դ��,׼������
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "������B��ֵ̫��,�޷��洢��result��", EXCEPTION_DETAIL);
			else
			{
				//���нض�(��ȥ��λ),ע��,�ض�֮������ָ�֮ǰ��ͬ
				offsetB = (int)(LengthB - result.Detail->AllocInt);
			}
		}

	}

	const char *StringAH = OperandA + offsetA,				//�ַ���A����ָ��
		*StringAT = OperandA + LengthA - 1,			//�ַ�����βָ��,Ҳ��д��λ��
		*StringBH = OperandB + offsetB,				//�ַ���B����ָ��
		*StringBT = OperandB + LengthB - 1;			//�ַ���B��βָ��,Ҳ��д��λ��
	char *StringRH = result.Detail->DataHead,
		*StringRT = result.Detail->pSRP - 1;


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

	result.Detail->Legal = true;
	result.Detail->LenInt = result.Detail->pSRP - StringRT;
	result.Detail->pSInt = StringRT;

	if (!ConfirmWontLossHighBit)
	{
		while (*result.Detail->pSInt == '0')result.Detail->pSInt++;				//ȥ�����λ������µ�'0'��ͷ
		result.Detail->LenInt = result.Detail->pSRP - result.Detail->pSInt;		//���¼��㳤��
		if (result.Detail->LenInt == 0)											//����������ֳ���Ϊ0,����"0"
		{
			result.Detail->LenInt = 1;
			result.Detail->pSInt = result.Detail->pSRP - 1;
			result.Detail->pSInt[0] = '0';
		}
	}
	return result;
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
BigFigure& core_IntSub(BigFigure & result, const char* OperandA, size_t LengthA, const char* OperandB, size_t LengthB, int borrow)
{
	//�ж��ڴ��Ƿ��㹻
	int buffer;										//����ʱ�Ļ�����
	int offsetA = 0, offsetB = 0;
	int ExtraSize;

	if (LengthA >= result.Detail->AllocInt)
	{
		//�ڴ治���Դ��,׼������
		if (!AutoExpand)
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "������A��ֵ̫��,�޷��洢��result��", EXCEPTION_DETAIL);
			else
			{
				//���нض�(��ȥ��λ),ע��,�ض�֮������ָ�֮ǰ��ͬ
				offsetA = (int)(LengthA - result.Detail->AllocInt);
			}
		}
	}
	if (LengthB >= result.Detail->AllocInt)
	{
		if (!AutoExpand)
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "������B��ֵ̫��,�޷��洢��result��", EXCEPTION_DETAIL);
			else
			{
				//���нض�(��ȥ��λ),ע��,�ض�֮������ָ�֮ǰ��ͬ
				offsetB = (int)(LengthB - result.Detail->AllocInt);
			}
		}
	}

	const char *StringAH = OperandA + offsetA,				//�ַ���A����ָ��
		*StringAT = OperandA + LengthA - 1,			//�ַ�����βָ��,Ҳ��д��λ��
		*StringBH = OperandB + offsetB,				//�ַ���B����ָ��
		*StringBT = OperandB + LengthB - 1;			//�ַ���B��βָ��,Ҳ��д��λ��
	char *StringRH = result.Detail->DataHead,
		*StringRT = result.Detail->pSRP - 1,
		*StringEH, *StringET;								//����Ľ����������ָ��
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
		while (*StringRT == '0')		//ȥ�����ǰ������0
			StringRT++;
		result.Detail->LenInt = result.Detail->pSRP - StringRT;
		result.Detail->pSInt = StringRT;
	}

	result.Detail->Legal = true;
	return result;
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
int core_FloatAdd(BigFigure & result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB)
{
	if (LengthA > result.Detail->AllocFloat || LengthB > result.Detail->AllocFloat)
	{
		//�ڴ治��,��Ҫ�������·����ڴ�
		if (AutoExpand)
		{
			//�����Զ���չ
			result.Expand(result.Detail->AllocInt, LengthA > LengthB ? LengthA : LengthB);
		}
		else if (ConfirmWontLossAccuracy)
		{
			if (LengthA > result.Detail->AllocFloat)
			{
				result.Detail->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "������A��С��λ̫��,�޷�ȫ�����浽result��", EXCEPTION_DETAIL);
			}
			else
			{
				result.Detail->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "������B��С��λ̫��,�޷�ȫ�����浽result��", EXCEPTION_DETAIL);
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
		size_t temp = LengthA > LengthB ? LengthA : LengthB;							//ȡ�����С��
		temp = temp < result.Detail->AllocFloat ? temp : result.Detail->AllocFloat;		//ȡ��һ�������±�
		StringRT = StringRH + temp;														//����С��������д��λ��
		*StringRT-- = 0;
		result.Detail->LenFloat = temp;
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
	result.Detail->Legal = true;
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
int core_FloatSub(BigFigure & result, const char * OperandA, size_t LengthA, const char* OperandB, size_t LengthB)
{
	if (LengthA > result.Detail->AllocFloat || LengthB > result.Detail->AllocFloat)
	{
		//�ڴ治��,��Ҫ�������·����ڴ�
		if (AutoExpand)
		{
			//�����Զ���չ
			result.Expand(result.Detail->AllocInt, LengthA > LengthB ? LengthA : LengthB);
		}
		else if (ConfirmWontLossAccuracy)
		{
			if (LengthA > result.Detail->AllocFloat)
			{
				result.Detail->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "������A��С��λ̫��,�޷�ȫ�����浽result��", EXCEPTION_DETAIL);
			}
			else
			{
				result.Detail->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "������B��С��λ̫��,�޷�ȫ�����浽result��", EXCEPTION_DETAIL);
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
		//�ҵ������Ķ�ȡλ��
		size_t temp = LengthA > LengthB ? LengthA : LengthB;		//ȡ�����С��
		temp = temp < result.Detail->AllocFloat ? temp : result.Detail->AllocFloat;							//ȡ��һ�������±�
		StringRT = StringRH + temp;																			//����С��������д��λ��
		*StringRT-- = 0;
		result.Detail->LenFloat = temp;
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
	result.Detail->Legal = true;
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
BigFigure& core_FloatCopy(BigFigure &result, const BigFigure &OperandA)
{
	if (result.Detail->AllocFloat < OperandA.Detail->LenFloat)						//���������ж�
	{
		//�ڴ治��
		if (AutoExpand)
		{
			result.Expand(result.Detail->AllocInt, OperandA.Detail->LenFloat);		//������չ�ڴ�
		}
		else
		{
			if (ConfirmWontLossAccuracy)
			{
				result.Detail->Legal = false;
				throw BFException(ERR_MAYACCURACYLOSS, "С�������ڴ治��,���ܶ�ʧ����", EXCEPTION_DETAIL);
			}
		}
	}
	strncpy(result.Detail->pSFloat, OperandA.Detail->pSFloat, result.Detail->AllocFloat);
	if (result.Detail->AllocFloat > OperandA.Detail->LenFloat)		//ȡ���µ�С��λ�ĳ���
		result.Detail->LenFloat = OperandA.Detail->LenFloat;
	else
		result.Detail->LenFloat = result.Detail->AllocFloat;
	result.Detail->pSFloat[result.Detail->LenFloat] = 0;			//д���ַ���������
	return result;
}

BigFigure & BFAdd(BigFigure & result, const BigFigure OperandA, const BigFigure & OperandB)
{
	bool minusA = OperandA.Detail->Minus, minusB = OperandB.Detail->Minus;

	if (!(minusA || minusB))
	{
		//�������
		core_IntAdd(result, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = false;
	}
	else if (minusA && !minusB)
	{
		//�������
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
		//�������
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
		//�������
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
		//�������
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
		//�������
		//-5-9=-14
		core_IntAdd(result, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = true;
	}
	else if (!minusA&&minusB)
	{
		//�������
		//9--5=14
		core_IntAdd(result, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = false;
	}
	else {
		//�������
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

//�ӿں���




//�Ƚ��������Ĵ�С
/*
����ֵ��ʾ�������Ĵ�С���
0,	OperandA==OperandB
1,	OperandA>OperandB
-1,	OperandA<OperandB
2,	OperandA>>OperandB
-2,	OperandA<<OperandB
*/
int BFCmp(const BigFigure &OperandA, const BigFigure &OperandB)
{
	int minus = 1;				//��Ϊ����ʱ,���صĽ���������෴,�˱������Ƶľ����������

								//�жϸ���
	if (OperandA.Detail->Minus)
	{
		if (OperandB.Detail->Minus)
			minus = -1;//A,Bͬ��
		else
			return -2;//A��B��
	}
	else
	{
		if (OperandB.Detail->Minus)
			return 2;//A��B��
					 //minusĬ��Ϊ1,����Ҫ�Ķ�
	}
	return BFCmp_abs(OperandA, OperandB, minus);

}
int BFCmp_abs(const BigFigure &OperandA, const BigFigure &OperandB)
{
	return BFCmp_abs(OperandA, OperandB, 1);
}
/*
�Ƚ��������ľ���ֵ��С
*/
int BFCmp_abs(const BigFigure &OperandA, const BigFigure &OperandB, int minus)
{
	//�ж�λ
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
		//����������������ֳ������,����бȽϸ���λ
		int temp = strcmp(OperandA.Detail->pSInt, OperandB.Detail->pSInt);
		if (temp)
		{
			//�Ƚϳ����,����
			return temp*minus;
		}
		else {
			//����������ȫ���,�������бȽ�С������
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

