#pragma once

#ifndef BFEXCEPTION_H
#define BFEXCEPTION_H



//����ֵ�Ķ���
#define ERR_UNKNOWEXCEPTION				0	//δ֪�Ĵ���
#define ERR_MEMORYALLOCATEDEXCEPTION	1	//�ڴ�������
#define ERR_NULLPOINTEXCEPTION			2	//��ָ��
#define ERR_ILLEGALNUMBER				3	//�Ƿ�������
#define ERR_DIVISORCANNOTBEZERO			4	//��������Ϊ0
#define ERR_ILLEGALPARAM				5	//��������
#define ERR_NUMBERTOOBIG				6	//Դ����̫��,�޷�װ��Ŀ�������
#define ERR_MAYACCURACYLOSS				7	//���ܶ�ʧ����(С������λ������,����ConfirmWontLossAccuracy==true)ʱ���׳�

//#define BFException(ErrVal) BFException(ErrVal,__FILE__,  __LINE__)
#define EXCEPTION_DETAIL __FILE__,  __LINE__
class BFException
{
private:
	int ErrVal;
	const char *File;
	int Line;
	const char* message;
public:
	BFException(int ErrVal, const char* _File, int _Line);
	BFException(int ErrVal, const char* detail, const char* _File, int _Line);
	~BFException();
	void GetMessage();       //���������Ϣ
	int GetID();
};

#endif