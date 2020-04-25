// testExtern.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "Extern.h"

//extern int externNumber;
//extern int add(int a, int b);
//extern int getStaticNumber();
int main()
{
	printf("访问外部文件的变量externNumber:%d\n", externNumber);
	printf("访问外部文件的变量externNumber:%d\n", getStaticNumber());
	printf("访问外部文件的函数2+ 3 = %d\n", add(2, 3));
}

