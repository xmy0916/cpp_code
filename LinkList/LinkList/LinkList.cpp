// LinkList.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
//线性表的链式存储结构
typedef struct LNode {
	int data;
	LNode *next;
}LNode, *LinkList;//不是LNode的变量 


enum Flag
{
	NoneFlag, CreatFailFlag, DeleteWrongFlag, InsertWrongFlag, CreatRoomFailFlag,
	CompareSuccessFlag, noNextFlag, noPriorFlag, FindWrongFlag, EmptyFlag
};

Flag CreatLinkList(LinkList &L);
Flag ListInsert_L(LinkList &head, int i, int num);
Flag GetElem_L(LinkList head, int i, int &num);
void printLinkList(LinkList &head);
void mergeLinkList(LinkList list1, LinkList list2, LinkList &listSum);

int main()
{
	LinkList list1 = NULL,list2 = NULL,listSum = NULL;
	CreatLinkList(list1);
	CreatLinkList(list2);
	CreatLinkList(listSum);
	
	ListInsert_L(list1, 0, 9);
	ListInsert_L(list1, 0, 7);
	ListInsert_L(list1, 0, 5);
	ListInsert_L(list1, 0, 3);
	ListInsert_L(list1, 0, 1);

	ListInsert_L(list2, 0, 6);
	ListInsert_L(list2, 0, 4);
	ListInsert_L(list2, 0, 2);

	printLinkList(list1);
	printLinkList(list2);

	mergeLinkList(list1, list2, listSum);
	printLinkList(listSum);
	return 0;

}

Flag CreatLinkList(LinkList &L)
{
	L = (LinkList)malloc(sizeof(LNode));
	//判断空间是否分配成功 
	if (!L) return CreatFailFlag;
	L->next = NULL;
	return NoneFlag;
}
Flag ListInsert_L(LinkList &head, int i, int num)
{
	LinkList p = head;
	//先定位到要插入的位置 
	int j = 0;//计数器
	while (p&&j < i) { p = p->next; ++j; }
	//判断插入位置是否合法 
	if (!p || j > i)return InsertWrongFlag;
	//插入 
	LinkList s = (LinkList)malloc(sizeof(LNode));
	s->data = num;
	s->next = p->next;
	p->next = s;
	return NoneFlag;
}
Flag GetElem_L(LinkList head, int i, int &num)
{
	//当第i个元素存在时，将其幅值给num并返回ok否则wrong
	LinkList p = head->next;
	//定位到要获取值的位置 
	int j = 0;
	while (p&&j < i)
	{
		p = p->next;
		++j;
	}
	//判断位置是否合法 
	if (!p || j > i)return FindWrongFlag;
	num = p->data;
	return NoneFlag;
}


void printLinkList(LinkList &head)
{
	//遍历链表 
	int j = 0;
	LinkList p = head->next;
	while (p != NULL) { printf("第%d个数是：%d\n", j, p->data); p = p->next; ++j; }
	//while(head->next != NULL){printf("第%d个数是：%d\n",j,head->next->data);head->next = head->next->next;++j;}
	printf("--------------------------------\n");
}

void mergeLinkList(LinkList list1, LinkList list2, LinkList &listSum)
{
	LinkList l1 = list1->next;
	LinkList l2 = list2->next;
	LinkList now = listSum;

	while (l1 && l2)
	{
		if (l1->data < l2->data)
		{
			now->next = l1;
			now = now->next;
			l1 = l1->next;
		}
		else
		{
			now->next = l2;
			now = now->next;
			l2 = l2->next;
		}
	}

	if (l1)
	{
		now->next = l1;
	}

	if (l2)
	{
		now->next = l2;
	}
	
}

