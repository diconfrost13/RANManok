// AutoPatchMan.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// �� ��ȣ


// CAutoPatchManApp:
// �� Ŭ������ ������ ���ؼ��� AutoPatchMan.cpp�� �����Ͻʽÿ�.
//

class CAutoPatchManApp : public CWinApp
{
public:
	CAutoPatchManApp();
	virtual ~CAutoPatchManApp();

// ������
	public:
	virtual BOOL InitInstance();

public:
	class CImpIDispatch* m_pDispOM;

protected:

#ifdef _DEBUG
	CMemoryState	*m_memState;
#endif

// ����

	DECLARE_MESSAGE_MAP()
	virtual BOOL InitApplication();
};

extern CAutoPatchManApp theApp;
