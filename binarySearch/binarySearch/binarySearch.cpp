#include "pch.h"
#include <iostream>


/*
int binarySearch(int[] nums, int target) {
	int left = 0, right = ...;

	while (...) {
		int mid = left + (right - left) / 2;
		if (nums[mid] == target) {
			...
		}
		else if (nums[mid] < target) {
			left = ...
		}
		else if (nums[mid] > target) {
			right = ...
		}
	}
	return ...;
}
*/

//注意：
//Tips1:分析二分查找的一个技巧是：不要出现 else，而是把所有情况用 else if 写清楚，这样可以清楚地展现所有细节
//Tips2:计算 mid 时需要防止溢出，代码中 left + (right - left) / 2 就和 (left + right) / 2 的结果相同，但是有效防止了 left 和 right 太大直接相加导致溢出。




//基本二分查找，搜索一个数，如果存在，返回其索引，否则返回 -1。
int binarySearch(int nums[], int target, int length)
{
	int left = 0;
	int right = length - 1; // 注意

	while (left <= right) { //注意
		int mid = left + (right - left) / 2;//注意
		if (nums[mid] == target)
			return mid;
		else if (nums[mid] < target)
			left = mid + 1; // 注意
		else if (nums[mid] > target)
			right = mid - 1; // 注意
	}
	return -1;
}

//寻找左侧边界的二分查找
//{1，2，3，4，5，5，5}
int left_bound(int nums[], int target, int length) {
	int left = 0, right = length - 1;
	// 搜索区间为 [left, right]
	while (left <= right) {
		int mid = left + (right - left) / 2;
		if (nums[mid] < target) {
			// 搜索区间变为 [mid+1, right]
			left = mid + 1;
		}
		else if (nums[mid] > target) {
			// 搜索区间变为 [left, mid-1]
			right = mid - 1;
		}
		else if (nums[mid] == target) {
			// 收缩右侧边界
			right = mid - 1;
		}
	}
	// 检查出界情况
	if (left >= length || nums[left] != target)
		return -1;
	return left;
}

//寻找右侧边界的二分查找
int right_bound(int nums[], int target, int length) {
	return 0;
}

int main()
{
	int nums[] = {6,6,6,6,6,6};
	printf("普通二分查找，找6在:%d\n", binarySearch(nums, 6, sizeof(nums) / sizeof(nums[0])));
	printf("左边界二分查找，找6在:%d\n", left_bound(nums, 6, sizeof(nums) / sizeof(nums[0])));
}