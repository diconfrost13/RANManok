// NewFolderDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "FileSystem.h"
#include "NewFolderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CNewFolderDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CNewFolderDlg, CDialog)
CNewFolderDlg::CNewFolderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewFolderDlg::IDD, pParent)
	, m_strFolderName(_T(""))
{
}

CNewFolderDlg::~CNewFolderDlg()
{
}

void CNewFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FOLDERNAME, m_strFolderName);
}


BEGIN_MESSAGE_MAP(CNewFolderDlg, CDialog)
END_MESSAGE_MAP()


// CNewFolderDlg �޽��� ó�����Դϴ�.
