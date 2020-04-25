#include "pch.h"
#include <iostream>
#include <windows.h>
#include <winbase.h>
#include <conio.h>
#include <cstdio>
#include <cstdlib>
#include <cv.h>
#include <highgui.h>
#include <cvaux.h>
#include <cxcore.h>
#include <string>
#include "tchar.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define ROW 288//行
#define COL 352//列

typedef struct CAMERA {
	Mat frame;
	Mat gray;
	Mat binary;
	Mat ninary_cut_noise;
	Mat edge;
}CAMERA;

HANDLE hComDev;
OVERLAPPED m_ov;
COMSTAT comstat;
DWORD m_dwCommEvents;
bool bRead = true;
bool bResult = true;
DWORD dwError = 0;
DWORD bytesRead = 0;


/**打开串口设备**/
bool openPort(LPCWSTR portname);
/**设置串口设备DCB参数**/
bool setupDcb(int rate_arg);
/**设置串口设备超时限制参数**/
bool setuptimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier,
	DWORD ReadTotalconstant, DWORD WriteTotalMultiplier,
	DWORD WriteTotalconstant);
/**串口收发数据**/
void ReceiveChar(unsigned char *RXBuffer);
bool WriteChar(char* m_szWriteBuffer, DWORD m_nToSend);

/**数组搜索最大最小数**/
int find_min(uint8_t arr[], int n);
int find_max(uint8_t arr[], int n);
int Deal_Binary_Image_Horizone(Mat binary_image);
int Deal_Binary_Image_vertical(Mat binary_image);
int distance_horizone = 0;
int distance_vertical = 0;
float chassis_distance_KP = -1;
float chassis_distance_KD = 0.5;
float chassis_distance_KP_vertical = -0.6;
float chassis_distance_KD_vertical = 0.5;

void send_wave(float arg1, float arg2, float arg3, float arg4);
int distance_exe_horizone(float,float,float,float);
int distance_exe_vertical(float, float, float, float);


int main(int argc, char**argv)
{
	CAMERA camera;
	VideoCapture capture(1);
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 300);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 300);

	if (!capture.isOpened())
	{
		cout << "camera open failed!" << endl;
		return -1;
	}

	bool openFlag = false;

	openFlag = openPort(_T("COM5"));
	if (openFlag)
	{
		printf("打开串口成功！\n");
		if (setupDcb(115200))
		{
			printf("波特率设置成功\n");
		}
		if (setuptimeout(0, 0, 0, 0, 0))
		{
			printf("超时限制设置成功\n");

			SetCommMask(hComDev, EV_RXCHAR);//当有字符在 inbuf 中时产生这个事件
				//清除串口的所有操作
			PurgeComm(hComDev, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
			try
			{
				while (1)
				{
					capture >> camera.frame;
					cvtColor(camera.frame, camera.gray, CV_BGR2GRAY);

					threshold(camera.gray, camera.binary, 220, 255, CV_THRESH_BINARY);
					//distance_horizone = Deal_Binary_Image_Horizone(camera.binary);
					distance_vertical = Deal_Binary_Image_vertical(camera.binary);
					//cv::Point start_horizone = cv::Point(0, distance_horizone); //直线起点
					//cv::Point end_horizone = cv::Point(352, distance_horizone);   //直线终点
					//cv::line(camera.frame, start_horizone, end_horizone, cv::Scalar(0, 0, 255));
					cv::Point start_vertical = cv::Point(distance_vertical, 0); //直线起点
					cv::Point end_vertical = cv::Point(distance_vertical, 288);   //直线终点
					cv::line(camera.frame, start_vertical, end_vertical, cv::Scalar(0, 0, 255));

					//int pwm_horizone = distance_exe_horizone(chassis_distance_KP, chassis_distance_KD, 75, distance_horizone);
					int pwm_vertical = distance_exe_vertical(chassis_distance_KP_vertical, chassis_distance_KD_vertical, 150, distance_vertical);
					//int pwm_vertical2 = distance_exe_vertical(chassis_distance_KP_vertical, chassis_distance_KD_vertical, 110, distance_vertical);
					char arr[15] = { 0 };
					sprintf_s(arr, "D=0 %dm", pwm_vertical);
					cout << "竖直 = "<< pwm_vertical << endl;
					WriteChar(arr, 15);
					//send_wave(distance_horizone,0,0,0);
					//blur(binary, ninary_cut_noise, Size(3, 3));

					//Canny(binary, edge, 3, 9, 3);

					namedWindow("二值化图片");
					imshow("二值化图片", camera.frame);
					int c = cvWaitKey(40);

					//if (c == 'q')
						//return 0;

					//WriteChar((char*)"please send data now", 20);
				}
				destroyAllWindows();
				CloseHandle(hComDev);
			}
			catch (const std::exception&)
			{
				destroyAllWindows();
				CloseHandle(hComDev);
				capture.release();
			}

		}
	}
	return 0;
}

/**打开串口设备**/
bool openPort(LPCWSTR portname)
{
	hComDev = CreateFile(portname, //串口号
		GENERIC_READ | GENERIC_WRITE,//允许读写
		0,//通讯设备以独占方式打开
		0,//无安全属性
		OPEN_EXISTING,//通讯设备已存在
		FILE_FLAG_OVERLAPPED,//异步I/O
		0);//通讯设备不能用模板打开
	if (hComDev == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hComDev);
		printf("打开串口失败！\n");
		return false;
	}
	else
	{
		return true;
	}
}
bool setupDcb(int rate_arg)
{
	DCB dcb;
	//int rate = rate_arg;
	memset(&dcb, 0, sizeof(dcb));
	if (!GetCommState(hComDev, &dcb))
	{
		return false;
	}
	else
	{
		// set DCB to configure the serial port
		//波特率为115200，无奇偶校验，8位数据位，1位停止位
		//BuildCommDCB("115200,N,8,1",&dcb);

		dcb.DCBlength = sizeof(dcb);
		// ---------- Serial Port Config -------
		dcb.BaudRate = rate_arg;//波特率
		dcb.Parity = NOPARITY;//奇偶校验
		dcb.fParity = 0;
		dcb.StopBits = ONESTOPBIT;//1位停止位
		dcb.ByteSize = 8;//8位数据位
		dcb.fOutxCtsFlow = 0;
		dcb.fOutxDsrFlow = 0;
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
		dcb.fDsrSensitivity = 0;
		dcb.fRtsControl = RTS_CONTROL_DISABLE;
		dcb.fOutX = 0;
		dcb.fInX = 0;
		//---------- misc parameters -----
		dcb.fErrorChar = 0;
		dcb.fBinary = 1;
		dcb.fNull = 0;
		dcb.fAbortOnError = 0;
		dcb.wReserved = 0;
		dcb.XonLim = 2;
		dcb.XoffLim = 4;
		dcb.XonChar = 0x11;
		dcb.XoffChar = 0x13;
		dcb.EvtChar = 0;

		// set DCB
		if (!SetCommState(hComDev, &dcb))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}
bool setuptimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD
	ReadTotalconstant, DWORD WriteTotalMultiplier,
	DWORD WriteTotalconstant)
{
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = ReadInterval;
	timeouts.ReadTotalTimeoutConstant = ReadTotalconstant;
	timeouts.ReadTotalTimeoutMultiplier = ReadTotalMultiplier;
	timeouts.WriteTotalTimeoutConstant = WriteTotalconstant;
	timeouts.WriteTotalTimeoutMultiplier = WriteTotalMultiplier;
	if (!SetCommTimeouts(hComDev, &timeouts))
		return false;
	else
		return true;
}
void ReceiveChar(unsigned char *RXBuffer)
{
	if (bRead)
	{
		bResult = ReadFile(hComDev,//Handle to Comm port
			RXBuffer,//RX Buffer Pointer
			1,       //Read one byte
			&bytesRead,//Stores number of bytes read
			&m_ov);  //pointer to the m_ov structure

		if (!bResult)
		{
			switch (dwError = GetLastError())
			{
			case ERROR_IO_PENDING:
			{
				bRead = false;
				break;
			}
			default:
			{
				break;
			}
			}
		}
		else
		{
			bRead = true;
		}
	}
	if (!bRead)
	{
		bRead = true;
		bResult = GetOverlappedResult(hComDev,//Handle to Comm port
			&m_ov,//Overlapped structure
			&bytesRead,//Stores number of bytes read
			true);// Wait flag
	}
}

bool WriteChar(char* m_szWriteBuffer, DWORD m_nToSend)
{
	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;
	DWORD BytesSent = 0;
	//HANDLE m_hWriteEvent;
	//ResetEvent(m_hWriteEvent);

	if (bWrite)
	{
		m_ov.Offset = 0;
		m_ov.OffsetHigh = 0;
		// Clear buffer
		bResult = WriteFile(hComDev, // Handle to COMM Port
			m_szWriteBuffer, // Pointer to message buffer in calling finction
			m_nToSend,      // Length of message to send
			&BytesSent,     // Where to store the number of bytes sent
			&m_ov);        // Overlapped structure
		if (!bResult)
		{
			DWORD dwError = GetLastError();
			switch (dwError)
			{
			case ERROR_IO_PENDING:
			{
				// continue to GetOverlappedResults()
				BytesSent = 0;
				bWrite = FALSE;
				break;
			}
			default:
			{
				// all other error codes
				break;
			}
			}
		}
	} // end if(bWrite)
	if (!bWrite)
	{
		bWrite = TRUE;
		bResult = GetOverlappedResult(hComDev,   // Handle to COMM port
			&m_ov,     // Overlapped structure
			&BytesSent,    // Stores number of bytes sent
			TRUE);         // Wait flag

// deal with the error code
		if (!bResult)
		{
			printf("GetOverlappedResults() in WriteFile()");
		}
	} // end if (!bWrite)
	// Verify that the data size send equals what we tried to send
	if (BytesSent != m_nToSend)
	{
		printf("WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n",
			(int)BytesSent, (int)strlen((char*)m_szWriteBuffer));
	}
	return true;
}

int find_max(uint8_t arr[], int n)
{
	int a = arr[0];
	for (int i = 0; i < n - 1; i++)
	{
		if (a <= arr[i])
			a = arr[i];
	}
	return a;
}

int find_min(uint8_t arr[], int n)
{
	int a = arr[0];
	for (int i = 0; i < n - 1; i++)
	{
		if (a >= arr[i])
			a = arr[i];
	}
	return a;
}

/**
input: 二值化的图
output: 距离
*/
int Deal_Binary_Image_Horizone(Mat binary_image)
{
	uint8_t white_num_line[ROW] = {0};
	for (int i = 0; i < binary_image.rows; ++i)
	{
		//获取第 i 行首像素指针
		uchar * p = binary_image.ptr<uchar>(i);
		//对第 i 行的每个像素(byte)操作
		for (int j = 0; j < binary_image.cols; ++j)
		{
			if (p[j] == 255)
			{
				white_num_line[i]++;
			}
		}
	}
	//uint8_t max_num = find_max(white_num_line, ROW);
	int first_find = 0;
	int min_line = 0;
	int max_line = 0;
	int mid_line = 0;
	for (int i = 0; i < ROW; i++)
	{
		if (white_num_line[i] >= 50 && first_find == 0)
		{
			min_line = i;
			first_find = 1;
		}
		if (first_find == 1 && white_num_line[i] >= 50)
		{
			max_line = i;
		}
	}
	mid_line = (int)(max_line + min_line) / 2;
	return mid_line;
}

/**
input: 二值化的图
output: 距离
*/
int Deal_Binary_Image_vertical(Mat binary_image)
{
	int first_find = 0;
	int min_line = 0;
	int max_line = 0;
	int mid_line = 0;
	uint8_t white_num_line[352] = { 0 };
	for (int i = 0; i < binary_image.cols; ++i)
	{
		for (int j = 0; j < binary_image.rows; ++j)
		{
			if (binary_image.at<uchar>(j, i) == 255)
			{
				white_num_line[i]++;
			}
		}
		if (white_num_line[i] >= 50 && first_find == 0)
		{
			min_line = i;
			first_find = 1;
		}
		if (first_find == 1 && white_num_line[i] >= 50)
		{
			max_line = i;
		}
	}
	//uint8_t max_num = find_max(white_num_line, ROW);
	mid_line = (int)(max_line + min_line) / 2;
	return mid_line;
}

char s[22] = { 'b','y',16,6 };
void send_wave(float arg1, float arg2, float arg3, float arg4) {

	s[2] = 16;  //length
	s[3] = 6;   //type
	s[20] = '\r';
	s[21] = '\n';
	memcpy(s + 4, &arg1, sizeof(arg1));
	memcpy(s + 8, &arg2, sizeof(arg1));
	memcpy(s + 12, &arg3, sizeof(arg1));
	memcpy(s + 16, &arg4, sizeof(arg1));
	//HAL_UART_Transmit_IT(&huart2, (uint8_t *)s, 22);
	WriteChar((char *)s, 22);

}

float chassis_PID_Sensor_Control_vertical(float kp,float kd,float target_distance,float now_distance)
{
	static float last_err;
	static float sum_err;
	static float last_dis;

	//    if(ChassisSignal.m_CtrlFlag._sensor_flag == 0)
	//    {
	//        last_err = 0;
	//        sum_err = 0;
	//        return 0;
	//    }


	if (distance_vertical == 0)
	{
		//uprintf(CMD_USART, "倒车");
		distance_vertical = last_dis;
	}
	//uprintf(CMD_USART,"error = %d\r\n",chassis.dis_laser1);
	if (sum_err >= 8000)
		sum_err = 8000;

	float distance_error = target_distance - now_distance;


	if (distance_error<4 && distance_error>-4)
		distance_error = 0;


	float P_out = distance_error * kp;
	float D_out = (last_err - distance_error) * kd;
	last_err = distance_error;
	sum_err += distance_error;
	float out = P_out + D_out;
	last_dis = distance_vertical;
	//uprintf(CMD_USART,"偏差距离 = %f\r\n",distance_error);
	return out;
}

float chassis_PID_Sensor_Control_horizone(float kp, float kd, float target_distance, float now_distance)
{
	static float last_err;
	static float sum_err;

	//    if(ChassisSignal.m_CtrlFlag._sensor_flag == 0)
	//    {
	//        last_err = 0;
	//        sum_err = 0;
	//        return 0;
	//    }


	if (distance_horizone == 0)
	{
		//uprintf(CMD_USART, "倒车");
		return 145;
	}
	//uprintf(CMD_USART,"error = %d\r\n",chassis.dis_laser1);
	if (sum_err >= 8000)
		sum_err = 8000;

	float distance_error = target_distance - now_distance;


	if (distance_error<4 && distance_error>-4)
		distance_error = 0;


	float P_out = distance_error * kp;
	float D_out = (last_err - distance_error) * kd;
	last_err = distance_error;
	sum_err += distance_error;
	float out = P_out + D_out;
	//uprintf(CMD_USART,"偏差距离 = %f\r\n",distance_error);
	return out;
}

int distance_exe_horizone(float kp,float kd,float target_distance,float now_distance)
{
	float distance_output = chassis_PID_Sensor_Control_horizone(kp,kd,target_distance, now_distance);//传感器距离环 


	if (distance_output > 1000)//距离pid
	{
		distance_output = 1000;
	}
	if (distance_output < -1000)
	{
		distance_output = -1000;
	}
	//motor0_error = -distance_output;
	//motor2_error = distance_output;
	return (int)distance_output;
}

int distance_exe_vertical(float kp, float kd, float target_distance, float now_distance)
{
	float distance_output = chassis_PID_Sensor_Control_vertical(kp, kd, target_distance, now_distance);//传感器距离环 


	if (distance_output > 1000)//距离pid
	{
		distance_output = 1000;
	}
	if (distance_output < -1000)
	{
		distance_output = -1000;
	}
	//motor0_error = -distance_output;
	//motor2_error = distance_output;
	return (int)distance_output;
}


