// File2ListDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SoundSourceManager.h"
#include "File2ListDialog.h"
#include ".\file2listdialog.h"
#include "MainFrm.h"
#include "SoundSourceManagerView.h"
#include "SoundSourceMan.h"

// CFile2ListDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CFile2ListDialog, CDialog)
CFile2ListDialog::CFile2ListDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CFile2ListDialog::IDD, pParent)
{
}

CFile2ListDialog::~CFile2ListDialog()
{
}

void CFile2ListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFile2ListDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BEGIN, OnBnClickedButtonBegin)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnBnClickedButtonFile)
	ON_BN_CLICKED(IDC_BUTTON_END, OnBnClickedButtonEnd)
END_MESSAGE_MAP()


// CFile2ListDialog �޽��� ó�����Դϴ�.

void CFile2ListDialog::OnBnClickedButtonBegin()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame	*pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CSoundSourceManagerView *pView = (CSoundSourceManagerView *) pMainFrame->GetActiveView();	

	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST_LOG);

	while ( pListBox->GetCount () ) pListBox->DeleteString ( 0 );

	CString strLog;
	CString strFileFind = pView->GetSSM()->GetDataDirectory () + "*.wav";

	CFileFind filefind;
	if ( !filefind.FindFile ( strFileFind ) )
	{
		pListBox->AddString ( "������ �������� �ʽ��ϴ�." );
		return ;
	}

	while ( filefind.FindNextFile () )
	{
		CString strFileName = filefind.GetFileName ();

        bool bFOUND = false;

		//
		pView->GetSSM ()->GoHead ();
		while ( !pView->GetSSM ()->IsEnd () )
		{
			SSoundSource* pRecord = pView->GetSSM()->GetRecord ();		
			if ( !pRecord )
			{			
				pListBox->AddString ( "���ڵ带 ã�� �� �����ϴ�." );
				return ;
			}

			if ( strFileName == pRecord->FileName )
			{
				bFOUND = true;
				break;
			}

			pView->GetSSM ()->GoNext ();
		}

        if ( !bFOUND )
		{
			strLog.Format ( "[N]%s", strFileName );
			pListBox->AddString ( strLog );
		}
	}
}

void CFile2ListDialog::OnBnClickedButtonFile()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LIST_LOG);	

	FILE* fp = fopen ( "file2list.txt", "wt" );
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

void CFile2ListDialog::OnBnClickedButtonEnd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnOK();
}
