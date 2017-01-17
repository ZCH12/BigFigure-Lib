#define _DLL_API_ 
#include  "BigFiugre.h"

#define CONST_OVER9 106		//��ֵ����'0'+'6'+1

//���ļ����ڴ�������㷨���ĺ���
//������Ԫ��������Ԫģ��

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
	int buffer;									//����ʱ�Ļ�����
	int index_r = (int)result.Detail->AllocInt - 1,
		index_A = (int)OperandA.Detail->LenInt - 1,
		index_B = (int)OperandB.Detail->LenInt - 1;	//���ڴ����λ���±�
	char *String1 = OperandA.Detail->pSInt,		//�����ַ�������ָ��
		*String2 = OperandB.Detail->pSInt;

	if (index_A > index_r)
	{
		//�ڴ治���Դ��,׼������
		if (ConfirmWontLossHighBit)
			throw BFException(ERR_NUMBERTOOBIG, "������A��ֵ̫��,�޷��洢��result��", EXCEPTION_DETAIL);
		else
		{
			//���нض�(��ȥ��λ),ע��,�ض�֮������ָ�֮ǰ��ͬ
			String1 += OperandA.Detail->LenInt - result.Detail->AllocInt;
			index_A = index_r;
		}
	}
	if (index_B > index_r)
	{
		//�ڴ治���Դ��,׼������
		if (ConfirmWontLossHighBit)
			throw BFException(ERR_NUMBERTOOBIG, "������B��ֵ̫��,�޷��洢��result��", EXCEPTION_DETAIL);
		else
		{
			//���нض�(��ȥ��λ),ע��,�ض�֮������ָ�֮ǰ��ͬ
			String2 += OperandB.Detail->LenInt - result.Detail->AllocInt;
			index_B = index_r;
		}
	}

	while (index_A >= 0 && index_B >= 0)
	{
		buffer = (int)String1[index_A] + String2[index_B] + carry;
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
		result.Detail->DataHead[index_r--] = (char)buffer;
		index_A--;
		index_B--;
	}

	//�����ڽ�λʱ,����A��������λʱ,Ҫ���Ͻ�λ,��û�н�λ����Ǵ���ĸ���
	while (carry&&index_A >= 0)
	{
		buffer = String1[index_A] + carry;
		if (buffer >= '9')
		{
			//����9,��Ҫ���н�λ
			buffer -= 10;
			carry = 1;
		}
		else
		{
			//����û�д���9,����Ҫ��λ
			carry = 0;
		}
		result.Detail->DataHead[index_r--] = (char)buffer;
		index_A--;
	}
	//�����ڽ�λʱ,����B��������λʱ,Ҫ���Ͻ�λ,��û�н�λ����Ǵ���ĸ���
	while (carry&&index_B >= 0)
	{
		buffer = String2[index_B] + carry;
		if (buffer >= '9')
		{
			//����9,��Ҫ���н�λ
			buffer -= 10;
			carry = 1;
		}
		else
		{
			//����û�д���9,����Ҫ��λ
			carry = 0;
		}
		result.Detail->DataHead[index_r--] = (char)buffer;
		index_B--;
	}
	//����ʣ�µ�,A��B����Ĳ�����û�н�λ��λ(������ط�ʱ,����ȷ���������Խ������)
	while (index_A >= 0)
		result.Detail->DataHead[index_r--] = String1[index_A--];
	while (index_B >= 0)
		result.Detail->DataHead[index_r--] = String2[index_B--];

	if (carry)
	{
		if (index_r >= 0)
			result.Detail->DataHead[index_r--] = '1';
	}
	result.Detail->Legal = true;
	result.Detail->LenInt = result.Detail->AllocInt - index_r - 1;
	result.Detail->pSInt = result.Detail->DataHead + index_r + 1;
	if (!ConfirmWontLossHighBit)
	{
		while (*result.Detail->pSInt == '0')result.Detail->pSInt++;
		result.Detail->LenInt = result.Detail->pSRP - result.Detail->pSInt;
		if (result.Detail->LenInt == 0)
		{
			result.Detail->LenInt = 1;
			result.Detail->pSInt = result.Detail->pSRP - 1;
			result.Detail->pSInt[0] = '0';
		}
	}
	return result;


}
template <class T>BigFigure& core_IntAdd_Basis(BigFigure & result, const BigFigure & OperandA, T OperandB, int carry)
{
	int index_p = result.Detail->IntAllocatedLen - 1, index_r = OperandA.Detail->LenInt - 1;
	char *SourceString = OperandA.Detail->pSInt, *String3 = result.Detail->StringHead;
	int buffer;
	for (; index_r >= 0 && OperandB != 0; index_r--)
	{
		buffer = SourceString[index_r];
		buffer += OperandB % 10 + carry;
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
		String3[index_p--] = (char)buffer;
	}

	if (OperandB != 0)
	{
		if (ConfirmWontLossHighBit)
		{
			result.Detail->Illage = false;
			throw BFException(ERR_NUMBERTOOBIG, "����̫���޷��洢");
		}
	}
	else {
		while (carry&&index_p >= 0 && index_r >= 0)
		{
			buffer = SourceString[index_r] + carry;
			if (buffer > '9')
			{
				buffer -= 10;
				carry = 1;
			}
			else
			{
				carry = 0;
			}
			String3[index_p] = (char)buffer;
			index_p--, index_r--;
		}
		if (carry)
		{
			String3[index_p--] = '1';
			carry = false;
		}

	}
	while (index_p >= 0 && index_r >= 0)
		String3[index_p--] = SourceString[index_r--];

	result.Detail->LenInt = result.Detail->IntAllocatedLen - index_p - 1;
	result.Detail->pSInt = String3 + (index_p == -1 ? 0 : index_p) + 1;
	result.Detail->Illage = false;
	return result;
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



