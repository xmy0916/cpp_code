// insert_sort.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <stdio.h>

 // 数组长度
#define LENGTH(array) ( (sizeof(array)) / (sizeof(array[0])) )

/*
 * 折半插入排序
 *
 * 参数说明：
 *     a -- 待排序的数组
 *     len -- 数组的长度
 */
void binaryInsertSort(int a[], int len)
{
	int i, j, k;

	for (i = 1; i < len; i++)
	{
		//为a[i]在前面的a[0...i-1]有序区间中找一个合适的位置
		int left = 0, right = 0;
		for (int p = 0; p < i; p++) {
			//折半查找应该插入的位置
			left = 0;
			right = i - 1;
			while (left <= right) {
				int m = (left + right) / 2;
				if (a[m] > a[i])
					right = m - 1;
				else
					left = m + 1;
			}
		}
		j = right;
		printf("%d\n", right + 1);
		//如找到了一个合适的位置
		if (j != i - 1)
		{
			//将比a[i]大的数据向后移
			int temp = a[i];
			for (k = i - 1; k > j; k--)
				a[k + 1] = a[k];
			//将a[i]放到正确位置上
			a[k + 1] = temp;
		}
	}
}

/*
 * 直接插入排序
 *
 * 参数说明：
 *     a -- 待排序的数组
 *     n -- 数组的长度
 */

void insertSort(int a[], int n)
{
	int temp, j, k;
	for (int i = 1; i < n; i++)
	{
		temp = a[i];
		for (j = i - 1; j >= 0; j--)
		{
			if (a[j] < temp)
				break;
		}
		if (j != i - 1)
		{
			for (k = i - 1; k > j; k--)
			{
				a[k + 1] = a[k];
			}
			a[k + 1] = temp;
		}
	}
}

int main()
{
	int i;
	int a[] = { 1,1,1,1,1,1,1,1,1};
	int ilen = LENGTH(a);


	printf("before sort:");
	for (i = 0; i < ilen; i++)
		printf("%d ", a[i]);
	printf("\n");

	binaryInsertSort(a, ilen);

	printf("after  sort:");
	for (i = 0; i < ilen; i++)
		printf("%d ", a[i]);
	printf("\n");
}