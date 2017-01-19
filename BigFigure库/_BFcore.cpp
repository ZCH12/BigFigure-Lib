#define _DLL_API_ 
#include  "BigFiugre.h"

#define CONST_OVER9 106		//��ֵ����'0'+'9'+1
#define INT_MAXLEN 32		//��ֵ����ģ��,��ʾ���������������͵ĳ���,���һ��������һ���������в���ʱ,�������ֳ���С���������,�򽫶������½��з���

//���ļ����ڴ�������㷨���ĺ���
//������Ԫ��������Ԫģ��

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
@param OperandB ������B
@param carry �Ƿ��λ(��ֵֻ����Ϊ1��0)
@return result
*/
BigFigure& core_IntAdd(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB, int carry)
{
	//�ж��ڴ��Ƿ��㹻
	int buffer;										//����ʱ�Ļ�����
	int offsetA = 0, offsetB = 0;


	if (OperandA.Detail->LenInt >= result.Detail->AllocInt)
	{
		//�ڴ治���Դ��,׼������
		if (AutoExpand)
		{
			//�Կռ������չ
			size_t temp1 = OperandA.Detail->LenInt >= OperandB.Detail->LenInt ? OperandA.Detail->LenInt : OperandB.Detail->LenInt;
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
				offsetA = (int)(OperandA.Detail->LenInt - result.Detail->AllocInt);
			}
		}
	}
	if (OperandB.Detail->LenInt >= result.Detail->AllocInt)
	{
		if (AutoExpand)
		{
			//�Կռ������չ
			result.Expand((OperandB.Detail->LenInt >= result.Detail->AllocInt ? OperandB.Detail->LenInt : result.Detail->AllocInt) + 1,
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
				offsetB = (int)(OperandB.Detail->LenInt - result.Detail->AllocInt);
			}
		}

	}

	char *StringAH = OperandA.Detail->pSInt + offsetA,	//�ַ���A����ָ��
		*StringAT = OperandA.Detail->pSRP - 1,			//�ַ�����βָ��,Ҳ��д��λ��
		*StringBH = OperandB.Detail->pSInt + offsetB,	//�ַ���B����ָ��
		*StringBT = OperandB.Detail->pSRP - 1,			//�ַ���B��βָ��,Ҳ��д��λ��
		*StringRH = result.Detail->DataHead,
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
//BF�ӻ������ֵ�ģ��,ֻ����������������,����Ǹ�����,����ת��ΪBFȻ�����core_IntAdd���м���,�˺���ֻ�����������ֵĴ���,�����һ��С�����ϴ˻�������,����Ҫ����core_FloatCopy������С�����ֵ�result
/*
@param result �洢����Ķ���
@param OperandA ������A
@param OperandB ������B(������������)
@param carry �Ƿ��λ(��ֵֻ����Ϊ1��0)
@return result
*/
template <class T>BigFigure& core_IntAdd_Basis(BigFigure & result, const BigFigure & OperandA, T OperandB, int carry)
{
	int buffer = 0;											//����ʱ�Ļ�����

	if (OperandA.Detail->LenInt >= result.Detail->AllocInt || (size_t)(buffer = (int)_CountBits<T>(OperandB)) >= result.Detail->AllocInt)
	{
		//һ�������ڴ�
		if (AutoExpand)
		{
			if (buffer)
				result.Expand((result.Detail->AllocInt > (size_t)buffer ? result.Detail->AllocInt : (size_t)buffer) + 2
					, result.Detail->AllocFloat);
			else
			{
				size_t temp = (result.Detail->AllocInt > OperandA.Detail->LenInt ? result.Detail->AllocInt : OperandA.Detail->LenInt) + 2;
				result.Expand(temp > 32 ? temp : 32, result.Detail->AllocFloat);
			}
		}
		else {
			if (ConfirmWontLossHighBit)
			{
				throw BFException(ERR_MAYACCURACYLOSS, "����޷����浽result��", EXCEPTION_DETAIL);
			}
		}
	}
	char *StringRH = result.Detail->DataHead,
		*StringRT = result.Detail->pSRP - 1,
		*StringAH = OperandA.Detail->pSInt,
		*StringAT = OperandA.Detail->pSRP - 1;

	while (StringRH <= StringRT&&StringAH <= StringAT)
	{
		buffer = *(StringAT--) + OperandB % 10 + carry;
		OperandB /= 10;
		if (buffer > '9')
		{
			buffer -= 10;
			carry = 1;
		}
		else
		{
			carry = 0;
		}
		*(StringRT--) = (char)buffer;
	}

	if (OperandB != 0)
	{
		while (carry&&StringRH <= StringRT&&OperandB != 0)
		{
			buffer = OperandB % 10 + carry;
			OperandB /= 10;
			if (buffer > 9)
			{
				buffer += 38;
				carry = 1;
			}
			else
			{
				buffer += '0';
				carry = 0;
			}
			*(StringRT--) = (char)buffer;
		}
		while (StringRH <= StringRT&&OperandB != 0)
		{
			*(StringRT--) = OperandB % 10 + '0';
			OperandB /= 10;
		}
	}
	if (carry&&StringRH <= StringRT)
		*(StringRT) = '1';
	else
		StringRT++;


	if (OperandB)
	{
		if (ConfirmWontLossHighBit)
		{
			result.Detail->Legal = false;
			throw BFException(ERR_NUMBERTOOBIG, "����̫���޷��洢", EXCEPTION_DETAIL);
		}
	}

	result.Detail->LenInt = result.Detail->pSRP - StringRT;
	result.Detail->pSInt = StringRT;
	result.Detail->Legal = true;
	return result;
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

void test(BigFigure & result, const BigFigure & OperandA, long long OperandB, int carry)
{
	core_IntAdd_Basis<long long>(result, OperandA, OperandB, 0);
	core_FloatCopy(result, OperandA);
}

/*
int core_FloatAdd(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB)
{
	int index_A, index_B = strlen(OperandB.Detail->NumFloat);
	char *String1 = OperandA.Detail->NumFloat, *String2 = OperandB.Detail->NumFloat, *String3 = result.Detail->NumFloat;
	int buffer;
	int carry = 0;

	if (OperandA.Detail->Accuracy)
		index_A = strlen(OperandA.Detail->NumFloat);
	else index_A = 0;

	if (OperandB.Detail->Accuracy)
		index_B = strlen(OperandB.Detail->NumFloat);
	else index_B = 0;


	if (index_A < index_B)
	{
		//���A��С��λ��B�Ķ�
		String3[index_B--] = 0;
		if ((int)result.Detail->Accuracy >= index_A)
		{
			while (index_B >= index_A)
			{
				String3[index_B] = String2[index_B];
				index_B--;
			}
		}
		else
		{
			if (ConfirmWontLossAccuracy)
			{
				//����
				result.Detail->Illage = true;
				throw BFException(ERR_MAYACCURACYLOSS, "Ŀ�����̫С,�޷��洢�㹻��С��λ,���ܶ�ʧ����");
			}
			else {
				//�ض�С��λ,��������
				index_B = result.Detail->Accuracy;
				while (index_B >= index_A)
				{
					String3[index_B] = String2[index_B];
					index_B--;
				}
			}
		}
		index_A = index_B;							//��A,Bͬ��,Ϊ����ֻʹ��index_A��׼��
	}
	else if (index_A > index_B)
	{
		//���A��С��λ��B�ĳ�
		String3[index_A--] = 0;
		if ((int)result.Detail->Accuracy >= index_B)
		{
			while (index_A >= index_B)
			{
				String3[index_A] = String1[index_A];
				index_A--;
			}
		}
		else
		{
			if (ConfirmWontLossAccuracy)
			{
				//����
				result.Detail->Illage = true;
				throw BFException(ERR_MAYACCURACYLOSS, "Ŀ�����̫С,�޷��洢�㹻��С��λ,���ܶ�ʧ����");
			}
			else {
				//�ض�С��λ,��������
				index_A = result.Detail->Accuracy;
				while (index_A <= index_B)
				{
					String3[index_A] = String1[index_A];
					index_A--;
				}
			}
		}
	}
	else
	{
		String3[index_A--] = 0;
	}
	//�����￪ʼ,�����ַ�������һ���α�
	for (; index_A >= 0; index_A--)
	{
		buffer = (int)String1[index_A] + String2[index_A] + carry;
		if (buffer >= CONST_OVER9)
		{
			buffer -= 58;
			carry = 1;
		}
		else
		{
			buffer -= '0';
			carry = 0;
		}
		String3[index_A] = (char)buffer;
	}

	return carry;
}



*/



