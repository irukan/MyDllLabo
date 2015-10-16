#include "stdafx.h"
#include <pdh.h>	// DWORD,PDH_FMT_LONG他
#include <pdhmsg.h>	// PDH_MORE_DATA
#pragma comment( lib, "pdh.lib" )


int
MakeCounterPath(LPTSTR	pszInstanceName,	// インスタンス名
	LPTSTR	pszCounterName,		// カウンター名
	LPTSTR	pszCounterPath,		// カウンターパス受け取りバッファー
	DWORD	dwCounterPathSize)	// カウンターパス受け取りバッファーのサイズ（文字数）
{
	PDH_COUNTER_PATH_ELEMENTS cpe;
	ZeroMemory(&cpe, sizeof(PDH_COUNTER_PATH_ELEMENTS));

	cpe.szMachineName = NULL;
	cpe.szObjectName = L"Process";
	cpe.szInstanceName = pszInstanceName;
	cpe.szParentInstance = NULL;
	cpe.dwInstanceIndex = 0;
	cpe.szCounterName = pszCounterName;

	// カウンターパスの作成
	if (ERROR_SUCCESS != PdhMakeCounterPath(&cpe,
		pszCounterPath,
		&dwCounterPathSize,
		0))
	{
		return -1;
	}

	return 1;
}

bool
GetPdhCounterValue(LPTSTR	pszInstanceName,	// インスタンス名
	LPTSTR	pszCounterName,		// カウンター名
	DWORD	dwFormat,			// 受け取る値の型の種類
	void*	pValue,				// 値受け取りバッファー
	DWORD	dwValueSize,		// 値受け取りバッファーのサイズ（byte）
	DWORD	dwSleepMilliSecond = 0)	// [in]値受け取り時にスリープをするか
{
	// 入力チェック
	if (NULL == pszInstanceName
		|| NULL == pszCounterName
		|| NULL == pValue
		|| 0 == dwValueSize)
	{
		//assert( !"入力エラー" );
		return false;
	}

	bool		bResult = false;		// 結果

	HQUERY		hQuery = NULL;		// 要求ハンドル
	HCOUNTER	hCounter = NULL;	// カウンターハンドル

									// カウンターパスの作成
	TCHAR szCounterPath[1024];
	if (1 != MakeCounterPath(pszInstanceName, pszCounterName, szCounterPath, 1024))
	{
		goto LABEL_END;
	}

	// 要求ハンドルの作成
	if (ERROR_SUCCESS != PdhOpenQuery(NULL,
		0,
		&hQuery))	// 要求ハンドル
	{
		goto LABEL_END;
	}

	// 作成したカウンターパスを要求ハンドルに登録。カウンターハンドルを得ておく。
	if (ERROR_SUCCESS != PdhAddCounter(hQuery,			// 要求ハンドル
		szCounterPath,
		0,
		&hCounter))	// カウンターハンドル
	{
		goto LABEL_END;
	}

	// 要求データの取得
	if (ERROR_SUCCESS != PdhCollectQueryData(hQuery))
	{
		goto LABEL_END;
	}
	if (0 < dwSleepMilliSecond)
	{
		Sleep(dwSleepMilliSecond);
		if (ERROR_SUCCESS != PdhCollectQueryData(hQuery))
		{
			goto LABEL_END;
		}
	}

LABEL_END:
	PDH_FMT_COUNTERVALUE	fmtValue;

	PdhGetFormattedCounterValue(hCounter, dwFormat, NULL, &fmtValue);

	bResult = true;

	PdhRemoveCounter(hCounter);
	PdhCloseQuery(hQuery);
	if (false == bResult)
	{	// 失敗
		//assert( !"失敗" );
		return false;
	}

	// 成功
	switch (dwFormat)
	{
	case PDH_FMT_LONG:
		if (dwValueSize != sizeof(LONG))
		{
			//assert( !"受け取る値の型の種類に対して値受け取りバッファーのサイズが不正" );
			return false;
		}
		else
		{
			LONG* plValue = (LONG*)pValue;
			*plValue = fmtValue.longValue;
		}
		break;
	case PDH_FMT_DOUBLE:
		if (dwValueSize != sizeof(double))
		{
			//assert( !"受け取る値の型の種類に対して値受け取りバッファーのサイズが不正" );
			return false;
		}
		else
		{
			double* pdValue = (double*)pValue;
			*pdValue = fmtValue.doubleValue;

		}
		break;
	case PDH_FMT_ANSI:
	case PDH_FMT_UNICODE:
	case PDH_FMT_LARGE:
	default:
		//assert( !"未対応" );
		return false;
	}
	return true;
}

bool convCharToTChar(const char* input, TCHAR* output)
{
	//TCHAR型に変換する為のバッファサイズを取得
	int size = MultiByteToWideChar(CP_ACP, 0, input, -1, NULL, 0);
	if (size >= wcslen(output)) {    //サイズが大きい
		return false;       //終了
	}

	//char型をTCHARに変換
	MultiByteToWideChar(CP_ACP, 0, input, -1, output, size);
	return true;
}

long getVirtualBytes(char* name)
{
	LONG value;

	TCHAR wName[255];
	convCharToTChar(name, wName);

	if (!GetPdhCounterValue(wName, L"Virtual Bytes", PDH_FMT_LONG, &value, sizeof(value)))
	{
		//assert( !"エラー" );
		return -1;
	}
	return value;
}

long getVirtualBytesPeak(char* name)
{
	LONG value;

	TCHAR wName[255];
	convCharToTChar(name, wName);

	if (!GetPdhCounterValue(wName, L"Virtual Bytes Peak", PDH_FMT_LONG, &value, sizeof(value)))
	{
		//assert( !"エラー" );
		return -1;
	}
	return value;
}

long getPrivateBytes(char* name)
{
	LONG value;

	TCHAR wName[255];
	convCharToTChar(name, wName);

	if (!GetPdhCounterValue(wName, L"Private Bytes", PDH_FMT_LONG, &value, sizeof(value)))
	{
		//assert( !"エラー" );
		return -1;
	}
	return value;
}

long getPrivateBytesPeak(char* name)
{
	LONG value;

	TCHAR wName[255];
	convCharToTChar(name, wName);

	if (!GetPdhCounterValue(wName, L"Private Bytes Peak", PDH_FMT_LONG, &value, sizeof(value)))
	{
		//assert( !"エラー" );
		return -1;
	}
	return value;
}

long getWorkingSet(char* name)
{
	LONG value;

	TCHAR wName[255];
	convCharToTChar(name, wName);

	if (!GetPdhCounterValue(wName, L"Working Set", PDH_FMT_LONG, &value, sizeof(value)))
	{
		//assert( !"エラー" );
		return -1;
	}
	return value;
}

long getWorkingSetPeak(char* name)
{
	LONG value;

	TCHAR wName[255];
	convCharToTChar(name, wName);

	if (!GetPdhCounterValue(wName, L"Working Set Peak", PDH_FMT_LONG, &value, sizeof(value)))
	{
		//assert( !"エラー" );
		return -1;
	}
	return value;
}

