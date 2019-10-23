#pragma once
#ifndef RXDEFECTDEF_H
#define RXDEFECTDEF_H
//--------------------2019.10.7���Ӻ������ر�----------------------//
//�������ȱ���ر�Сʱ��Ϊ������������ȱ�ݵ�ʱ�򣬲�������ֵ�˲�����������ֵ�˲��İ뾶��Ϊ1
#include <opencv2\opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
typedef unsigned int uint;

//����������Ϣ�ṹ��
typedef struct _tagRxDetInfo_{
	unsigned int   uImgWidth; 	  // ͼ�����ؿ�
	unsigned int   uImgHeight;	  // ͼ�����ظ�
	unsigned int   uImgChannel;	  // ͼ��ͨ���� 
	unsigned char* pImgPtr;   	  // ͼ������ָ��
	int uDetLightDark;            // ����ȱ�ݵ�ѡ����ȱ��Ϊ1����ȱ��Ϊ-1
	double uMillimeterPixelRatio;  // �������ر�
	unsigned int   uMedianMaskSize;// ��ֵģ���С���������룬Ĭ��ֵΪ3������ȱ���ر�Сʱ����������Ϊ1
	unsigned int   uMeanMaskSize;  // ��ֵģ���С����Χ3-100����СֵӦΪ3��Ϊ����������Ĭ��ֵ��Ϊ15
	unsigned int   uLowThreshold;  // �ָ����ֵ����Χ0-100,Ĭ��ֵ��Ϊ2
	unsigned int   uHighThreshold; // �ָ����ֵ����Χ0-100��Ĭ��ֵ��Ϊ10����ʵ�ʲ�����ʱ�򣬸���ֵ����СֵӦ�뵱ǰ����ֵ��ȣ�����������������ʵ��
	unsigned int   uLowTErodeNum;  // ����ֵ�����ʴ��������Χ0-10��Ĭ��ֵ��Ϊ0��һ������²��õ��ڣ�������ȱ�ݺͷ�ȱ��ճ������������Ե��ڴ˲�����ʹȱ�ݺͷ�ȱ�ݶϿ�
	unsigned int   uLowTDilateNum; // ����ֵ������ʹ�������Χ0-10��Ĭ��ֵ��Ϊ0��һ������²��õ��ڣ�����⺣��״ȱ��ʱ���Խ������Ϊ1��ʹ��������ȱ��������������ͨ��
	unsigned int   uHighTErodeNum; // ����ֵ�����ʴ������ȡֵ����Ϊ0-10��Ĭ��ֵΪ0,һ������²��õ��ڣ��˲�����Ϊȥ��αȱ�ݣ�ȷ���õ��ķָ���ȫ������ȱ�ݣ��ǶԸ���ֵ�ָ�����һ������
	unsigned int   uDilateNum;	   // �ؽ�������ʹ�����ȡֵ����Ϊ0-10��Ĭ��ֵΪ0��һ������²��õ���
	unsigned int   uErodeNum;      // �ؽ������ʴ������ȡֵ����Ϊ0-10��Ĭ��ֵΪ1��һ������²��õ��� 
	unsigned int   uMinArea;       // ȱ����С�����ȥ��αȱ�ݣ���ȱ�ݵ����С��uMinAreaʱ������ȱ�ݽ��б�ע��ȡֵ����Ϊ0-100��Ĭ��ֵΪ10��һ������²��õ��ڣ�
	unsigned int   uMaxArea;       // ȱ����������ȥ��αȱ�ݣ���ȱ�ݵ��������uMaxAreaʱ������ȱ�ݽ��б�ע��ȡֵ����Ϊ100-20000��Ĭ��ֵΪ8000������ȥ�������ı�Եαȱ�ݣ�
	unsigned int   uLengthWidthRatio;//ȱ�ݳ����ȣ�ȥ��αȱ�ݣ���ȱ�ݳ����ȴ���uLengthWidthRatioʱ������ȱ�ݽ��б�ע��ȡֵ����Ϊ4-100��Ĭ��ֵΪ4������ȥ�������ı�Եαȱ�ݣ�һ������²��õ���
}RxDetInfo;



// ȱ����Ϣ�ṹ��
typedef struct _tagRxDefInfo{
	double    uDefAreaMillimeter; // ȱ���������λ����
	double    uDefDiaMillimeter;  // ȱ��ֱ������λ����
	unsigned int uPtNumber;       // ȱ�����ظ���
	unsigned int uDefDia;         // ȱ��ֱ�������ظ���
	unsigned int    uContourPtNumber; // ȱ�����������ظ���
	unsigned int    pDefTypePtr;      // ȱ�����ͣ�1-��ʾ���ף�2-��ʾ�������ף�3��ʾ���ƣ�4��ʾ����
	unsigned int*   pPtXPosPtr;       // ȱ������������X����(RxDefResult*)malloc(sizeof(RxDefResult))
	unsigned int*   pPtYPosPtr;       // ȱ������������Y����  
}RxDefInfo;



// ��������Ϣ�ṹ��
typedef struct _tagRxDefResult_
{
	unsigned int    uDefNumber;     // ����ȱ��, 0��ʾ��ȱ�ݣ�1��ʾ1��ȱ��
	RxDefInfo*      pDefInfoPtr;    // ȱ����Ϣ�ṹ��ָ��
}RxDefResult;



extern "C" _declspec(dllexport) int findDefect(RxDetInfo* pRxInfo, RxDefResult* pRxDefResult);
extern "C" _declspec(dllexport) void resetDefectInfo();


// ��������:   ���ͼ��ȱ��
// ��������ֵ��������ɹ����������ʧ��
// ��������ֵ��pRxInfo  - ָ���������ݽṹ��ָ��
//	           pRxDefResult - ָ��������ݽṹ��ָ��
// ���ߣ�JTY
// �汾��2.2
// ʱ�䣺2019.10.8
// int findDefect(RxDetInfo* pRxInfo, RxDefResult* pRxDefResult);

// ��������: ��ǰ�������λ����Ҫ�����ͷ�dll�ڲ�������ڴ��Լ�ȱ��ͳ����Ϣ����
// ��������ֵ����
// ��������ֵ���� 
// ���ߣ�JTY
// �汾��2.2
// ʱ�䣺2019.10.8
// void resetDefectInfo();
#endif