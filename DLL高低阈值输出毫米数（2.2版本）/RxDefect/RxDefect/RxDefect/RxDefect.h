#pragma once
#ifndef RXDEFECTDEF_H
#define RXDEFECTDEF_H
//--------------------2019.10.7增加毫米像素比----------------------//
//当被测的缺陷特别小时，为椒盐噪声类型缺陷的时候，不进行中值滤波操作，将中值滤波的半径设为1
#include <opencv2\opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
typedef unsigned int uint;

//输入待检测信息结构体
typedef struct _tagRxDetInfo_{
	unsigned int   uImgWidth; 	  // 图像像素宽
	unsigned int   uImgHeight;	  // 图像像素高
	unsigned int   uImgChannel;	  // 图像通道数 
	unsigned char* pImgPtr;   	  // 图像数据指针
	int uDetLightDark;            // 亮暗缺陷的选择，亮缺陷为1，暗缺陷为-1
	double uMillimeterPixelRatio;  // 毫米像素比
	unsigned int   uMedianMaskSize;// 中值模板大小；用来降噪，默认值为3，当其缺陷特别小时，将其设置为1
	unsigned int   uMeanMaskSize;  // 均值模板大小；范围3-100，最小值应为3，为奇数递增，默认值设为15
	unsigned int   uLowThreshold;  // 分割低阈值；范围0-100,默认值设为2
	unsigned int   uHighThreshold; // 分割高阈值；范围0-100，默认值设为10，在实际操作的时候，高阈值的最小值应与当前低阈值相等，设置两个滑条联动实现
	unsigned int   uLowTErodeNum;  // 低阈值结果腐蚀次数；范围0-10，默认值设为0，一般情况下不用调节，当出现缺陷和非缺陷粘连的情况，可以调节此参数，使缺陷和非缺陷断开
	unsigned int   uLowTDilateNum; // 低阈值结果膨胀次数；范围0-10，默认值设为0，一般情况下不用调节，当检测海绵状缺陷时可以将其调节为1，使检测出来的缺陷完整，具有连通性
	unsigned int   uHighTErodeNum; // 高阈值结果腐蚀次数；取值设置为0-10，默认值为0,一般情况下不用调节，此参数是为去除伪缺陷，确保得到的分割结果全部是真缺陷，是对高阈值分割结果的一个补充
	unsigned int   uDilateNum;	   // 重建结果膨胀次数；取值设置为0-10，默认值为0，一般情况下不用调节
	unsigned int   uErodeNum;      // 重建结果腐蚀次数；取值设置为0-10，默认值为1，一般情况下不用调节 
	unsigned int   uMinArea;       // 缺陷最小面积；去除伪缺陷，当缺陷的面积小于uMinArea时，不对缺陷进行标注；取值设置为0-100，默认值为10；一般情况下不用调节；
	unsigned int   uMaxArea;       // 缺陷最大面积；去除伪缺陷，当缺陷的面积大于uMaxArea时，不对缺陷进行标注；取值设置为100-20000，默认值为8000，用于去除狭长的边缘伪缺陷；
	unsigned int   uLengthWidthRatio;//缺陷长宽比；去除伪缺陷，当缺陷长宽比大于uLengthWidthRatio时，不对缺陷进行标注；取值设置为4-100，默认值为4，用于去除狭长的边缘伪缺陷；一般情况下不用调节
}RxDetInfo;



// 缺陷信息结构体
typedef struct _tagRxDefInfo{
	double    uDefAreaMillimeter; // 缺陷面积，单位毫米
	double    uDefDiaMillimeter;  // 缺陷直径，单位毫米
	unsigned int uPtNumber;       // 缺陷像素个数
	unsigned int uDefDia;         // 缺陷直径的像素个数
	unsigned int    uContourPtNumber; // 缺陷外轮廓像素个数
	unsigned int    pDefTypePtr;      // 缺陷类型，1-表示气孔，2-表示缩松缩孔，3表示裂纹，4表示夹渣
	unsigned int*   pPtXPosPtr;       // 缺陷外轮廓像素X坐标(RxDefResult*)malloc(sizeof(RxDefResult))
	unsigned int*   pPtYPosPtr;       // 缺陷外轮廓像素Y坐标  
}RxDefInfo;



// 输出检测信息结构体
typedef struct _tagRxDefResult_
{
	unsigned int    uDefNumber;     // 有无缺陷, 0表示无缺陷，1表示1个缺陷
	RxDefInfo*      pDefInfoPtr;    // 缺陷信息结构体指针
}RxDefResult;



extern "C" _declspec(dllexport) int findDefect(RxDetInfo* pRxInfo, RxDefResult* pRxDefResult);
extern "C" _declspec(dllexport) void resetDefectInfo();


// 函数功能:   检测图像缺陷
// 函数返回值：零代表成功，非零代表失败
// 函数输入值：pRxInfo  - 指向输入数据结构体指针
//	           pRxDefResult - 指向输出数据结构体指针
// 作者：JTY
// 版本：2.2
// 时间：2019.10.8
// int findDefect(RxDetInfo* pRxInfo, RxDefResult* pRxDefResult);

// 函数功能: 当前检测结果复位，主要用于释放dll内部申请的内存以及缺陷统计信息清零
// 函数返回值：无
// 函数输入值：无 
// 作者：JTY
// 版本：2.2
// 时间：2019.10.8
// void resetDefectInfo();
#endif