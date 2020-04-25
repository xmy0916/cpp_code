#include "pch.h"
#include <stdio.h>
#include <stdlib.h>
typedef struct DuLNode {
	int data;
	DuLNode *prior;
	DuLNode *next;
}DuLNode, *DuLinkList;

#define ERROR -1
#define OK 0 

int CreatList_DuL(DuLinkList &L);
int Length_DuL(DuLinkList L);
DuLinkList GetElem_DuL(DuLinkList L, int i);
int ListInset_DuL(DuLinkList &L, int i, int num);
void printfDuLinkList(DuLinkList L);
int ListDelete_DuL(DuLinkList &L, int i);

int main()
{
	DuLinkList head = NULL;
	if (CreatList_DuL(head) != ERROR)
	{
		printf("创建链表成功\n");
	}
	else
	{
		printf("创建链表失败\n");
		return -1;
	}
	printf("链表长:%d\r\n", Length_DuL(head));
	ListInset_DuL(head, 0, 5);
	ListInset_DuL(head, 0, 4);
	ListInset_DuL(head, 0, 3);
	ListInset_DuL(head, 0, 2);
	ListInset_DuL(head, 0, 1);
	printf("链表长:%d\r\n", Length_DuL(head));
	printfDuLinkList(head);
	ListDelete_DuL(head, 2);
	printf("-------删除后-------\n");
	printfDuLinkList(head);
	return 0;
}

int CreatList_DuL(DuLinkList &L)
{
	//分配空间 
	L = (DuLinkList)malloc(sizeof(DuLNode));
	//判断是否创建成功 
	if (!L) return ERROR;
	L->next = L->prior = NULL;
	return OK;
}

int ListInset_DuL(DuLinkList &L, int i, int num)
{
	//插入位置i的合法位置是[0,表长]
	DuLinkList p = NULL, s = NULL;
	if ((p = GetElem_DuL(L, i)) == NULL) return ERROR;
	if ((s = (DuLinkList)malloc(sizeof(DuLNode))) == NULL) return ERROR;

	s->data = num;
	if (p->next != NULL)
	{
		s->next = p->next;
		p->next->prior = s;
		p->next = s;
		s->prior = p;
	}else
	{
		s->next = p->next;
		p->next = s;
		s->prior = p;
	}
     
	return OK;
}

int Length_DuL(DuLinkList L)
{
	int p = 0;//计数器
	DuLinkList head = L->next;
	while (head != NULL)
	{
		p++;
		head = head->next;
	}
	free(head);
	return p;
}

DuLinkList GetElem_DuL(DuLinkList L, int i)
{
	//i的取值[0,表长-1] 
	DuLinkList temp = L;
	//判断位置是否合理
	if (i < 0 || i > Length_DuL(L))
		return NULL;
	while (i--)
	{
		temp = temp->next;
	}
	return temp;
}

void printfDuLinkList(DuLinkList L)
{
	if (L == NULL)
		return;

	DuLinkList temp = L->next;
	int i = 0;
	while (temp)
	{
		printf("第%d个数据是：%d\r\n", i++, temp->data);
		temp = temp->next;
	}
}

int ListDelete_DuL(DuLinkList &L, int i)
{
	//删除位置i的合法位置是[0,表长]
	DuLinkList p = NULL,q = NULL;
	if ((p = GetElem_DuL(L, i)) == NULL) return ERROR;

	q = p->next;
	if (q->next == NULL)
	{
		p->next = q->next;
		free(q);
	}else
	{
		p->next = q->next;
		p->next->prior = p;
		free(q);
	}
	return OK;
}
