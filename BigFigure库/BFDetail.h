#pragma once

//�洢���ݵĽṹ��
struct BFDetail
{
	size_t ReferCount;      //���ü���,�����жϺ�ʱ����
	size_t AllocInt;		//�������ֵ��ѷ����ڴ泤��
	size_t AllocFloat;      //���������ֵ����ȷ��
	size_t LenInt;			//�����������ֵ���Чλ��(ʵ�ʳ���)
	size_t LenFloat;		//С���������ֵ���Чλ��(ʵ�ʳ���)
	bool Minus;             //��ʾ�Ƿ�Ϊ����,���Ϊ����,���ֵΪ1
	bool Legal;				//��ʾ�Ƿ�Ϊ�Ƿ�����,����ǷǷ�����,���ֵΪtrue
	char *pSInt;			//��������������ֵ��ַ������׵�ַ
	char *pSRP;				//�������ֵ�β��ַ(���ڿ��ټ���д��λ��)(����С�������ڵ�λ��)
	char *pSFloat;			//������ĸ��������ֵ��ַ������׵�ַ
	char *DataHead;			//����������ַ����ռ����ָ��
};