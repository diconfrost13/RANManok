#pragma once

// MinWebLauncher.h : MinWebLauncher.DLL�� �⺻ ��� �����Դϴ�.

#if !defined( __AFXCTL_H__ )
#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.

class CMinWebLauncherApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

// install ��ġ �Լ�
void DonwloadInst();