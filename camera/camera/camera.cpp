
#include "pch.h"
#include<opencv2/opencv.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

Mat srcImage, srcImage1, midImage, dstImage;

int main()
{
	srcImage1 = imread("E:\\vs_code\\camera\\2.jpg");
	//图片缩放
	resize(srcImage1, srcImage, Size(srcImage1.cols / 2, srcImage1.rows / 2), (0, 0), (0, 0), 3);
	//边缘检测
	Canny(srcImage, midImage, 50, 200, 3);
	//灰度变换
	cvtColor(midImage, dstImage, CV_GRAY2BGR);

	vector<Vec2f>lines;//定义矢量结构存放得到的线段矢量集合
	//霍夫变换
	HoughLines(
		midImage, //输入图像
		lines,//线条的输出矢量
		1, //搜索时进步尺寸的单位半径
		CV_PI / 180,//搜索时进步尺寸的单位角度
		150,//平面阈值参数，达到此值才可以被检测
		0,//进步尺寸rho的除数距离
		0//进步尺寸单位角度的除数距离
	);

	for (size_t i = 0; i < lines.size(); i++);
	{
		int i = 0;
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
		line(dstImage, pt1, pt2, Scalar(55, 100, 195), 1, LINE_AA);
	}
	imshow("【原始图】", srcImage);
	imshow("【边缘检测后的效果图】", midImage);
	imshow("【效果图】", dstImage);
	waitKey(0);
	return 0;
}