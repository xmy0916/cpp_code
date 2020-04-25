//kinect.cpp
#include "stdafx.h"
#include "afxdialogex.h"
#include "mfcKinectBodyCheck.h"
#include "mfcKinectBodyCheckDlg.h"
#include <Windows.h>

#include"kinect.h"
#include"stdio.h"
#include "Resource.h"
using namespace cv;
/* ���������������޳�û������������
* ��������������������
* ���������ߩ��������ߩ�
* ����������������������
* ����������������������
* ���������ש������ס���
* ����������������������
* �������������ߡ�������
* ����������������������
* ����������������������Code is far away from bug with the animal protecting
* ������������������    ���ޱ���, ������bug
* ������������������
* ��������������������������
* �������������������������ǩ�
* ����������������������������
* �������������������ש�����
* �������������ϩϡ����ϩ�
* �������������ߩ������ߩ�
* ��������������������������������*/

///���캯��
CBodyBasics::CBodyBasics()
{
	mySensor = nullptr;
	myColorSource = nullptr;
	pColorReader = nullptr;
	myDescription = nullptr;

	pBuffer = NULL;
	pColorFrame = NULL;
	m_pColorRGBX = new RGBQUAD[cColorWidth*cColorHeight];
	//-----------------��ʼ������------//
	myBodyCount = 0;
	myBodySource = nullptr;
	myBodyReader = nullptr;
	myBodyFrame = nullptr;
	myMapper = nullptr;
}


/// ��������
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
	mySensor->Open();//��kinect
	mySensor->get_ColorFrameSource(&myColorSource);//�򿪲�ɫ֡��Դ
	myColorSource->OpenReader(&pColorReader);//�򿪲�ɫ֡��ȡ��
	SafeRelease(myColorSource);
	//**********************����ΪColorFrame�Ķ�ȡǰ׼��**************************


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


	HRESULT hr = pColorReader->AcquireLatestFrame(&pColorFrame);
	//���²�ɫ֡



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
			//һ����������Դ,��ɫͼ��
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
			BodyUpdate();//�������
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

		//��ȡ��ɫͼ�����������
		DrawMat(showImage, IDC_ColorImage); 


	}

}

void      CBodyBasics::BodyUpdate()
{

	//while (myBodyReader->AcquireLatestFrame(&myBodyFrame) != S_OK); //��ȡ����ͼ��
	HRESULT hr = myBodyReader->AcquireLatestFrame(&myBodyFrame);
	if (SUCCEEDED(hr))
	{
		myBodySource->get_BodyCount(&myBodyCount);
		IBody   **  myBodyArr = new IBody *[myBodyCount];       //Ϊ���������ݵ�������׼��
		for (int i = 0; i < myBodyCount; i++)
			myBodyArr[i] = nullptr;

		if (myBodyFrame->GetAndRefreshBodyData(myBodyCount, myBodyArr) == S_OK)     //������������������
			for (int i = 0; i < myBodyCount; i++)
			{
				BOOLEAN     result = false;
				if (myBodyArr[i]->get_IsTracked(&result) == S_OK && result) //���ж��Ƿ���⵽
				{
					Joint   myJointArr[JointType_Count];
					if (myBodyArr[i]->GetJoints(JointType_Count, myJointArr) == S_OK)   //�����⵽�Ͱѹؽ��������뵽���鲢��ͼ
					{
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

					}

				}
			}
		DrawMat(copy, IDC_STATIC);
		delete[]myBodyArr;
		myBodyFrame->Release();

	}
}

void   CBodyBasics::draw(Mat & img, Joint & r_1, Joint & r_2, ICoordinateMapper * myMapper)
{
	//�������ؽڵ������߶ε����ˣ����ҽ���״̬����
	if (r_1.TrackingState == TrackingState_Tracked && r_2.TrackingState == TrackingState_Tracked)
	{
		ColorSpacePoint t_point;    //Ҫ�ѹؽڵ��õ�����������µĵ�ת���ɲ�ɫ�ռ�ĵ�
		Point   p_1, p_2;
		myMapper->MapCameraPointToColorSpace(r_1.Position, &t_point);
		p_1.x = t_point.X;
		p_1.y = t_point.Y;
		myMapper->MapCameraPointToColorSpace(r_2.Position, &t_point);
		p_2.x = t_point.X;
		p_2.y = t_point.Y;

		/*  line(img, p_1, p_2, Vec3b(0, 255, 0), 5);
		circle(img, p_1, 10, Vec3b(255, 0, 0), -1);
		circle(img, p_2, 10, Vec3b(255, 0, 0), -1);*/
		line(img, p_1, p_2, Scalar(0, 255, 0), 5);
		circle(img, p_1, 10, Scalar(255, 0, 0), -1);
		circle(img, p_2, 10, Scalar(255, 0, 0), -1);
	}
}


void    CBodyBasics::DrawMat(cv::Mat & img, UINT nID)
{
	cv::Mat imgTmp;
	CRect rect;
	CmfcKinectBodyCheckDlg::s_pDlg->GetDlgItem(nID)->GetClientRect(&rect);  // ��ȡ�ؼ���С
	cv::resize(img, imgTmp, cv::Size(rect.Width(), rect.Height()));// ��С��Ŵ�Mat������

	// תһ�¸�ʽ ,��ο��Է�����,
	switch (imgTmp.channels())
	{
	case 1:
		cv::cvtColor(imgTmp, imgTmp, CV_GRAY2BGRA); // GRAY��ͨ��
		break;
	case 3:
		cv::cvtColor(imgTmp, imgTmp, CV_BGR2BGRA);  // BGR��ͨ��
		break;
	default:
		break;
	}

	int pixelBytes = imgTmp.channels()*(imgTmp.depth() + 1); // ����һ�����ض��ٸ��ֽ�
	// ����bitmapinfo(����ͷ)
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
	// Mat.data + bitmap����ͷ -> MFC
	CDC *pDC = CmfcKinectBodyCheckDlg::s_pDlg->GetDlgItem(nID)->GetDC();
	::StretchDIBits(
		pDC->GetSafeHdc(),
		0, 0, rect.Width(), rect.Height(),
		0, 0, rect.Width(), rect.Height(),
		imgTmp.data,
		&bitInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);
	CmfcKinectBodyCheckDlg::s_pDlg->ReleaseDC(pDC);
}