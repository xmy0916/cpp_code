//--------------------------------------【程序说明】-------------------------------------------
//		程序说明：抠图和动作识别组成的自动拍照程序
//		程序描述：基于Kinect的体感拍照和抠图拍照技术
//		开发测试所用IDE版本：Visual Studio 2013
//		开发测试所用OpenCV版本：	3.0 beta
//		开发测试所使用的语音识别库：Microsoft Speech Platform - Runtime (Version 11)
//		开发测试所使用硬件：	KinectV2 Xbox
//		操作系统：Windows 10
//		Kinect SDK版本：KinectSDK-v2.0-PublicPreview1409-Setup 
//		2016年3月 Created by @胡保林
//------------------------------------------------------------------------------------------------


#include "myKinect.h"
#include <iostream>
#include<Windows.h>
#include<time.h>	//时间头文件
#include<opencv2/opencv.hpp>
#include <sstream>		//数字转字符串
using namespace std;
using namespace cv;
static long depthnumber;
static int photocount=0;
bool bTakePhoto = true;
bool bChange = true;
char imagepath[100];
FILE *fp;
/// Initializes the default Kinect sensor
HRESULT CBodyBasics::InitializeDefaultSensor()
{
	//用于判断每次读取操作的成功与否
	HRESULT hr;

	//搜索kinect
	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr)){
		return hr;
	}

	//找到kinect设备
	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;//读取骨架
		IDepthFrameSource* pDepthFrameSource = NULL;//读取深度信息
		IColorFrameSource* pColorFrameSource = NULL;//读取彩色信息
		IBodyIndexFrameSource* pBodyIndexFrameSource = NULL;//读取背景二值图
		IInfraredFrameSource* pInfraredFrameSource = NULL;

		//打开kinect
		hr = m_pKinectSensor->Open();

		//coordinatemapper
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		//bodyframe    每种图都是通过source，reader，frame三个类
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		//depth frame
		if (SUCCEEDED(hr)){
			hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
		}

		if (SUCCEEDED(hr)){
			hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
		}

		//color frame
		if (SUCCEEDED(hr)){
			hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
		}

		if (SUCCEEDED(hr)){
			hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
		}

		//body index frame
		if (SUCCEEDED(hr)){
			hr = m_pKinectSensor->get_BodyIndexFrameSource(&pBodyIndexFrameSource);
		}

		if (SUCCEEDED(hr)){
			hr = pBodyIndexFrameSource->OpenReader(&m_pBodyIndexFrameReader);
		}

		//IR	frame
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_InfraredFrameSource(&pInfraredFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pInfraredFrameSource->OpenReader(&m_pInfraredFrameReader);
		}

		

		SafeRelease(pBodyFrameSource);
		SafeRelease(pDepthFrameSource);
		SafeRelease(pColorFrameSource);
		SafeRelease(pBodyIndexFrameSource);
		SafeRelease(pInfraredFrameSource);
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		std::cout << "Kinect initialization failed!" << std::endl;
		return E_FAIL;
	}

	//skeletonImg,用于画骨架、背景二值图的MAT
	skeletonImg.create(cDepthHeight, cDepthWidth, CV_8UC3);
	skeletonImg.setTo(0);

	//depthImg,用于画深度信息的MAT
	depthImg.create(cDepthHeight, cDepthWidth, CV_8UC1);
	depthImg.setTo(0);

	//IrImg用于画红外信息的MAT
	IrImg.create(cDepthHeight, cDepthWidth, CV_8UC1);
	IrImg.setTo(0);

	//colorImg,用于画彩色信息的MAT
	colorImg.create(cColorHeight, cColorWidth, CV_8UC3);
	colorImg.setTo(0);

	//初始化背景图片
	//backjpg=imread("image/001.jpg");
	 /* IMAGE_LIST_FILE 存放图像的名字*/
	 fp = fopen(IMAGE_LIST_FILE, "r");
	 if (fscanf(fp, "%s ", imagepath) > 0)
		 backjpg = imread(imagepath);
	 //fscanf(fp, "%s ", imagepath);
	 return hr;
}


/// Main processing function
void CBodyBasics::Update()
{
	//每次先清空skeletonImg
	//skeletonImg.setTo(0);

	UINT    colorDataSize = cColorHeight * cColorWidth;
	UINT    depthDataSize = cDepthHeight * cDepthWidth;
	UINT    bodyDataSize = cDepthHeight * cDepthWidth;
	Mat background;               //获取背景图

	//if (ChangeBackground())
	//{
	//	if (fscanf(fp, "%s ", imagepath) > 0)
	//		backjpg = imread(imagepath);
	//	else
	//	{
	//		rewind(fp);
	//		fscanf(fp, "%s ", imagepath);
	//		backjpg = imread(imagepath);
	//	}
	//}
	bool bchange=ChangeBackground();
	resize(backjpg, background, Size(cColorWidth, cColorHeight));   //调整至彩色图像的大小
	//如果丢失了kinect，则不继续操作
	if (!m_pBodyFrameReader)
	{
		return;
	}

	IBodyFrame* pBodyFrame = NULL;//骨架信息
	IDepthFrame* pDepthFrame = NULL;//深度信息
	IColorFrame* pColorFrame = NULL;//彩色信息
	IBodyIndexFrame* pBodyIndexFrame = NULL;//背景二值图
	IInfraredFrame* pInfraredFrame = NULL;

	//记录每次操作的成功与否
	HRESULT hr = S_OK;

	//---------------------------------------获取背景二值图并显示---------------------------------
	if (SUCCEEDED(hr)){
		hr = m_pBodyIndexFrameReader->AcquireLatestFrame(&pBodyIndexFrame);//获得背景二值图信息
	}
	BYTE *bodyIndexArray = new BYTE[cDepthHeight * cDepthWidth];//背景二值图是8为uchar，有人是黑色，没人是白色
	if (SUCCEEDED(hr)){
		
		pBodyIndexFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, bodyIndexArray);

		//把背景二值图画到MAT里
	//uchar* skeletonData = (uchar*)skeletonImg.data;
	//	for (int j = 0; j < cDepthHeight * cDepthWidth; ++j){
	//		*skeletonData = bodyIndexArray[j]; ++skeletonData;
	//		*skeletonData = bodyIndexArray[j]; ++skeletonData;
	//		*skeletonData = bodyIndexArray[j]; ++skeletonData;
	//	}
	}
	SafeRelease(pBodyIndexFrame);//必须要释放，否则之后无法获得新的frame数据

	//-----------------------获取深度数据并显示--------------------------
	if (SUCCEEDED(hr)){
		hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);//获得深度数据
	}
	UINT16 *depthArray = new UINT16[cDepthHeight * cDepthWidth];//深度数据是16位unsigned int
	if (SUCCEEDED(hr)){
		
		pDepthFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, depthArray);

		//把深度数据画到MAT中
		//uchar* depthData = (uchar*)depthImg.data;
		//for (int j = 0; j < cDepthHeight * cDepthWidth; ++j){
		//	*depthData = depthArray[j];
		//	++depthData;
		//}
		//delete[] depthArray;
	}
	depthnumber++;
	SafeRelease(pDepthFrame);//必须要释放，否则之后无法获得新的frame数据
	//namedWindow("depthImg", 0);
	//resizeWindow("depthImg", 640, 480);
	//imshow("depthImg", depthImg);

	//cv::waitKey(5);

	//-----------------------获取红外数据并显示--------------------------
	//if (SUCCEEDED(hr)){
	//	HRESULT hr = m_pInfraredFrameReader->AcquireLatestFrame(&pInfraredFrame);
	//}
	//方案一
	//if (SUCCEEDED(hr)){
	//	UINT16 *IrArray = new UINT16[cDepthHeight * cDepthWidth];//红外数据是16位unsigned int
	//	pInfraredFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, IrArray);

	//	//把ir数据画到MAT中
	//	uchar* irData = (uchar*)IrImg.data;
	//	for (int j = 0; j < cDepthHeight * cDepthWidth; ++j){
	//		*irData = IrArray[j];
	//		++irData;
	//	}
	//	delete[] IrArray;
	//}
	////depthnumber++;
	//SafeRelease(pInfraredFrame);//必须要释放，否则之后无法获得新的frame数据
	//namedWindow("IrImg", 0);
	//resizeWindow("IrImg", 640, 480);
	//imshow("IrImg", IrImg);

	//cv::waitKey(5);

	//-----------------------获取彩色数据并显示--------------------------

	//UINT nBufferSize_coloar = 0; 
	DepthSpacePoint * output = new DepthSpacePoint[colorDataSize];
	//RGBQUAD *pBuffer_color = NULL;
	Mat colorData(cColorHeight, cColorWidth, CV_8UC4);        //准备buffer
	//RGBQUAD* m_pColorRGBX = new RGBQUAD[cColorWidth * cColorHeight];
	if (SUCCEEDED(hr)){
		hr = m_pColorFrameReader->AcquireLatestFrame(&pColorFrame);//获得彩色数据
	}
	if (SUCCEEDED(hr))
	{
		pColorFrame->CopyConvertedFrameDataToArray(colorDataSize * 4, colorData.data, ColorImageFormat_Bgra);
	}
	//if (SUCCEEDED(hr)){
	//	ColorImageFormat imageFormat = ColorImageFormat_None;
	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
	//	}
	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
	//	}
	//	if (SUCCEEDED(hr))
	//	{
	//		if (imageFormat == ColorImageFormat_Bgra)//这里有两个format，不知道具体含义，大概一个预先分配内存，一个需要自己开空间吧  
	//		{
	//			hr = pColorFrame->AccessRawUnderlyingBuffer(&nBufferSize_coloar, reinterpret_cast<BYTE**>(&pBuffer_color));
	//		}
	//		else if (m_pColorRGBX)
	//		{
	//			pBuffer_color = m_pColorRGBX;
	//			nBufferSize_coloar = cColorWidth * cColorHeight * sizeof(RGBQUAD);
	//			hr = pColorFrame->CopyConvertedFrameDataToArray(nBufferSize_coloar, reinterpret_cast<BYTE*>(pBuffer_color), ColorImageFormat_Bgra);
	//		}
	//		else
	//		{
	//			hr = E_FAIL;
	//		}
	//		uchar* p_mat = colorImg.data;
	//		const RGBQUAD* pBufferEnd = pBuffer_color + (cColorWidth * cColorHeight);
	//		while (pBuffer_color < pBufferEnd)
	//		{
	//			*p_mat = pBuffer_color->rgbBlue;
	//			p_mat++;
	//			*p_mat = pBuffer_color->rgbGreen;
	//			p_mat++;
	//			*p_mat = pBuffer_color->rgbRed;
	//			p_mat++;
	//			++pBuffer_color;
	//		}
	//	}
	//}
	//delete[] m_pColorRGBX;
	SafeRelease(pColorFrame);//必须要释放，否则之后无法获得新的frame数据
	//namedWindow("colorImg", 0);
	//resizeWindow("colorImg",1024,720);
	//imshow("colorImg", colorImg);

	//cv::waitKey(5);
	 copy = background.clone();                  //复制一份背景图来做处理
	if (m_pCoordinateMapper->MapColorFrameToDepthSpace(depthDataSize, depthArray, colorDataSize, output) == S_OK)
	{
		for (int i = 0; i < cColorHeight; ++i)
		for (int j = 0; j < cColorWidth; ++j)
		{
			DepthSpacePoint tPoint = output[i * cColorWidth + j];    //取得彩色图像上的一点，此点包含了它对应到深度图上的坐标
			if (tPoint.X >= 0 && tPoint.X < cDepthWidth && tPoint.Y >= 0 && tPoint.Y < cDepthHeight)  //判断是否合法
			{
				int index = (int)tPoint.Y * cDepthWidth + (int)tPoint.X; //取得彩色图上那点对应在BodyIndex里的值(注意要强转)
				if (bodyIndexArray[index] <= 5)           //bodyData[index]表示该点的人的编号，范围是0-5，不在这个范围表示该点不是人体而是背景。
					//如果判断出彩色图上某点是人体，就用它来替换背景图上对应的点
				{
					Vec4b   color = colorData.at<Vec4b>(i, j);
					copy.at<Vec3b>(i, j) = Vec3b(color[0], color[1], color[2]);
				}
			}
		}
		imshow("colorImg", copy);
	}
	delete[] bodyIndexArray;
	delete[] depthArray;
	delete[] output;
	//照相函数，右手向前伸0.4m即可触发照相
	TakePhoto();
	//SafeRelease(m_pCoordinateMapper);
	//-----------------------------获取骨架并显示----------------------------
	if (SUCCEEDED(hr)){
		hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);//获取骨架信息
	}
	if (SUCCEEDED(hr))
	{
		IBody* ppBodies[BODY_COUNT] = { 0 };//每一个IBody可以追踪一个人，总共可以追踪六个人

		if (SUCCEEDED(hr))
		{
			//把kinect追踪到的人的信息，分别存到每一个IBody中
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			//对每一个IBody，我们找到他的骨架信息，并且画出来
			ProcessBody(BODY_COUNT, ppBodies);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);//释放所有
		}
	}
	SafeRelease(pBodyFrame);//必须要释放，否则之后无法获得新的frame数据

}

/// Handle new body data
void CBodyBasics::ProcessBody(int nBodyCount, IBody** ppBodies)
{
	//记录操作结果是否成功
	HRESULT hr;

	//对于每一个IBody
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody)//还没有搞明白这里pBody和下面的bTracked有什么区别
		{
			BOOLEAN bTracked = false;
			hr = pBody->get_IsTracked(&bTracked);

			if (SUCCEEDED(hr) && bTracked)
			{
				//Joint joints[JointType_Count];//存储关节点类
				HandState leftHandState = HandState_Unknown;//左手状态
				HandState rightHandState = HandState_Unknown;//右手状态

				//获取左右手状态
				pBody->get_HandLeftState(&leftHandState);
				pBody->get_HandRightState(&rightHandState);

				//存储深度坐标系中的关节点位置
				DepthSpacePoint *depthSpacePosition = new DepthSpacePoint[_countof(joints)];

				//获得关节点类
				hr = pBody->GetJoints(_countof(joints), joints);
				if (SUCCEEDED(hr))
				{
					for (int j = 0; j < _countof(joints); ++j)
					{
						//将关节点坐标从摄像机坐标系（-1~1）转到深度坐标系（424*512）
						m_pCoordinateMapper->MapCameraPointToDepthSpace(joints[j].Position, &depthSpacePosition[j]);
					}


					////------------------------hand state left  and  right-------------------------------
					//DrawHandState(depthSpacePosition[JointType_HandLeft], leftHandState);
					//DrawHandState(depthSpacePosition[JointType_HandRight], rightHandState);

					////---------------------------body-------------------------------
					//DrawBone(joints, depthSpacePosition, JointType_Head, JointType_Neck);
					//DrawBone(joints, depthSpacePosition, JointType_Neck, JointType_SpineShoulder);
					//DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_SpineMid);
					//DrawBone(joints, depthSpacePosition, JointType_SpineMid, JointType_SpineBase);
					//DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_ShoulderRight);
					//DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_ShoulderLeft);
					//DrawBone(joints, depthSpacePosition, JointType_SpineBase, JointType_HipRight);
					//DrawBone(joints, depthSpacePosition, JointType_SpineBase, JointType_HipLeft);

					//// -----------------------Right Arm ------------------------------------ 
					//DrawBone(joints, depthSpacePosition, JointType_ShoulderRight, JointType_ElbowRight);
					//DrawBone(joints, depthSpacePosition, JointType_ElbowRight, JointType_WristRight);
					//DrawBone(joints, depthSpacePosition, JointType_WristRight, JointType_HandRight);
					//DrawBone(joints, depthSpacePosition, JointType_HandRight, JointType_HandTipRight);
					//DrawBone(joints, depthSpacePosition, JointType_WristRight, JointType_ThumbRight);

					////----------------------------------- Left Arm--------------------------
					//DrawBone(joints, depthSpacePosition, JointType_ShoulderLeft, JointType_ElbowLeft);
					//DrawBone(joints, depthSpacePosition, JointType_ElbowLeft, JointType_WristLeft);
					//DrawBone(joints, depthSpacePosition, JointType_WristLeft, JointType_HandLeft);
					//DrawBone(joints, depthSpacePosition, JointType_HandLeft, JointType_HandTipLeft);
					//DrawBone(joints, depthSpacePosition, JointType_WristLeft, JointType_ThumbLeft);

					//// ----------------------------------Right Leg--------------------------------
					//DrawBone(joints, depthSpacePosition, JointType_HipRight, JointType_KneeRight);
					//DrawBone(joints, depthSpacePosition, JointType_KneeRight, JointType_AnkleRight);
					//DrawBone(joints, depthSpacePosition, JointType_AnkleRight, JointType_FootRight);

					//// -----------------------------------Left Leg---------------------------------
					//DrawBone(joints, depthSpacePosition, JointType_HipLeft, JointType_KneeLeft);
					//DrawBone(joints, depthSpacePosition, JointType_KneeLeft, JointType_AnkleLeft);
					//DrawBone(joints, depthSpacePosition, JointType_AnkleLeft, JointType_FootLeft);
				}
				delete[] depthSpacePosition;
			}
		}
	}
	//namedWindow("skeletonImg", 0);
	//resizeWindow("skeletonImg", 640, 480);
	//cv::imshow("skeletonImg", skeletonImg);
	//cv::waitKey(5);
}


//画手的状态
void CBodyBasics::DrawHandState(const DepthSpacePoint depthSpacePosition, HandState handState)
{
	//给不同的手势分配不同颜色
	CvScalar color;
	switch (handState){
	case HandState_Open:
		color = cvScalar(255, 0, 0);
		break;
	case HandState_Closed:
		color = cvScalar(0, 255, 0);
		break;
	case HandState_Lasso:
		color = cvScalar(0, 0, 255);
		break;
	default://如果没有确定的手势，就不要画
		return;
	}

	circle(skeletonImg,
		cvPoint(depthSpacePosition.X, depthSpacePosition.Y),
		20, color, -1);
}


/// Draws one bone of a body (joint to joint)
void CBodyBasics::DrawBone(const Joint* pJoints, const DepthSpacePoint* depthSpacePosition, JointType joint0, JointType joint1)
{
	TrackingState joint0State = pJoints[joint0].TrackingState;
	TrackingState joint1State = pJoints[joint1].TrackingState;

	// If we can't find either of these joints, exit
	if ((joint0State == TrackingState_NotTracked) || (joint1State == TrackingState_NotTracked))
	{
		return;
	}

	// Don't draw if both points are inferred
	if ((joint0State == TrackingState_Inferred) && (joint1State == TrackingState_Inferred))
	{
		return;
	}

	CvPoint p1 = cvPoint(depthSpacePosition[joint0].X, depthSpacePosition[joint0].Y),
		p2 = cvPoint(depthSpacePosition[joint1].X, depthSpacePosition[joint1].Y);

	circle(skeletonImg,p1,3,(255,255,0),8,-1);
	circle(skeletonImg, p2, 3, (255, 255, 0), 8, -1);
	// We assume all drawn bones are inferred unless BOTH joints are tracked
	if ((joint0State == TrackingState_Tracked) && (joint1State == TrackingState_Tracked))
	{
		//非常确定的骨架，用白色直线
		line(skeletonImg, p1, p2, cvScalar(255, 255, 255), 8);
	}
	else
	{
		//不确定的骨架，用红色直线
		line(skeletonImg, p1, p2, cvScalar(0, 0, 255), 8);
	}
}

//保存深度图像
void CBodyBasics::SaveDepthImg()
{
	//string str = (num2str)depthnumber;
	stringstream stream;
	string str;
	stream << depthnumber;        //从long型数据输入
	stream >> str;   //转换为 string
	imwrite(str + "depthnumber.bmp", depthImg);
	cout << str + "depthnumber.bmp" << endl;

}
//照相
void CBodyBasics::TakePhoto()
{	
	//定义人体一些骨骼点，方面表示
	Joint righthand = joints[JointType_HandRight];
	Joint lefthand = joints[JointType_HandLeft];
	Joint spinemid = joints[JointType_SpineMid];
	Joint head = joints[JointType_Head];
	stringstream stream;
	string str;
	if (spinemid.Position.Z < 0.5)		//判断人体重心离Kinect 的距离，小于0.5则直接返回，这使得数据已经不准确了，避免误操作
		return;
	//判断原则：右手的中心离身体重心在Z轴上的距离大于给定阈值（Z_THRESHOUD）且现在没在拍照，
	//避免一直触发拍照，也可以设置等待时间，这样可以实现连拍
	if (spinemid.Position.Z - righthand.Position.Z >= Z_THRESHOUD&&bTakePhoto)
	{
		bTakePhoto = false;
		photocount++;
		stream << photocount;        //从long型数据输入
		stream >> str;   //转换为 string
		string filepath = "D:/pic/";		//保存到指定文件夹里面
		imwrite(filepath+str + ".jpg", copy);
		cout << "成功照第" << photocount << "张相" << endl;
	}
	if (spinemid.Position.Z - righthand.Position.Z < Z_THRESHOUD)		//没有检测到指定动作，则表示没有在拍照
	{
		bTakePhoto = TRUE;
		return;

	}
		
}
//切换背景
bool CBodyBasics::ChangeBackground()
{
	//定义人体一些骨骼点，方面表示
	Joint righthand = joints[JointType_HandRight];
	Joint head = joints[JointType_Head];
	Joint spinebase = joints[JointType_SpineBase];
	if (spinebase.Position.Z<0.5)		//判断人体重心离Kinect 的距离，小于0.5则直接返回，这使得数据已经不准确了，避免误操作
		return false;
	//判断原则：右手的中心离身体重心在X轴上的距离大于给定阈值（X_THRESHOUD）且现在没在切换背景时，
	//避免一直触发切换，也可以另一种方式，设置一个等待时间，这样可以实现快速切换多张背景。
	if (righthand.Position.X - head.Position.X >= X_THRESHOUD&&bChange)
	{
		bChange = FALSE;
		if (fscanf(fp, "%s ", imagepath) > 0)		//读取背景图片的本地路径
			backjpg = imread(imagepath);			//读取背景图片
		else
		{
			rewind(fp);								//文件指针复位，即重新指向最开始位置
			fscanf(fp, "%s ", imagepath);			//读取背景图片的本地路径
			backjpg = imread(imagepath);			//读取背景图片
		}
		return true;
	}
	if (righthand.Position.X - head.Position.X < X_THRESHOUD)  //没有检测到指定作，则表明没有在切换背景，
	{
		bChange = TRUE;
		return false;
	}
		
	return false;
	
}
/**************************************************************************************************/
/// Constructor
CBodyBasics::CBodyBasics() :
m_pKinectSensor(NULL),
m_pCoordinateMapper(NULL),
m_pBodyFrameReader(NULL),
m_pInfraredFrameReader(NULL),
m_pColorFrameReader(NULL){}

/// Destructor
CBodyBasics::~CBodyBasics()
{
	SafeRelease(m_pBodyFrameReader);
	SafeRelease(m_pCoordinateMapper); 
	SafeRelease(m_pInfraredFrameReader);
	SafeRelease(m_pColorFrameReader);
	fclose(fp);
	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}
	SafeRelease(m_pKinectSensor);
}



