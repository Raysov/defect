#include "RxDefect.h"


RxDefResult*  pRxDefResult;
unsigned int*   trueXPosPtr;   // 缺陷像素X坐标
unsigned int*   trueYPosPtr;   // 缺陷像素Y坐标



// 函数功能:   将uchar类型的数据转换为Mat类型
// 函数返回值：零代表成功，非零代表失败
// 函数输入值：img  - 指向输入数据指针
//             img_width - 图像宽度
//             img_height- 图像高度
//             img_channel- 图像通道数
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
		for (int i = 0; i<img_height; i++) //行数--高度 
		{
			unsigned char* data = resultImg.ptr<unsigned char>(i); //指向第i行的数据
			for (int j = 0; j< img_width; j++) //列数 -- 宽度
			{
				data[j] = img[i*img_width + j];
			}
		}
		return resultImg;
	}
}



//lhImageCmp()和Reconstruct()函数实现形态学重构
//比较两个图像是否相同， 0 相同
int  lhImageCmp(const cv::Mat& img1, const cv::Mat& img2)
{
	assert(img1.cols == img2.cols && img1.rows == img2.rows && img1.size() == img2.size());
	return memcmp(img1.data, img2.data, img1.rows*img1.step);//第三个参数为图像的行数*每行的字节数

}


void Reconstruct(cv::Mat src, cv::Mat biaoji, cv::Mat& dst)
{
	/*功能：实现形态学重构
	src是原图像(包含所有缺陷，伪缺陷和真缺陷)，biaoji是标记图像(仅包含真缺陷)，dst是重构后的输出图*/
	cv::Mat se = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::Mat tmp1(src.size(), CV_8U), tmp2(src.size(), CV_8U);
	(cv::min)(src, biaoji, dst);
	//bitwise_and(src, biaoji, dst);
	do
	{
		dst.copyTo(tmp1);
		dilate(dst, biaoji, se);             //膨胀操作
		(cv::min)(src, biaoji, dst);
		//bitwise_and(src, biaoji, dst);
	} while (lhImageCmp(tmp1, dst) != 0);
}





// 函数功能:   检测图像缺陷
// 函数返回值：零代表成功，非零代表失败
// 函数输入值：pRxInfo  - 指向输入数据结构体指针
//	           pRxDefResult - 指向输出数据结构体指针
// 作者：JTY
// 版本：2.2
// 时间：2019.10.8
int findDefect(RxDetInfo* pRxInfo, RxDefResult* pRxDefResult)
{
	//参数定义
	bool bDefSigned = false;  //判断缺陷有无的标志位，标记缺陷标志位

	cv::Mat  ROIimg, ROIimgBackup, befDefectImg, DenoiseImage, blurImage;
	double tempV;             //亮缺陷处理时所用的临时变量
	double changkuanbi;       //所有缺陷的最小外接矩形的长宽比
	double contArea;          //缺陷轮廓面积
	double contMaxDiam;       //所有缺陷的最小外接矩形中宽和高中的较大值
	double contMinDiam;       //所有缺陷的最小外接矩形中宽和高中的较小值
	double contArea2;         //真缺陷的缺陷轮廓面积
	double changkuanbi2;      //真缺陷的最小外接矩形的长宽比
	double contMaxDiam2;      //真缺陷的最小外接矩形中宽和高中的较大值
	double contMinDiam2;      //真缺陷的最小外接矩形中宽和高中的较小值
	unsigned int QikongArea = 0;   //气孔总面积
	unsigned int SuosongArea = 0;  //缩松缩孔总面积
	unsigned int LiewenArea = 0;   //裂纹总面积
	unsigned int JiazhaArea = 0;   //夹渣总面积


	//将uchar*数据转化为Mat类,如果为单通道图像则转为3通道图像
	ROIimg = ucharToMat(pRxInfo->pImgPtr, pRxInfo->uImgWidth, pRxInfo->uImgHeight, pRxInfo->uImgChannel);
	if (ROIimg.channels() == 1)
	{
		cvtColor(ROIimg, ROIimg, CV_GRAY2BGR);
	}


	//轮毂图像缺陷检测
	//第一步：图像导入
	ROIimg.copyTo(ROIimgBackup);              //ROIimgBackup为备份，对图像进行处理，处理完后需拷贝回ROIimg图像
	ROIimgBackup.copyTo(befDefectImg);	      //开始检测前备份图ROIimgBackup至befDefectImg，用于防止检测叠加的现象，对befDefectImg进行检测
	if (ROIimgBackup.channels() != 1)         //ROIimgBackup转为单通道灰度图像
		cvtColor(ROIimgBackup, ROIimgBackup, CV_BGR2GRAY);


	//第二步：图像通道数转换及检测图备份
	int rows = befDefectImg.rows, cols = befDefectImg.cols;
	cv::Mat srcImgS8(rows, cols, CV_8UC1, cv::Scalar::all(0));                    //新建空白图像并初始化为0，8位单通道


	//首先得到需要检测的灰度图像，避免检测累加，每次用检测前备份图befDefectImg对检测图srcImgS8、显示图ROIimgBackup复位
	if (befDefectImg.channels() != 1)                                     //保证srcImgS8为灰度图，ROIimgBackup为彩色图
	{
		cvtColor(befDefectImg, srcImgS8, CV_BGR2GRAY);
		befDefectImg.copyTo(ROIimgBackup);
	}
	else{
		befDefectImg.copyTo(srcImgS8);//befDefectImg此时为感兴趣区域的灰度图
		cvtColor(befDefectImg, ROIimgBackup, CV_GRAY2BGR);
	}


	//第三步：去噪（中值滤波）
	medianBlur(srcImgS8, DenoiseImage, pRxInfo->uMedianMaskSize);
	//第四步：求取模板图像，滤波方式选择（均值，中值，高斯）
	blur(DenoiseImage, blurImage, cv::Size(pRxInfo->uMeanMaskSize, pRxInfo->uMeanMaskSize));




	//第五步：高低阈值重构得到亮暗缺陷
	//得到低阈值分割图像
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
	cv::Mat elementSegImg = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(-1, -1));   //自定义矩形结构元素,Point(-1, -1)表明锚点位于模板中心
	erode(SegYuzhiImage, SegYuzhiImage, elementSegImg, cv::Point(-1, -1), pRxInfo->uLowTErodeNum);   //腐蚀操作断开高阈值分割结果物体间的连接
	dilate(SegYuzhiImage, SegYuzhiImage, elementSegImg, cv::Point(-1, -1), pRxInfo->uLowTDilateNum); //海绵型缺陷进行膨胀



	//得到高阈值分割图像
	cv::Mat SeedImage = cv::Mat(DenoiseImage.size(), DenoiseImage.type());
	SeedImage = (pRxInfo->uDetLightDark)*(DenoiseImage - blurImage);
	for (int i = 0; i < SeedImage.rows; ++i)
	{
		uchar *tempP_seed = SeedImage.ptr<uchar>(i);
		for (int j = 0; j < SeedImage.cols; ++j)
		{
			tempV = tempP_seed[j];
			if (tempV>pRxInfo->uHighThreshold)              //工程下6.jpg分割阈值设为12
			{
				tempP_seed[j] = 255;
			}
			else
			{
				tempP_seed[j] = 0;
			}
		}
	}
	//对种子点图像进行腐蚀
	cv::Mat element00 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(-1, -1));   //自定义矩形结构元素,Point(-1, -1)表明锚点位于模板中心
	erode(SeedImage, SeedImage, element00, cv::Point(-1, -1), pRxInfo->uHighTErodeNum); //高阈值分割图像(种子点)腐蚀
	//重建结果
	Reconstruct(SegYuzhiImage, SeedImage, srcImgS8);//分割后的缺陷图像



	//第六步：膨胀产生外围的区域，使完全包括缺陷范围
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3), cv::Point(-1, -1));    //自定义椭圆形结构元素
	//腐蚀产生真正的缺陷
	erode(srcImgS8, srcImgS8, element, cv::Point(-1, -1), pRxInfo->uErodeNum);    //腐蚀使缺陷面积收缩
	//膨胀产生缺陷的外围区域
	dilate(srcImgS8, srcImgS8, element, cv::Point(-1, -1), pRxInfo->uDilateNum);  //Point为锚的位置，第五个参数为膨胀次数 



	//第七步：去除伪缺陷，保留真缺陷
	vector< vector<cv::Point> > contours;                                            //双层结构，轮廓包括轮廓内的点，所有轮廓包括多个轮廓
	//设置为此参数CV_RETR_CCOMP，保证DrawContours的max_level=0时不画出孔洞的边框。
	findContours(srcImgS8, contours, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);    //Opencv自带的寻找轮廓函数
	//第三个参数CV_RETR_CCOMP：为提取所有轮廓,并将他们组织为两层：顶层是各部分的外部边界，第二层是空洞的边界；                                                                                      
	//第四个参数为轮廓检索模式，压缩横竖对角方向的的元素，只保留该方向上的终点坐标
	vector< vector<cv::Point> > contours_true;
	for (decltype(contours.size()) i = 0; i < contours.size(); ++i)             //循环，遍历所有找到的轮廓
	{
		contArea = fabs(contourArea(contours[i]));                              //求取各个缺陷的面积，取浮点型绝对值
		cv::RotatedRect minRect = minAreaRect(contours[i]);                         //求取最小外接矩形	             
		contMaxDiam = minRect.size.width >= minRect.size.height ? minRect.size.width : minRect.size.height; //取宽和高中的较大值
		contMinDiam = minRect.size.width >= minRect.size.height ? minRect.size.height : minRect.size.width; //取宽和高中的较小值 
		changkuanbi = contMaxDiam / contMinDiam;                                //最小外接矩形的长宽比


		if ((contArea <= pRxInfo->uMinArea) || (contArea >= pRxInfo->uMaxArea) || (changkuanbi > pRxInfo->uLengthWidthRatio)) //找真缺陷，即去除面积小于100像素或大于8000或长宽比大于4的超级狭长的伪缺陷
		{
			swap(contours[i], contours[contours.size() - 1]);
			contours.pop_back();
			i = i - 1;
		}
	}
	contours_true = contours;
	pRxDefResult->uDefNumber = contours_true.size();
	/*去除伪缺陷以及轮廓的标注,得到真实缺陷的个数（轮廓个数）↑*/



	//第八步:如果没有缺陷，然后进行信息输出
	if (0 == contours_true.size())                         //若不存在被标记的缺陷，NULL
	{
		pRxDefResult->pDefInfoPtr = NULL;

	}


	else if (0 != contours_true.size())
	{
		//第八步：对真正的轮廓进行处理，输出真缺陷的信息
		pRxDefResult->pDefInfoPtr = new RxDefInfo[contours_true.size()];


		for (decltype(contours_true.size()) j = 0; j < contours_true.size(); ++j)
		{
			drawContours(ROIimgBackup, contours_true, j, cv::Scalar(0, 0, 255)/*, CV_FILLED*/);
			bDefSigned = true;

			decltype(contours_true[j].size()) pointNum = contours_true[j].size();
			pRxDefResult->pDefInfoPtr[j].uContourPtNumber = pointNum;

			unsigned int *trueXPosPtr = new unsigned int[pointNum];  //定义一个uint类型的指针变量，并为其分配内存空间，用来存储X坐标值
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
			/*得到每个轮廓的像素坐标↑*/



			/*识别出当前缺陷的类型↓*/
			contArea2 = fabs(cv::contourArea(contours_true[j]));                             //求取真缺陷的单个缺陷的面积，取浮点型绝对值
			cv::RotatedRect minRect2 = cv::minAreaRect(contours_true[j]);                    //求取真缺陷的最小外接矩形，在.h文件中声明会出错	             
			contMaxDiam2 = minRect2.size.width >= minRect2.size.height ? minRect2.size.width : minRect2.size.height;  //取宽和高中的较大值
			contMinDiam2 = minRect2.size.width >= minRect2.size.height ? minRect2.size.height : minRect2.size.width;  //取宽和高中的较小值
			changkuanbi2 = contMaxDiam2 / contMinDiam2;                                  //真缺陷的最小外接矩形的长宽比


			unsigned int  DefTypeArray[4] = { 1, 2, 3, 4 };

			if (pRxInfo->uDetLightDark == 1)
			{
				if (bDefSigned == true)                                   //若存在被标记的缺陷，则输出1,2.....
				{
					if (changkuanbi2 <= 3)                                //长宽比小于等于3的为气孔或者缩松缩孔
					{
						if (contArea2 >pRxInfo->uMinArea  && contArea2 < 400)     //面积大于200小于400像素的是缩松缩孔
						{
							pRxDefResult->pDefInfoPtr[j].uPtNumber = contArea2;        //输出缺陷面积（像素个数）
							pRxDefResult->pDefInfoPtr[j].uDefDia = contMaxDiam2;       //输出缺陷的直径（像素个数）
							pRxDefResult->pDefInfoPtr[j].uDefAreaMillimeter = contArea2*(pRxInfo->uMillimeterPixelRatio); //输出缺陷面积（毫米）
							pRxDefResult->pDefInfoPtr[j].uDefDiaMillimeter = contMaxDiam2*(pRxInfo->uMillimeterPixelRatio);//输出缺陷的直径（毫米）
							pRxDefResult->pDefInfoPtr[j].pDefTypePtr = DefTypeArray[1];//输出缺陷类型
						}
						if (contArea2 >= 400)                             //面积大于400像素的是气孔                  
						{
							pRxDefResult->pDefInfoPtr[j].uPtNumber = contArea2;
							pRxDefResult->pDefInfoPtr[j].uDefDia = contMaxDiam2;
							pRxDefResult->pDefInfoPtr[j].uDefAreaMillimeter = contArea2*(pRxInfo->uMillimeterPixelRatio); //输出缺陷面积（毫米）
							pRxDefResult->pDefInfoPtr[j].uDefDiaMillimeter = contMaxDiam2*(pRxInfo->uMillimeterPixelRatio);//输出缺陷的直径（毫米）
							pRxDefResult->pDefInfoPtr[j].pDefTypePtr = DefTypeArray[0];
						}

					}
					else if (changkuanbi2 > 3)                           //长宽比大于3的为裂纹
					{
						pRxDefResult->pDefInfoPtr[j].uPtNumber = contArea2;
						pRxDefResult->pDefInfoPtr[j].uDefDia = contMaxDiam2;
						pRxDefResult->pDefInfoPtr[j].uDefAreaMillimeter = contArea2*(pRxInfo->uMillimeterPixelRatio); //输出缺陷面积（毫米）
						pRxDefResult->pDefInfoPtr[j].uDefDiaMillimeter = contMaxDiam2*(pRxInfo->uMillimeterPixelRatio);//输出缺陷的直径（毫米）
						pRxDefResult->pDefInfoPtr[j].pDefTypePtr = DefTypeArray[2];
					}
				}//if (bDefSigned == true) 
			}//if (pRxInfo->uDetLightDark==1)
			else if (pRxInfo->uDetLightDark == -1)
			{
				pRxDefResult->pDefInfoPtr[j].uPtNumber = contArea2;
				pRxDefResult->pDefInfoPtr[j].uDefDia = contMaxDiam2;
				pRxDefResult->pDefInfoPtr[j].uDefAreaMillimeter = contArea2*(pRxInfo->uMillimeterPixelRatio); //输出缺陷面积（毫米）
				pRxDefResult->pDefInfoPtr[j].uDefDiaMillimeter = contMaxDiam2*(pRxInfo->uMillimeterPixelRatio);//输出缺陷的直径（毫米）
				pRxDefResult->pDefInfoPtr[j].pDefTypePtr = DefTypeArray[3]; //当检测暗缺陷的时候默认为夹渣
			}
		}//for (decltype(contours_true.size()) j = 0; j < contours_true.size(); ++j)
		/*识别出当前缺陷的类型↑*/
	}

	if (ROIimg.channels() == 1)                  //转为3通道图像，保证检测图为3通道图像
		cvtColor(ROIimg, ROIimg, CV_GRAY2BGR);

	ROIimgBackup.copyTo(ROIimg);                 //将ROIimgBackup的缺陷检测结果拷贝回ROIimg区域

	return 0;
}




// 函数功能: 当前检测结果复位，主要用于释放dll内部申请的内存以及缺陷统计信息清零
// 函数返回值：无
// 函数输入值：无 
// 作者：JTY
// 版本：2.2
// 时间：2019.10.8
void resetDefectInfo()
{
	delete[]trueXPosPtr;
	delete[]trueYPosPtr;
	delete[]pRxDefResult;
}