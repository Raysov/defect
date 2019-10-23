#ifndef ZXDEFECTDEF_H
#define ZXDEFECTDEF_H
#include <opencv2\opencv.hpp>
#include <iostream>
#include <vector>
#include <windows.h>

using namespace cv;
using namespace std;
typedef unsigned int uint;

// ����������Ϣ�ṹ��
typedef struct _tagRxDetInfo_{
	unsigned int   uImgWidth; 	// ͼ�����ؿ�
	unsigned int   uImgHeight;	// ͼ�����ظ�
	unsigned int   uImgChannel;	// ͼ��ͨ����
	unsigned char* pImgPtr;   	// ͼ������ָ��
	unsigned int   backgSize; 	// ��ֵģ���С����СֵӦΪ3��Ϊ����������Ĭ��ֵ��Ϊ15
	unsigned int   fengeT;	    // �ָ���ֵ��0-50,Ĭ��ֵӦ��Ϊ5
	unsigned int   erodeNum;	// �ָ�����ʴ������ȡֵ����Ϊ1-5��Ĭ��ֵΪ2
	unsigned int   dilateNum;	// �ָ������ʹ�����ȡֵ����Ϊ1-5��Ĭ��ֵΪ2
}RxDetInfo;

// ȱ����Ϣ�ṹ��
typedef struct _tagRxDefInfo{
	unsigned int    uPtNumber;         // ȱ�����ظ���
	unsigned int    uContourPtNumber;  // ȱ�����������ظ���
	unsigned int    pDefTypePtr;       // ȱ�����ͣ�1-��ʾ���ף�2-��ʾ�������ף�3��ʾ����
	unsigned int*   pPtXPosPtr;        // ȱ������������X����
	unsigned int*   pPtYPosPtr;        // ȱ������������Y����
}RxDefInfo;


// ��������Ϣ�ṹ��
typedef struct _tagRxDefResult_
{
	unsigned int    uDefNumber;     // ����ȱ��, 0��ʾ��ȱ�ݣ�1��ʾ1��ȱ��
	RxDefInfo*      pDefInfoPtr;    //ȱ����Ϣ�ṹ��ָ��
}RxDefResult;


extern "C" _declspec(dllexport) int findDefect(RxDetInfo* pRxInfo, RxDefResult* pRxDefResult);
extern "C" _declspec(dllexport) void resetDefectInfo();


// ��������:   ���ͼ��ȱ��
// ��������ֵ�������ɹ����������ʧ��
// ��������ֵ��pRxInfo  - ָ���������ݽṹ��ָ��
//	           pRxDefResult - ָ��������ݽṹ��ָ��
// ���ߣ�JTY
// �汾��1.1
// ʱ�䣺2019.7.17
// int findDefect(RxDetInfo* pRxInfo, RxDefResult* pRxDefResult);

// ��������: ��ǰ�������λ����Ҫ�����ͷ�dll�ڲ�������ڴ��Լ�ȱ��ͳ����Ϣ����
// ��������ֵ����
// ��������ֵ���� 
// ���ߣ�JTY
// �汾��1.1
// ʱ�䣺2019.7.17
// void resetDefectInfo();
#endif