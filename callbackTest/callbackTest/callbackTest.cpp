#include "pch.h"
#include <iostream>
using namespace std;
class TestCallback {
public:
	int num = 5;
	static int num2;
	void call(void *p,void (TestCallback::*callback)(void));
	void callBack5();
};
int TestCallback::num2 = 6;//静态变量必须初始化


void TestCallback::call(void *p,void (TestCallback::*callback)(void))
{
	cout << "------调用回调------" << endl;
	TestCallback *temp = (TestCallback *)p;
	(temp->*callback)();
	cout << "------调用结束" << endl;
	cout << endl;
}


void TestCallback::callBack5()
{
	cout << "------start func5------" << endl;
	cout << "func5的num = " << this->num << endl;
	cout << "func5的num2 = " << this->num2 << endl;
	cout << "------end func5------" << endl;
}

int main()
{
	TestCallback t;
	t.call(&t,&TestCallback::callBack5);
}
