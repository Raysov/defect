#include "RxDefect.h"

uchar* matToUchar(cv::Mat img, unsigned int img_width, unsigned int img_height);

int main()
{

	cv::Mat src = cv::imread("E:/project of ruixin/��������/���X����ͼ��/samples/�ַ�/ȱ��/108.JPG");
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
	//�������رȣ�һ������Ϊ���ٺ���
	input.uMillimeterPixelRatio = 10;
	//��ֵģ���Сѡ��Ĭ��Ϊ3����ȱ���ر�С��ʱ������Ϊ1
	input.uMedianMaskSize = 3;
	//�ص���ڵ�3����������ֵģ���С������ֵ������ֵ
	input.uMeanMaskSize = 15;
	input.uLowThreshold = 7;
	input.uHighThreshold = 12;

	input.uLowTErodeNum = 0;
	input.uLowTDilateNum = 0;
	input.uHighTErodeNum = 0;
	input.uErodeNum = 1;
	input.uDilateNum = 1;
	//����ȱ��ѡ��
	input.uDetLightDark = 1;//1Ϊ��ȱ�ݣ�-1Ϊ��ȱ��
	//ȥ��αȱ�ݲ���
	input.uMinArea = 5;
	input.uMaxArea = 1000;
	input.uLengthWidthRatio = 4;


	RxDefResult result;


	//����ȱ�ݼ���㷨������ʱ��
	//DWORD start_time = GetTickCount();
	findDefect(&input, &result);
	//DWORD end_time = GetTickCount();
	//cout << "The run time is:" << (end_time - start_time) << "ms!" << endl;

	cout << result.uDefNumber << endl;

	for (int i = 0; i < result.uDefNumber; ++i)
	{
		cout << result.pDefInfoPtr[i].pDefTypePtr << endl;
		cout << result.pDefInfoPtr[i].uDefDiaMillimeter << endl;
		cout << result.pDefInfoPtr[i].uDefAreaMillimeter << endl; //coutֱ�����ָ�룬�������ָ��ָ�������,���ȱ���������
		cout << result.pDefInfoPtr[i].uContourPtNumber << endl;   //���ȱ�����������������أ�
		for (int j = 0; j < result.pDefInfoPtr[i].uContourPtNumber; ++j)
		{
			cout << result.pDefInfoPtr[i].pPtXPosPtr[j] << endl;  //C++�У���������������ָ�����ͣ�����ʹ��static_castת��һ�����õ����ַ
			cout << result.pDefInfoPtr[i].pPtYPosPtr[j] << endl;
		}
	}
	resetDefectInfo();
	system("pause");
	cv::waitKey();
	return 0;
}



// ��������:   ��uchar���͵�����ת��ΪMat����
// ��������ֵ�������ɹ����������ʧ��
// ��������ֵ��img  - ���뷨��Mat��ͼ��img_width-ͼ���ȣ�img_height-ͼ��߶�
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
		for (int i = 0; i<img_height; ++i) //����--�߶�
		{
			unsigned char* data = img.ptr<unsigned char>(i); //ָ���i�е����� 
			for (int j = 0; j<img_width; ++j) //���� -- ��� 
			{
				p[i*img_width + j] = data[j];
			}
		}
		return p;
	}
}