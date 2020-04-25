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

#define zuo1 3
#define you1 4
#define zuo2 5
#define you2 6
#define zuo3 7
#define you3 8

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
	return num;
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
	num = *STK.top;
	*STK.top--;
	return num;
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
/*
//判断括号是否有效，有效的话返回1，无效返回0； {zuo1,zuo1,you1,you1};
int isValid(int *s)
{
	int i=0,num;
	while(s[i]){    //当字符串不为空时，进入循环
		if(s[i]){          //  当栈不空，进入循环
			if(s[i]==zuo1|| s[i]==zuo2|| s[i]==zuo3){
				Push(STK, s[i]);           //如果是三种左括号，压栈
			}
			else if(s[i]=you1){    //如果是第1种右括号
				if(GetTop(STK, num)==zuo1)
				{
					Pop(STK, num);    //当栈顶元素为第1种左括号，弹栈
				}
				else
				printf("不是\n");    //否则，则为无效括号
				return 0;
			}
			else if(s[i]=you2){   //如果是第2种右括号
				if(GetTop(STK, num)==zuo2)
				{
					Pop(STK, num);    //当栈顶元素为第2种左括号，弹栈
				}
				else
				printf("不是\n");     //否则，则为无效括号
				return 0;
			}
			else if(s[i]=you3){    //如果是第3种右括号
				if(GetTop(STK, num)==zuo3)
				{
					Pop(STK, num);    //当栈顶元素为第3种左括号，弹栈
				}
				else
				printf("不是\n");    //否则，则为无效括号
				return 0;
			}
			i++;
		}
		else{
			Push(STK,s[i]);		// 当栈为空，将元素压栈
			i++;
		}
	}
	if(STK.top == STK.base){
		printf("是\n");     //判断最后如果是空栈，则为有效括号，返回1
		return 1;
	}
	else{
		printf("不是\n");
		return 0;        //否则，为无效括浩，返回0
	}
}*/

int judgeBracket(SqStack &STK, int s[])
{
	int num;//定义插入位置的前一个字符为num;
	int i = 0;
	while (s[i]) {
		if (s[i] == zuo) {
			Push(STK, s[i]);  //如果插入的是左括号，压栈 
		}
		else if (s[i] == you) {  //如果插入的是右括号，进入if语句 
			if (GetTop(STK, num) != ERROR)//如果插入位置前有字符，进入if语句；否则返回error 
			{
				if (num != zuo)
					return ERROR;  //如果插入位置前字符不是左括号，返回error 
				else
					Pop(STK, num);//插入位置前字符是左括号，将 前字符 弹栈 
			}
			else
				return ERROR;
		}
		++i;//令i加1后，赋值给下一轮=-= 
	}
	if (!(STK.top == STK.base))
		return ERROR;   //如果栈不为空，返回error,即不是有效 
	else
		return OK;
}

int dc(int *arr, int length)
{
	SqStack STK;
	InitStack(STK);
	int flag = 0;
	if (length < 1)
	{
		printf("不是\n");
		return 0;        //若数组为空，返回 
	}
	if (length % 2 == 0) {
		//个数为偶数，遍历数组，将元素入栈 
		for (int i = 0; i < length / 2; i++)
		{
			Push(STK, arr[i]);  
		}
		flag = 1;
	}
	else {
		//个数为奇数，遍历数组，将元素入栈 
		for (int i = 0; i < (length - 1) / 2; i++)
		{
			Push(STK, arr[i]);  
		}
		flag = 2;
	}

	STK.top--;//指向首元素
	if (flag == 1)
	{   
		//处理偶数情况
		for (int j = 0; j < length / 2; j++)
		{
			if (*STK.top == arr[length / 2 + j])
			{
				STK.top--;
			}
			else
			{
				break;
			}
		}
	}
	else if (flag == 2)
	{
		//处理奇数情况
		for (int j = 0; j < (length - 1) / 2; j++)
		{
			if (*STK.top == arr[(length - 1) / 2 + j + 1])
			{
				STK.top--;
			}
			else
			{
				break;
			}
		}
	}

	//若栈为空，则为对称 
	if (STK.top == STK.base-1)
		return 1;        
	else
		return 0;
}




int main()
{
	int arr[5] = { 1,2,3,2,1 };
	int length = sizeof(arr) / sizeof(int);
	if (dc(arr, length) == 0)
	{
		printf("不");
	}
	else
	{
		printf("是");
	}

	/*int s[4] = {zuo1,zuo1,you1,you1};
	InitStack(STK);
	if(isValid(s) == OK)
		printf("ok");
	else
		printf("wrong");*/

		/*char temp[5] = {'a','b','c','d'};
		for(int i = 0; i < 5; i++)
		{
			printf("第%d个是：%c\n",i,temp[i]);
		}*/


		/*SqStack stack;
		int s[5] = { zuo,you,zuo,zuo,zuo };
		InitStack(stack);
		if (judgeBracket(stack, s) == ERROR)
		{
			printf("不是\n");
		}
		else
		{
			printf("是\n");
		}
		return 0;*/



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

}

/*//【】{}（）
bool is_Valid(char * s){
	//空字符串显然符合
	if(*s == 0) return true;

	int len = strlen(s);

	//奇数长度的字符串显然不符合
	if(len & 1) return false;

	char stack[len];
	int top = -1;
	for(int i=0; i<len; ++i){
		//如果是左括号们，欢迎入栈
		if(s[i] == '(' || s[i] == '[' || s[i] == '{')
			stack[++top] = s[i];
		//不是左括号们，如果栈空则无法配对，不符合
		else if(top == -1) return false;
		//不是左括号们，栈非空，当前和栈顶配对，符合
		else if(s[i] == stack[top]+1 || s[i] == stack[top]+2)  //ascii码的数值
			stack[top--] = 0;
		//不是左括号们，栈非空，当前和栈顶不配对，不符合
		else return false;
	}
	//最后栈为空则符合，不为空则不符合
	return top == -1;
}
*/
