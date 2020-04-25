//kinect.cpp
#include "stdafx.h"
#include "afxdialogex.h"
#include "mfctest.h"
#include "mfctestDlg.h"
#include <Windows.h>

#include"kinect.h"
#include"stdio.h"
#include "Resource.h"
using namespace cv;

/* ━━━━━━神兽出没━━━━━━
* 　　　┏┓　　　┏┓
* 　　┏┛┻━━━┛┻┓
* 　　┃　　　　　　　┃
* 　　┃　　　━　　　┃
* 　　┃　┳┛　┗┳　┃
* 　　┃　　　　　　　┃
* 　　┃　　　┻　　　┃
* 　　┃　　　　　　　┃
* 　　┗━┓　　　┏━┛Code is far away from bug with the animal protecting
* 　　　　┃　　　┃    神兽保佑, 代码无bug
* 　　　　┃　　　┃
* 　　　　┃　　　┗━━━┓
* 　　　　┃　　　　　　　┣┓
* 　　　　┃　　　　　　　┏┛
* 　　　　┗┓┓┏━┳┓┏┛
* 　　　　　┃┫┫　┃┫┫
* 　　　　　┗┻┛　┗┻┛
* ━━━━━━━━━━━━━━━━*/

///构造函数
CBodyBasics::CBodyBasics()
{
	mySensor = nullptr;
	myColorSource = nullptr;
	pColorReader = nullptr;
	myDescription = nullptr;

	pBuffer = NULL;
	pColorFrame = NULL;
	m_pColorRGBX = new RGBQUAD[cColorWidth*cColorHeight];
	//-----------------初始化骨骼------//
	myBodyCount = 0;
	myBodySource = nullptr;
	myBodyReader = nullptr;
	myBodyFrame = nullptr;
	myMapper = nullptr;
}


/// 析构函数
CBodyBasics::~CBodyBasics()
{
	if (m_pColorRGBX)
	{
		delete[] m_pColorRGBX;
		m_pColorRGBX = NULL;
	}
	SafeRelease(pColorReader);

	if (mySensor)
	{
		mySensor->Close();
	}
	SafeRelease(mySensor);


	myDescription->Release();
	//myColorReader->Release();
	myColorSource->Release();

	myBodyReader->Release();
	myBodySource->Release();


}


HRESULT CBodyBasics::InitializeDefaultSensor()
{
	HRESULT hr;

	hr = GetDefaultKinectSensor(&mySensor);
	mySensor->Open();//打开kinect
	mySensor->get_ColorFrameSource(&myColorSource);//打开彩色帧的源
	myColorSource->OpenReader(&pColorReader);//打开彩色帧读取器
	SafeRelease(myColorSource);
	//**********************以上为ColorFrame的读取前准备**************************


	mySensor->get_BodyFrameSource(&myBodySource);

	myBodySource->OpenReader(&myBodyReader);

	mySensor->get_CoordinateMapper(&myMapper);


	return hr;
}

void CBodyBasics::Update()
{

	if (!pColorReader)
	{
		return;
	}
	cout << "1" << endl;


	HRESULT hr = pColorReader->AcquireLatestFrame(&pColorFrame);
	//更新彩色帧
	
	while (FAILED(hr))
	{
		SafeRelease(pColorFrame);
		SafeRelease(pColorReader);

		hr = GetDefaultKinectSensor(&mySensor);
		mySensor->get_ColorFrameSource(&myColorSource);//打开彩色帧的源
		myColorSource->OpenReader(&pColorReader);//打开彩色帧读取器
		SafeRelease(myColorSource);

		Sleep(60);
		if (SUCCEEDED(hr))
		{
			hr = pColorReader->AcquireLatestFrame(&pColorFrame);
		}

		cout << "MultiSourceFrame" << rand() % 100 << endl;
	}


	if (SUCCEEDED(hr))
	{
		pFrameDescription = NULL;
		nWidth = 0;
		nHeight = 0;
		UINT nBufferSize = 0;
		ColorImageFormat imageFormat = ColorImageFormat_None;

		RGBQUAD *pBuffer = NULL;

		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_FrameDescription(&pFrameDescription);
			//一共六种数据源,彩色图像
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Width(&nWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Height(&nHeight);
		}


		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
		}

		if (SUCCEEDED(hr))
		{
			if (imageFormat == ColorImageFormat_Bgra)
			{
				hr = pColorFrame->AccessRawUnderlyingBuffer(&nBufferSize, reinterpret_cast<BYTE**>(&pBuffer));

			}
			else if (m_pColorRGBX)
			{
				pBuffer = m_pColorRGBX;

				nBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);
				hr = pColorFrame->CopyConvertedFrameDataToArray(nBufferSize, reinterpret_cast<BYTE*>(pBuffer), ColorImageFormat_Bgra);


			}
			else
			{
				hr = E_FAIL;
			}
		}
		if (SUCCEEDED(hr))
		{

			ProcessColor(pBuffer, nWidth, nHeight);
			copy = ColorImage1.clone();
			BodyUpdate();//骨骼检测
		}
		SafeRelease(pFrameDescription);
	}
	SafeRelease(pColorFrame);

}
void CBodyBasics::ProcessColor(RGBQUAD* pBuffer, int nWidth, int nHeight)
{
	// Make sure we've received valid data
	if (pBuffer && (nWidth == cColorWidth) && (nHeight == cColorHeight))

	{

		Mat ColorImage(nHeight, nWidth, CV_8UC4, pBuffer);
		ColorImage1 = ColorImage;
		resize(ColorImage, showImage, Size(nWidth / 2, nHeight / 2));

		//读取彩色图像并输出到矩阵
		DrawMat(showImage, IDC_ColorImage);
		

	}

}

void      CBodyBasics::BodyUpdate()
{

	//while (myBodyReader->AcquireLatestFrame(&myBodyFrame) != S_OK); //读取身体图像
	HRESULT hr = myBodyReader->AcquireLatestFrame(&myBodyFrame);
	if (SUCCEEDED(hr))
	{
		myBodySource->get_BodyCount(&myBodyCount);
		IBody   **  myBodyArr = new IBody *[myBodyCount];       //为存身体数据的数组做准备
		for (int i = 0; i < myBodyCount; i++)
			myBodyArr[i] = nullptr;

		if (myBodyFrame->GetAndRefreshBodyData(myBodyCount, myBodyArr) == S_OK)     //把身体数据输入数组
			for (int i = 0; i < myBodyCount; i++)
			{
				BOOLEAN     result = false;
				if (myBodyArr[i]->get_IsTracked(&result) == S_OK && result) //先判断是否侦测到
				{
					Joint   myJointArr[JointType_Count];
					if (myBodyArr[i]->GetJoints(JointType_Count, myJointArr) == S_OK)   //如果侦测到就把关节数据输入到数组并画图
					{
						//---------------------------------画身体----------------------------------------//
						draw(copy, myJointArr[JointType_Head], myJointArr[JointType_Neck], myMapper);
						draw(copy, myJointArr[JointType_Neck], myJointArr[JointType_SpineShoulder], myMapper);

						draw(copy, myJointArr[JointType_SpineShoulder], myJointArr[JointType_ShoulderLeft], myMapper);
						draw(copy, myJointArr[JointType_SpineShoulder], myJointArr[JointType_SpineMid], myMapper);
						draw(copy, myJointArr[JointType_SpineShoulder], myJointArr[JointType_ShoulderRight], myMapper);

						draw(copy, myJointArr[JointType_ShoulderLeft], myJointArr[JointType_ElbowLeft], myMapper);
						draw(copy, myJointArr[JointType_SpineMid], myJointArr[JointType_SpineBase], myMapper);
						draw(copy, myJointArr[JointType_ShoulderRight], myJointArr[JointType_ElbowRight], myMapper);

						draw(copy, myJointArr[JointType_ElbowLeft], myJointArr[JointType_WristLeft], myMapper);
						draw(copy, myJointArr[JointType_SpineBase], myJointArr[JointType_HipLeft], myMapper);
						draw(copy, myJointArr[JointType_SpineBase], myJointArr[JointType_HipRight], myMapper);
						draw(copy, myJointArr[JointType_ElbowRight], myJointArr[JointType_WristRight], myMapper);

						draw(copy, myJointArr[JointType_WristLeft], myJointArr[JointType_ThumbLeft], myMapper);
						draw(copy, myJointArr[JointType_WristLeft], myJointArr[JointType_HandLeft], myMapper);
						draw(copy, myJointArr[JointType_HipLeft], myJointArr[JointType_KneeLeft], myMapper);
						draw(copy, myJointArr[JointType_HipRight], myJointArr[JointType_KneeRight], myMapper);
						draw(copy, myJointArr[JointType_WristRight], myJointArr[JointType_ThumbRight], myMapper);
						draw(copy, myJointArr[JointType_WristRight], myJointArr[JointType_HandRight], myMapper);

						draw(copy, myJointArr[JointType_HandLeft], myJointArr[JointType_HandTipLeft], myMapper);
						draw(copy, myJointArr[JointType_KneeLeft], myJointArr[JointType_FootLeft], myMapper);
						draw(copy, myJointArr[JointType_KneeRight], myJointArr[JointType_FootRight], myMapper);
						draw(copy, myJointArr[JointType_HandRight], myJointArr[JointType_HandTipRight], myMapper);

						//---------------------------------画关节角度------------------------------------//
						drawAngle(copy, myJointArr[JointType_Head], myJointArr[JointType_Neck], myJointArr[JointType_SpineShoulder], JointType_Neck, myMapper);
						
						drawAngle(copy, myJointArr[JointType_ShoulderLeft], myJointArr[JointType_ElbowLeft], myJointArr[JointType_WristLeft], JointType_ElbowLeft, myMapper);
						drawAngle(copy, myJointArr[JointType_ElbowLeft], myJointArr[JointType_WristLeft], myJointArr[JointType_HandLeft], JointType_WristLeft, myMapper);
						
						drawAngle(copy, myJointArr[JointType_ShoulderRight], myJointArr[JointType_ElbowRight], myJointArr[JointType_WristRight], JointType_ElbowRight, myMapper);
						drawAngle(copy, myJointArr[JointType_ElbowRight], myJointArr[JointType_WristRight], myJointArr[JointType_HandRight], JointType_WristRight, myMapper);
					}

				}
			}
		DrawMat(copy, IDC_STATIC);
		delete[]myBodyArr;
		myBodyFrame->Release();
		PrintData(IDC_EDIT1, "疲劳强度：%d",100);

	}
}

void   CBodyBasics::draw(Mat & img, Joint & r_1, Joint & r_2, ICoordinateMapper * myMapper)
{
	//用两个关节点来做线段的两端，并且进行状态过滤
	if (r_1.TrackingState == TrackingState_Tracked && r_2.TrackingState == TrackingState_Tracked)
	{
		ColorSpacePoint t_point;    //要把关节点用的摄像机坐标下的点转换成彩色空间的点
		Point   p_1, p_2;
		myMapper->MapCameraPointToColorSpace(r_1.Position, &t_point);
		p_1.x = t_point.X;
		p_1.y = t_point.Y;
		myMapper->MapCameraPointToColorSpace(r_2.Position, &t_point);
		p_2.x = t_point.X;
		p_2.y = t_point.Y;


		line(img, p_1, p_2, Scalar(0, 255, 0), 5);
		circle(img, p_1, 10, Scalar(255, 0, 0), -1);
		circle(img, p_2, 10, Scalar(255, 0, 0), -1);
	}
}

double get_angle(double x1, double y1, double x2, double y2, double x3, double y3)
{
	double theta = atan2(x1 - x3, y1 - y3) - atan2(x2 - x3, y2 - y3);
	if (theta > 3.1415926535)
		theta -= 2 * 3.1415926535;
	if (theta < -3.1415926535)
		theta += 2 * 3.1415926535;

	theta = abs(theta * 180.0 / 3.1415926535);
	return theta;
}

void CBodyBasics::drawAngle(Mat & img, Joint & r_1, Joint & r_2, Joint & r_3, int jointType, ICoordinateMapper * myMapper)
{
	if (r_1.TrackingState == TrackingState_Tracked && r_2.TrackingState == TrackingState_Tracked && r_3.TrackingState == TrackingState_Tracked)
	{
		ColorSpacePoint t_point1,t_point2,t_point3;    //要把关节点用的摄像机坐标下的点转换成彩色空间的点
		Point   p_1,p_2,p_3;
		myMapper->MapCameraPointToColorSpace(r_1.Position, &t_point1);
		p_1.x = t_point1.X;
		p_1.y = t_point1.Y;
		myMapper->MapCameraPointToColorSpace(r_2.Position, &t_point2);
		p_2.x = t_point2.X;
		p_2.y = t_point2.Y;
		myMapper->MapCameraPointToColorSpace(r_3.Position, &t_point3);
		p_3.x = t_point3.X;
		p_3.y = t_point3.Y;
		double angle = get_angle(p_1.x, p_1.y, p_3.x, p_3.y, p_2.x, p_2.y);
		String s = std::to_string((int)angle);
		switch (jointType)
		{
		case JointType_Neck:
			putText(img, s, p_2, FONT_HERSHEY_COMPLEX, 2, Scalar(0, 255, 255), 2, 0);
			break;
		case JointType_ElbowLeft:
			putText(img, s, p_2, FONT_HERSHEY_COMPLEX, 2, Scalar(0, 255, 255), 2, 0);
			break;
		case JointType_WristLeft:
			putText(img, s, p_2, FONT_HERSHEY_COMPLEX, 2, Scalar(0, 255, 255), 2, 0);
			break;
		case JointType_ElbowRight:
			putText(img, s, p_2, FONT_HERSHEY_COMPLEX, 2, Scalar(0, 255, 255), 2, 0);
			break;
		case JointType_WristRight:
			putText(img, s, p_2, FONT_HERSHEY_COMPLEX, 2, Scalar(0, 255, 255), 2, 0);
			break;
		default:
			break;
		}
	}
}


void    CBodyBasics::DrawMat(cv::Mat & img, UINT nID)
{
	cv::Mat imgTmp;
	CRect rect;
	CmfctestDlg::s_pDlg->GetDlgItem(nID)->GetClientRect(&rect);  // 获取控件大小
	cv::resize(img, imgTmp, cv::Size(rect.Width(), rect.Height()));// 缩小或放大Mat并备份

	// 转一下格式 ,这段可以放外面,
	switch (imgTmp.channels())
	{
	case 1:
		cv::cvtColor(imgTmp, imgTmp, CV_GRAY2BGRA); // GRAY单通道
		break;
	case 3:
		cv::cvtColor(imgTmp, imgTmp, CV_BGR2BGRA);  // BGR三通道
		break;
	default:
		break;
	}

	int pixelBytes = imgTmp.channels()*(imgTmp.depth() + 1); // 计算一个像素多少个字节
	// 制作bitmapinfo(数据头)
	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
	bitInfo.bmiHeader.biWidth = imgTmp.cols;
	bitInfo.bmiHeader.biHeight = -imgTmp.rows;
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biSizeImage = 0;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0;
	// Mat.data + bitmap数据头 -> MFC
	CDC *pDC = CmfctestDlg::s_pDlg->GetDlgItem(nID)->GetDC();
	::StretchDIBits(
		pDC->GetSafeHdc(),
		0, 0, rect.Width(), rect.Height(),
		0, 0, rect.Width(), rect.Height(),
		imgTmp.data,
		&bitInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);
	CmfctestDlg::s_pDlg->ReleaseDC(pDC);
}

void    CBodyBasics::PrintData(UINT nID, char *fmt, ...)
{
	char uart_buffer[100 + 1];
	int size;

	va_list arg_ptr;

	va_start(arg_ptr, fmt);

	size = vsnprintf(uart_buffer, 100 + 1, fmt, arg_ptr);
	va_end(arg_ptr);

	CString str = CString(uart_buffer);
	CmfctestDlg::s_pDlg->GetDlgItem(nID)->SetWindowText(str);
}
