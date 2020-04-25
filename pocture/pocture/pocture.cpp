#include "pch.h"
#include <iostream>
#include <opencv/cv.hpp>
#include <opencv2/core.hpp>
#include <highgui/highgui.hpp>
#include<map>

using namespace cv;
using namespace std;

//初始化这些即可
string sourcePath = "101.jpg";//原始图像路径
string repalcePicturePath = "D:\\imgs\\";//用于替换的图片的路径
string pictureFormat = "*.jpg"; //读取格式是.jpg可以换成其他，但是文件夹下要统一
string correctSizePicturePath = "D:\\imgs\\resize\\";//把用于替换的图片修正成小碎片的尺寸存放的路径
string resultPicturePath = "result.jpg";//结果存放的路径以及命名
#define replaceWidth 500//替换像素的宽度
#define replaceHeight 500//替换像素的高度
#define alpha 0.2//图片融合相似度系数,系数越小越接近原始图（0-1范围）
#define beta (1 - alpha)


Mat spliteTest(Mat image, int x, int y, int width, int height);
void mergeTest(Mat result, Mat piecse, int x, int y, int width, int height);
float cal_mean_stddev(Mat src);
void convert(Mat &src, int width, int height);

int main()
{
	//计算替换图片的灰度均值
	//把替换图片的均值和路径存到map中键值是灰度均值，实值是路径
	//遍历切割结果，计算切片的灰度均值，用替换图片替换并且合并到结果矩阵
	cout << "开始读取目标图片" << endl;
	Mat image = imread(sourcePath);//加载原始图像
	Mat source;
	image.copyTo(source);
	cout << "完成读取目标图片" << endl;
	cout << "开始马赛克转换并修正图片" << endl;
	convert(image, replaceWidth, replaceHeight);
	cout << "转换完成" << endl;
	cout << "开始融合优化" << endl;
	addWeighted(image, alpha, source, beta, 0.0, image);
	cout << "优化完成，优化相似度：" << beta << endl;
	cout << "开始将结果写入本地" << endl;
	imwrite(resultPicturePath, image);
	cout << "写入成功，文件路径：" << resultPicturePath << endl;
	cvWaitKey(0);

}


Mat spliteTest(Mat image, int x, int y, int width, int height)
{
	long cols = image.cols;
	long rows = image.rows;
	Rect rect(x, y, width, height);
	Mat nullImage(width, height, CV_8UC3);
	nullImage = image(rect);
	return nullImage;
}

void mergeTest(Mat result, Mat piecse, int x, int y, int width, int height)
{
	Rect rect(x, y, width, height);
	piecse.copyTo(result(rect));
}

float cal_mean_stddev(Mat src) {
	Mat gray, mat_mean, mat_stddev;
	cvtColor(src, gray, CV_RGB2GRAY); // 转换为灰度图
	meanStdDev(gray, mat_mean, mat_stddev);
	float m;
	//m = mat_stddev.at<double>(0, 0);
	m = mat_mean.at<double>(0, 0);
	//cout << "灰度均值是：" << m << endl;
	return m;
}

void convert(Mat &src, int width, int height)
{
	string pattern = repalcePicturePath + pictureFormat;
	vector<cv::String> fileName;
	map<int, string> strMap;//定义map类用均值作为键值来查找文件的路径
	glob(pattern, fileName, false);//查找符合特定规则的文件路径名

	//for循环遍历用于替换的图片
	for (int i = 0; i < fileName.size(); i++)
	{
		string imgName = fileName[i];
		Mat temp = imread(imgName);
		//修改替换图片到马赛克图大小
		resize(temp, temp, Size(width, height));
		imwrite(correctSizePicturePath + to_string(i) + ".jpg", temp);
		//map类用均值作为键值来查找文件的路径
		pair<float, string> value(cal_mean_stddev(temp), correctSizePicturePath + to_string(i) + ".jpg");
		strMap.insert(value);
	}

	cout << "原始col:" << src.cols << endl;
	cout << "原始row:" << src.rows << endl;
	src = (src.cols % width == 0) ? src : spliteTest(src, 0, 0, src.cols - src.cols % width, src.rows);
	src = (src.rows % height == 0) ? src : spliteTest(src, 0, 0, src.cols, src.rows - src.rows % height);
	cout << "修正col:" << src.cols << endl;
	cout << "修正row:" << src.rows << endl;

	for (int i = 0; i < src.rows / height; i++)
	{
		for (int j = 0; j < src.cols / width; j++)
		{
			static int n = 0;
			float error = 0;
			float lasterror = 10000000;
			string path = "";
			Mat tempPieces = spliteTest(src, j * width, i * height, width, height);
			float ave = cal_mean_stddev(tempPieces);
			map<int, string>::iterator strmap_iter = strMap.begin();
			for (; strmap_iter != strMap.end(); strmap_iter++)
			{
				if (fabsf(ave - strmap_iter->first) < 1)
				{
					path = strmap_iter->second;
					break;
				}


				error = fabsf(ave - strmap_iter->first);
				if (error < lasterror)
				{
					lasterror = error;
					path = strmap_iter->second;
				}
			}
			Mat tempimg = imread(path);
			mergeTest(src, tempimg, j * width, i * height, width, height);
		}
	}
}
