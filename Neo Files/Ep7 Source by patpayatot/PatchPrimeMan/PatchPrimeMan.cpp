// PatchPrimeMan.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "PatchPrimeMan.h"
#include "PatchPrimeManDlg.h"
#include "RANPARAM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern int VERSION;
extern int SGAMEVER;

// CPatchPrimeManApp

BEGIN_MESSAGE_MAP(CPatchPrimeManApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPatchPrimeManApp ����

CPatchPrimeManApp::CPatchPrimeManApp()
{
}


// ������ CPatchPrimeManApp ��ü�Դϴ�.

CPatchPrimeManApp theApp;


// CPatchPrimeManApp �ʱ�ȭ

BOOL CPatchPrimeManApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControls()�� �ʿ��մϴ�. 
	// InitCommonControls()�� ������� ������ â�� ���� �� �����ϴ�.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();
	
	if( m_lpCmdLine != NULL )
	{
		TCHAR szCmdLine[MAX_PATH] = {0};

		StringCchCopy( szCmdLine, MAX_PATH, m_lpCmdLine );

		TCHAR *token(NULL);
		token = _tcstok( szCmdLine, _T(" ") );
		if( token == NULL )	return FALSE;
		VERSION = _ttoi( token );

		token = _tcstok( NULL, _T(" ") );
		if( token == NULL )	return FALSE;
		SGAMEVER = _ttoi( token );
	}
	else
	{
		//	�߸��� ����
		VERSION = -1; // ���� ���� ������ ���, ���Ŀ��� ���� �Ѿ���� ���� ���
	}

	CPatchPrimeManDlg dlg;
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