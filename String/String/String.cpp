#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#define ERROR -1
#define OK 0
//串的堆分配存储表示结构体
typedef struct HString {
	char *ch;//ch指向字符串的首地址
	int length;//字符串的长度
}HString;

//初始化字符串
int InitString(HString &s)
{
	s.ch = NULL;
	s.length = 0;
	return OK;
}
//用char数组来初始化串T
int StrAssign(HString &T, char *chars)
{ 
	//用char数组生成HString对象
	//清空T
	if (T.ch) free(T.ch);
	//求chars的长度
	int length = strlen(chars);
	//判断传进来的数组是否是空的
	if (!length) { T.ch = NULL; T.length = 0; }
	else 
	{
		//不是空的则为T开辟空间并将数组值拷贝到T中
		if(!(T.ch = (char *)malloc(length * sizeof(char))))
			return ERROR;
		for (int i = 0; i < length; i++)
		{
			T.ch[i] = chars[i];
		}
		T.length = length;
	}
	return OK;
}

int StrLength(HString S)
{
	//返回S的元素个数，称为串长
	return S.length;
}

int StrCompare(HString S, HString T) 
{
	//比较S，T的元素大小差
	//若S，T的元素都相同，就返回S，T的长度差
	//比如S = "abcde",T = "abcdefgh"则返回3
	//若元素不同就返回S，T那个序列更大
	//比如S = "abdfg",T = "abcfg",显然S > T所以返回大于零
	for (int i = 0; i < S.length && i < T.length; i++)
	{
		if (S.ch[i] != T.ch[i]) return S.ch[i] - T.ch[i];
	}
	return S.length - T.length;
}

int ClearString(HString &S)
{
	//将S清空
	if (S.ch) { 
		free(S.ch); 
		S.ch = NULL; 
	}
	S.length = 0;
	return OK;
}

int Concat(HString &T, HString S1, HString S2)
{
	//用T返回S1+S2的新串
	//比如S1 = "abc",S2 = "def",则 T = "abcdef"
	//清空目标串T
	if (T.ch) free(T.ch);
	//给目标串T分配S1加S2的长度空间
	if (!(T.ch = (char *)malloc((S1.length + S2.length) * sizeof(char)))) return ERROR;
	T.length = S1.length + S2.length;
	//把S1拷贝到T中
	for (int i = 0; i < S1.length; i++)
	{
		T.ch[i] = S1.ch[i];
	}
	//把S2拷贝到T中
	for (int i = S1.length; i < T.length; i++)
	{
		T.ch[i] = S2.ch[i - S1.length];
	}
	return OK;
}
int SubString(HString &Sub, HString S, int pos, int len)
{
	//S = "abcdefg"
	//pos = 1
	//len = 4
	//用Sub返回串S第POS个字符起长度为len的子串
	//判断子串位置是否合理
	if (pos<1 || pos>S.length || len < 0 || len > S.length - pos + 1)
	{
		return ERROR;
	}
	if (Sub.ch) free(Sub.ch);
	if (!len) { Sub.ch = NULL; Sub.length = 0; }
	else
	{
		Sub.ch = (char *)malloc(len * sizeof(char));
		for (int i = 0; i < len; i++)
		{
			Sub.ch[i] = S.ch[i + pos - 1];
		}
		Sub.length = len;
	}
	return OK;
}

int charAt(HString S, int pos,char &res)
{
	if (pos<1 || pos>S.length)
	{
		return ERROR;
	}
	res = S.ch[pos - 1];
}

void printfHString(HString S)
{
	printf("----------开始打印字符串----------------\n");
	for (int i = 0;i < S.length; i++)
	{
		printf("%c", *S.ch++);
	}
	printf("\n----------结束打印字符串----------------\n");
}

int main()
{
	char ch[5] = { 'a','b','c','d' };
	char ch2[5] = { 'a','b','c'};
	HString s;
	printf("char大小：%d\n", sizeof(char));
	printf("int大小：%d\n", sizeof(int));
	printf("float大小：%d\n", sizeof(float));
	printf("double大小：%d\n", sizeof(double));

	InitString(s);
	StrAssign(s, &ch[0]);
	printf("s的长度是：%d\n", StrLength(s));
	printfHString(s);
	HString S,T;
	InitString(S);
	InitString(T);
	StrAssign(S, &ch[0]);
	StrAssign(T, &ch2[0]);
	/*printf("S比T大:%d\n", StrCompare(S, T));
	ClearString(S);*/
	Concat(s, S, T);
	printfHString(s);
	SubString(S, s, 1, 4);
	printfHString(S);
	char temp = '\0';
	charAt(S, 5, temp);
	printf("字符是:%c\n", temp);
	return 0;
}