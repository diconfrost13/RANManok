// RanOnlienInstaller.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CRanOnlienInstallerApp:
// �� Ŭ������ ������ ���ؼ��� RanOnlienInstaller.cpp�� �����Ͻʽÿ�.
//

class CRanOnlienInstallerApp : public CWinApp
{
public:
	CRanOnlienInstallerApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CRanOnlienInstallerApp theApp;
