// DlgWarningWhql.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "AutoPatchMan.h"
#include "DlgWarningWhql.h"
#include ".\dlgwarningwhql.h"
#include "LauncherText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDlgWarningWhql ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgWarningWhql, CDialog)
CDlgWarningWhql::CDlgWarningWhql(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWarningWhql::IDD, pParent),
	m_bCheckWhql(TRUE)
{
}

CDlgWarningWhql::~CDlgWarningWhql()
{
}

void CDlgWarningWhql::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_NVIDIA_LINK, m_ctrlNVidiaLink);
	DDX_Control(pDX, IDC_STATIC_ATI_LINK, m_ctrlATILink);
}


BEGIN_MESSAGE_MAP(CDlgWarningWhql, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgWarningWhql �޽��� ó�����Դϴ�.

BOOL CDlgWarningWhql::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlNVidiaLink.SetHyperlink("http://www.nvidia.com/content/drivers/drivers.asp");
	m_ctrlATILink.SetHyperlink("http://mirror.ati.com/support/driver.html");

	SetWindowText( ID2LAUNCHERTEXT("IDD_DIALOG_WARNING_WHQL"));
	SetDlgItemText( IDC_VIDEO_STATIC, ID2LAUNCHERTEXT("IDC_VIDEO_STATIC") );
	SetDlgItemText( IDC_NVIDIA_STATIC, ID2LAUNCHERTEXT("IDC_NVIDIA_STATIC") );
	SetDlgItemText( IDC_STATIC_NVIDIA_LINK, ID2LAUNCHERTEXT("IDC_STATIC_NVIDIA_LINK") );	
	SetDlgItemText( IDC_ATI_STATIC, ID2LAUNCHERTEXT("IDC_ATI_STATIC") );
	SetDlgItemText( IDC_STATIC_ATI_LINK, ID2LAUNCHERTEXT("IDC_STATIC_ATI_LINK") );
	SetDlgItemText( IDC_CHECK_NOCHECK, ID2LAUNCHERTEXT("IDC_CHECK_NOCHECK") );
	SetDlgItemText( IDOK, ID2LAUNCHERTEXT("WARNING_ID_OK") );


	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgWarningWhql::OnBnClickedOk()
{
	CButton* pButton = (CButton*) GetDlgItem ( IDC_CHECK_NOCHECK );
	m_bCheckWhql = pButton->GetCheck()==FALSE;

	OnOK();
}
