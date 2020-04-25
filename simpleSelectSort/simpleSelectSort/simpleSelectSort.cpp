// simpleSelectSort.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
void simpleSelectionSort(int list[], int len);
int main()
{
	int arr[5] = { 1,6,5,3,4 };
	simpleSelectionSort(arr, 5);
	for (int i = 0; i < 5; i++)
	{
		std::cout << arr[i] << std::endl;
	}
}

void simpleSelectionSort(int list[], int len) 
{

	// 需要遍历获得最小值的次数

	// 要注意一点，当要排序 N 个数，已经经过 N-1 次遍历后，已经是有序数列

	for (int i = 0; i < len - 1; i++) {

		int temp = 0;

		int index = i; // 用来保存最小值得索引



		// 寻找第i个小的数值

		for (int j = i + 1; j < len; j++) {//N - 1,N - 2...0

			if (list[index] > list[j]) {

				index = j;

			}

		}



		// 将找到的第i个小的数值放在第i个位置上
		temp = list[index];

		list[index] = list[i];

		list[i] = temp;
	}

}
