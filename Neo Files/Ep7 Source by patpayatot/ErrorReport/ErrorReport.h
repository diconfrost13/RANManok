// ErrorReport.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CErrorReportApp:
// �� Ŭ������ ������ ���ؼ��� ErrorReport.cpp�� �����Ͻʽÿ�.
//

class CErrorReportApp : public CWinApp
{
public:
	CErrorReportApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CErrorReportApp theApp;
