// VersionManager.h : VersionManager ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"       // �� ��ȣ


// CVersionManagerApp:
// �� Ŭ������ ������ ���ؼ��� VersionManager.cpp�� �����Ͻʽÿ�.
//

class CVersionManagerApp : public CWinApp
{
public:
	CVersionManagerApp();


// ������
public:
	virtual BOOL InitInstance();

// ����
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL InitApplication();
};

extern CVersionManagerApp theApp;
