#pragma once
#ifndef RXDEFECTDEF_H
#define RXDEFECTDEF_H
#include <opencv2\opencv.hpp>
#include <iostream>
#include <vector>
#include <windows.h>


using namespace cv;
using namespace std;
typedef unsigned int uint;


// ����������Ϣ�ṹ��
typedef struct _tagRxDetInfo_{
	unsigned int   uImgWidth; 	  // ͼ�����ؿ�
	unsigned int   uImgHeight;	  // ͼ�����ظ�
	unsigned int   uImgChannel;	  // ͼ��ͨ����
	unsigned char* pImgPtr;   	  // ͼ������ָ��
	unsigned int   uMeanMaskSize; // ��ֵģ���С����Χ3-100����СֵӦΪ3��Ϊ����������Ĭ��ֵ��Ϊ15
	int uDetLightDark;            // ����ȱ�ݵ�ѡ����ȱ��Ϊ1����ȱ��Ϊ-1
	unsigned int   uLowThreshold; // �ָ����ֵ����Χ0-50,Ĭ��ֵ��Ϊ2
	unsigned int   uHighThreshold;// �ָ����ֵ����Χ0-50��Ĭ��ֵ��Ϊ10����ʵ�ʲ�����ʱ�򣬸���ֵ����СֵӦ�뵱ǰ����ֵ��ȣ�����������������ʵ��
	unsigned int   uErodeNum;	  // �ָ�����ʴ������ȡֵ����Ϊ0-5��Ĭ��ֵΪ0,һ������²��õ���
	unsigned int   uDilateNum;	  // �ָ������ʹ�����ȡֵ����Ϊ0-5��Ĭ��ֵΪ1��һ������²��õ���
	unsigned int   uMinArea;      // ȥ��αȱ�ݣ���ȱ�ݵ����С��uMinAreaʱ������ȱ�ݽ��б�ע��ȡֵ����Ϊ0-100��Ĭ��ֵΪ10��һ������²��õ���
	unsigned int   uMaxArea;      // ȥ��αȱ�ݣ���ȱ�ݵ��������uMaxAreaʱ������ȱ�ݽ��б�ע��ȡֵ����Ϊ100-10000��Ĭ��ֵΪ8000������ȥ�������ı�Եαȱ�ݣ�һ������²��õ���
	unsigned int   uLengthWidthRatio;//ȥ��αȱ�ݣ���ȱ�ݳ���ȴ���uLengthWidthRatioʱ������ȱ�ݽ��б�ע��ȡֵ����Ϊ4-100��Ĭ��ֵΪ4������ȥ�������ı�Եαȱ�ݣ�һ������²��õ���
}RxDetInfo;



// ȱ����Ϣ�ṹ��
typedef struct _tagRxDefInfo{
	unsigned int    uPtNumber;    // ȱ�����ظ�����ȱ�������
	unsigned int    uContourPtNumber; //ȱ�����������ظ���
	unsigned int    uDefDia;      //ȱ��ֱ����9.7���ӣ�
	unsigned int    pDefTypePtr;  // ȱ�����ͣ�1��ʾ���ף�2��ʾ�������ף�3��ʾ���ƣ�4��ʾ����
	unsigned int*   pPtXPosPtr;   // ȱ������������X����(RxDefResult*)malloc(sizeof(RxDefResult))
	unsigned int*   pPtYPosPtr;   // ȱ������������Y����
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
// �汾��2.0
// ʱ�䣺2019.9.11
// int findDefect(RxDetInfo* pRxInfo, RxDefResult* pRxDefResult);

// ��������: ��ǰ�������λ����Ҫ�����ͷ�dll�ڲ�������ڴ��Լ�ȱ��ͳ����Ϣ����
// ��������ֵ����
// ��������ֵ���� 
// ���ߣ�JTY
// �汾��2.0
// ʱ�䣺2019.9.11
// void resetDefectInfo();
#endif
