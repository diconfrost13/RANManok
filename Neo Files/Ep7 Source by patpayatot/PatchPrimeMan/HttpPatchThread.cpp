#include "StdAfx.h"

#include <algorithm>

#include "httppatchthread.h"
#include "Resource.h"

#include "s_CHttpPatch.h"
#include "GlobalVariable.h"
#include "LogControl.h"
#include "CompactFdi.h"
#include "RANPARAM.h"
#include "../Common/SUBPATH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CHttpPatchThread, CLPatchThread)

CHttpPatchThread::CHttpPatchThread( LPVOID pParam, const TCHAR * szAppPath, DWORD nThreadID ) :
	CLPatchThread( pParam, szAppPath, nThreadID )
{
}

void CHttpPatchThread::LauncherPatch()
{
	CHttpPatch* pHttpPatch	= m_pPatchThreadParam->pHttpPatch;
	const int sPatchVer		= m_pPatchThreadParam->sPatchVer;

	CString str;

	if ( IsForceTerminate() ) return;

	// ������ ���� �ڵ�
	//
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_011, 0 );
	
	if ( !GETFILE_USEHTTP ( pHttpPatch, "\\", NS_GLOBAL_VAR::g_szLauncherCabFile, "" ) )
	{
		if ( !IsForceTerminate() )
		{
			CHAR * szListMessage = new CHAR[MAX_PATH];
			StringCchCopy( szListMessage, MAX_PATH, pHttpPatch->GetErrMsg() );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_020, 0 );
			SetFail (); // MEMO : ó������ �ٽ� �ٿ�޾ƾ� �Ѵ�.
		}
		goto LFail;
	}
	else ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_013, 0 );

	//	������ Ran Auto Patch ���� ( *.exe )
	//	
	DELETEFILE(FALSE); // cab Ȯ���ڰ� ������� false

	if ( IsForceTerminate() ) goto LFail;

	// ������ ���� ���� ����
	DeleteLauncherFile();

	//	�� ���� ��ġ
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_014, 0 );

	str.Format ( "%s%s%s", NS_GLOBAL_VAR::strProFile,SUBPATH::SAVE_ROOT, NS_GLOBAL_VAR::g_szLauncherCabFile );
	if ( !Extract ( m_szAppPath, str.GetString() ) )
	{
		SetExtractError(); // Cab ���� Ǯ�� ����

		if ( !IsForceTerminate() )
		{
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_012, 0 );
			SetFail ();
		}
		goto LFail;
	}
	else ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_015, 0 );

	if ( IsForceTerminate() ) goto LFail;

	//	Ran Auto Patch ���� �ø���
	//
	if ( !VersionUp( sPatchVer ) )
	{		
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_016, 0 );
		goto LFail;
	}
	else ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_017, 0 );

	DELETEFILE(); // �ٿ�ε� �� Ran Auto Patch ���� ( *.exe.cab )

	return;

LFail:
	DELETEFILE(); // �ٿ�ε� �� Ran Auto Patch ���� ( *.exe.cab )
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

	//	'\\'���ڸ� '/'�� �����Ѵ�.
	std::replace ( strRemoteSubPath.begin(), strRemoteSubPath.end(), '\\','/' );	

	// MEMO : �н� ����
	#if !defined(_DEBUG) && !defined(DAUMPARAM) &&!defined(GS_PARAM)
	{
		if( m_pPatchThreadParam->sGameVer != -1 )
		{
			CString strFolder;
			strFolder.Format( "/%04d", m_pPatchThreadParam->sGameVer );
			strFolder += strRemoteSubPath.c_str();
			strRemoteSubPath = strFolder;
		}
	}
	#endif

	strRemoteSubPath += strFileName;

	std::string strLocalFullPath;
	strLocalFullPath += NS_GLOBAL_VAR::strProFile.GetString();	
	strLocalFullPath += SUBPATH::SAVE_ROOT;
	strLocalFullPath += strTempDir.GetString();
	strLocalFullPath += strFileName;

	static int nTRY = 0;
	int nTRY_FILESIZE_CHECK = 0;
	int nERROR_RETRY = 0;
	int nADDRESS_NULL_COUNT = 0;

	while ( nTRY_FILESIZE_CHECK < 3 )
	{
		if( IsForceTerminate() ) return FALSE;

		//	NOTE
		//		�ִ� �õ� ȸ�� �ʰ���
		if ( nADDRESS_NULL_COUNT == RANPARAM::MAX_HTTP ) return FALSE;

		if ( RANPARAM::MAX_HTTP == nTRY ) nTRY = 0;
		if ( nERROR_RETRY == 3 ) return FALSE;		

		static const CString strHTTP = "http://";

		CString strRealAddress = RANPARAM::HttpAddressTable[nTRY];
		if ( !strRealAddress.GetLength () )
		{
			nADDRESS_NULL_COUNT++;
			nTRY++;
			continue;
		}

		//	�� üũ�� ����ߴٴ� ���� nADDRESS_NULL_COUNT�� �ʱ�ȭ�ؾ����� �ǹ��Ѵ�.
		nADDRESS_NULL_COUNT = 0;

		CString strHttpAddress = strHTTP + strRealAddress;

		if ( NET_ERROR == pHttpPatch->SetBaseURL ( strHttpAddress ) )
		{
			//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_020, 0 );
			nTRY++;
			nERROR_RETRY++;
			continue;
		}

		if ( NET_ERROR == pHttpPatch->GetFile ( strRemoteSubPath.c_str(), strLocalFullPath.c_str() ) )
		{
			//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_020, 0 );
			nTRY ++;
			nERROR_RETRY++;
			continue;
		}

		if( IsForceTerminate() ) return FALSE;

		ULONGLONG ulRECEIVED, ulTOTALSIZE;
		NS_LOG_CONTROL::GetProcessCurPosition ( &ulRECEIVED, &ulTOTALSIZE );

		if ( ulRECEIVED != ulTOTALSIZE )
		{
			nTRY_FILESIZE_CHECK++;
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_018, 0 );
			continue;
		}
		else
		{
			return TRUE; // NET_OK, ������ ��
		}

		Sleep( 0 );
	}

	return FALSE;
}

BEGIN_MESSAGE_MAP(CHttpPatchThread, CLPatchThread)
END_MESSAGE_MAP()