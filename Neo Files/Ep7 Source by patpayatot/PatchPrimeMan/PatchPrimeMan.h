// PatchPrimeMan.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// �� ��ȣ


// CPatchPrimeManApp:
// �� Ŭ������ ������ ���ؼ��� PatchPrimeMan.cpp�� �����Ͻʽÿ�.
//

class CPatchPrimeManApp : public CWinApp
{
public:
	CPatchPrimeManApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CPatchPrimeManApp theApp;
