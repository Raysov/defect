#include "RxDefect.h"


RxDefResult*  pRxDefResult;
unsigned int*   trueXPosPtr;   // ȱ������X����
unsigned int*   trueYPosPtr;   // ȱ������Y����



// ��������:   ��uchar���͵�����ת��ΪMat����
// ��������ֵ�������ɹ����������ʧ��
// ��������ֵ��img  - ָ����������ָ��
//             img_width - ͼ����
//             img_height- ͼ��߶�
//             img_channel- ͼ��ͨ����
cv::Mat ucharToMat(uchar *img, unsigned int img_width, unsigned int img_height, unsigned int img_channel)
{
	if (img_channel == 3)
	{
		cv::Mat resultImg(cv::Size(img_width, img_height), CV_8UC3);
		for (int i = 0; i < img_width * img_height * 3; i++)
		{
			resultImg.at<cv::Vec3b>(i / (img_width * 3), (i % (img_width * 3)) / 3)[i % 3] = img[i];
		}
		return resultImg;
	}
	else if (img_channel == 1)
	{
		cv::Mat resultImg(cv::Size(img_width, img_height), CV_8UC1);
		for (int i = 0; i<img_height; i++) //����--�߶� 
		{
			unsigned char* data = resultImg.ptr<unsigned char>(i); //ָ���i�е�����
			for (int j = 0; j< img_width; j++) //���� -- ���
			{
				data[j] = img[i*img_width + j];
			}
		}
		return resultImg;
	}
}



//lhImageCmp()��Reconstruct()����ʵ����̬ѧ�ع�
//�Ƚ�����ͼ���Ƿ���ͬ�� 0 ��ͬ
int  lhImageCmp(const cv::Mat& img1, const cv::Mat& img2)
{
	assert(img1.cols == img2.cols && img1.rows == img2.rows && img1.size() == img2.size());
	return memcmp(img1.data, img2.data, img1.rows*img1.step);//����������Ϊͼ�������*ÿ�е��ֽ���

}


void Reconstruct(cv::Mat src, cv::Mat biaoji, cv::Mat& dst)
{
	/*���ܣ�ʵ����̬ѧ�ع�
	src��ԭͼ��(��������ȱ�ݣ�αȱ�ݺ���ȱ��)��biaoji�Ǳ��ͼ��(��������ȱ��)��dst���ع�������ͼ*/
	cv::Mat se = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::Mat tmp1(src.size(), CV_8U), tmp2(src.size(), CV_8U);
	(cv::min)(src, biaoji, dst);
	//bitwise_and(src, biaoji, dst);
	do
	{
		dst.copyTo(tmp1);
		dilate(dst, biaoji, se);             //���Ͳ���
		(cv::min)(src, biaoji, dst);
		//bitwise_and(src, biaoji, dst);
	} while (lhImageCmp(tmp1, dst) != 0);
}





// ��������:   ���ͼ��ȱ��
// ��������ֵ�������ɹ����������ʧ��
// ��������ֵ��pRxInfo  - ָ���������ݽṹ��ָ��
//	           pRxDefResult - ָ��������ݽṹ��ָ��
// ���ߣ�JTY
// �汾��2.2
// ʱ�䣺2019.10.8
int findDefect(RxDetInfo* pRxInfo, RxDefResult* pRxDefResult)
{
	//��������
	bool bDefSigned = false;  //�ж�ȱ�����޵ı�־λ�����ȱ�ݱ�־λ

	cv::Mat  ROIimg, ROIimgBackup, befDefectImg, DenoiseImage, blurImage;
	double tempV;             //��ȱ�ݴ���ʱ���õ���ʱ����
	double changkuanbi;       //����ȱ�ݵ���С��Ӿ��εĳ����
	double contArea;          //ȱ���������
	double contMaxDiam;       //����ȱ�ݵ���С��Ӿ����п�͸��еĽϴ�ֵ
	double contMinDiam;       //����ȱ�ݵ���С��Ӿ����п�͸��еĽ�Сֵ
	double contArea2;         //��ȱ�ݵ�ȱ���������
	double changkuanbi2;      //��ȱ�ݵ���С��Ӿ��εĳ����
	double contMaxDiam2;      //��ȱ�ݵ���С��Ӿ����п�͸��еĽϴ�ֵ
	double contMinDiam2;      //��ȱ�ݵ���С��Ӿ����п�͸��еĽ�Сֵ
	unsigned int QikongArea = 0;   //���������
	unsigned int SuosongArea = 0;  //�������������
	unsigned int LiewenArea = 0;   //���������
	unsigned int JiazhaArea = 0;   //���������


	//��uchar*����ת��ΪMat��,���Ϊ��ͨ��ͼ����תΪ3ͨ��ͼ��
	ROIimg = ucharToMat(pRxInfo->pImgPtr, pRxInfo->uImgWidth, pRxInfo->uImgHeight, pRxInfo->uImgChannel);
	if (ROIimg.channels() == 1)
	{
		cvtColor(ROIimg, ROIimg, CV_GRAY2BGR);
	}


	//���ͼ��ȱ�ݼ��
	//��һ����ͼ����
	ROIimg.copyTo(ROIimgBackup);              //ROIimgBackupΪ���ݣ���ͼ����д�����������追����ROIimgͼ��
	ROIimgBackup.copyTo(befDefectImg);	      //��ʼ���ǰ����ͼROIimgBackup��befDefectImg�����ڷ�ֹ�����ӵ����󣬶�befDefectImg���м��
	if (ROIimgBackup.channels() != 1)         //ROIimgBackupתΪ��ͨ���Ҷ�ͼ��
		cvtColor(ROIimgBackup, ROIimgBackup, CV_BGR2GRAY);


	//�ڶ�����ͼ��ͨ����ת�������ͼ����
	int rows = befDefectImg.rows, cols = befDefectImg.cols;
	cv::Mat srcImgS8(rows, cols, CV_8UC1, cv::Scalar::all(0));                    //�½��հ�ͼ�񲢳�ʼ��Ϊ0��8λ��ͨ��


	//���ȵõ���Ҫ���ĻҶ�ͼ�񣬱������ۼӣ�ÿ���ü��ǰ����ͼbefDefectImg�Լ��ͼsrcImgS8����ʾͼROIimgBackup��λ
	if (befDefectImg.channels() != 1)                                     //��֤srcImgS8Ϊ�Ҷ�ͼ��ROIimgBackupΪ��ɫͼ
	{
		cvtColor(befDefectImg, srcImgS8, CV_BGR2GRAY);
		befDefectImg.copyTo(ROIimgBackup);
	}
	else{
		befDefectImg.copyTo(srcImgS8);//befDefectImg��ʱΪ����Ȥ����ĻҶ�ͼ
		cvtColor(befDefectImg, ROIimgBackup, CV_GRAY2BGR);
	}


	//��������ȥ�루��ֵ�˲���
	medianBlur(srcImgS8, DenoiseImage, pRxInfo->uMedianMaskSize);
	//���Ĳ�����ȡģ��ͼ���˲���ʽѡ�񣨾�ֵ����ֵ����˹��
	blur(DenoiseImage, blurImage, cv::Size(pRxInfo->uMeanMaskSize, pRxInfo->uMeanMaskSize));




	//���岽���ߵ���ֵ�ع��õ�����ȱ��
	//�õ�����ֵ�ָ�ͼ��
	cv::Mat SegYuzhiImage = cv::Mat(DenoiseImage.size(), DenoiseImage.type());
	SegYuzhiImage = (pRxInfo->uDetLightDark)*(DenoiseImage - blurImage);
	for (int i = 0; i < SegYuzhiImage.rows; ++i)
	{
		uchar *tempP_pre = SegYuzhiImage.ptr<uchar>(i);
		for (int j = 0; j < SegYuzhiImage.cols; ++j)
		{
			tempV = tempP_pre[j];
			if (tempV>pRxInfo->uLowThreshold)
			{
				tempP_pre[j] = 255;
			}
			else
			{
				tempP_pre[j] = 0;
			}
		}
	}
	cv::Mat elementSegImg = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(-1, -1));   //�Զ�����νṹԪ��,Point(-1, -1)����ê��λ��ģ������
	erode(SegYuzhiImage, SegYuzhiImage, elementSegImg, cv::Point(-1, -1), pRxInfo->uLowTErodeNum);   //��ʴ�����Ͽ�����ֵ�ָ�������������
	dilate(SegYuzhiImage, SegYuzhiImage, elementSegImg, cv::Point(-1, -1), pRxInfo->uLowTDilateNum); //������ȱ�ݽ�������



	//�õ�����ֵ�ָ�ͼ��
	cv::Mat SeedImage = cv::Mat(DenoiseImage.size(), DenoiseImage.type());
	SeedImage = (pRxInfo->uDetLightDark)*(DenoiseImage - blurImage);
	for (int i = 0; i < SeedImage.rows; ++i)
	{
		uchar *tempP_seed = SeedImage.ptr<uchar>(i);
		for (int j = 0; j < SeedImage.cols; ++j)
		{
			tempV = tempP_seed[j];
			if (tempV>pRxInfo->uHighThreshold)              //������6.jpg�ָ���ֵ��Ϊ12
			{
				tempP_seed[j] = 255;
			}
			else
			{
				tempP_seed[j] = 0;
			}
		}
	}
	//�����ӵ�ͼ����и�ʴ
	cv::Mat element00 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(-1, -1));   //�Զ�����νṹԪ��,Point(-1, -1)����ê��λ��ģ������
	erode(SeedImage, SeedImage, element00, cv::Point(-1, -1), pRxInfo->uHighTErodeNum); //����ֵ�ָ�ͼ��(���ӵ�)��ʴ
	//�ؽ����
	Reconstruct(SegYuzhiImage, SeedImage, srcImgS8);//�ָ���ȱ��ͼ��



	//�����������Ͳ�����Χ������ʹ��ȫ����ȱ�ݷ�Χ
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3), cv::Point(-1, -1));    //�Զ�����Բ�νṹԪ��
	//��ʴ����������ȱ��
	erode(srcImgS8, srcImgS8, element, cv::Point(-1, -1), pRxInfo->uErodeNum);    //��ʴʹȱ���������
	//���Ͳ���ȱ�ݵ���Χ����
	dilate(srcImgS8, srcImgS8, element, cv::Point(-1, -1), pRxInfo->uDilateNum);  //PointΪê��λ�ã����������Ϊ���ʹ��� 



	//���߲���ȥ��αȱ�ݣ�������ȱ��
	vector< vector<cv::Point> > contours;                                            //˫��ṹ���������������ڵĵ㣬�������������������
	//����Ϊ�˲���CV_RETR_CCOMP����֤DrawContours��max_level=0ʱ�������׶��ı߿�
	findContours(srcImgS8, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);    //Opencv�Դ���Ѱ����������
	//����������CV_RETR_CCOMP��Ϊ��ȡ��������,����������֯Ϊ���㣺�����Ǹ����ֵ��ⲿ�߽磬�ڶ����ǿն��ı߽磻                                                                                      
	//���ĸ�����Ϊ��������ģʽ��ѹ�������ԽǷ���ĵ�Ԫ�أ�ֻ�����÷����ϵ��յ�����
	vector< vector<cv::Point> > contours_true;
	for (decltype(contours.size()) i = 0; i < contours.size(); ++i)             //ѭ�������������ҵ�������
	{
		contArea = fabs(contourArea(contours[i]));                              //��ȡ����ȱ�ݵ������ȡ�����;���ֵ
		cv::RotatedRect minRect = minAreaRect(contours[i]);                         //��ȡ��С��Ӿ���	             
		contMaxDiam = minRect.size.width >= minRect.size.height ? minRect.size.width : minRect.size.height; //ȡ��͸��еĽϴ�ֵ
		contMinDiam = minRect.size.width >= minRect.size.height ? minRect.size.height : minRect.size.width; //ȡ��͸��еĽ�Сֵ 
		changkuanbi = contMaxDiam / contMinDiam;                                //��С��Ӿ��εĳ����


		if ((contArea <= pRxInfo->uMinArea) || (contArea >= pRxInfo->uMaxArea) || (changkuanbi > pRxInfo->uLengthWidthRatio)) //����ȱ�ݣ���ȥ�����С��100���ػ����8000�򳤿�ȴ���4�ĳ���������αȱ��
		{
			swap(contours[i], contours[contours.size() - 1]);
			contours.pop_back();
			i = i - 1;
		}
	}
	contours_true = contours;
	pRxDefResult->uDefNumber = contours_true.size();
	/*ȥ��αȱ���Լ������ı�ע,�õ���ʵȱ�ݵĸ�����������������*/



	//�ڰ˲�:���û��ȱ�ݣ�Ȼ�������Ϣ���
	if (0 == contours_true.size())                         //�������ڱ���ǵ�ȱ�ݣ�NULL
	{
		pRxDefResult->pDefInfoPtr = NULL;

	}


	else if (0 != contours_true.size())
	{
		//�ڰ˲������������������д��������ȱ�ݵ���Ϣ
		pRxDefResult->pDefInfoPtr = new RxDefInfo[contours_true.size()];


		for (decltype(contours_true.size()) j = 0; j < contours_true.size(); ++j)
		{
			drawContours(ROIimgBackup, contours_true, j, cv::Scalar(0, 0, 255)/*, CV_FILLED*/);
			bDefSigned = true;

			decltype(contours_true[j].size()) pointNum = contours_true[j].size();
			pRxDefResult->pDefInfoPtr[j].uContourPtNumber = pointNum;

			unsigned int *trueXPosPtr = new unsigned int[pointNum];  //����һ��uint���͵�ָ���������Ϊ������ڴ�ռ䣬�����洢X����ֵ
			unsigned int *trueYPosPtr = new unsigned int[pointNum];


			for (decltype(pointNum) a = 0; a < pointNum; ++a)
			{
				unsigned int tempX = contours_true[j][a].x;
				unsigned int tempY = contours_true[j][a].y;
				trueXPosPtr[a] = tempX;
				trueYPosPtr[a] = tempY;
			}
			pRxDefResult->pDefInfoPtr[j].pPtXPosPtr = trueXPosPtr;
			pRxDefResult->pDefInfoPtr[j].pPtYPosPtr = trueYPosPtr;
			/*�õ�ÿ�����������������*/



			/*ʶ�����ǰȱ�ݵ����͡�*/
			contArea2 = fabs(cv::contourArea(contours_true[j]));                             //��ȡ��ȱ�ݵĵ���ȱ�ݵ������ȡ�����;���ֵ
			cv::RotatedRect minRect2 = cv::minAreaRect(contours_true[j]);                    //��ȡ��ȱ�ݵ���С��Ӿ��Σ���.h�ļ������������	             
			contMaxDiam2 = minRect2.size.width >= minRect2.size.height ? minRect2.size.width : minRect2.size.height;  //ȡ��͸��еĽϴ�ֵ
			contMinDiam2 = minRect2.size.width >= minRect2.size.height ? minRect2.size.height : minRect2.size.width;  //ȡ��͸��еĽ�Сֵ
			changkuanbi2 = contMaxDiam2 / contMinDiam2;                                  //��ȱ�ݵ���С��Ӿ��εĳ����


			unsigned int  DefTypeArray[4] = { 1, 2, 3, 4 };

			if (pRxInfo->uDetLightDark == 1)
			{
				if (bDefSigned == true)                                   //�����ڱ���ǵ�ȱ�ݣ������1,2.....
				{
					if (changkuanbi2 <= 3)                                //�����С�ڵ���3��Ϊ���׻�����������
					{
						if (contArea2 >pRxInfo->uMinArea  && contArea2 < 400)     //�������200С��400���ص�����������
						{
							pRxDefResult->pDefInfoPtr[j].uPtNumber = contArea2;        //���ȱ����������ظ�����
							pRxDefResult->pDefInfoPtr[j].uDefDia = contMaxDiam2;       //���ȱ�ݵ�ֱ�������ظ�����
							pRxDefResult->pDefInfoPtr[j].uDefAreaMillimeter = contArea2*(pRxInfo->uMillimeterPixelRatio); //���ȱ����������ף�
							pRxDefResult->pDefInfoPtr[j].uDefDiaMillimeter = contMaxDiam2*(pRxInfo->uMillimeterPixelRatio);//���ȱ�ݵ�ֱ�������ף�
							pRxDefResult->pDefInfoPtr[j].pDefTypePtr = DefTypeArray[1];//���ȱ������
						}
						if (contArea2 >= 400)                             //�������400���ص�������                  
						{
							pRxDefResult->pDefInfoPtr[j].uPtNumber = contArea2;
							pRxDefResult->pDefInfoPtr[j].uDefDia = contMaxDiam2;
							pRxDefResult->pDefInfoPtr[j].uDefAreaMillimeter = contArea2*(pRxInfo->uMillimeterPixelRatio); //���ȱ����������ף�
							pRxDefResult->pDefInfoPtr[j].uDefDiaMillimeter = contMaxDiam2*(pRxInfo->uMillimeterPixelRatio);//���ȱ�ݵ�ֱ�������ף�
							pRxDefResult->pDefInfoPtr[j].pDefTypePtr = DefTypeArray[0];
						}

					}
					else if (changkuanbi2 > 3)                           //����ȴ���3��Ϊ����
					{
						pRxDefResult->pDefInfoPtr[j].uPtNumber = contArea2;
						pRxDefResult->pDefInfoPtr[j].uDefDia = contMaxDiam2;
						pRxDefResult->pDefInfoPtr[j].uDefAreaMillimeter = contArea2*(pRxInfo->uMillimeterPixelRatio); //���ȱ����������ף�
						pRxDefResult->pDefInfoPtr[j].uDefDiaMillimeter = contMaxDiam2*(pRxInfo->uMillimeterPixelRatio);//���ȱ�ݵ�ֱ�������ף�
						pRxDefResult->pDefInfoPtr[j].pDefTypePtr = DefTypeArray[2];
					}
				}//if (bDefSigned == true) 
			}//if (pRxInfo->uDetLightDark==1)
			else if (pRxInfo->uDetLightDark == -1)
			{
				pRxDefResult->pDefInfoPtr[j].uPtNumber = contArea2;
				pRxDefResult->pDefInfoPtr[j].uDefDia = contMaxDiam2;
				pRxDefResult->pDefInfoPtr[j].uDefAreaMillimeter = contArea2*(pRxInfo->uMillimeterPixelRatio); //���ȱ����������ף�
				pRxDefResult->pDefInfoPtr[j].uDefDiaMillimeter = contMaxDiam2*(pRxInfo->uMillimeterPixelRatio);//���ȱ�ݵ�ֱ�������ף�
				pRxDefResult->pDefInfoPtr[j].pDefTypePtr = DefTypeArray[3]; //����ⰵȱ�ݵ�ʱ��Ĭ��Ϊ����
			}
		}//for (decltype(contours_true.size()) j = 0; j < contours_true.size(); ++j)
		/*ʶ�����ǰȱ�ݵ����͡�*/
	}

	if (ROIimg.channels() == 1)                  //תΪ3ͨ��ͼ�񣬱�֤���ͼΪ3ͨ��ͼ��
		cvtColor(ROIimg, ROIimg, CV_GRAY2BGR);

	ROIimgBackup.copyTo(ROIimg);                 //��ROIimgBackup��ȱ�ݼ����������ROIimg����

	return 0;
}




// ��������: ��ǰ�������λ����Ҫ�����ͷ�dll�ڲ�������ڴ��Լ�ȱ��ͳ����Ϣ����
// ��������ֵ����
// ��������ֵ���� 
// ���ߣ�JTY
// �汾��2.2
// ʱ�䣺2019.10.8
void resetDefectInfo()
{
	delete[]trueXPosPtr;
	delete[]trueYPosPtr;
	delete[]pRxDefResult;
}