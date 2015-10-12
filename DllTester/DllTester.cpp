// DllTester.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <windows.h>

int main()
{

	HMODULE dll = LoadLibrary(L"myDLL");
	if (NULL == dll)
		return 1;

	typedef void(*FUNC1)(void);

	FUNC1 func1 = (FUNC1)GetProcAddress(dll, "hello");
	func1();

	FreeLibrary(dll);
    return 0;
}

