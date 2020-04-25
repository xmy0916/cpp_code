// shellSort.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <stdio.h>
#define LEN(arr) sizeof(arr) / sizeof(arr[0])
void shellSort(int arr[], int len);
int main()
{
	int arr[] = { 5,7,8,3,1,2,4,6 };
	shellSort(arr, LEN(arr));
	for (int i = 0; i < LEN(arr); i++)
	{
		printf("%d,", arr[i]);
	}
	printf("\n");

}

void insertSort(int arr[], int inc, int i)
{
	int insertNumber = arr[i];
	int j = i - inc;
	for (; j >= 0 && insertNumber < arr[j]; j -= inc)
	{
		arr[j + inc] = arr[j];
	}
	arr[j + inc] = insertNumber;
}

void shellSort(int arr[],int len)
{
	for (int inc = len / 2; inc > 0; inc /= 2)
	{
		for (int i = inc; i < len; i++)
		{
			insertSort(arr, inc, i);
		}
	}
}
