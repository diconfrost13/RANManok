#include "stdafx.h"
#include "VersionManager.h"
#include "DlgNewFolder.h"
#include ".\dlgnewfolder.h"

// CDlgNewFolder ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgNewFolder, CDialog)
CDlgNewFolder::CDlgNewFolder(CString strTemp, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewFolder::IDD, pParent)
{
	m_strFolder = strTemp;
}

CDlgNewFolder::~CDlgNewFolder()
{
}

void CDlgNewFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditFolder);
}


BEGIN_MESSAGE_MAP(CDlgNewFolder, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgNewFolder �޽��� ó�����Դϴ�.

BOOL CDlgNewFolder::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_EditFolder.SetWindowText(m_strFolder);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgNewFolder::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_EditFolder.GetWindowText(m_strFolder);
	OnOK();
}
