//--------------------轮毂缺陷检测算法---JTY于2019.6.27写于丹东锐新射线--------------------//
#pragma once
#include <opencv2\opencv.hpp>
#include <iostream>
#include <vector>
#include <windows.h>

using namespace cv;
using namespace std;
typedef uint UINT;


typedef struct Result
{
	Mat BiaojiImg;                  //标记的缺陷图像
	string bufbDefSigned;           //有无缺陷
	string bufQuexian;              //缺陷类型
	double DefectArea;              //缺陷总面积
}Result;


extern "C" _declspec(dllexport)  Result findDefect(Mat& ROIimg, UINT backgSize, UINT fengeT, UINT erodeNum, UINT dilateNum);