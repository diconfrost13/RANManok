// TexEncrypt.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CTexEncryptApp:
// �� Ŭ������ ������ ���ؼ��� TexEncrypt.cpp�� �����Ͻʽÿ�.
//

class CTexEncryptApp : public CWinApp
{
public:
	CTexEncryptApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CTexEncryptApp theApp;
