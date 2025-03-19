// CWavFileIntegrity.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SoundSourceManager.h"
#include "WavFileIntegrity.h"
#include "MainFrm.h"
#include "SoundSourceManagerView.h"
#include "SoundSourceMan.h"
#include ".\wavfileintegrity.h"


// CWavFileIntegrity ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CWavFileIntegrity, CDialog)
CWavFileIntegrity::CWavFileIntegrity(CWnd* pParent /*=NULL*/)
	: CDialog(CWavFileIntegrity::IDD, pParent)
{
}

CWavFileIntegrity::~CWavFileIntegrity()
{
}

void CWavFileIntegrity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWavFileIntegrity, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BEGIN, OnBnClickedButtonBegin)
	ON_BN_CLICKED(IDC_BUTTON_END, OnBnClickedButtonEnd)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnBnClickedButtonFile)
END_MESSAGE_MAP()


// CWavFileIntegrity �޽��� ó�����Դϴ�.

void CWavFileIntegrity::OnBnClickedButtonBegin()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame	*pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CSoundSourceManagerView *pView = (CSoundSourceManagerView *) pMainFrame->GetActiveView();

	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST_LOG);

	CString strLog;
	CString strFileName;
	CString strSrcDir = pView->GetSSM()->GetDataDirectory ();

	while ( pListBox->GetCount () ) pListBox->DeleteString ( 0 );

	//	�۾� ���
	pListBox->AddString ( "---- �۾���� ----");
	pListBox->AddString ( strSrcDir );
	pListBox->AddString ( "------------------");

	pView->GetSSM()->GoHead ();
	while ( !pView->GetSSM()->IsEnd () )
	{
		SSoundSource* pRecord = pView->GetSSM()->GetRecord ();		
		if ( !pRecord )
		{			
			pListBox->AddString ( "���ڵ带 ã�� �� �����ϴ�." );
			return ;
		}

		strFileName = strSrcDir + pRecord->FileName;

		CFileFind filefind;
		if ( !filefind.FindFile ( strFileName ) )
		{
			strLog.Format ( "[N]%d - %s - %s", pRecord->ID, pRecord->FileName, pRecord->Comment );
			pListBox->AddString ( strLog );
		}

		pView->GetSSM()->GoNext ();
	}
}

void CWavFileIntegrity::OnBnClickedButtonEnd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnOK();
}

void CWavFileIntegrity::OnBnClickedButtonFile()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST_LOG);	

	FILE* fp = fopen ( "fileintegrity.txt", "wt" );
	if ( fp )
	{
		char szItem[256];
		const int nCount = pListBox->GetCount ();
		for ( int i = 0; i < nCount; i++ )
		{
			pListBox->GetText ( i, szItem );
			fprintf ( fp, "%s\n", szItem );
		}

		fclose ( fp );
	}
}
