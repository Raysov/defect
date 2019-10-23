//--------------------���ȱ�ݼ���㷨---JTY��2019.6.27д�ڵ�����������--------------------//
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
	Mat BiaojiImg;                  //��ǵ�ȱ��ͼ��
	string bufbDefSigned;           //����ȱ��
	string bufQuexian;              //ȱ������
	double DefectArea;              //ȱ�������
}Result;


extern "C" _declspec(dllexport)  Result findDefect(Mat& ROIimg, UINT backgSize, UINT fengeT, UINT erodeNum, UINT dilateNum);