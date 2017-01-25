#include  "BigFiugre.h"
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

BigFigure & BFAdd(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB)
{
	bool minusA = OperandA.Detail->Minus, minusB = OperandB.Detail->Minus;
	if (!(minusA || minusB))
	{
		//�������
		core_IntAdd(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = false;
	}
	else if (minusA && !minusB)
	{
		//�������
		if (BFCmp_abs(OperandA, OperandB, 1) > 0)
		{
			//-5+3=-2
			core_IntSub(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatSub(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
			result.Detail->Minus = true;
		}
		else
		{
			//-2+5=3
			core_IntSub(result.Detail, OperandB.Detail->pSInt, OperandB.Detail->LenInt, OperandA.Detail->pSInt, OperandA.Detail->LenInt, core_FloatSub(result.Detail, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat));
			result.Detail->Minus = false;
		}
	}
	else if (!minusA&&minusB)
	{
		//�������
		if (BFCmp_abs(OperandA, OperandB, 1) >= 0)
		{
			//5-3=2
			core_IntSub(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatSub(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
			result.Detail->Minus = false;
		}
		else
		{
			//2-5=-3
			core_IntSub(result.Detail, OperandB.Detail->pSInt, OperandB.Detail->LenInt, OperandA.Detail->pSInt, OperandA.Detail->LenInt, core_FloatSub(result.Detail, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat));
			result.Detail->Minus = true;
		}
	}
	else {
		//�������
		core_IntAdd(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = true;
	}
	return result;
}

BigFigure & BFSub(BigFigure & result, const BigFigure & OperandA, const BigFigure & OperandB)
{
	bool minusA = OperandA.Detail->Minus, minusB = OperandB.Detail->Minus;

	if (!(minusA || minusB))
	{
		//�������
		if (BFCmp_abs(OperandA, OperandB, 1) >= 0)
		{
			//5-1=4
			core_IntSub(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatSub(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
			result.Detail->Minus = false;
		}
		else
		{
			//5-6=-1
			core_IntSub(result.Detail, OperandB.Detail->pSInt, OperandB.Detail->LenInt, OperandA.Detail->pSInt, OperandA.Detail->LenInt, core_FloatSub(result.Detail, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat));
			result.Detail->Minus = true;
		}
	}
	else if (minusA && !minusB)
	{
		//�������
		//-5-9=-14
		core_IntAdd(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = true;
	}
	else if (!minusA&&minusB)
	{
		//�������
		//9--5=14
		core_IntAdd(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatAdd(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
		result.Detail->Minus = false;
	}
	else {
		//�������
		if (BFCmp_abs(OperandA, OperandB, 1) >= 0)
		{
			//-5--1=-4
			core_IntSub(result.Detail, OperandA.Detail->pSInt, OperandA.Detail->LenInt, OperandB.Detail->pSInt, OperandB.Detail->LenInt, core_FloatSub(result.Detail, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat));
			result.Detail->Minus = true;
		}
		else
		{
			//-5--6=1
			core_IntSub(result.Detail, OperandB.Detail->pSInt, OperandB.Detail->LenInt, OperandA.Detail->pSInt, OperandA.Detail->LenInt, core_FloatSub(result.Detail, OperandB.Detail->pSFloat, OperandB.Detail->LenFloat, OperandA.Detail->pSFloat, OperandA.Detail->LenFloat));
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
			//else      //A��B��   minusĬ��Ϊ1,����Ҫ�Ķ�
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
		return 2 * minus;
	else if (OperandA.Detail->LenInt < OperandB.Detail->LenInt)
		return -2 * minus;
	else
	{
		//����������������ֳ������,����бȽϸ���λ
		int temp = strcmp(OperandA.Detail->pSInt, OperandB.Detail->pSInt);
		if (temp)
			return temp*minus;		//�Ƚϳ����,����
		else
		{
			//����������ȫ���,�������бȽ�С������
			char *StringAH = OperandA.Detail->pSFloat,
				*StringBH = OperandB.Detail->pSFloat;

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

