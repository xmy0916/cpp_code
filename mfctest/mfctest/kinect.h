//kinect.h
#pragma once
#include <Kinect.h>
#include "cv.h"
#include <opencv2/opencv.hpp>   //opencvͷ�ļ�
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp> 

using namespace cv;
using namespace std;            //�����ռ�

template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

class CBodyBasics
{

public:
	static const int        cColorWidth = 1920;
	static const int        cColorHeight = 1080;
	CBodyBasics();
	~CBodyBasics();
	friend class CSerialPortTestDlg;
	void                    Update();//��ùǼܡ�������ֵͼ�������Ϣ
	HRESULT                 InitializeDefaultSensor();//���ڳ�ʼ��kinect

private:
	IKinectSensor          *mySensor;//kinectԴ
	IColorFrameSource       * myColorSource;
	UINT16                  *pBuffer;
	IColorFrame            * pColorFrame;
	IColorFrameReader     *pColorReader;//���ڲ�ɫ���ݶ�ȡ
	IFrameDescription      * myDescription;
	RGBQUAD*                m_pColorRGBX;
	IFrameDescription      * pFrameDescription;
	IBodyFrameSource        * myBodySource;
	IBodyFrameReader        * myBodyReader;
	IBodyFrame               * myBodyFrame;
	ICoordinateMapper        * myMapper;

	cv::Mat                 copy;           // ��ɫͼ
	cv::Mat                 showImage;
	cv::Mat                 ColorImage1;
	//int colorHeight = 0, colorWidth = 0;
	int myBodyCount;
	int nWidth;
	int nHeight;
public:
	void    DrawMat(cv::Mat & img, UINT nID);
	void    PrintData(UINT nID, char *fmt, ...);
	void    ProcessColor(RGBQUAD* pBuffer, int nWidth, int nHeight);
	//void    updataToSurface(Mat &img );
	void    draw(Mat & img, Joint & r_1, Joint & r_2, ICoordinateMapper * myMapper);//���߸�Բ
	void     BodyUpdate();
	void    drawAngle(Mat & img, Joint & r_1, Joint & r_2, Joint & r_3, int jointType, ICoordinateMapper * myMapper);//���ؽڽǶ�



};
