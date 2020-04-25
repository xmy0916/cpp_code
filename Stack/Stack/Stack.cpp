#include "pch.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#define ERROR -1
#define OK 0
#define STACK_INIT_SIZE 100//存储空间初始分配空间
#define STACKINCREMENT 10//存储空间分配增量
#define zuo 1
#define you 2

typedef struct SqStack {
	int *base;
	int *top;
	int stackSize;
}SqStack;

int InitStack(SqStack &STK)
{
	STK.base = (int *)malloc(STACK_INIT_SIZE * sizeof(int));//基地址
	if (!STK.base)return ERROR;
	STK.top = STK.base;
	STK.stackSize = STACK_INIT_SIZE;
	return OK;
}

int GetTop(SqStack STK, int &num)
{
	if (STK.top == STK.base) return ERROR;
	num = *(STK.top - 1);
	return OK;
}

int GetBase(SqStack STK, int &num)
{
	if (STK.top == STK.base) return ERROR;
	num = *(STK.base);
	return OK;
}

int Pop(SqStack &STK, int &num)
{
	//判断栈是否为空
	if (STK.top == STK.base) return ERROR;
	num = *--STK.top;
	return OK;
}

int Push(SqStack &STK, int num)
{
	//判断是否栈满要加空间
	if (STK.top - STK.base >= STK.stackSize)
	{
		STK.base = (int *)realloc(STK.base, (STK.stackSize + STACKINCREMENT) * sizeof(int));
		if (!STK.base) return ERROR;

		STK.top = STK.base + STK.stackSize;
		STK.stackSize += STACKINCREMENT;
	}

	*STK.top = num;
	STK.top++;
	return OK;

}

int GetLength(SqStack STK)
{
	return STK.top - STK.base;
}

void printStack(SqStack STK)
{
	int *temp = STK.top - 1;
	printf("栈中数据是：");
	while (temp >= STK.base)
	{
		printf("%d,", *temp);
		temp--;
	}
	printf("\n");
}

int judgeBracket(SqStack &STK,int s[])
{

}

int main()
{
	/*SqStack sq;
	if (InitStack(sq) == ERROR)
	{
		printf("空间分配失败\n");
		return -1;
	}
	else
	{
		printf("栈初始化成功\n");
	}
	printStack(sq);
	Push(sq, 5);
	Push(sq, 4);
	Push(sq, 3);
	Push(sq, 2);
	printStack(sq);
	int num = 0;
	Pop(sq,num);
	printStack(sq);
	GetTop(sq, num);
	printf("top是：%d\n", num);
	GetBase(sq, num);
	printf("base是：%d\n", num);*/
	SqStack stack;
	int s[5] = { zuo,zuo,you,you,zuo };
	InitStack(stack);
	if (judgeBracket(stack, s) == ERROR)
	{
		printf("不是\n");
	}
	else
	{
		printf("是\n");
	}
	return 0;
}