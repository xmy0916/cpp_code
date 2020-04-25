// check.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "pch.h"
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <Kinect.h>

using namespace std;
using namespace cv;

int main(void)
{
	// 1a.获取感应器
	IKinectSensor* pSensor = nullptr;
	GetDefaultKinectSensor(&pSensor);
	// 1b. 打开感应器
	pSensor->Open();
	/****************2.打开深度图像阅读器************************/
		// 取得深度数据
	IDepthFrameSource* pDepthSource = nullptr;
	pSensor->get_DepthFrameSource(&pDepthSource);
	// 取得深度数据的描述信息（宽、高）
	int        iDepthWidth = 0;
	int        iDepthHeight = 0;
	IFrameDescription* pDepthDescription = nullptr;
	pDepthSource->get_FrameDescription(&pDepthDescription);
	pDepthDescription->get_Width(&iDepthWidth);
	pDepthDescription->get_Height(&iDepthHeight);
	// 打开深度数据阅读器
	IDepthFrameReader* pDepthReader = nullptr;
	pDepthSource->OpenReader(&pDepthReader);
	pDepthDescription->Release();
	pDepthDescription = nullptr;
	// 释放变量pDepthSource
	pDepthSource->Release();
	pDepthSource = nullptr;

	/******************3.打开彩色图像阅读器**********************/
		// 取得彩色数据
	IColorFrameSource* pColorSource = nullptr;
	pSensor->get_ColorFrameSource(&pColorSource);
	// 取得彩色数据的描述信息（宽、高）
	int        iColorWidth = 0;
	int        iColorHeight = 0;
	IFrameDescription* pColorDescription = nullptr;
	pColorSource->get_FrameDescription(&pColorDescription);
	pColorDescription->get_Width(&iColorWidth);
	pColorDescription->get_Height(&iColorHeight);
	// 打开彩色数据阅读器
	IColorFrameReader* pColorReader = nullptr;
	pColorSource->OpenReader(&pColorReader);
	pColorDescription->Release();
	pColorDescription = nullptr;
	// 释放变量pColorSource
	pColorSource->Release();
	pColorSource = nullptr;

	/*******************4.打开人体索引的阅读器*******************/
		// 取得BodyIndex数据
	IBodyIndexFrameSource* pBodyIndexSource = nullptr;
	pSensor->get_BodyIndexFrameSource(&pBodyIndexSource);
	// 取得深度数据的描述信息（宽、高）
	int        iBodyIndexWidth = 0;
	int        iBodyIndexHeight = 0;
	IFrameDescription* pBodyIndexDescription = nullptr;
	pBodyIndexSource->get_FrameDescription(&pBodyIndexDescription);
	pBodyIndexDescription->get_Width(&iBodyIndexWidth);
	pBodyIndexDescription->get_Height(&iBodyIndexHeight);
	// 打开深度数据阅读器
	IBodyIndexFrameReader* pBodyIndexReader = nullptr;
	pBodyIndexSource->OpenReader(&pBodyIndexReader);
	pBodyIndexDescription->Release();
	pBodyIndexDescription = nullptr;
	// 释放变量pBodyIndexSource
	pBodyIndexSource->Release();
	pBodyIndexSource = nullptr;

	/*************5.为各种图像建立buffer，准备坐标转换*************/
	UINT    iColorDataSize = iColorHeight * iColorWidth;
	UINT    iDepthDataSize = iDepthHeight * iDepthWidth;
	UINT    iBodyIndexDataSize = iBodyIndexHeight * iBodyIndexWidth;
	//获取背景图并调整至彩色图像的大小
	Mat temp = imread("004.jpg"), background;
	resize(temp, background, Size(iColorWidth, iColorHeight));
	//开启mapper                                                          
	ICoordinateMapper   * myMaper = nullptr;
	pSensor->get_CoordinateMapper(&myMaper);
	//准备buffer
	Mat ColorData(iColorHeight, iColorWidth, CV_8UC4);
	UINT16  * DepthData = new UINT16[iDepthDataSize];
	BYTE    * BodyData = new BYTE[iBodyIndexDataSize];
	DepthSpacePoint * output = new DepthSpacePoint[iColorDataSize];

	/***********6.把各种图像读进buffer里，然后进行坐标转换以及替换**************/

	while (1)
	{
		//读取color图
		IColorFrame * pColorFrame = nullptr;
		while (pColorReader->AcquireLatestFrame(&pColorFrame) != S_OK);
		pColorFrame->CopyConvertedFrameDataToArray(iColorDataSize * 4, ColorData.data, ColorImageFormat_Bgra);
		pColorFrame->Release();

		//读取depth图
		IDepthFrame * pDepthframe = nullptr;
		while (pDepthReader->AcquireLatestFrame(&pDepthframe) != S_OK);
		pDepthframe->CopyFrameDataToArray(iDepthDataSize, DepthData);
		pDepthframe->Release();

		//读取BodyIndex图
		IBodyIndexFrame * pBodyIndexFrame = nullptr;
		while (pBodyIndexReader->AcquireLatestFrame(&pBodyIndexFrame) != S_OK);
		pBodyIndexFrame->CopyFrameDataToArray(iBodyIndexDataSize, BodyData);
		pBodyIndexFrame->Release();

		//复制一份背景图来做处理
		Mat copy = background.clone();
		if (myMaper->MapColorFrameToDepthSpace(iDepthDataSize, DepthData, iColorDataSize, output) == S_OK)
		{
			for (int i = 0; i < iColorHeight; ++i)
				for (int j = 0; j < iColorWidth; ++j)
				{
					//取得彩色图像上包含对应到深度图上的坐标的一点
					DepthSpacePoint tPoint = output[i * iColorWidth + j];
					//判断这个点是否合法
					if (tPoint.X >= 0 && tPoint.X < iDepthWidth && tPoint.Y >= 0 && tPoint.Y < iDepthHeight)
					{
						//取得彩色图上那点对应在BodyIndex里的值
						int index = (int)tPoint.Y * iDepthWidth + (int)tPoint.X;
						//判断出彩色图上某点是人体，就用替换背景图上对应点
						if (BodyData[index] <= 5)
						{
							Vec4b   color = ColorData.at<Vec4b>(i, j);
							copy.at<Vec3b>(i, j) = Vec3b(color[0], color[1], color[2]);
						}
					}
				}
			imshow("Background Remove", copy);
		}
		if (waitKey(30) == VK_ESCAPE) {
			break;
		}
	}
	delete[] DepthData;
	delete[] BodyData;
	delete[] output;

	// 1c.关闭感应器
	pSensor->Close();
	// 1d.释放感应器
	pSensor->Release();
	pSensor = nullptr;

	return 0;
}




//using   namespace   std;
//using   namespace   cv;
//
//void    draw(Mat & img, Joint & r_1, Joint & r_2, ICoordinateMapper * myMapper);
//int main(void)
//{
//	IKinectSensor   * mySensor = nullptr;
//	GetDefaultKinectSensor(&mySensor);
//	mySensor->Open();
//
//	IColorFrameSource   * myColorSource = nullptr;
//	mySensor->get_ColorFrameSource(&myColorSource);
//
//	IColorFrameReader   * myColorReader = nullptr;
//	myColorSource->OpenReader(&myColorReader);
//
//	int colorHeight = 0, colorWidth = 0;
//	IFrameDescription   * myDescription = nullptr;
//	myColorSource->get_FrameDescription(&myDescription);
//	myDescription->get_Height(&colorHeight);
//	myDescription->get_Width(&colorWidth);
//
//	IColorFrame * myColorFrame = nullptr;
//	Mat original(colorHeight, colorWidth, CV_8UC4);
//
//	//**********************以上为ColorFrame的读取前准备**************************
//
//	IBodyFrameSource    * myBodySource = nullptr;
//	mySensor->get_BodyFrameSource(&myBodySource);
//
//	IBodyFrameReader    * myBodyReader = nullptr;
//	myBodySource->OpenReader(&myBodyReader);
//
//	int myBodyCount = 0;
//	myBodySource->get_BodyCount(&myBodyCount);
//
//	IBodyFrame  * myBodyFrame = nullptr;
//
//	ICoordinateMapper   * myMapper = nullptr;
//	mySensor->get_CoordinateMapper(&myMapper);
//
//	//**********************以上为BodyFrame以及Mapper的准备***********************
//	while (1)
//	{
//
//		while (myColorReader->AcquireLatestFrame(&myColorFrame) != S_OK);
//		myColorFrame->CopyConvertedFrameDataToArray(colorHeight * colorWidth * 4, original.data, ColorImageFormat_Bgra);
//		Mat copy = original.clone();        //读取彩色图像并输出到矩阵
//
//		while (myBodyReader->AcquireLatestFrame(&myBodyFrame) != S_OK); //读取身体图像
//		IBody   **  myBodyArr = new IBody *[myBodyCount];       //为存身体数据的数组做准备
//		for (int i = 0; i < myBodyCount; i++)
//			myBodyArr[i] = nullptr;
//
//		if (myBodyFrame->GetAndRefreshBodyData(myBodyCount, myBodyArr) == S_OK)     //把身体数据输入数组
//			for (int i = 0; i < myBodyCount; i++)
//			{
//				BOOLEAN     result = false;
//				if (myBodyArr[i]->get_IsTracked(&result) == S_OK && result) //先判断是否侦测到
//				{
//					Joint   myJointArr[JointType_Count];
//					if (myBodyArr[i]->GetJoints(JointType_Count, myJointArr) == S_OK)   //如果侦测到就把关节数据输入到数组并画图
//					{
//						draw(copy, myJointArr[JointType_Head], myJointArr[JointType_Neck], myMapper);
//						draw(copy, myJointArr[JointType_Neck], myJointArr[JointType_SpineShoulder], myMapper);
//
//						draw(copy, myJointArr[JointType_SpineShoulder], myJointArr[JointType_ShoulderLeft], myMapper);
//						draw(copy, myJointArr[JointType_SpineShoulder], myJointArr[JointType_SpineMid], myMapper);
//						draw(copy, myJointArr[JointType_SpineShoulder], myJointArr[JointType_ShoulderRight], myMapper);
//
//						draw(copy, myJointArr[JointType_ShoulderLeft], myJointArr[JointType_ElbowLeft], myMapper);
//						draw(copy, myJointArr[JointType_SpineMid], myJointArr[JointType_SpineBase], myMapper);
//						draw(copy, myJointArr[JointType_ShoulderRight], myJointArr[JointType_ElbowRight], myMapper);
//
//						draw(copy, myJointArr[JointType_ElbowLeft], myJointArr[JointType_WristLeft], myMapper);
//						draw(copy, myJointArr[JointType_SpineBase], myJointArr[JointType_HipLeft], myMapper);
//						draw(copy, myJointArr[JointType_SpineBase], myJointArr[JointType_HipRight], myMapper);
//						draw(copy, myJointArr[JointType_ElbowRight], myJointArr[JointType_WristRight], myMapper);
//
//						draw(copy, myJointArr[JointType_WristLeft], myJointArr[JointType_ThumbLeft], myMapper);
//						draw(copy, myJointArr[JointType_WristLeft], myJointArr[JointType_HandLeft], myMapper);
//						draw(copy, myJointArr[JointType_HipLeft], myJointArr[JointType_KneeLeft], myMapper);
//						draw(copy, myJointArr[JointType_HipRight], myJointArr[JointType_KneeRight], myMapper);
//						draw(copy, myJointArr[JointType_WristRight], myJointArr[JointType_ThumbRight], myMapper);
//						draw(copy, myJointArr[JointType_WristRight], myJointArr[JointType_HandRight], myMapper);
//
//						draw(copy, myJointArr[JointType_HandLeft], myJointArr[JointType_HandTipLeft], myMapper);
//						draw(copy, myJointArr[JointType_KneeLeft], myJointArr[JointType_FootLeft], myMapper);
//						draw(copy, myJointArr[JointType_KneeRight], myJointArr[JointType_FootRight], myMapper);
//						draw(copy, myJointArr[JointType_HandRight], myJointArr[JointType_HandTipRight], myMapper);
//					}
//				}
//			}
//		delete[]myBodyArr;
//		myBodyFrame->Release();
//		myColorFrame->Release();
//
//		imshow("TEST", copy);
//		if (waitKey(30) == VK_ESCAPE)
//			break;
//	}
//	myMapper->Release();
//
//	myDescription->Release();
//	myColorReader->Release();
//	myColorSource->Release();
//
//	myBodyReader->Release();
//	myBodySource->Release();
//	mySensor->Close();
//	mySensor->Release();
//
//	return  0;
//}
//
//void    draw(Mat & img, Joint & r_1, Joint & r_2, ICoordinateMapper * myMapper)
//{
//	//用两个关节点来做线段的两端，并且进行状态过滤
//	if (r_1.TrackingState == TrackingState_Tracked && r_2.TrackingState == TrackingState_Tracked)
//	{
//		ColorSpacePoint t_point;    //要把关节点用的摄像机坐标下的点转换成彩色空间的点
//		Point   p_1, p_2;
//		myMapper->MapCameraPointToColorSpace(r_1.Position, &t_point);
//		p_1.x = t_point.X;
//		p_1.y = t_point.Y;
//		myMapper->MapCameraPointToColorSpace(r_2.Position, &t_point);
//		p_2.x = t_point.X;
//		p_2.y = t_point.Y;
//
//		line(img, p_1, p_2, Vec3b(0, 255, 0), 5);
//		circle(img, p_1, 10, Vec3b(255, 0, 0), -1);
//		circle(img, p_2, 10, Vec3b(255, 0, 0), -1);
//	}
//}