#ifndef ZXDEFECTDEF_H
#define ZXDEFECTDEF_H
#include <opencv2\opencv.hpp>
#include <iostream>
#include <vector>
#include <windows.h>

using namespace cv;
using namespace std;
typedef unsigned int uint;

// 输入待检测信息结构体
typedef struct _tagRxDetInfo_{
	unsigned int   uImgWidth; 	// 图像像素宽
	unsigned int   uImgHeight;	// 图像像素高
	unsigned int   uImgChannel;	// 图像通道数
	unsigned char* pImgPtr;   	// 图像数据指针
	unsigned int   backgSize; 	// 均值模板大小，最小值应为3，为奇数递增，默认值设为15
	unsigned int   fengeT;	    // 分割阈值，0-50,默认值应设为5
	unsigned int   erodeNum;	// 分割结果腐蚀次数，取值设置为1-5，默认值为2
	unsigned int   dilateNum;	// 分割结果膨胀次数，取值设置为1-5，默认值为2
}RxDetInfo;

// 缺陷信息结构体
typedef struct _tagRxDefInfo{
	unsigned int    uPtNumber;         // 缺陷像素个数
	unsigned int    uContourPtNumber;  // 缺陷外轮廓像素个数
	unsigned int    pDefTypePtr;       // 缺陷类型，1-表示气孔，2-表示缩松缩孔，3表示裂纹
	unsigned int*   pPtXPosPtr;        // 缺陷外轮廓像素X坐标
	unsigned int*   pPtYPosPtr;        // 缺陷外轮廓像素Y坐标
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
// 版本：1.1
// 时间：2019.7.17
// int findDefect(RxDetInfo* pRxInfo, RxDefResult* pRxDefResult);

// 函数功能: 当前检测结果复位，主要用于释放dll内部申请的内存以及缺陷统计信息清零
// 函数返回值：无
// 函数输入值：无 
// 作者：JTY
// 版本：1.1
// 时间：2019.7.17
// void resetDefectInfo();
#endif