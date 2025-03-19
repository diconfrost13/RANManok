#include "StdAfx.h"
#include "./httpthread.h"
#include "./Resource.h"

#include "../netclientlib/s_CHttpPatch.h"
#include "../mfcexlib/RANPARAM.h"
#include "./LogControl.h"
#include "LauncherText.h"
#include "../Common/SUBPATH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CHttpThread, CLPatchThread)

BOOL CHttpThread::LoadListServer()
{
	CString str;
	str.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::g_szServerFileList );

	FILE* fp = fopen( str.GetString(), "rb" );
	if( !fp ) return FALSE;

	SFILENODE FileInfo;

	// ù��° ���� ����
	if( 1 != fread ( &FileInfo, VER_1_OFFSET, 1, fp ) )
	{
		fclose ( fp );
		return FALSE;
	}

	// ���� �ɼ� ����
	INT nVerOffset(0);

	switch( FileInfo.Ver )
	{
	case 1: nVerOffset = VER_1_OFFSET; break;
	case 2: nVerOffset = VER_2_OFFSET; break;
	default:
		::AfxMessageBox( _T("[ERROR]:Filelist is different!") );
		return FALSE;
	}

	// ���� ������ �ɼ� ��ŭ ���� �������� �ڷ� �̵��Ѵ�.
	LONG lOffset = nVerOffset - VER_1_OFFSET;

	if( fseek( fp, lOffset, SEEK_CUR) )
	{
		fclose ( fp );
		return FALSE;
	}

	// �ι�° ���� ����
	if( 1 != fread ( &FileInfo, nVerOffset, 1, fp ) )
	{
		fclose ( fp );
		return FALSE;
	}

	for( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE* pNewFile = new SFILENODE;
		if( 1 != fread( pNewFile, nVerOffset, 1, fp ) )
		{
			fclose ( fp );
			return FALSE;
		}

		m_vectorServerFile.push_back( pNewFile );
	}

	fclose ( fp );

	return TRUE;
}

BOOL CHttpThread::LoadListClient()
{
	CString str;
	str.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::g_szClientFileList );

	FILE* fp = fopen( str.GetString(), "rb" );
	if( !fp ) return FALSE;

	SFILENODE FileInfo;

	if( 1 != fread ( &FileInfo, VER_1_OFFSET, 1, fp ) )
	{
		fclose ( fp );
		return FALSE;
	}

	// ���� �ɼ� ����
	INT nVerOffset(0);

	switch( FileInfo.Ver )
	{
	case 1: nVerOffset = VER_1_OFFSET; break;
	case 2: nVerOffset = VER_2_OFFSET; break;
	default:
		::AfxMessageBox( _T("[ERROR]:Filelist is different!") );
		return FALSE;
	}

	// ���� ������ �ɼ� ��ŭ ���� �������� �ڷ� �̵��Ѵ�.
	LONG lOffset = nVerOffset - VER_1_OFFSET;

	if( fseek( fp, lOffset, SEEK_CUR) )
	{
		fclose ( fp );
		return FALSE;
	}

	if( 1 != fread ( &FileInfo, nVerOffset, 1, fp ) )
	{
		fclose ( fp );
		return FALSE;
	}

	for( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE NewFile;
		if( 1 != fread( &NewFile, nVerOffset, 1, fp ) )
		{
			fclose ( fp );
			return FALSE;
		}

		m_ClientFileTree.Add( &NewFile );
	}

	fclose ( fp );

	return TRUE;
}

BOOL CHttpThread::LoadList()
{
	//	<--	����Ʈ �б�	-->	//
	if( !LoadListServer() )		return FALSE;
	if( IsForceTerminate() )	return FALSE;
	if( !LoadListClient() )		return FALSE;

	return TRUE;
}

BOOL CHttpThread::GETFILE_USEHTTP( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir )
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

	#if !defined(_DEBUG) && !defined(KR_PARAM)
	{
		CString strFolder;
		strFolder.Format( "/%04d", m_pPatchThreadParam->sGameVer );
		strFolder += strSubPath.c_str();
		strSubPath = strFolder;
	}
	#endif

	strSubPath += strFileName;

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
			strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 47 );
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
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 46 ), 0 ); // ���� �޴� ���� ũ�Ⱑ ���� �ʴ�.
			continue;
		}
		else
		{
			return TRUE;
		}

		Sleep( 0 );
	}

	return FALSE;
}

BEGIN_MESSAGE_MAP(CHttpThread, CLPatchThread)
END_MESSAGE_MAP()