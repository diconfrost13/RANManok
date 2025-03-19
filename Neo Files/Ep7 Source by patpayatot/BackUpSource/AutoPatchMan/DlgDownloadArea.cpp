// DlgDownloadArea.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "AutoPatchMan.h"
#include "DlgDownloadArea.h"
#include ".\dlgdownloadarea.h"
#include "LauncherText.h"


// CDlgDownloadArea ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgDownloadArea, CDialog)
CDlgDownloadArea::CDlgDownloadArea(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDownloadArea::IDD, pParent)
	, m_nDownloadArea(0)
{
}

CDlgDownloadArea::~CDlgDownloadArea()
{
}

void CDlgDownloadArea::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_AREA1, m_nDownloadArea);
}


BEGIN_MESSAGE_MAP(CDlgDownloadArea, CDialog)
END_MESSAGE_MAP()


// CDlgDownloadArea �޽��� ó�����Դϴ�.

BOOL CDlgDownloadArea::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	SetWindowText( ID2LAUNCHERTEXT("IDD_DIALOG_DOWNLOAD_AREA"));
	SetDlgItemText( IDC_AREA_STATIC, ID2LAUNCHERTEXT("IDC_AREA_STATIC") );
	SetDlgItemText( IDOK, ID2LAUNCHERTEXT("DOWNAREA_ID_OK") );
	
	SetDlgItemText( IDC_RADIO_AREA1, m_strAreaName[0] );
	SetDlgItemText( IDC_RADIO_AREA2, m_strAreaName[1] );
	SetDlgItemText( IDC_RADIO_AREA3, m_strAreaName[2] );
//	SetDlgItemText( IDC_RADIO_AREA4, m_strAreaName[3] );
//	SetDlgItemText( IDC_RADIO_AREA5, m_strAreaName[4] );
//	SetDlgItemText( IDC_RADIO_AREA6, m_strAreaName[5] ); // �߱����� �߰�

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
