#include "stdafx.h"
#include <pdh.h>	// DWORD,PDH_FMT_LONG��
#include <pdhmsg.h>	// PDH_MORE_DATA
#pragma comment( lib, "pdh.lib" )


int
MakeCounterPath(LPTSTR	pszInstanceName,	// �C���X�^���X��
	LPTSTR	pszCounterName,		// �J�E���^�[��
	LPTSTR	pszCounterPath,		// �J�E���^�[�p�X�󂯎��o�b�t�@�[
	DWORD	dwCounterPathSize)	// �J�E���^�[�p�X�󂯎��o�b�t�@�[�̃T�C�Y�i�������j
{
	PDH_COUNTER_PATH_ELEMENTS cpe;
	ZeroMemory(&cpe, sizeof(PDH_COUNTER_PATH_ELEMENTS));

	cpe.szMachineName = NULL;
	cpe.szObjectName = L"Process";
	cpe.szInstanceName = pszInstanceName;
	cpe.szParentInstance = NULL;
	cpe.dwInstanceIndex = 0;
	cpe.szCounterName = pszCounterName;

	// �J�E���^�[�p�X�̍쐬
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
GetPdhCounterValue(LPTSTR	pszInstanceName,	// �C���X�^���X��
	LPTSTR	pszCounterName,		// �J�E���^�[��
	DWORD	dwFormat,			// �󂯎��l�̌^�̎��
	void*	pValue,				// �l�󂯎��o�b�t�@�[
	DWORD	dwValueSize,		// �l�󂯎��o�b�t�@�[�̃T�C�Y�ibyte�j
	DWORD	dwSleepMilliSecond = 0)	// [in]�l�󂯎�莞�ɃX���[�v�����邩
{
	// ���̓`�F�b�N
	if (NULL == pszInstanceName
		|| NULL == pszCounterName
		|| NULL == pValue
		|| 0 == dwValueSize)
	{
		//assert( !"���̓G���[" );
		return false;
	}

	bool		bResult = false;		// ����

	HQUERY		hQuery = NULL;		// �v���n���h��
	HCOUNTER	hCounter = NULL;	// �J�E���^�[�n���h��

									// �J�E���^�[�p�X�̍쐬
	TCHAR szCounterPath[1024];
	if (1 != MakeCounterPath(pszInstanceName, pszCounterName, szCounterPath, 1024))
	{
		goto LABEL_END;
	}

	// �v���n���h���̍쐬
	if (ERROR_SUCCESS != PdhOpenQuery(NULL,
		0,
		&hQuery))	// �v���n���h��
	{
		goto LABEL_END;
	}

	// �쐬�����J�E���^�[�p�X��v���n���h���ɓo�^�B�J�E���^�[�n���h���𓾂Ă����B
	if (ERROR_SUCCESS != PdhAddCounter(hQuery,			// �v���n���h��
		szCounterPath,
		0,
		&hCounter))	// �J�E���^�[�n���h��
	{
		goto LABEL_END;
	}

	// �v���f�[�^�̎擾
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
	{	// ���s
		//assert( !"���s" );
		return false;
	}

	// ����
	switch (dwFormat)
	{
	case PDH_FMT_LONG:
		if (dwValueSize != sizeof(LONG))
		{
			//assert( !"�󂯎��l�̌^�̎�ނɑ΂��Ēl�󂯎��o�b�t�@�[�̃T�C�Y���s��" );
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
			//assert( !"�󂯎��l�̌^�̎�ނɑ΂��Ēl�󂯎��o�b�t�@�[�̃T�C�Y���s��" );
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
		//assert( !"���Ή�" );
		return false;
	}
	return true;
}

bool convCharToTChar(const char* input, TCHAR* output)
{
	//TCHAR�^�ɕϊ�����ׂ̃o�b�t�@�T�C�Y���擾
	int size = MultiByteToWideChar(CP_ACP, 0, input, -1, NULL, 0);
	if (size >= wcslen(output)) {    //�T�C�Y���傫��
		return false;       //�I��
	}

	//char�^��TCHAR�ɕϊ�
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
		//assert( !"�G���[" );
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
		//assert( !"�G���[" );
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
		//assert( !"�G���[" );
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
		//assert( !"�G���[" );
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
		//assert( !"�G���[" );
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
		//assert( !"�G���[" );
		return -1;
	}
	return value;
}

