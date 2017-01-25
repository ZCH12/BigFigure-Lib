#define _DLL_API_ 
#include "NumStringDetail.h"

inline void NumStringDetail::NumCheck(const char * NumString)
{
	const char *tempString = NumString;

	size_t StringLen = strlen(NumString);
	if (StringLen == 0)
		throw BFException(ERR_ILLEGALNUMBER, "�ַ�������Ϊ��", EXCEPTION_DETAIL);

	if (this->NumString&&this->NumString != NumString)
	{
		this->NumString = new char[StringLen + 1];
		strcpy(this->NumString, NumString);
	}

	bool HasNumPre = false;			//��¼��ָ������ǰ���Ƿ�������
	bool HasPoint = false;			//��¼�Ƿ���С����
	bool Scinotation = false;		//��¼�Ƿ��Կ�ѧ���������б��
	bool IntBeZero = true;			//��¼���������Ƿ�Ϊ0,���ھ���������±��Ƿ���ҵ���Чλ������


	//��ʼ����
	for (; *tempString != 0; tempString++)
	{
		//��ʼ�����ж�
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
							//��������,���õ�һ����Ч���ֵ��ַ�����ָ��
							this->IntStart_p = tempString - NumString;		//����Ϊ0ʱ,���ֵ�һ����Ч��λ,����������ʼ��λ��
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
				//�����Ų����������м�,������Ч
				throw BFException(ERR_ILLEGALNUMBER, "�����м䲻�ܲ��Ӹ���", EXCEPTION_DETAIL);
			}
			if (Scinotation)
			{
				if (this->ExpMinus)
				{
					//��ָ���������и���ʱ(������������),������Ч
					throw BFException(ERR_ILLEGALNUMBER, "���ܴ��ڶ������", EXCEPTION_DETAIL);
				}
				else
					this->ExpMinus = 1;
			}
			else
			{
				//�����������
				if (this->RadixMinus)
				{
					//�ڵ����������и���ʱ(������������),������Ч
					throw BFException(ERR_ILLEGALNUMBER, "���ܴ��ڶ������", EXCEPTION_DETAIL);
				}
				else
					this->RadixMinus = 1;
			}
			continue;
		}
		else if (*tempString == '.')
		{
			if (Scinotation) {
				//ָ��������С����,������Ч
				throw BFException(ERR_ILLEGALNUMBER, "ָ������ֻ��Ϊ����", EXCEPTION_DETAIL);
			}
			else {
				if (HasPoint)
				{
					//��������С����,������Ч
					throw BFException(ERR_ILLEGALNUMBER, "һ������ֻ����һ��С����", EXCEPTION_DETAIL);
				}
				else
				{
					//���������ĳ���

					if (HasNumPre)
						this->IntLen = tempString - NumString - this->IntStart_p;		//�����������ֵĳ���
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
				//һ�����ֳ�������E,���ִ���
				throw BFException(ERR_ILLEGALNUMBER, "һ������������һ��ָ��", EXCEPTION_DETAIL);
			}
			else
			{
				if (HasPoint)
				{
					//����ΪС��
					if (HasNumPre)
						this->FloatLen = tempString - NumString - this->FloatStart_p;	//����С�����ֳ���,���С������û������,��رձ��
					else
						HasPoint = false;
				}
				else
				{
					//�ڵ���Ϊ����
					//���������ĳ���
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
				//�����Ų����������м�,������Ч
				throw BFException(ERR_ILLEGALNUMBER, "�����м䲻�ܲ��ӼӺ�", EXCEPTION_DETAIL);
			}
			continue;
		}
		else {
			//���������ַ�,���ַǷ�
			throw BFException(ERR_ILLEGALNUMBER, "�����ַ������ܲ��������޹��ַ�", EXCEPTION_DETAIL);
		}
	}

	//����for
	if (HasPoint)
	{
		const char *temp = NumString + this->FloatStart_p;
		while (this->FloatLen&& temp[this->FloatLen] == '0')	//ȥ��С������'0'
			this->FloatLen--;
	}

	this->IntBeZero = IntBeZero;
	if (Scinotation)			//�ж��Ƿ�Ϊ��ѧ��������ʾ����
	{
		//��ѧ��������ʾ����
		if (HasNumPre)			//�ж�ָ��λ�Ƿ�������
		{
			this->Mode = 2;		//��ʼ��Ϊ2,Mode��Ϊ3,4
			this->ExpLen = tempString - NumString - this->ExpStart_p;	//����ָ��λ�ĳ���
		}
		else
		{
			this->Mode = 0;		//��ʼ��Ϊ0,Mode��Ϊ1,2
		}
	}
	else
	{
		//�ñ�׼��ʾ����ʾ������
		this->Mode = 0;		//��ʼ��Ϊ0,Mode��Ϊ1,2
		if (HasPoint)		//���㻹δ����ĳ���ֵ
			this->FloatLen = tempString - NumString - this->FloatStart_p;
		else
			this->IntLen = tempString - NumString - this->IntStart_p;
	}

	if (HasPoint)					//�ж��Ƿ�ΪС��
	{
		//С��,����һ���ж�С��λ�ĳ���,���С��λΪ0,����С��
		if (this->FloatLen)			//���С��λ������,��������С��
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
		//����
		this->Mode += 1;
	}
	if ((!this->FloatLen&&IntBeZero) || !(HasNumPre || HasPoint))
	{
		//����Ϊ0,�������:
		//1.����Ϊ0
		//2.С��Ϊ0
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
