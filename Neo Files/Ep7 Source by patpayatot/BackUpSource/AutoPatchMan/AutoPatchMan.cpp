// AutoPatchMan.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "AutoPatchMan.h"
#include "AutoPatchManDlg.h"
#include "Custsite.h"
#include "DaumGameParameter.h"
#include "RANPARAM.h"
#include "LauncherText.h"

#include "GlobalVariable.h"
#include "../MfcExLib/MinBugTrap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAutoPatchManApp

BEGIN_MESSAGE_MAP(CAutoPatchManApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAutoPatchManApp ����

CAutoPatchManApp::CAutoPatchManApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
	m_pDispOM = NULL;

#ifdef _DEBUG
	m_memState = NULL;
#endif

	BUG_TRAP::BugTrapInstall( std::string(_T("Launcher")));
}


CAutoPatchManApp::~CAutoPatchManApp()
{
	if ( m_pDispOM != NULL)
	{
		//This is the way that the driller sample disposes 
		//the m_pDispOM object.
		//Note that the CImpIDispatch::Release() method 
		//is (intentionally?) never disposing its object! 
		delete m_pDispOM;
	}

#ifdef _DEBUG
	if (m_memState != NULL)
	{
		m_memState->DumpAllObjectsSince();

		delete m_memState;
	}
#endif
}


// ������ CAutoPatchManApp ��ü�Դϴ�.

CAutoPatchManApp theApp;


// CAutoPatchManApp �ʱ�ȭ

BOOL CAutoPatchManApp::InitInstance()
{

	#ifdef KR_PARAM
	{
		//	Note : ���� ���� �Ķ��Ÿ Ȯ��.
		//
		CDaumGameParameter cDGP;
		char szParameter[MAX_DGPARAMETER_LENGTH];
		if ( cDGP.Get ( "Parameter", szParameter, MAX_DGPARAMETER_LENGTH ) == FALSE )
		{
			CString strMsg;
//			strMsg = ID2LAUNCHERTEXT( "IDS_MESSAGE",1 ); // ������ ���� 
			strMsg.LoadString(IDS_MESSAGE1);
			MessageBox ( NULL, strMsg.GetString() , "RAN-Online", MB_OK );
			return FALSE;
		}
		//	���� ���� �Ķ��Ÿ�� ������ �����ݴϴ�.
		//cDGP.Empty();
	}
	#endif	// KR_PARAM

	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControls()�� �ʿ��մϴ�. 
	// InitCommonControls()�� ������� ������ â�� ���� �� �����ϴ�.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Create an IDispatch class for extending the Dynamic HTML Object Model 
	m_pDispOM = new CImpIDispatch;

	// Set our control containment up but using our control container 
	// management class instead of MFC's default
	AfxEnableControlContainer(new CCustomOccManager);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

#ifdef _DEBUG
	m_memState = new CMemoryState;
	m_memState->Checkpoint();
#endif

	CAutoPatchManDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ ��ȭ ���ڰ� Ȯ���� ���� �������� ��� ó����
		// �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ ��ȭ ���ڰ� ��Ҹ� ���� �������� ��� ó����
		// �ڵ带 ��ġ�մϴ�.
	}

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�
	// ���� ���α׷��� ���� �� �ֵ��� FALSE�� ��ȯ�մϴ�.
	return FALSE;
}

BOOL CAutoPatchManApp::InitApplication()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if ( FindWindow ( NULL, NS_GLOBAL_VAR::strTitle.GetString() ) )
	{		
		return FALSE;
	}

	return CWinApp::InitApplication();
}
