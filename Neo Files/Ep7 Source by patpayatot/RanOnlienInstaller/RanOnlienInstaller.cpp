// RanOnlienInstaller.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "RanOnlienInstaller.h"
#include "RanOnlienInstallerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRanOnlienInstallerApp

BEGIN_MESSAGE_MAP(CRanOnlienInstallerApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CRanOnlienInstallerApp ����

CRanOnlienInstallerApp::CRanOnlienInstallerApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}


// ������ CRanOnlienInstallerApp ��ü�Դϴ�.

CRanOnlienInstallerApp theApp;


// CRanOnlienInstallerApp �ʱ�ȭ

BOOL CRanOnlienInstallerApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControls()�� �ʿ��մϴ�. 
	// InitCommonControls()�� ������� ������ â�� ���� �� �����ϴ�.
	InitCommonControls();

	CWinApp::InitInstance();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	CRanOnlienInstallerDlg dlg;
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
