#include "StdAfx.h"
#include ".\mintimer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

minTimer::minTimer(void)
{
}

minTimer::~minTimer(void)
{	
}

/** 
* rdtsc �� ��Ƽ��/AMD �� ����
* �ٸ� cpu ���� �������� �ʾ� ����
* CPU Ŭ���� ���ϸ� ���� ���ؼ� ����� ����
*/
/*
DWORD minTimer::getRDTSCount()
{
	DWORD dwHi, dwLow;

	__asm
	{
		rdtsc              ; Ŭ��ī��Ʈ�� edx:eax�������Ϳ� ����
		mov	dwHi, edx      ; ���� dword �޸𸮿� ���
		mov dwLow, eax     ; ���� dword �޸𸮿� ���
	}
	return MAKEULONG(dwLow, dwHi);
}
*/