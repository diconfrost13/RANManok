// SoundSourceManager.h : SoundSourceManager ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // �� ��ȣ


// CSoundSourceManagerApp:
// �� Ŭ������ ������ ���ؼ��� SoundSourceManager.cpp�� �����Ͻʽÿ�.
//

class CSoundSourceManagerApp : public CWinApp
{
public:
	CSoundSourceManagerApp();

public:
	CString m_strAppPath;

// ������
public:
	virtual BOOL InitInstance();

// ����
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CSoundSourceManagerApp theApp;
