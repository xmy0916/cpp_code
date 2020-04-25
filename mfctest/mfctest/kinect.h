//kinect.h
#pragma once
#include <Kinect.h>
#include "cv.h"
#include <opencv2/opencv.hpp>   //opencv头文件
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp> 

using namespace cv;
using namespace std;            //命名空间

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
	void                    Update();//获得骨架、背景二值图和深度信息
	HRESULT                 InitializeDefaultSensor();//用于初始化kinect

private:
	IKinectSensor          *mySensor;//kinect源
	IColorFrameSource       * myColorSource;
	UINT16                  *pBuffer;
	IColorFrame            * pColorFrame;
	IColorFrameReader     *pColorReader;//用于彩色数据读取
	IFrameDescription      * myDescription;
	RGBQUAD*                m_pColorRGBX;
	IFrameDescription      * pFrameDescription;
	IBodyFrameSource        * myBodySource;
	IBodyFrameReader        * myBodyReader;
	IBodyFrame               * myBodyFrame;
	ICoordinateMapper        * myMapper;

	cv::Mat                 copy;           // 彩色图
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
	void    draw(Mat & img, Joint & r_1, Joint & r_2, ICoordinateMapper * myMapper);//画线跟圆
	void     BodyUpdate();
	void    drawAngle(Mat & img, Joint & r_1, Joint & r_2, Joint & r_3, int jointType, ICoordinateMapper * myMapper);//画关节角度



};
