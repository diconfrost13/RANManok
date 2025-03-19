#include "StdAfx.h"
#include "./AutoPatchThread.h"
#include "./Resource.h"

#include "./LogControl.h"
#include "../netclientlib/s_CHttpPatch.h"
#include "../mfcexlib/RANPARAM.h"
#include "../MfcExLib/ExLibs/CompactFdi.h"
#include "../EngineLib/Common/StringUtils.h"
#include "LauncherText.h"
#include "../Common/SUBPATH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CAutoPatchThread, CHttpThread)

CAutoPatchThread::CAutoPatchThread( LPVOID pParam, DWORD nThreadID )
{
	VERIFY( pParam != NULL);
	m_pPatchThreadParam = (S_PATCH_THREAD_PARAM *)pParam;

	m_nDlgThreadID = nThreadID;
	m_sFailGameVer = 0;
}

void CAutoPatchThread::ThreadMain()
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
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 49 ), 0 );
	// ���� ����Ʈ �ٿ�ε�
	if ( !GETFILE_USEHTTP ( pHttpPatch, "\\", NS_GLOBAL_VAR::strServerCabFileList.GetString(), "" ) )
	{	
		if ( !IsForceTerminate() )
		{
			// ����Ʈ ������ ����
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 50 ), 0 );
		}

		goto LFail;
	}

	

	if ( IsForceTerminate() ) goto LFail;

	//	���� Ǯ������ ��ü ��� ���� �� ���� Ǯ��&��ġ
	str = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strServerCabFileList;

	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 51 ), 0 );
	if ( !Extract ( NS_GLOBAL_VAR::strAppPath.GetString (), str.GetString() ) )
	{
		// ����Ʈ ����Ǯ�� ����
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 52 ), 0 );
		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 4, 100 );

	//Initialize(); // MEMO : �Լ� ���� �ȵ�.

	if ( IsForceTerminate() ) goto LFail;

	// ���� ����Ʈ �� �� �� ��� �ۼ�
	// ��ġ ���� ����Ʈ �ε�
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 53 ), 0 );
	if ( !LoadList() )
	{
		// ����Ʈ �ε忡 ����
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 54 ), 0 );
		goto LFail;
	}

	if ( IsForceTerminate() ) goto LFail;

	// ��ġ ���� ����Ʈ �� ����
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 57 ), 0 );
	if ( !MakeNewList ( cPatchVer, sPatchVer, cGameVer, sGameVer ) )
	{
		// ����Ʈ �񱳿� ����
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 58 ), 0 );
		goto LFail;
	}    

	NS_LOG_CONTROL::SetProcessAllPosition ( 10, 100 );

	if ( IsForceTerminate() ) goto LFail;

	//	�� ��Ͽ� ���� ���� �ٿ� �ε�
	str = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownloadTemp;
	CreateDirectory ( str.GetString(), NULL );
	if ( !DownloadFilesByHttp ( pHttpPatch ) )
	{
		if ( !IsForceTerminate() )
		{
			// ��ġ ���� ������ ����
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 55 ), 0 );
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
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 59 ), 0 );

			//	Note : DS list ����
			//
			CString strDownList = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownList;
			DeleteFile ( strDownList.GetString() );

			// MEMO : ī�� ����Ʈ�� ���Ŀ� ��ġ�� ������ �ٿ�ε� ���� �ʾƵ� �ȴ�.
			//
			CString strCopyList = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT  + NS_GLOBAL_VAR::strCopyList;
			CopyFile( strCopyList.GetString(), strDownList.GetString(), FALSE );
		}

		bInstalling = TRUE; // ��ġ���̴�.

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 80, 100 );

//	Package
//	if(!CompressFile()){
//		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 71 ), 0 );
//		goto LFail;
//	}

	NS_LOG_CONTROL::SetProcessAllPosition ( 90, 100 );

	//	Note : DS list ����
	//	
	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT  + NS_GLOBAL_VAR::strDownList;
	DeleteFile( strTemp.GetString() );

	// Note : ī�� ����Ʈ ����
	//
	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT  + NS_GLOBAL_VAR::strCopyList;
	DeleteFile ( strTemp.GetString() );

	//	<--	Ŭ���̾�Ʈ ��ϸ� ����������� ��ü	-->	//
	cFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szClientFileList;
	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList;

	DeleteFile( cFileList.GetString () );
	MoveFile( sFileList.GetString(), cFileList.GetString() );

	sFileList = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strServerCabFileList;
	DeleteFile ( sFileList.GetString() );

	DeleteDownFiles();
	DeleteNotFoundFile();
//	DeleteCompressFile();
//	if(!OptimizeCompressFile()){
//		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 72 ), 0 );
//		goto LFail;
//	}

    
	Destroy ();	

	// Ŭ���̾�Ʈ ���� ����
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 60 ), 0 );
	if ( !VersionUp ( sGameVer ) )
	{
		// Ŭ���̾�Ʈ ���� ���� ����
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 61 ), 0 );
		goto LFail;
	}

	// ��� ��ġ ����
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 62 ), 0 );
	NS_LOG_CONTROL::SetProcessAllPosition ( 100, 100 );

	return ;

LFail:
	//	�ٿ�ε��� ���ϵ��� �ջ�Ǿ� ������ ��쿡��
	//	����Ʈ�� ó������ �ٽ� �ۼ��ؾ��ϱ⶧���� ����Ʈ�� �������� �ʴ´�.
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
	sFileList = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strServerCabFileList;
	DeleteFile ( sFileList.GetString() );	

	Destroy ();

	SetFail();

	return ;
}

BOOL CAutoPatchThread::SaveDownList( int sGameVer )
{
	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownList;

	FILE* fp = fopen ( strTemp, "wb" );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

	FileInfo.Ver = sGameVer;
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]SaveDownList File Game Version Write Failed!" );
		return FALSE;
	}

	FileInfo.Ver = (int) m_vectorDownFile.size ();
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]SaveDownList File Size Write Failed!" );
		return FALSE;
	}

	for ( int i = 0; i < (int)m_vectorDownFile.size (); i++ )
	{
		SFILENODE* pNewFile = m_vectorDownFile[i];

		if ( 1 != fwrite ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose ( fp );

			CDebugSet::ToLogFile( "[ERROR]SaveDownList File Write Failed!" );
			return FALSE;
		}
	}

	fclose ( fp );
	
	return TRUE;
}

BOOL CAutoPatchThread::LoadDownList()
{
	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownList;

	FILE* fp = fopen ( strTemp, "rb" );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]LoadDownList File Game Version Read Failed!" );
		return FALSE;
	}
	m_sFailGameVer = FileInfo.Ver;

	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]LoadDownList File Size Read Failed!" );
		return FALSE;
	}	

	for ( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE* pNewFile = new SFILENODE;
		if ( 1 != fread ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose ( fp );

			CDebugSet::ToLogFile( "[ERROR]LoadDownList File Read Failed!" );
			return FALSE;
		}

		m_mapDownedFile.insert ( std::make_pair(std::string(pNewFile->FileName), pNewFile) );
	}

	fclose ( fp );

	return TRUE;
}

BOOL CAutoPatchThread::SaveCopyList( int sGameVer )
{
	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strCopyList;

	FILE* fp = fopen ( strTemp, "wb" );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

	FileInfo.Ver = sGameVer;
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]SaveCopyList File Game Version Write Failed!" );
		return FALSE;
	}

	FileInfo.Ver = (int) m_vectorCopyFile.size ();
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]SaveCopyList File Size Write Failed!" );
		return FALSE;
	}

	for ( int i = 0; i < (int)m_vectorCopyFile.size (); i++ )
	{
		SFILENODE* pNewFile = m_vectorCopyFile[i];

		if ( 1 != fwrite ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose ( fp );

			CDebugSet::ToLogFile( "[ERROR]SaveCopyList File Write Failed!" );
			return FALSE;
		}
	}

	fclose ( fp );

	return TRUE;
}

BOOL CAutoPatchThread::LoadCopyList()
{
	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strCopyList;

	FILE* fp = fopen ( strTemp, "rb" );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]LoadCopyList File Game Version Read Failed!" );
		return FALSE;
	}
	m_sFailGameVer = FileInfo.Ver;

	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile( "[ERROR]LoadCopyList File Size Read Failed!" );
		return FALSE;
	}	

	for ( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE* pNewFile = new SFILENODE;
		if ( 1 != fread ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose ( fp );

			CDebugSet::ToLogFile( "[ERROR]LoadCopyList File Read Failed!" );
			return FALSE;
		}

		m_mapCopiedFile.insert ( std::make_pair(std::string(pNewFile->FileName), pNewFile) );
/*
		if(strstr(pNewFile->SubPath,NS_GLOBAL_VAR::strCompDir)){ // ������ �������� �н��� �˻��ؼ� �˾ƺ���.
			m_vecCompFile.push_back(pNewFile);
		}
*/
	}

	fclose ( fp );

	return TRUE;
}

BOOL CAutoPatchThread::MakeNewList( const int cPatchVer, const int sPatchVer, const int cGameVer, const int sGameVer )
{
	NS_LOG_CONTROL::SetProcessCurPosition ( 0, 1 );
	{
		FILEVECTOR::size_type nServerFileSize = m_vectorServerFile.size ();
		for ( FILEVECTOR::size_type i = 0; i < nServerFileSize; i++ )
		{	
			SFILENODE* pServerGetFile = m_vectorServerFile[i];

			CPatchSetNode* pNode = m_ClientFileTree.Find ( pServerGetFile );
			if ( pNode )	//	���� ����
			{
				SFILENODE* pClientFile = pNode->GetFile ();
				pNode->SetUseFlag ();

				if ( pClientFile->Ver < pServerGetFile->Ver )
				{
					SFILENODE* pNewFile = new SFILENODE;
					*pNewFile = *pServerGetFile;
					m_vectorNewFile.push_back ( pNewFile );
				}
			}
			else	//	�߰��Ǵ� ����
			{			
				SFILENODE* pNewFile = new SFILENODE;
				*pNewFile = *pServerGetFile;
				m_vectorNewFile.push_back ( pNewFile );
			}

			NS_LOG_CONTROL::SetProcessCurPosition ( i, nServerFileSize );

			if ( IsForceTerminate() )	return FALSE;
		}
	}
	NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );

	//	<--	������ ���� ������ Ŭ���̾�Ʈ�� ������ ��� ���� -->	//
	m_ClientFileTree.GetNotUseItem ( &m_vectorDeleteFile );

	LoadDownList ();	

	return TRUE;
}

BOOL CAutoPatchThread::DownloadFilesByHttp( CHttpPatch* pHttpPatch )
{
	int DownCount = 0;

	// Temp �����н��� �����Ѵ�.
	//
	CString	DownloadDir( NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownloadTemp );

	int nVectorNewFileSize = (int)m_vectorNewFile.size();
	for ( int i = 0; i < nVectorNewFileSize; ++i )
	{
		SFILENODE* pNewFile = m_vectorNewFile[i];		

		FILEMAP_ITER found = m_mapDownedFile.find( std::string ( pNewFile->FileName ) );
		FILEMAP_ITER iter_end = m_mapDownedFile.end();

		BOOL bAlreadyDown( FALSE );
		if ( found != iter_end ) bAlreadyDown = TRUE; // Note : �̹� ���������� �ٿ�ε� �Ϸ�� ��.

		if ( IsForceTerminate() ) return FALSE;


		//	<--	ī��Ʈ�� ���� �����ϰ� �ִ°��� �����Ѵ�.	-->	//
		//	���� ù��°���� �ް� ������, ī��Ʈ�� 1�� �Ǵ� ���̴�.
		//	�Ϸ� ������ �ƴ϶�, ���� �����̴�.			
		if ( !bAlreadyDown ) // MEMO : �̹� �ٿ���� ������ ����Ʈ�� ǥ������ �ʴ´�.
		{
			CString	strMsg;
			strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 47 ); 
			// ����ȭ ���� ������ �޼����� ������ �׻� �޸𸮸� ���� �����Ѵ�.
			CHAR * szListMessage = new CHAR[256]; 
			wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );
		}

		ULONGLONG TotalPos = 10 + (++DownCount * 60) / nVectorNewFileSize;			
		NS_LOG_CONTROL::SetProcessAllPosition( TotalPos, 100 );

		CString FullSubPath = pNewFile->SubPath;

		if ( !bAlreadyDown ) //	Note : ���� �ٿ�ε� �ȵ� �͸� GetFile�Ѵ�.
		{
			if ( !GETFILE_USEHTTP ( pHttpPatch, FullSubPath.GetString(), pNewFile->FileName ) )
			{
				return FALSE;
			}
		}

		//	<--	���������� �ٿ�ε� ������ ����Ʈ �ۼ��Ѵ�.	--> //
		m_vectorDownFile.push_back( pNewFile );

		if( IsForceTerminate() ) return FALSE;

		Sleep( 0 ); // ���Ϲ޴ٰ� ���ߴ°� ������
	}

	if ( m_vectorNewFile.size() != m_vectorDownFile.size () )
	{
		return FALSE; // �ٿ���� ���ϰ� ��Ͽ� �ִ� ������ ������ �ٸ��ٸ� ����
	}

	LoadCopyList (); // ��ġ�ߴ� ����Ʈ�� �ε��Ѵ�.

	return TRUE;
}

BOOL CAutoPatchThread::Installing()
{
	CString strTemp;
	int InstallCount = 0;
	int nMaxPos = 0;

	int nVectorDownFileSize = (int)m_vectorDownFile.size();
	for ( int i = 0; i < nVectorDownFileSize; ++i )
	{
		SFILENODE* pNewFile = m_vectorDownFile[i];	

		//	Note : �̹� ���������� ��ġ �Ϸ�� ��.
		FILEMAP_ITER found = m_mapCopiedFile.find ( std::string ( pNewFile->FileName ) );
		FILEMAP_ITER iter_end = m_mapCopiedFile.end ();

		BOOL bAlreadyCopy(FALSE);
		if ( found != iter_end ) bAlreadyCopy = TRUE;

		if ( IsForceTerminate() ) return FALSE;

		CString	Seperator = "\\";
		CString FullSubPath = pNewFile->SubPath;
		CStringArray SubPathArray;

		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( Seperator );
		STRUTIL::StringSeparate ( FullSubPath, SubPathArray );		

		//	<--	���� ���丮 �����	-->	//
		CString FullPath;
		FullPath.Format( "%s", NS_GLOBAL_VAR::strAppPath.GetString() );
		for ( int i = 0; i < SubPathArray.GetCount (); i++ )
		{
			FullPath += SubPathArray[i];			
			CreateDirectory ( FullPath.GetString(), NULL );
			FullPath += "\\";
		}

		//	<--	���� Ǯ������ ��ü ��� ���� �� ���� Ǯ��&��ġ	-->	//
		NS_LOG_CONTROL::SetProcessCurPosition ( 0, 1 );
		{
			if ( !bAlreadyCopy )
			{
				CString	strMsg;
				strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 48 );
				CHAR * szListMessage = new CHAR[256];
				wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
				::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );
			}

			ULONGLONG TotalPos = 70 + (++InstallCount * 20) / nVectorDownFileSize;
			NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );

			if ( !bAlreadyCopy )
			{
				CString	DownloadDir;
				DownloadDir = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownloadTemp + pNewFile->FileName;		

				CString	DestFile;
				DestFile.Format( "%s%s", FullPath.GetString(), pNewFile->FileName );
				DestFile = DestFile.Left ( DestFile.ReverseFind ( '.' ) );

				SetFileAttributes( DestFile.GetString(), FILE_ATTRIBUTE_NORMAL );
				DeleteFile( DestFile.GetString() );

				if( !Extract ( FullPath.GetString(), DownloadDir.GetString() ) )
				{
					DownloadDir.MakeLower();
					DownloadDir = DownloadDir.Left( DownloadDir.ReverseFind( '.' ) );

					if( ( DownloadDir.Find( _T(".dll") ) == -1 ) && ( !IsForceTerminate() ) )
					{
						CHAR * szListMessage = new CHAR[MAX_PATH];
						StringCchCopy ( szListMessage, MAX_PATH, GetErrorMsg() );
						::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

						SetExtractError (); // �����޽��� : �����϶� ��� �õ��Ѵ�. ���ѷ��� ���� ��.��;;

						return FALSE;
					}
				}

				

				SetFileAttributes ( DestFile.GetString(), FILE_ATTRIBUTE_NORMAL );
			}
		}
		NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );

//		if(strstr(pNewFile->SubPath,NS_GLOBAL_VAR::strCompDir)){ // ������ �������� �н��� �˻��ؼ� �˾ƺ���.
//			m_vecCompFile.push_back(pNewFile);
//		}

		//	<--	���������� ��ġ������ ����Ʈ �ۼ��Ѵ�. --> //
		m_vectorCopyFile.push_back ( pNewFile );

		if( IsForceTerminate() ) return FALSE;

		Sleep( 0 );
	}

	if ( m_vectorNewFile.size() != m_vectorCopyFile.size () )
	{
		return FALSE; // MEMO : ������Ʈ�� ���� ������ ��ġ�� ���� ������ ��ġ���� ������...
	}

	return TRUE;
}

BOOL CAutoPatchThread::DeleteNotFoundFile()
{
	CString	str, strCompFile;
	int nCnt = static_cast<int>( m_vectorDeleteFile.size() );

	strCompFile = NS_GLOBAL_VAR::strAppPath + RANPARAM::RPFDataPath;

	m_FileSystem.OpenFileSystem(strCompFile);	


	for ( int i = 0; i < nCnt; i++ )
	{
		SFILENODE* pNewFile = m_vectorDeleteFile[i];

/*
		if(strstr(pNewFile->SubPath,NS_GLOBAL_VAR::strCompDir)){
			CString strSubPath, strSubDir;
			
			strSubPath = pNewFile->SubPath;
			strSubDir = strSubPath.Right( strSubPath.GetLength() - (int)strlen("\\package") ); // \\package ���� ��
			
			m_FileSystem.ChangeDir(strSubDir);
			
			str.Format( "%s",pNewFile->FileName );
			str.MakeLower();
			str = str.Left( str.ReverseFind ( '.' ) );
			
			m_FileSystem.Remove(pNewFile->FileName);			
			
			continue;
		}
*/
		str.Format( "%s%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), pNewFile->SubPath, pNewFile->FileName );
		str.MakeLower();
		str = str.Left( str.ReverseFind ( '.' ) );

		DeleteFile( str.GetString() );
	}

	m_FileSystem.CloseFileSystem();

	return TRUE;
}

BOOL CAutoPatchThread::DeleteDownFiles()
{
	CString	strTemp;
	int nCnt = static_cast<int>( m_vectorDownFile.size() );

	for ( int i = 0; i < nCnt; ++i )
	{
		SFILENODE* pNewFile = m_vectorDownFile[i];
		strTemp.Format( "%s%s%s%s", NS_GLOBAL_VAR::strProFile.GetString(), SUBPATH::SAVE_ROOT, 
						            NS_GLOBAL_VAR::strDownloadTemp.GetString() , pNewFile->FileName );

		DeleteFile( strTemp.GetString() );
	}

	strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownloadTemp;
	RemoveDirectory ( strTemp.GetString() );

	return TRUE;
}


/*
BOOL CAutoPatchThread::DeleteCompressFile()
{
	CString	strTemp;
	int nCnt = static_cast<int>( m_vecCompFile.size() );

	for ( int i = 0; i < nCnt; ++i )
	{
		SFILENODE* pNewFile = m_vecCompFile[i];
		strTemp.Format( "%s%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), pNewFile->SubPath, pNewFile->FileName );
		strTemp.MakeLower();
		strTemp = strTemp.Left( strTemp.ReverseFind ( '.' ) );

		DeleteFile( strTemp.GetString() );
	}

	// directory ���� �ϴ� �κ�
	CString strDir[5];
	strDir[0].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data\\glogic\\level");
	strDir[1].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data\\glogic\\npctalk");
	strDir[2].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data\\glogic\\quest");
	strDir[3].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data\\glogic");
	strDir[4].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data");

	for(int i = 0; i < 5 ; ++i){
		RemoveDirectory(strDir[i].GetString());			
	}

	return TRUE;

}
*/


BOOL CAutoPatchThread::Destroy()
{
	for ( int i = 0; i < (int)m_vectorServerFile.size (); i++ )
	{
		delete m_vectorServerFile[i];
	}
	m_vectorServerFile.clear();

	for ( int i = 0; i < (int)m_vectorNewFile.size (); i++ )
	{
		delete m_vectorNewFile[i];
	}
	m_vectorNewFile.clear();
	m_vectorDownFile.clear(); // MEMO : �ߺ��� �����Ͷ� delete �� �ʿ����.

	// m_mapDownedFile ���� ///////////////////////////
	FILEMAP_ITER iter = m_mapDownedFile.begin ();
	FILEMAP_ITER iter_end = m_mapDownedFile.end ();
	for ( ; iter != iter_end; iter++ )
	{
		delete (*iter).second;
	}
	m_mapDownedFile.clear();
	m_vectorCopyFile.clear();

	// m_mapDownedFile ���� ///////////////////////////
	iter = m_mapCopiedFile.begin ();
	iter_end = m_mapCopiedFile.end ();
	for ( ; iter != iter_end; iter++ )
	{
		delete (*iter).second;
	}
	m_mapCopiedFile.clear();
	m_vectorDeleteFile.clear();
//	m_vecCompFile.clear();

	return TRUE;
}

BOOL CAutoPatchThread::VersionUp( int sGameVer )
{
	CString strTemp;
	strTemp.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), g_szClientVerFile );
	SetFileAttributes( strTemp.GetString(), FILE_ATTRIBUTE_NORMAL );

	FILE* cfp = fopen( strTemp.GetString(), "rb+" );	
	if ( cfp )
	{		
		fseek( cfp, sizeof ( int ) * 1, SEEK_SET );
		if ( 1 != fwrite( &sGameVer, sizeof( int ), 1, cfp ) )
		{
			fclose( cfp );
			return FALSE;
		}
		fclose( cfp );
	}

	return TRUE;
}



/*
BOOL CAutoPatchThread::CompressFile()
{
	CString strCompPath;
	strCompPath = NS_GLOBAL_VAR::strAppPath + RANPARAM::RPFDataPath;

	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 70 ), 0 );

	if(!m_FileSystem.OpenFileSystem(strCompPath)){ // ���� ��ü�� �������
		m_FileSystem.NewFileSystem(strCompPath);
		m_FileSystem.OpenFileSystem(strCompPath);
	}

    TCHAR strFileName[256]; // ���� �̸�
	CString strFilePath; // ���� ��ü���
	CString strSubDir; // ���Ͻý��� subpath
	CString strSubPath; // ���� subpath
	CString strTemp;
	for ( int i = 0; i < (int)m_vecCompFile.size (); i++ )
	{
		strSubPath = m_vecCompFile[i]->SubPath;
		strSubDir = strSubPath.Right( strSubPath.GetLength() - (int)strlen("\\package") ); // \\package �̺κ� ����
		if(!m_FileSystem.ChangeDir(strSubDir)){ // ������ �������� �ʾ��� ��� ���� ����
			m_FileSystem.ChangeDir(m_FileSystem.GetCurDir());

			int count = 0;

			if(strcmp(m_FileSystem.GetCurDir(),"/") == 0){ // ����� �̻� 
				strTemp = strSubDir;	
			}else{
				count =(int) ( strSubDir.GetLength() - strlen(m_FileSystem.GetCurDir()));
				strTemp = strSubDir.Right(count); 
			}
			
			CString token;
			
			int curPos = 0;
			token = strTemp.Tokenize("\\",curPos);
			while(token != ""){
				if(!m_FileSystem.AddDir(token))	return FALSE;
				if(!m_FileSystem.ChangeDir(token)) return FALSE;
				token= strTemp.Tokenize("\\",curPos);
			}			
			
		}

		strFilePath.Format("%s%s%s",NS_GLOBAL_VAR::strAppPath.GetString(),m_vecCompFile[i]->SubPath,m_vecCompFile[i]->FileName);
		strFilePath = strFilePath.Left(strFilePath.ReverseFind('.'));
		m_FileSystem.GetNameOnly(strFileName,strFilePath);
		if(m_FileSystem.CheckNameExist((LPCTSTR)strFileName)) // ���� ������ �ִ��� Ȯ��
		{
			m_FileSystem.Remove(strFileName);
		}

		if(!m_FileSystem.AddFile(LPCTSTR(strFilePath))) // ���� �߰�
		{
			return FALSE;
		}
		if(!m_FileSystem.ChangeDir("/"))  return FALSE;
	}

	m_FileSystem.CloseFileSystem();
	return TRUE;
}
*/
/*
BOOL CAutoPatchThread::OptimizeCompressFile()
{

	CString strCompPath, strPath;
	strCompPath = NS_GLOBAL_VAR::strAppPath + RANPARAM::RPFDataPath;
	strPath = NS_GLOBAL_VAR::strAppPath + "package\\temp.rpf";

	m_FileSystem.OpenFileSystem(strCompPath);

	if(!m_FileSystem.OptimizeFileSystem(strPath)){
		m_FileSystem.CloseFileSystem();
		return FALSE;
	}

	m_FileSystem.CloseFileSystem();
	remove(strCompPath);
	rename(strPath,strCompPath);

	return TRUE;
}
*/

BEGIN_MESSAGE_MAP(CAutoPatchThread, CHttpThread)
END_MESSAGE_MAP()