// myDLL.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include <iostream>

using namespace std;

void WINAPI hello()
{
	cout << "Hello" << endl;
}

void WINAPI echo(char* str)
{
	cout << "Hello " << str << endl;
}

int WINAPI addInt(int d1, int d2)
{
	return d1 + d2;
}

char* WINAPI addStr(char* s1, char s2)
{
	return addStr(s1, s2);
}