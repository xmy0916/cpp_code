#include "pch.h"
#include "Extern.h"

int externNumber = 1000;
static int staticNumber = 2000;


int add(int a, int b)
{
	return a + b;
}

void setStaticNumber(int num)
{
	if(num < 100&& num>0)
		staticNumber = num;
}

int getStaticNumber()
{
	return staticNumber;
}