//--------------------------------------������˵����-------------------------------------------
//		����˵������ͼ�Ͷ���ʶ����ɵ��Զ����ճ���
//		��������������Kinect��������պͿ�ͼ���ռ���
//		������������IDE�汾��Visual Studio 2013
//		������������OpenCV�汾��	3.0 beta
//		����������ʹ�õ�����ʶ��⣺Microsoft Speech Platform - Runtime (Version 11)
//		����������ʹ��Ӳ����	KinectV2 Xbox
//		����ϵͳ��Windows 10
//		Kinect SDK�汾��KinectSDK-v2.0-PublicPreview1409-Setup 
//		2016��3�� Created by @������
//------------------------------------------------------------------------------------------------


#include "myKinect.h"
#include <iostream>
#include<Windows.h>
#include<time.h>	//ʱ��ͷ�ļ�
#include<opencv2/opencv.hpp>
#include <sstream>		//����ת�ַ���
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
	//�����ж�ÿ�ζ�ȡ�����ĳɹ����
	HRESULT hr;

	//����kinect
	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr)){
		return hr;
	}

	//�ҵ�kinect�豸
	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;//��ȡ�Ǽ�
		IDepthFrameSource* pDepthFrameSource = NULL;//��ȡ�����Ϣ
		IColorFrameSource* pColorFrameSource = NULL;//��ȡ��ɫ��Ϣ
		IBodyIndexFrameSource* pBodyIndexFrameSource = NULL;//��ȡ������ֵͼ
		IInfraredFrameSource* pInfraredFrameSource = NULL;

		//��kinect
		hr = m_pKinectSensor->Open();

		//coordinatemapper
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		//bodyframe    ÿ��ͼ����ͨ��source��reader��frame������
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

	//skeletonImg,���ڻ��Ǽܡ�������ֵͼ��MAT
	skeletonImg.create(cDepthHeight, cDepthWidth, CV_8UC3);
	skeletonImg.setTo(0);

	//depthImg,���ڻ������Ϣ��MAT
	depthImg.create(cDepthHeight, cDepthWidth, CV_8UC1);
	depthImg.setTo(0);

	//IrImg���ڻ�������Ϣ��MAT
	IrImg.create(cDepthHeight, cDepthWidth, CV_8UC1);
	IrImg.setTo(0);

	//colorImg,���ڻ���ɫ��Ϣ��MAT
	colorImg.create(cColorHeight, cColorWidth, CV_8UC3);
	colorImg.setTo(0);

	//��ʼ������ͼƬ
	//backjpg=imread("image/001.jpg");
	 /* IMAGE_LIST_FILE ���ͼ�������*/
	 fp = fopen(IMAGE_LIST_FILE, "r");
	 if (fscanf(fp, "%s ", imagepath) > 0)
		 backjpg = imread(imagepath);
	 //fscanf(fp, "%s ", imagepath);
	 return hr;
}


/// Main processing function
void CBodyBasics::Update()
{
	//ÿ�������skeletonImg
	//skeletonImg.setTo(0);

	UINT    colorDataSize = cColorHeight * cColorWidth;
	UINT    depthDataSize = cDepthHeight * cDepthWidth;
	UINT    bodyDataSize = cDepthHeight * cDepthWidth;
	Mat background;               //��ȡ����ͼ

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
	resize(backjpg, background, Size(cColorWidth, cColorHeight));   //��������ɫͼ��Ĵ�С
	//�����ʧ��kinect���򲻼�������
	if (!m_pBodyFrameReader)
	{
		return;
	}

	IBodyFrame* pBodyFrame = NULL;//�Ǽ���Ϣ
	IDepthFrame* pDepthFrame = NULL;//�����Ϣ
	IColorFrame* pColorFrame = NULL;//��ɫ��Ϣ
	IBodyIndexFrame* pBodyIndexFrame = NULL;//������ֵͼ
	IInfraredFrame* pInfraredFrame = NULL;

	//��¼ÿ�β����ĳɹ����
	HRESULT hr = S_OK;

	//---------------------------------------��ȡ������ֵͼ����ʾ---------------------------------
	if (SUCCEEDED(hr)){
		hr = m_pBodyIndexFrameReader->AcquireLatestFrame(&pBodyIndexFrame);//��ñ�����ֵͼ��Ϣ
	}
	BYTE *bodyIndexArray = new BYTE[cDepthHeight * cDepthWidth];//������ֵͼ��8Ϊuchar�������Ǻ�ɫ��û���ǰ�ɫ
	if (SUCCEEDED(hr)){
		
		pBodyIndexFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, bodyIndexArray);

		//�ѱ�����ֵͼ����MAT��
	//uchar* skeletonData = (uchar*)skeletonImg.data;
	//	for (int j = 0; j < cDepthHeight * cDepthWidth; ++j){
	//		*skeletonData = bodyIndexArray[j]; ++skeletonData;
	//		*skeletonData = bodyIndexArray[j]; ++skeletonData;
	//		*skeletonData = bodyIndexArray[j]; ++skeletonData;
	//	}
	}
	SafeRelease(pBodyIndexFrame);//����Ҫ�ͷţ�����֮���޷�����µ�frame����

	//-----------------------��ȡ������ݲ���ʾ--------------------------
	if (SUCCEEDED(hr)){
		hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);//����������
	}
	UINT16 *depthArray = new UINT16[cDepthHeight * cDepthWidth];//���������16λunsigned int
	if (SUCCEEDED(hr)){
		
		pDepthFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, depthArray);

		//��������ݻ���MAT��
		//uchar* depthData = (uchar*)depthImg.data;
		//for (int j = 0; j < cDepthHeight * cDepthWidth; ++j){
		//	*depthData = depthArray[j];
		//	++depthData;
		//}
		//delete[] depthArray;
	}
	depthnumber++;
	SafeRelease(pDepthFrame);//����Ҫ�ͷţ�����֮���޷�����µ�frame����
	//namedWindow("depthImg", 0);
	//resizeWindow("depthImg", 640, 480);
	//imshow("depthImg", depthImg);

	//cv::waitKey(5);

	//-----------------------��ȡ�������ݲ���ʾ--------------------------
	//if (SUCCEEDED(hr)){
	//	HRESULT hr = m_pInfraredFrameReader->AcquireLatestFrame(&pInfraredFrame);
	//}
	//����һ
	//if (SUCCEEDED(hr)){
	//	UINT16 *IrArray = new UINT16[cDepthHeight * cDepthWidth];//����������16λunsigned int
	//	pInfraredFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, IrArray);

	//	//��ir���ݻ���MAT��
	//	uchar* irData = (uchar*)IrImg.data;
	//	for (int j = 0; j < cDepthHeight * cDepthWidth; ++j){
	//		*irData = IrArray[j];
	//		++irData;
	//	}
	//	delete[] IrArray;
	//}
	////depthnumber++;
	//SafeRelease(pInfraredFrame);//����Ҫ�ͷţ�����֮���޷�����µ�frame����
	//namedWindow("IrImg", 0);
	//resizeWindow("IrImg", 640, 480);
	//imshow("IrImg", IrImg);

	//cv::waitKey(5);

	//-----------------------��ȡ��ɫ���ݲ���ʾ--------------------------

	//UINT nBufferSize_coloar = 0; 
	DepthSpacePoint * output = new DepthSpacePoint[colorDataSize];
	//RGBQUAD *pBuffer_color = NULL;
	Mat colorData(cColorHeight, cColorWidth, CV_8UC4);        //׼��buffer
	//RGBQUAD* m_pColorRGBX = new RGBQUAD[cColorWidth * cColorHeight];
	if (SUCCEEDED(hr)){
		hr = m_pColorFrameReader->AcquireLatestFrame(&pColorFrame);//��ò�ɫ����
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
	//		if (imageFormat == ColorImageFormat_Bgra)//����������format����֪�����庬�壬���һ��Ԥ�ȷ����ڴ棬һ����Ҫ�Լ����ռ��  
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
	SafeRelease(pColorFrame);//����Ҫ�ͷţ�����֮���޷�����µ�frame����
	//namedWindow("colorImg", 0);
	//resizeWindow("colorImg",1024,720);
	//imshow("colorImg", colorImg);

	//cv::waitKey(5);
	 copy = background.clone();                  //����һ�ݱ���ͼ��������
	if (m_pCoordinateMapper->MapColorFrameToDepthSpace(depthDataSize, depthArray, colorDataSize, output) == S_OK)
	{
		for (int i = 0; i < cColorHeight; ++i)
		for (int j = 0; j < cColorWidth; ++j)
		{
			DepthSpacePoint tPoint = output[i * cColorWidth + j];    //ȡ�ò�ɫͼ���ϵ�һ�㣬�˵����������Ӧ�����ͼ�ϵ�����
			if (tPoint.X >= 0 && tPoint.X < cDepthWidth && tPoint.Y >= 0 && tPoint.Y < cDepthHeight)  //�ж��Ƿ�Ϸ�
			{
				int index = (int)tPoint.Y * cDepthWidth + (int)tPoint.X; //ȡ�ò�ɫͼ���ǵ��Ӧ��BodyIndex���ֵ(ע��Ҫǿת)
				if (bodyIndexArray[index] <= 5)           //bodyData[index]��ʾ�õ���˵ı�ţ���Χ��0-5�����������Χ��ʾ�õ㲻��������Ǳ�����
					//����жϳ���ɫͼ��ĳ�������壬���������滻����ͼ�϶�Ӧ�ĵ�
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
	//���ຯ����������ǰ��0.4m���ɴ�������
	TakePhoto();
	//SafeRelease(m_pCoordinateMapper);
	//-----------------------------��ȡ�Ǽܲ���ʾ----------------------------
	if (SUCCEEDED(hr)){
		hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);//��ȡ�Ǽ���Ϣ
	}
	if (SUCCEEDED(hr))
	{
		IBody* ppBodies[BODY_COUNT] = { 0 };//ÿһ��IBody����׷��һ���ˣ��ܹ�����׷��������

		if (SUCCEEDED(hr))
		{
			//��kinect׷�ٵ����˵���Ϣ���ֱ�浽ÿһ��IBody��
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			//��ÿһ��IBody�������ҵ����ĹǼ���Ϣ�����һ�����
			ProcessBody(BODY_COUNT, ppBodies);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);//�ͷ�����
		}
	}
	SafeRelease(pBodyFrame);//����Ҫ�ͷţ�����֮���޷�����µ�frame����

}

/// Handle new body data
void CBodyBasics::ProcessBody(int nBodyCount, IBody** ppBodies)
{
	//��¼��������Ƿ�ɹ�
	HRESULT hr;

	//����ÿһ��IBody
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody)//��û�и���������pBody�������bTracked��ʲô����
		{
			BOOLEAN bTracked = false;
			hr = pBody->get_IsTracked(&bTracked);

			if (SUCCEEDED(hr) && bTracked)
			{
				//Joint joints[JointType_Count];//�洢�ؽڵ���
				HandState leftHandState = HandState_Unknown;//����״̬
				HandState rightHandState = HandState_Unknown;//����״̬

				//��ȡ������״̬
				pBody->get_HandLeftState(&leftHandState);
				pBody->get_HandRightState(&rightHandState);

				//�洢�������ϵ�еĹؽڵ�λ��
				DepthSpacePoint *depthSpacePosition = new DepthSpacePoint[_countof(joints)];

				//��ùؽڵ���
				hr = pBody->GetJoints(_countof(joints), joints);
				if (SUCCEEDED(hr))
				{
					for (int j = 0; j < _countof(joints); ++j)
					{
						//���ؽڵ���������������ϵ��-1~1��ת���������ϵ��424*512��
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


//���ֵ�״̬
void CBodyBasics::DrawHandState(const DepthSpacePoint depthSpacePosition, HandState handState)
{
	//����ͬ�����Ʒ��䲻ͬ��ɫ
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
	default://���û��ȷ�������ƣ��Ͳ�Ҫ��
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
		//�ǳ�ȷ���ĹǼܣ��ð�ɫֱ��
		line(skeletonImg, p1, p2, cvScalar(255, 255, 255), 8);
	}
	else
	{
		//��ȷ���ĹǼܣ��ú�ɫֱ��
		line(skeletonImg, p1, p2, cvScalar(0, 0, 255), 8);
	}
}

//�������ͼ��
void CBodyBasics::SaveDepthImg()
{
	//string str = (num2str)depthnumber;
	stringstream stream;
	string str;
	stream << depthnumber;        //��long����������
	stream >> str;   //ת��Ϊ string
	imwrite(str + "depthnumber.bmp", depthImg);
	cout << str + "depthnumber.bmp" << endl;

}
//����
void CBodyBasics::TakePhoto()
{	
	//��������һЩ�����㣬�����ʾ
	Joint righthand = joints[JointType_HandRight];
	Joint lefthand = joints[JointType_HandLeft];
	Joint spinemid = joints[JointType_SpineMid];
	Joint head = joints[JointType_Head];
	stringstream stream;
	string str;
	if (spinemid.Position.Z < 0.5)		//�ж�����������Kinect �ľ��룬С��0.5��ֱ�ӷ��أ���ʹ�������Ѿ���׼ȷ�ˣ����������
		return;
	//�ж�ԭ�����ֵ�����������������Z���ϵľ�����ڸ�����ֵ��Z_THRESHOUD��������û�����գ�
	//����һֱ�������գ�Ҳ�������õȴ�ʱ�䣬��������ʵ������
	if (spinemid.Position.Z - righthand.Position.Z >= Z_THRESHOUD&&bTakePhoto)
	{
		bTakePhoto = false;
		photocount++;
		stream << photocount;        //��long����������
		stream >> str;   //ת��Ϊ string
		string filepath = "D:/pic/";		//���浽ָ���ļ�������
		imwrite(filepath+str + ".jpg", copy);
		cout << "�ɹ��յ�" << photocount << "����" << endl;
	}
	if (spinemid.Position.Z - righthand.Position.Z < Z_THRESHOUD)		//û�м�⵽ָ�����������ʾû��������
	{
		bTakePhoto = TRUE;
		return;

	}
		
}
//�л�����
bool CBodyBasics::ChangeBackground()
{
	//��������һЩ�����㣬�����ʾ
	Joint righthand = joints[JointType_HandRight];
	Joint head = joints[JointType_Head];
	Joint spinebase = joints[JointType_SpineBase];
	if (spinebase.Position.Z<0.5)		//�ж�����������Kinect �ľ��룬С��0.5��ֱ�ӷ��أ���ʹ�������Ѿ���׼ȷ�ˣ����������
		return false;
	//�ж�ԭ�����ֵ�����������������X���ϵľ�����ڸ�����ֵ��X_THRESHOUD��������û���л�����ʱ��
	//����һֱ�����л���Ҳ������һ�ַ�ʽ������һ���ȴ�ʱ�䣬��������ʵ�ֿ����л����ű�����
	if (righthand.Position.X - head.Position.X >= X_THRESHOUD&&bChange)
	{
		bChange = FALSE;
		if (fscanf(fp, "%s ", imagepath) > 0)		//��ȡ����ͼƬ�ı���·��
			backjpg = imread(imagepath);			//��ȡ����ͼƬ
		else
		{
			rewind(fp);								//�ļ�ָ�븴λ��������ָ���ʼλ��
			fscanf(fp, "%s ", imagepath);			//��ȡ����ͼƬ�ı���·��
			backjpg = imread(imagepath);			//��ȡ����ͼƬ
		}
		return true;
	}
	if (righthand.Position.X - head.Position.X < X_THRESHOUD)  //û�м�⵽ָ�����������û�����л�������
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



