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
			core_Expand(result, (temp1 > result->AllocInt ? temp1 : result->AllocInt) + 1,
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
			core_Expand(result, (LengthB >= result->AllocInt ? LengthB : result->AllocInt) + 1,
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
			core_Expand(result, result->AllocInt + temp, result->AllocFloat);

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
			core_Expand(result, result->AllocInt, LengthA > LengthB ? LengthA : LengthB);
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
			core_Expand(result, result->AllocInt, LengthA > LengthB ? LengthA : LengthB);
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
	if (result->AllocFloat < LengthA)							//���������ж�
	{
		//�ڴ治��
		if (AutoExpand)
		{
			core_Expand(result, result->AllocInt, LengthA);		//������չ�ڴ�
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




BFDetail * core_BFAdd(BFDetail * result, const BFDetail * OperandA, const BFDetail * OperandB)
{
	bool minusA = OperandA->Minus, minusB = OperandB->Minus;
	if (!(minusA || minusB))
	{
		//�������
		core_IntAdd(result, OperandA->pSInt, OperandA->LenInt, OperandB->pSInt, OperandB->LenInt, core_FloatAdd(result, OperandA->pSFloat, OperandA->LenFloat, OperandB->pSFloat, OperandB->LenFloat));
		result->Minus = false;
	}
	else if (minusA && !minusB)
	{
		//�������
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
		//�������
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
		//�������
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
		//�������
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
		//�������
		//-5-9=-14
		core_IntAdd(result, OperandA->pSInt, OperandA->LenInt, OperandB->pSInt, OperandB->LenInt, core_FloatAdd(result, OperandA->pSFloat, OperandA->LenFloat, OperandB->pSFloat, OperandB->LenFloat));
		result->Minus = true;
	}
	else if (!minusA&&minusB)
	{
		//�������
		//9--5=14
		core_IntAdd(result, OperandA->pSInt, OperandA->LenInt, OperandB->pSInt, OperandB->LenInt, core_FloatAdd(result, OperandA->pSFloat, OperandA->LenFloat, OperandB->pSFloat, OperandB->LenFloat));
		result->Minus = false;
	}
	else {
		//�������
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
		delete[]  OperandDetail->DataHead;
		 OperandDetail->DataHead = temp;
		 OperandDetail->pSRP = temp + IntSize;
		if ( OperandDetail->LenInt > IntSize)			//Ϊ���ضϵ��������¼��㳤��
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
		throw BFException(ERR_MEMORYALLOCATEDEXCEPTION, "�����ڴ�������(String)", EXCEPTION_DETAIL);
	}
	return OperandDetail;
}

//��BFת��Ϊ���ֽ������(����)
//�˺�����Ҫ�Լ�������,��Ӧ�õ������ķ�װ�汾
char* core_toString(BFDetail * OperandDetail,size_t &length, bool UseScinotation, bool ReserveZero)
{
	char *tempString = new char[OperandDetail->LenInt + OperandDetail->LenFloat + 7];	//�½�һ���㹻������ݵĻ�����;							//���滺������ַ
	int skip;											//��������λ��ʡ�Ե�λ���������ռ���ָ��
														//std::string RetVal;								//���ص��ַ����ĳ���
	size_t index_p = 0;									//д��λ�ñ��
	size_t r_index;										//��Ԫ�����ж�ȡ���ݵ��±�
	size_t sign = ScinotationLen;						//��¼��Ч���ֵĸ���(���ڿ�ѧ������ʱʹ��)

	if (!OperandDetail->Legal)
	{
		//�Ƿ����������Nan
		strcpy(tempString, "NaN");
		length = 3;
		return tempString;
	}
	else
	{
		if (OperandDetail->Minus)								//�������
		{
			tempString[index_p++] = '-';				//д�븺��
			sign++;										//ʹ���Ų�������Чλ
		}

		if (UseScinotation)
		{
			//ʹ�ÿ�ѧ���������
			if (OperandDetail->LenInt == 1 && OperandDetail->pSInt[0] == '0')
			{
				//������������Ϊ0
				r_index = 0;
				skip = 0;

				while (OperandDetail->pSFloat[r_index] == '0' && r_index < OperandDetail->AllocFloat) r_index++;	//�ҵ���Чλ

				if (OperandDetail->pSFloat[r_index] != 0)
				{
					skip = -(int)r_index - 1;								//����ʡ�Ե�λ��
					tempString[index_p++] = OperandDetail->pSFloat[r_index++];		//�����һλ
					if (OperandDetail->pSFloat[r_index] != 0)
					{
						//���滹����Чλ,�������
						tempString[index_p++] = '.';						//���С����
						sign++;												//ʹС���㲻������Чλ
						while (OperandDetail->pSFloat[r_index] != 0 && r_index < OperandDetail->AllocFloat&&index_p < sign)	//���ʣ����Чλ
							tempString[index_p++] = OperandDetail->pSFloat[r_index++];
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
				skip = (int)OperandDetail->LenInt - 1;
				tempString[index_p++] = OperandDetail->pSInt[0];							//�ѵ�һλ����
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
					tempString[index_p++] = '.';							//С�����������

				if (OperandDetail->AllocFloat &&OperandDetail->pSFloat[0] != 0)
				{
					if (OperandDetail->LenInt < ScinotationLen)
					{
						//��������Чλ����,������С��λ
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
			strncpy(tempString + index_p, OperandDetail->pSInt, OperandDetail->LenInt);

			index_p += OperandDetail->LenInt;
			if (OperandDetail->LenFloat)
			{
				r_index = 0;
				tempString[index_p++] = '.';
				while (OperandDetail->pSFloat[r_index] != 0)
					tempString[index_p++] = OperandDetail->pSFloat[r_index++];

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


//��һ�������ֵ���Ƶ���ǰ������,���������໥����
void core_CopyDetail(BFDetail* Dest, const BFDetail * Source)
{
	Dest->Minus = Source->Minus;
	Dest->Legal = Source->Legal;

	try
	{
		if (Dest->AllocInt < Source->LenInt)
		{
			if (ConfirmWontLossHighBit)
				throw BFException(ERR_NUMBERTOOBIG, "���������ڴ治��", EXCEPTION_DETAIL);
		}
		if (Dest->AllocFloat < Source->LenFloat)
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
		//�ռ��㹻����,���и���
		Dest->pSInt = Dest->pSRP - Source->LenInt;			//�ҵ�д��λ��
		Dest->LenInt = Source->LenInt;
		strcpy(Dest->pSInt, Source->pSInt);
	}
	else
	{
		//�ض��Ը���
		Dest->pSInt = Dest->DataHead;
		strncpy(Dest->pSInt, Source->pSInt + Source->LenInt - Dest->AllocInt, Dest->AllocInt);
		while (Dest->pSInt[0] == '0')Dest->pSInt++;					//ȥ������ǰ���0
		Dest->LenInt = Dest->pSRP - Dest->pSInt;			//���������ĳ���
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
		//ԴΪ��
		if (!Dest->AllocFloat)
		{
			Dest->LenFloat = 0;
			Dest->pSFloat[0] = 0;
		}
	}

	return;
}



//�Ƚ��������Ĵ�С
/*
����ֵ��ʾ�������Ĵ�С���
0,	OperandA==OperandB
1,	OperandA>OperandB
-1,	OperandA<OperandB
2,	OperandA>>OperandB
-2,	OperandA<<OperandB
*/
int core_BFCmp(const BFDetail *OperandA, const BFDetail *OperandB)
{
	int minus = 1;				//��Ϊ����ʱ,���صĽ���������෴,�˱������Ƶľ����������

								//�жϸ���
	if (OperandA->Minus)
	{
		if (OperandB->Minus)
			minus = -1;//A,Bͬ��
		else
			return -2;//A��B��
	}
	else
	{
		if (OperandB->Minus)
			return 2;//A��B��
					 //else      //A��B��   minusĬ��Ϊ1,����Ҫ�Ķ�
	}
	return core_BFCmp_abs(OperandA, OperandB, minus);

}
int core_BFCmp_abs(const BFDetail*OperandA, const BFDetail *OperandB)
{
	return core_BFCmp_abs(OperandA, OperandB, 1);
}
/*
�Ƚ��������ľ���ֵ��С
*/
int core_BFCmp_abs(const BFDetail*OperandA, const BFDetail *OperandB, int minus)
{
	//�ж�λ
	if (OperandA->LenInt > OperandB->LenInt)
		return 2 * minus;
	else if (OperandA->LenInt < OperandB->LenInt)
		return -2 * minus;
	else
	{
		//����������������ֳ������,����бȽϸ���λ
		int temp = strcmp(OperandA->pSInt, OperandB->pSInt);
		if (temp)
			return temp*minus;		//�Ƚϳ����,����
		else
		{
			//����������ȫ���,�������бȽ�С������
			char *StringAH = OperandA->pSFloat,
				*StringBH = OperandB->pSFloat;

			while (*StringAH && *StringAH == *StringBH)StringAH++, StringBH++;		//�ҵ��������,����ȷ�����Ƕ���Ϊ'\0'
			if (*StringBH && *StringAH > *StringBH)
				return minus;
			else if (*StringAH && *StringAH < *StringBH)
				return -minus;
			else
			{
				//�ҵ�����ȵ���(���ܴ�����ȵ�С��ĩβ��0̫��)
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

