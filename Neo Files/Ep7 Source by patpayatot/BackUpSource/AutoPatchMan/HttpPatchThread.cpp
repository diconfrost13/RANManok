#include "StdAfx.h"
#include <algorithm>
#include "httppatchthread.h"
#include "Resource.h"

#include "s_CHttpPatch.h"
#include "RANPARAM.h"
#include "LogControl.h"
#include "CompactFdi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CHttpPatchThread, CLPatchThread)

CHttpPatchThread::CHttpPatchThread( LPVOID pParam, DWORD nThreadID ) :
	m_nDlgThreadID(nThreadID),
	m_sFailGameVer(0)
{
	VERIFY( pParam != NULL);
	m_pPatchThreadParam = (S_PATCH_THREAD_PARAM *)pParam;
}

void CHttpPatchThread::ThreadMain()
{
	CHttpPatch* pHttpPatch	= m_pPatchThreadParam->pHttpPatch;
	const bool	bUseHttp	= m_pPatchThreadParam->bUseHttp;
	const int	cPatchVer	= m_pPatchThreadParam->cPatchVer;
	const int	sPatchVer	= m_pPatchThreadParam->sPatchVer;
	const int	cGameVer	= m_pPatchThreadParam->cGameVer;
	const int	sGameVer	= m_pPatchThreadParam->sGameVer;	

	CString cFileList, sFileList, str, strTemp;
	BOOL bInstalling(FALSE);

	NS_LOG_CONTROL::SetProcessAllPosition ( 0, 100 );

	if ( IsForceTerminate() ) return ;

	// ����Ʈ ������
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_049, 0 );
	// ���� ����Ʈ �ٿ�ε�
	if ( !GETFILE_USEHTTP ( pHttpPatch, "\\", NS_GLOBAL_VAR::strServerCabFileList.GetString(), "" ) )
	{	
		if ( !IsForceTerminate() )
		{
			// ����Ʈ ������ ����
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_050, 0 );
		}

		goto LFail;
	}

	if ( IsForceTerminate() ) goto LFail;

	//	���� Ǯ������ ��ü ��� ���� �� ���� Ǯ��&��ġ
	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strServerCabFileList;
	// ����Ʈ ���� Ǯ�� ����
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_051, 0 );
	if ( !Extract ( NS_GLOBAL_VAR::strAppPath.GetString (), str.GetString() ) )
	{
		// ����Ʈ ����Ǯ�� ����
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_052, 0 );
		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 4, 100 );

	//Initialize(); // MEMO : �Լ� ���� �ȵ�.

	if ( IsForceTerminate() ) goto LFail;

	// ���� ����Ʈ �� �� �� ��� �ۼ�
	// ��ġ ���� ����Ʈ �ε�
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_053, 0 );
	if ( !LoadList() )
	{
		// ����Ʈ �ε忡 ����
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_054, 0 );
		goto LFail;
	}

	if ( IsForceTerminate() ) goto LFail;

	// ��ġ ���� ����Ʈ �� ����
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_057, 0 );
	if ( !MakeNewList ( cPatchVer, sPatchVer, cGameVer, sGameVer ) )
	{
		// ����Ʈ �񱳿� ����
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_058, 0 );
		goto LFail;
	}    

	NS_LOG_CONTROL::SetProcessAllPosition ( 10, 100 );

	if ( IsForceTerminate() ) goto LFail;

	//	�� ��Ͽ� ���� ���� �ٿ� �ε�
	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
	CreateDirectory ( str.GetString(), NULL );
	if ( !DownloadFilesByHttp ( pHttpPatch ) )
	{
		if ( !IsForceTerminate() )
		{
			// ��ġ ���� ������ ����
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_055, 0 );
		}

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 70, 100 );

	if ( IsForceTerminate() ) goto LFail;

	//	���� ����
	if ( !Installing() )
	{	
		//	���� ���ᰡ �ƴ� ��¥ ����
		//
		if ( IsExtractError() )
		{
			// ��ġ ������ �Ϻ� �ջ�
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_059, 0 );

			//	Note : DS list ����
			//
			CString strDownList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownList;
			DeleteFile ( strDownList.GetString() );

			// MEMO : ī�� ����Ʈ�� ���Ŀ� ��ġ�� ������ �ٿ�ε� ���� �ʾƵ� �ȴ�.
			//
			CString strCopyList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strCopyList;
			CopyFile( strCopyList.GetString(), strDownList.GetString(), FALSE );
		}

		bInstalling = TRUE; // ��ġ���̴�.

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 90, 100 );

	//	<--	Ŭ���̾�Ʈ ��ϸ� ����������� ��ü	-->	//
	cFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szClientFileList;
	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList;

	DeleteFile( cFileList.GetString () );
	MoveFile( sFileList.GetString(), cFileList.GetString() );

	//	Note : DS list ����
	//	
	strTemp = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownList;
	DeleteFile( strTemp.GetString() );

	// Note : ī�� ����Ʈ ����
	//
	strTemp = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strCopyList;
	DeleteFile ( strTemp.GetString() );

	sFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::strServerCabFileList );
	DeleteFile ( sFileList.GetString() );

	DeleteDownFiles();
	DeleteNotFoundFile();

	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
	RemoveDirectory ( str.GetString() );

	Destroy ();	

	// Ŭ���̾�Ʈ ���� ����
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_060, 0 );
	if ( !VersionUp ( sGameVer ) )
	{
		// Ŭ���̾�Ʈ ���� ���� ����
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_061, 0 );
		goto LFail;
	}

	// ��� ��ġ ����
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_062, 0 );
	NS_LOG_CONTROL::SetProcessAllPosition ( 100, 100 );

	return ;

LFail:
	//	�ٿ�ε��� ���ϵ��� �ջ�Ǿ� ������ ��쿡��
	//	����Ʈ�� ó������ �ٽ� �ۼ��ؾ��ϱ⶧����
	//	����Ʈ�� �������� �ʴ´�.
	if ( IsExtractError () )
	{
		if ( IsForceTerminate () )
		{
			SaveDownList ( sGameVer );
		}

		SaveCopyList ( sGameVer ); // MEMO :��ġ�� ����Ʈ�� ��ġ�߿� ��ҵǾ������� ������ �����Ѵ�.
	}
	else
	{
		SaveDownList ( sGameVer );

		if( bInstalling ) SaveCopyList ( sGameVer ); // MEMO :��ġ�߿� �ߴܵǸ� ī�� ����Ʈ �ۼ�.
	}

	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList;
	DeleteFile ( sFileList.GetString() );
	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strServerCabFileList;
	DeleteFile ( sFileList.GetString() );	

	Destroy ();

	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList;
	DeleteFile ( sFileList.GetString() );

	SetFail();

	return ;
}

BOOL CHttpPatchThread::GETFILE_USEHTTP( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir )
{
	if ( !pHttpPatch )
	{
		GASSERT ( 0 && "�߸��� ������ ������ �� �ֽ��ϴ�." );
		return FALSE;
	}

	if ( !strFileName.size () )
	{
		GASSERT ( 0 && "������ �������� �ʾҽ��ϴ�." );
		return FALSE;
	}

	std::string strSubPath( strRemoteSubPath );

	//	'\\'���ڸ� '/'�� �����Ѵ�.
	std::replace ( strSubPath.begin(), strSubPath.end(), '\\','/' );

	#if !defined(_DEBUG) && !defined(DAUMPARAM)
	{
		CString strFolder;
		strFolder.Format( "/%04d", m_pPatchThreadParam->sGameVer );
		strFolder += strSubPath.c_str();
		strSubPath = strFolder;
	}
	#endif

	strSubPath += strFileName;

	std::string strLocalFullPath;
	strLocalFullPath += NS_GLOBAL_VAR::strAppPath.GetString();	
	strLocalFullPath += strTempDir.GetString();
	strLocalFullPath += strFileName;

	static int nTRY = 0;
	int nTRY_FILESIZE_CHECK = 0;
	int nERROR_RETRY = 0;
	int nADDRESS_NULL_COUNT = 0;
	while ( nTRY_FILESIZE_CHECK < 3 )
	{
		Sleep( 0 );

		//	���� �����
		if ( IsForceTerminate () ) return FALSE;		

		//	NOTE
		//		�ִ� �õ� ȸ�� �ʰ���
		if ( nADDRESS_NULL_COUNT == RANPARAM::MAX_HTTP ) return FALSE;

		if ( RANPARAM::MAX_HTTP == nTRY ) nTRY = 0;
		if ( nERROR_RETRY == 5 ) return FALSE;		

		static const CString strHTTP = "http://";

		CString strRealAddress = RANPARAM::HttpAddressTable[nTRY];
		if ( !strRealAddress.GetLength () )
		{
			nADDRESS_NULL_COUNT++;		//	MAX_HTTP�� ��� ���ΰ�?
			nTRY++;
			continue;
		}

		//	�� üũ�� ����ߴٴ� ���� nADDRESS_NULL_COUNT�� �ʱ�ȭ�ؾ����� �ǹ��Ѵ�.
		nADDRESS_NULL_COUNT = 0;

		CString strHttpAddress = strHTTP + strRealAddress; // "http://ranp.daumgame.com"

		if ( NET_ERROR == pHttpPatch->SetBaseURL ( strHttpAddress ) )
		{			
			//NS_LOG_CONTROL::Write ( pHttpPatch->GetErrMsg() );
			nTRY++;
			nERROR_RETRY++;
			continue;
		}

		if ( NET_ERROR == pHttpPatch->GetFile ( strSubPath.c_str(), strLocalFullPath.c_str() ) )
		{
			//NS_LOG_CONTROL::Write ( pHttpPatch->GetErrMsg() );

			CString	strMsg;
			strMsg.LoadString( IDS_MESSAGE_047 );
			CHAR * szListMessage = new CHAR[256];
			wsprintf( szListMessage, "%s %s", strMsg.GetString(), strFileName.c_str() );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

			nTRY ++;
			nERROR_RETRY++;
			continue;
		}

		//	���� �����
		if ( IsForceTerminate () ) return FALSE;	

		ULONGLONG ulRECEIVED, ulTOTALSIZE;
		NS_LOG_CONTROL::GetProcessCurPosition ( &ulRECEIVED, &ulTOTALSIZE );

		if ( ulRECEIVED != ulTOTALSIZE )
		{
			nTRY_FILESIZE_CHECK++;
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_046, 0 ); // ���� �޴� ���� ũ�Ⱑ ���� �ʴ�.
			continue;
		}
		else
		{
			//NOTE	
			//	NET_OK
			//	������ ��
			return TRUE;
		}        
	}

	return FALSE;
}

BEGIN_MESSAGE_MAP(CHttpPatchThread, CLPatchThread)
END_MESSAGE_MAP()