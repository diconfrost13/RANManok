// PatchPrimeManDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "PatchPrimeMan.h"
#include "PatchPrimeManDlg.h"

#include "RANPARAM.h"
#include "s_CHttpPatch.h"
#include "LogControl.h"
#include "CompactFdi.h"
#include "GlobalVariable.h"
#include "StringUtils.h"

#include "LPatchThread.h"
#include "HttpPatchThread.h"
#include "../Common/SUBPATH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////////
//	���� ������ �׽�Ʈ
//#define	__ALPHA__
//
////////////////////////////////////////////////////////////////////////////////

char*	g_szAlphaServerIP = "mincontrol.mincoms.co.kr";
char*	g_szAlphaID = "ranalpha";
char*	g_szAlphaPW = "ranalpha";

//char*	g_szServerIP = "211.224.129.142";
char*	g_szBetaID = "ranbeta";
char*	g_szBetaPW = "tjdrlduq";


char	g_szAppPath[MAX_PATH] = {0};
char	g_szTitle[] = "Launcher Patch";
char	g_szAutoPatchTitle[] = "Ran Launcher";

int		VERSION = -1;
int		SGAMEVER = -1;

CPatchPrimeManDlg::CPatchPrimeManDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPatchPrimeManDlg::IDD, pParent),
	//m_pFtpPatch( NULL ),
	m_pHttpPatch( NULL ),
	m_bForceTerminate( FALSE ),	
	m_bUseHttp( false ),
	m_sPatchVer( 0 ),
	m_sGameVer( 0 ),
	m_pThread(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPatchPrimeManDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_ALL, m_ctrlProgressAll);
	DDX_Control(pDX, IDC_LIST_LOG, m_ctrlListBox);
}

BEGIN_MESSAGE_MAP(CPatchPrimeManDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnBnClickedButtonExit)
END_MESSAGE_MAP()


// CPatchPrimeManDlg �޽��� ó����

BOOL CPatchPrimeManDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	SetAppPath ();
	SetWindowText ( g_szTitle );

	RANPARAM::LOAD ( g_szAppPath );
	TCHAR szPROFILE[MAX_PATH] = {0};
	TCHAR szFullPath[MAX_PATH] = {0};
	SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

	StringCchCopy( szFullPath, MAX_PATH, szPROFILE );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::SAVE_ROOT );
	CreateDirectory( szFullPath, NULL );

	NS_GLOBAL_VAR::strProFile = szPROFILE;

	//	�⺻ ����Ÿ ����
	//m_pFtpPatch = new CPatch;
	m_pHttpPatch = new CHttpPatch;

	//	�������� ��� ����
	//m_pFtpPatch->SetForceTerminateToggle ( &m_bForceTerminate );
	m_pHttpPatch->SetForceTerminateToggle ( &m_bForceTerminate );
	SetCabForceTerminate ( &m_bForceTerminate );

	//	�����Ȳ ����
	ULONGLONG* pCurPos = NULL;
	ULONGLONG* pCurEnd = NULL;
	NS_LOG_CONTROL::GetProcessCurPositionAddr ( &pCurPos, &pCurEnd );	
	//m_pFtpPatch->SetProgressValue ( pCurPos, pCurEnd );
	m_pHttpPatch->SetProgressValue ( pCurPos, pCurEnd );
	SetCabProgressValue ( pCurPos, pCurEnd );

	if ( VERSION < 0 ) // -1
	{
		m_bForceTerminate = TRUE; // ���������� ���� �ִ�.
		SendMessage ( WM_CLOSE );
		return TRUE;
	}
	m_sPatchVer = VERSION;
	m_sGameVer = SGAMEVER;
	
	ListAddString( IDS_MESSAGE_001 ); // ���� ���� ���
	WaitForLauncherClose();

	ListAddString( IDS_MESSAGE_002 ); // ��ġ�� ����
	m_bUseHttp = true; // ������ HTTP ���
	if ( m_bUseHttp )
	{
		BEGIN_PATCH ();
	}
	//else
	//{
	//	m_nFtpTry = 0;
	//	BEGIN_FTP_THREAD ();
	//}
	
	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CPatchPrimeManDlg::OnPaint() 
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

void	CPatchPrimeManDlg::DeleteInvalidFile ()
{
	CString strDirectory;
	CString strDirectoryFile;
	strDirectory.Format ( "%sData\\GLogicServer", g_szAppPath );
	strDirectoryFile = strDirectory + "\\*.*";

	CFileFind finder;
	BOOL bWorking = finder.FindFile ( strDirectoryFile.GetString () );

	CString strTemp;
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!
		if (finder.IsDots())
			continue;

        strTemp = finder.GetFilePath();
		DeleteFile ( strTemp.GetString() );
	}
	finder.Close ();

	RemoveDirectory ( strDirectory.GetString() );
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ���
// �ý��ۿ��� �� �Լ��� ȣ���մϴ�.
//
HCURSOR CPatchPrimeManDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//BOOL	CPatchPrimeManDlg::ConnectServer ( CString strFtpAddress )
//{
//	int nRetCode;	
//    
//#ifdef	__ALPHA__
//	nRetCode = m_pFtpPatch->Connect( strFtpAddress.GetString (),
//						21,
//						g_szAlphaID,
//						g_szAlphaPW,
//						RANPARAM::bUsePassiveDN );
//#else
//	nRetCode = m_pFtpPatch->Connect( strFtpAddress.GetString (),
//						21,
//						g_szBetaID,
//						g_szBetaPW,
//						RANPARAM::bUsePassiveDN );
//#endif
//    	
//	if (nRetCode == NET_ERROR)
//	{
//		return FALSE;
//	}
//
//	return TRUE;
//}

//BOOL	CPatchPrimeManDlg::DisconnectServer ()
//{
//	if ( m_pFtpPatch )
//	{
//		m_pFtpPatch->DisConnect();
//	}
//
//	return TRUE;
//}

void	CPatchPrimeManDlg::SetAppPath ()
{
	// Note : ���������� ��θ� ã�Ƽ� �����Ѵ�.
	//
	CString m_strAppPath;
	CString strAppPath;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strAppPath = szPath;

	if ( !strAppPath.IsEmpty() )
	{
		m_strAppPath = strAppPath.Left ( strAppPath.ReverseFind ( '\\' ) );
		
		if ( !m_strAppPath.IsEmpty() )
		if ( m_strAppPath.GetAt(0) == '"' )
			m_strAppPath = m_strAppPath.Right ( m_strAppPath.GetLength() - 1 );

        m_strAppPath += '\\';
		StringCchCopy ( g_szAppPath, MAX_PATH, m_strAppPath.GetString() );
	}
}

void	CPatchPrimeManDlg::WaitForLauncherClose()
{
	while ( TRUE )
	{
		HWND hWndPatchPrime = ::FindWindow ( NULL, g_szAutoPatchTitle );

		if ( hWndPatchPrime ) // ��ġ�� �ױ⸦ ��ٸ���.
		{
			Sleep( 1 ); // ���� ��� ���״´ٸ�...?
		}
		else
		{
			return;
		}
	}
}

BOOL CPatchPrimeManDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	switch ( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			switch ( pMsg->wParam )
			{
			case VK_RETURN:
			case VK_ESCAPE:
				return TRUE;
			}
		}
		break;

	case WM_SYSKEYDOWN:
		{
			if ( pMsg->wParam == VK_F4 )
			{
				OnBnClickedButtonExit ();
				return TRUE;
			}
		}
		break;

	case WM_LISTADDSTRING:
		{
			ListAddString( static_cast<UINT>(pMsg->wParam) );
			return TRUE;
		}
		break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CPatchPrimeManDlg::OnBnClickedButtonExit()
{
	m_bForceTerminate = TRUE; // ���� �ߴ�
	m_pThread->KillThread(); // ������ �ߴ�

	ListAddString( IDS_MESSAGE_003 ); // ���� ���̴�.	
	OnOK();
}

void CPatchPrimeManDlg::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if( !m_bForceTerminate )
	{
		OnKillThread(); // �����ư�� �������� �ʾҴٸ� ������ ����
	}

	//	FTP ���� ����
	//m_pFtpPatch->DisConnect ();

	//SAFE_DELETE ( m_pFtpPatch );
	SAFE_DELETE ( m_pHttpPatch );
 
	CDialog::PostNcDestroy();
}

void	CPatchPrimeManDlg::UpdateControls()
{
	static const ULONGLONG UL100 = 100UL;
	static const ULONGLONG UL1 = 1UL;

	ULONGLONG Pos = 0;
	ULONGLONG End = 0;
	
	int	AllPercent= 0;	
	
	NS_LOG_CONTROL::GetProcessCurPosition ( &Pos, &End );	
	End = ( !End )? UL1 : End;
	AllPercent = int( (Pos*UL100) / End);

	m_ctrlProgressAll.SetPos ( AllPercent );
}

void	CPatchPrimeManDlg::BEGIN_PATCH ()
{
	static S_PATCH_THREAD_PARAM sParam;
	//sParam.pFtpPatch = m_pFtpPatch;
	sParam.pHttpPatch = m_pHttpPatch;
	sParam.bUseHttp = m_bUseHttp;
	sParam.sPatchVer = m_sPatchVer;
	sParam.sGameVer = m_sGameVer;

	m_pThread = new CHttpPatchThread( &sParam, g_szAppPath, AfxGetThread()->m_nThreadID );

	if (m_pThread == NULL)
		return;

	ASSERT_VALID(m_pThread);
	m_pThread->m_pThreadParams = NULL;
	
	if ( !m_pThread->CreateThread(CREATE_SUSPENDED) )
	{
		delete m_pThread;
		return;
	}

	VERIFY(m_pThread->SetThreadPriority(THREAD_PRIORITY_IDLE));
	m_pThread->ResumeThread();

	SetTimer ( E_START_PATCH, 30, NULL ); // ������ ������ Ÿ�̸� �۵��� ������...?
}

void	CPatchPrimeManDlg::END_PATCH ()
{
	m_ctrlProgressAll.SetPos ( 100 );

	if ( !m_pThread->IsFail() ) // ���а� �ƴϸ�...
	{
		ListAddString( IDS_MESSAGE_004 ); // �غ�Ϸ�
		CREATEPROCESS(); // ���� ����
	}
}

BOOL CPatchPrimeManDlg::CREATEPROCESS()
{

	// ���� ����� �Ķ��Ÿ ���� ( �� �α��� �� �� ���Ľ� �ʿ� )
	CWinApp *pApp = AfxGetApp();
	if( !pApp )	return FALSE;

	CString StrCmdLine = pApp->m_lpCmdLine;

	STRUTIL::ClearSeparator ();
	STRUTIL::RegisterSeparator ( "/" );
	STRUTIL::RegisterSeparator ( " " );

	CStringArray strCmdArray;
	STRUTIL::StringSeparate ( StrCmdLine, strCmdArray );

	CString strVALUE, strKEY, strToken;
	int nPos(-1);

	// 0,1�� ���������� �������
	for( int i=2; i<strCmdArray.GetCount(); ++i )
	{

		strToken = strCmdArray.GetAt(i);
		strVALUE += " " + strToken;

	}

	CString strTemp;
	CString strLauncherFile( NS_GLOBAL_VAR::g_szLauncherCabFile );
	strLauncherFile = strLauncherFile.Left( strLauncherFile.ReverseFind( '.' ) );

	strTemp.Format( "\"%s%s\"", g_szAppPath,strLauncherFile);
	

	if( ! ShellExecute( NULL , "open", strTemp.GetBuffer() ,strVALUE.GetBuffer(), NULL, SW_SHOW) )
	{
		MessageBox ( "CreateProcess Launcher", "ERROR", MB_OK|MB_ICONEXCLAMATION );
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CPatchPrimeManDlg::OnKillThread()
{
	VERIFY(SetEvent(m_pThread->m_hEventKill));
	
	// wait for all threads to finish shutdown
	BOOL bThreadsLeft(TRUE);

	while (bThreadsLeft)
	{
		Sleep( 1 );// 200ms for every 100 threads
		bThreadsLeft = FALSE;
		
		if (WaitForSingleObject(m_pThread->m_hEventDead, 0) == WAIT_TIMEOUT)
			bThreadsLeft = TRUE;
	}

	VERIFY(WaitForSingleObject(m_pThread->m_hThread, INFINITE) == WAIT_OBJECT_0);
	delete m_pThread;
}

void CPatchPrimeManDlg::ListAddString( const CString & strLog )
{
	CString strLog_COPY = strLog;
	if ( strLog_COPY[strLog_COPY.GetLength()-1] == '\n' )
	{
		strLog_COPY = strLog_COPY.Left ( strLog_COPY.GetLength()-1 );
		if ( strLog_COPY[strLog_COPY.GetLength()-1] == '\r' )
		{
			strLog_COPY = strLog_COPY.Left ( strLog_COPY.GetLength()-1 );
		}
	}

	int nIndex = m_ctrlListBox.AddString( strLog_COPY );
	m_ctrlListBox.SetCurSel( nIndex );
}

void CPatchPrimeManDlg::ListAddString( UINT nIDS )
{
	CString strLog_COPY;
	strLog_COPY.LoadString( nIDS );

	int nIndex = m_ctrlListBox.AddString ( strLog_COPY );
	m_ctrlListBox.SetCurSel ( nIndex );
}