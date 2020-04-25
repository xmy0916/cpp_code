#include "myKinect.h"
#include <iostream>
#include<opencv2/opencv.hpp>
#include<Windows.h>
using namespace std;
using namespace cv;

int main()
{
	cout << endl << "\t����ʼִ��~\n\n"
		<< "\t���¡�S����ʱ���������ͼ��~!\n";
	cout << endl;
	cout << endl;
	CBodyBasics myKinect;
	HRESULT hr = myKinect.InitializeDefaultSensor();
	if (SUCCEEDED(hr)){
		while (1){
			myKinect.Update();
			if (char(waitKey(1)) != 'S')
				;
			else
			{
				myKinect.SaveDepthImg();
			}

		}
	}
	else{
		cout << "kinect initialization failed!" << endl;
		system("pause");
	}
}