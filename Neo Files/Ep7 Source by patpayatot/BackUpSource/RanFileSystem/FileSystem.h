// FileSystem.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// �� ��ȣ


// CFileSystemApp:
// �� Ŭ������ ������ ���ؼ��� FileSystem.cpp�� �����Ͻʽÿ�.
//

class CFileSystemApp : public CWinApp
{
public:
	CFileSystemApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CFileSystemApp theApp;
