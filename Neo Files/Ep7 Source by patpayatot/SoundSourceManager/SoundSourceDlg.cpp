// SoundSourceDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "SoundSourceManager.h"
#include "SoundSourceDlg.h"

#include "MainFrm.h"
#include "SoundSourceManagerView.h"

#include "dsutil.h"
#include "SoundSourceMan.h"

// CSoundSourceDlg ��ȭ �����Դϴ�.
static	int		g_SamplesPerSec	= 22050;
static	int		g_Channels		= 1;
static	int		g_Bits			= 16;

IMPLEMENT_DYNAMIC(CSoundSourceDlg, CDialog)
CSoundSourceDlg::CSoundSourceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSoundSourceDlg::IDD, pParent)
	, m_valPathFile(_T(""))
	, m_valComment(_T(""))
	, m_valStaticGroup(_T(""))
	, m_valBufferCount(4)
{
	m_eDlgType = eNew;
}

CSoundSourceDlg::~CSoundSourceDlg()
{
}

void CSoundSourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATHFILE, m_valPathFile);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_ctrlType);
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_valComment);
	DDX_Text(pDX, IDC_STATIC_GROUP, m_valStaticGroup);
	DDX_Control(pDX, IDC_BUTTON_FINDFILE, m_ctrlButtonFindFile);
	DDX_Text(pDX, IDC_EDIT_BUFFER, m_valBufferCount);
}


BEGIN_MESSAGE_MAP(CSoundSourceDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_FINDFILE, OnBnClickedButtonFindfile)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSoundSourceDlg �޽��� ó�����Դϴ�.

void CSoundSourceDlg::OnBnClickedButtonFindfile()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//	<--	�� ���� ����� ����	-->	//
	//	<**	�̷��� ���� ������, ���� ���̾�αװ� �߸鼭
	//		�� ���� ������� ��� ���ư� ������.
	//	**>
	UpdateData ( TRUE );

	//	<--	���� ���� ���� -->	//
	CString szFilter = "Wav Files( *.wav ) |*.wav|";
	
	//	<--	���Ͽ���	-->	//
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter, this );

    if ( dlg.DoModal() == IDOK )
	{
		//	<--	���� �׽�Ʈ	-->	//
		if ( !ValidateWaveFile ( dlg.GetFileName().GetString() ) )
		{		
			return;
		}

		if ( strcmp ( m_valPathFile.GetString (), dlg.GetPathName () ) )
		{
			m_bChangeFile = TRUE;
		}

		m_OriginFileName = dlg.GetFileName();
		m_valPathFile = dlg.GetPathName();

		UpdateData ( FALSE );
	}
}

BOOL CSoundSourceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	for ( int i = 0; i < NTypeDesc::DescCount; i++ )
	{
		CString	Temp = NTypeDesc::Desc[i];
		m_ctrlType.AddString ( Temp.GetString() );
	}

	//	<--	����Ÿ ����	-->	//
	if ( m_eDlgType == eNew )
	{
		m_valStaticGroup = "�Է�";
		m_ctrlType.SetCurSel ( 0 );		
	}
	else
	{
		CMainFrame	*pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CSoundSourceManagerView *pView = (CSoundSourceManagerView *) pMainFrame->GetActiveView ();		

		m_valStaticGroup = "����";

		SSoundSource	*pSoundSource;
        pSoundSource = pView->GetSSM()->FindRecord ( m_ID );

		CString	FullPath = pView->GetSSM()->GetDataDirectory();
		m_valPathFile = FullPath + pSoundSource->FileName;		
		m_OriginFileName = pSoundSource->OriginFileName;

		m_valBufferCount = pSoundSource->BufferCount;

		m_ctrlType.SetCurSel ( (int)pSoundSource->TYPE );
		m_valComment = pSoundSource->Comment;		
	}

	m_bChangeFile = FALSE;

	UpdateData ( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
void CSoundSourceDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData ( TRUE );

    if ( !m_valPathFile.GetLength() || !m_valComment.GetLength() )
	{
		return;
	}

	CMainFrame	*pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CSoundSourceManagerView *pView = (CSoundSourceManagerView *) pMainFrame->GetActiveView();

	//	<--	����Ÿ ����	-->	//
	if ( m_eDlgType == eNew )
	{
		SSoundSource	*pNewSoundSource = new SSoundSource;

	////////////////////////////////////////////////////////////////////////////////
	//	��ȣ	:	$1
	//	���	:	���� ID ����
	//	����	:	���� ID�� ����� ���ڵ忡 ���
		pView->GetSSM()->GenerateUniqueKey();
		WORD	UniqueID = pView->GetSSM()->GetUniqueKey();
	////////////////////////////////////////////////////////////////////////////////	

		//////////////////////////////////////////////
		//	<--	���� �ð� Ȯ��
		char	szTime[128];
		SYSTEMTIME	UniversalTime;
		SYSTEMTIME	LocalTime;

		//	<--0	Universal Time ��������
		GetSystemTime( &UniversalTime );
		//	-->0	Universal Time ��������

		//	<--0	TimeZone ��������
		TIME_ZONE_INFORMATION TimeZone;
		GetTimeZoneInformation ( &TimeZone );
		//	-->0	TimeZone ��������

		//	<--0	Universal Time�� TimeZone�� �ռ�
		SystemTimeToTzSpecificLocalTime ( &TimeZone, &UniversalTime, &LocalTime );
		//	-->0	Universal Time�� TimeZone�� �ռ�

		sprintf ( szTime, "%d-%02d-%02d %02d:%02d", LocalTime.wYear, LocalTime.wMonth,
			LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute );	
		//	-->	���� �ð� Ȯ��
		////////////////////////////////////////////////


		//	<--	���� �̸� ����
		//	<**	TYPE 2�ڸ� + UniqueID 6�ڸ�
		//	**>
		CString	FileName;
		FileName.Format ( "%02d%06d.wav",
			(int)m_ctrlType.GetCurSel(), UniqueID );
		//	-->
		
		pNewSoundSource->State = TRUE;
		pNewSoundSource->ID = UniqueID;
		pNewSoundSource->TYPE = (ESoundType)m_ctrlType.GetCurSel();
		pNewSoundSource->FileName = FileName;
		pNewSoundSource->LastUpdated = szTime;
		pNewSoundSource->Comment = m_valComment;
		pNewSoundSource->OriginFileName = m_OriginFileName;
//		NewSoundSource.FullPath = m_valPathFile;
        pNewSoundSource->BufferCount = (WORD)m_valBufferCount;


		pView->GetSSM()->AddRecord ( pNewSoundSource, m_valPathFile );
	}
	//	<--	�̹� �ִ� ����Ÿ ����	-->	//
	else
	{
        SSoundSource	*pModifySoundSource = pView->GetSSM()->GetRecord ( );

		//////////////////////////////////////////////
		//	<--	���� �ð� Ȯ��
		char	szTime[128];
		SYSTEMTIME	UniversalTime;
		SYSTEMTIME	LocalTime;

		//	<--0	Universal Time ��������
		GetSystemTime( &UniversalTime );
		//	-->0	Universal Time ��������

		//	<--0	TimeZone ��������
		TIME_ZONE_INFORMATION TimeZone;
		GetTimeZoneInformation ( &TimeZone );
		//	-->0	TimeZone ��������

		//	<--0	Universal Time�� TimeZone�� �ռ�
		SystemTimeToTzSpecificLocalTime ( &TimeZone, &UniversalTime, &LocalTime );
		//	-->0	Universal Time�� TimeZone�� �ռ�

		sprintf ( szTime, "%d-%02d-%02d %02d:%02d", LocalTime.wYear, LocalTime.wMonth,
			LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute );	
		//	-->	���� �ð� Ȯ��
		////////////////////////////////////////////////

		//	<--	���� �̸� ����
		//	<**	TYPE 2�ڸ� + UniqueID 6�ڸ�
		//	**>
		CString	FileName;
		FileName.Format ( "%02d%06d.wav",
			(int)(ESoundType)m_ctrlType.GetCurSel(), pModifySoundSource->ID );
		//	-->		
		
		pModifySoundSource->State = TRUE;
		//pModifySoundSource->ID = UniqueID;	<--	�Ϻη� ǥ���� �а���	-->	//
		pModifySoundSource->TYPE = (ESoundType)m_ctrlType.GetCurSel();		
		pModifySoundSource->FileName = FileName;
		pModifySoundSource->LastUpdated = szTime;
		pModifySoundSource->Comment = m_valComment;
		pModifySoundSource->OriginFileName = m_OriginFileName;
//		pModifySoundSource->FullPath = m_valPathFile;

		pModifySoundSource->BufferCount = (WORD)m_valBufferCount;

		pView->GetSSM()->SetRecord ( pModifySoundSource->ID, m_bChangeFile, m_valPathFile );
	}

	OnOK();
}

void	CSoundSourceDlg::SetState ( EDlgType DlgType, WORD ID )
{
	m_eDlgType = DlgType;
	m_ID = ID;
}

BOOL	CSoundSourceDlg::ValidateWaveFile( const TCHAR* strFileName )
{    
	CWaveFile waveFile;

	if( -1 == GetFileAttributes(strFileName) )
		return FALSE;

	// Load the wave file
	if( FAILED( waveFile.Open( (char*)strFileName, NULL, WAVEFILE_READ ) ) )
	{
		waveFile.Close();
		MessageBox ("���̺� ������ �ƴմϴ�.");
		return FALSE;
	}
	else // The load call succeeded
	{
		WAVEFORMATEX* pwfx = waveFile.GetFormat();
		if( pwfx->wFormatTag != WAVE_FORMAT_PCM )
		{
			MessageBox ("���̺� ������ �ݵ�� PCM���� �̾�� �մϴ�.");
			return FALSE;
		}

		// Update the UI controls to show the sound as the file is loaded
		waveFile.Close();


		// Get the samples per sec from the wave file
		DWORD dwSamplesPerSec = waveFile.m_pwfx->nSamplesPerSec;
		DWORD dwChannels = waveFile.m_pwfx->nChannels;
		DWORD dwBits = waveFile.m_pwfx->wBitsPerSample;

		//	<--	���� ���� ��	-->	//		
		if ( (dwSamplesPerSec == g_SamplesPerSec) &&
			(dwChannels == g_Channels )	&&
			(dwBits == g_Bits ) )
		{
			return TRUE;
		}
		
		CString	TempString;
		TempString.Format ( "������ ��ȣ�Ȱ� ���� �������� �����ּž� �մϴ�.\n"
			"SamplesPerSec : %d[%d], Channels : %d[%d], Bits : %d[%d]",
			dwSamplesPerSec, g_SamplesPerSec, dwChannels, g_Channels, dwBits, g_Bits );

		MessageBox ( TempString.GetString() );
        
		return FALSE;
	}
}
