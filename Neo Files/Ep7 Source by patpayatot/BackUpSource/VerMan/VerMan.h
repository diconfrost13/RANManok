// VerMan.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// �� ��ȣ


// CVerManApp:
// �� Ŭ������ ������ ���ؼ��� VerMan.cpp�� �����Ͻʽÿ�.
//

class CVerManApp : public CWinApp
{
public:
	CVerManApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CVerManApp theApp;
