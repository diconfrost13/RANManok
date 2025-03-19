// VerManDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "VerMan.h"
#include "VerManDlg.h"
#include ".\vermandlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CVerManDlg ��ȭ ����



CVerManDlg::CVerManDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVerManDlg::IDD, pParent)
	, m_valPatch(_T(""))
	, m_valClient(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVerManDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
	DDX_Text(pDX, IDC_EDIT_PATCH, m_valPatch);
	DDX_Text(pDX, IDC_EDIT_CLIENT, m_valClient);
}

BEGIN_MESSAGE_MAP(CVerManDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_DUMMY, OnBnClickedButtonDummy)
END_MESSAGE_MAP()


// CVerManDlg �޽��� ó����

BOOL CVerManDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	DWORD	PatchVer = 0;
	DWORD	ClientVer = 0;

	FILE* fp = fopen ( "cVer.bin", "rb" );
	if ( fp )
	{
		if ( 1 != fread ( &PatchVer, sizeof ( DWORD ), 1, fp ) )
		{
			return TRUE;
		}

		if ( 1 != fread ( &ClientVer, sizeof ( DWORD ), 1, fp ) )
		{
			return TRUE;
		}

		fclose ( fp );
	}

	m_valPatch.Format ( "%d", PatchVer );
	m_valClient.Format ( "%d", ClientVer );

	UpdateData ( FALSE );


	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

void CVerManDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CVerManDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CVerManDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVerManDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData ( TRUE );

	DWORD	PatchVer = atoi ( m_valPatch.GetString () );
	DWORD	ClientVer = atoi ( m_valClient.GetString () );

	FILE* fp = fopen ( "cVer.bin", "wb" );
	if ( fp )
	{
		fwrite ( &PatchVer, sizeof ( DWORD ), 1, fp );
		fwrite ( &ClientVer, sizeof ( DWORD ), 1, fp );

		fclose ( fp );
	}
	OnOK();
}

void CVerManDlg::OnBnClickedButtonDummy()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	FILE* fp = fopen ( "cFileList.bin", "wb" );
	if ( !fp )
	{
		return ;
	}
	
	//	NOTE
	//		ù��° ���� ���� [������ �ƹ��� �ϵ� ���� ����]
	SFILENODE FileInfo;
	FileInfo.Ver = 1;
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		return ;
	}

	//	NOTE
	//		�ι�° ���� ����
	FileInfo.Ver = 1;
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		return ;
	}

	strcpy ( FileInfo.FileName, "a.bat" );
	strcpy ( FileInfo.SubPath, "\\" );
	FileInfo.Ver = 1;
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		return ;
	}

	fclose ( fp );
}
