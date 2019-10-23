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


// 输入待检测信息结构体
typedef struct _tagRxDetInfo_{
	unsigned int   uImgWidth; 	  // 图像像素宽
	unsigned int   uImgHeight;	  // 图像像素高
	unsigned int   uImgChannel;	  // 图像通道数
	unsigned char* pImgPtr;   	  // 图像数据指针
	unsigned int   uMeanMaskSize; // 均值模板大小，范围3-100，最小值应为3，为奇数递增，默认值设为15
	int uDetLightDark;            // 亮暗缺陷的选择，亮缺陷为1，暗缺陷为-1
	unsigned int   uLowThreshold; // 分割低阈值，范围0-50,默认值设为2
	unsigned int   uHighThreshold;// 分割高阈值，范围0-50，默认值设为10，在实际操作的时候，高阈值的最小值应与当前低阈值相等，设置两个滑条联动实现
	unsigned int   uErodeNum;	  // 分割结果腐蚀次数，取值设置为0-5，默认值为0,一般情况下不用调节
	unsigned int   uDilateNum;	  // 分割结果膨胀次数，取值设置为0-5，默认值为1，一般情况下不用调节
	unsigned int   uMinArea;      // 去除伪缺陷，当缺陷的面积小于uMinArea时，不对缺陷进行标注；取值设置为0-100，默认值为10；一般情况下不用调节
	unsigned int   uMaxArea;      // 去除伪缺陷，当缺陷的面积大于uMaxArea时，不对缺陷进行标注；取值设置为100-10000，默认值为8000，用于去除狭长的边缘伪缺陷；一般情况下不用调节
	unsigned int   uLengthWidthRatio;//去除伪缺陷，当缺陷长宽比大于uLengthWidthRatio时，不对缺陷进行标注；取值设置为4-100，默认值为4，用于去除狭长的边缘伪缺陷；一般情况下不用调节
}RxDetInfo;



// 缺陷信息结构体
typedef struct _tagRxDefInfo{
	unsigned int    uPtNumber;    // 缺陷像素个数（缺陷面积）
	unsigned int    uContourPtNumber; //缺陷外轮廓像素个数
	unsigned int    uDefDia;      //缺陷直径（9.7增加）
	unsigned int    pDefTypePtr;  // 缺陷类型，1表示气孔，2表示缩松缩孔，3表示裂纹，4表示夹渣
	unsigned int*   pPtXPosPtr;   // 缺陷外轮廓像素X坐标(RxDefResult*)malloc(sizeof(RxDefResult))
	unsigned int*   pPtYPosPtr;   // 缺陷外轮廓像素Y坐标
}RxDefInfo;



// 输出检测信息结构体
typedef struct _tagRxDefResult_
{
	unsigned int    uDefNumber;     // 有无缺陷, 0表示无缺陷，1表示1个缺陷
	RxDefInfo*      pDefInfoPtr;    //缺陷信息结构体指针
}RxDefResult;


extern "C" _declspec(dllexport) int findDefect(RxDetInfo* pRxInfo, RxDefResult* pRxDefResult);
extern "C" _declspec(dllexport) void resetDefectInfo();


// 函数功能:   检测图像缺陷
// 函数返回值：零代表成功，非零代表失败
// 函数输入值：pRxInfo  - 指向输入数据结构体指针
//	           pRxDefResult - 指向输出数据结构体指针
// 作者：JTY
// 版本：2.0
// 时间：2019.9.11
// int findDefect(RxDetInfo* pRxInfo, RxDefResult* pRxDefResult);

// 函数功能: 当前检测结果复位，主要用于释放dll内部申请的内存以及缺陷统计信息清零
// 函数返回值：无
// 函数输入值：无 
// 作者：JTY
// 版本：2.0
// 时间：2019.9.11
// void resetDefectInfo();
#endif
