#include "RxDefect.h"

uchar* matToUchar(cv::Mat img, unsigned int img_width, unsigned int img_height);

int main()
{

	cv::Mat src = cv::imread("E:/project of ruixin/资料数据/轮毂X射线图像/samples/轮辐/缺陷/108.JPG");
	cv::Size ResImgSiz = cv::Size(src.cols, src.rows);
	resize(src, src, ResImgSiz);
	if (src.channels() == 3)
	{
		cvtColor(src, src, CV_BGR2GRAY);
	}
	uchar * p1 = matToUchar(src, src.cols, src.rows);
	uint cha = src.channels();


	RxDetInfo input;
	input.uImgChannel = cha;
	input.pImgPtr = p1;
	input.uImgWidth = src.cols;
	input.uImgHeight = src.rows;
	//毫米像素比，一个像素为多少毫米
	input.uMillimeterPixelRatio = 10;
	//均值模板大小选择，默认为3，当缺陷特别小的时候将其设为1
	input.uMedianMaskSize = 3;
	//重点调节的3各参数，均值模板大小、低阈值、高阈值
	input.uMeanMaskSize = 15;
	input.uLowThreshold = 7;
	input.uHighThreshold = 12;

	input.uLowTErodeNum = 0;
	input.uLowTDilateNum = 0;
	input.uHighTErodeNum = 0;
	input.uErodeNum = 1;
	input.uDilateNum = 1;
	//亮暗缺陷选择
	input.uDetLightDark = 1;//1为亮缺陷，-1为暗缺陷
	//去除伪缺陷参数
	input.uMinArea = 5;
	input.uMaxArea = 1000;
	input.uLengthWidthRatio = 4;


	RxDefResult result;


	//测试缺陷检测算法的运算时间
	//DWORD start_time = GetTickCount();
	findDefect(&input, &result);
	//DWORD end_time = GetTickCount();
	//cout << "The run time is:" << (end_time - start_time) << "ms!" << endl;

	cout << result.uDefNumber << endl;

	for (int i = 0; i < result.uDefNumber; ++i)
	{
		cout << result.pDefInfoPtr[i].pDefTypePtr << endl;
		cout << result.pDefInfoPtr[i].uDefDiaMillimeter << endl;
		cout << result.pDefInfoPtr[i].uDefAreaMillimeter << endl; //cout直接输出指针，是输出的指针指向的内容,输出缺陷面积点数
		cout << result.pDefInfoPtr[i].uContourPtNumber << endl;   //输出缺陷外轮廓点数（像素）
		for (int j = 0; j < result.pDefInfoPtr[i].uContourPtNumber; ++j)
		{
			cout << result.pDefInfoPtr[i].pPtXPosPtr[j] << endl;  //C++中，如果变量或对象是指针类型，可以使用static_cast转换一下来得到其地址
			cout << result.pDefInfoPtr[i].pPtYPosPtr[j] << endl;
		}
	}
	resetDefectInfo();
	system("pause");
	cv::waitKey();
	return 0;
}



// 函数功能:   将uchar类型的数据转换为Mat类型
// 函数返回值：零代表成功，非零代表失败
// 函数输入值：img  - 输入法人Mat类图像，img_width-图像宽度，img_height-图像高度
uchar* matToUchar(cv::Mat img, unsigned int img_width, unsigned int img_height)
{
	if (img.channels() == 3)
	{
		uchar * p = new uchar[img_width * img_height * 3];
		for (int i = 0; i < img_width * img_height * 3; i++)
		{
			p[i] = (uchar)img.at<cv::Vec3b>(i / (img_width * 3), (i % (img_width * 3)) / 3)[i % 3];
		}
		return p;
	}
	else if (img.channels() == 1)
	{
		uchar * p = new uchar[img_width * img_height];
		for (int i = 0; i<img_height; ++i) //行数--高度
		{
			unsigned char* data = img.ptr<unsigned char>(i); //指向第i行的数据 
			for (int j = 0; j<img_width; ++j) //列数 -- 宽度 
			{
				p[i*img_width + j] = data[j];
			}
		}
		return p;
	}
}