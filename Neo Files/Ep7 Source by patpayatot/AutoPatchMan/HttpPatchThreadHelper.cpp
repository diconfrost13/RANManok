#include "StdAfx.h"
#include ".\httppatchthread.h"
#include "Resource.h"

#include "LogControl.h"
#include "StringUtils.h"
#include "CompactFdi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL CHttpPatchThread::LoadList()
{
	//	<--	����Ʈ �б�	-->	//
	CString str;
	FILE* fp;

	str.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::g_szServerFileList );
	fp = fopen( str.GetString(), "rb" );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;
	//	NOTE
	//		ù��° ���� ���� [������ �ƹ��� �ϵ� ���� ����]
	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		return FALSE;
	}

	//	NOTE
	//		�ι�° ���� ����
	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		return FALSE;
	}	

	CString strTemp;

	for ( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE* pNewFile = new SFILENODE;
		if ( 1 != fread ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			return FALSE;
		}

		strTemp = pNewFile->FileName;
		strTemp = strTemp.Left( strTemp.ReverseFind( '.' ) );
		strTemp.MakeUpper();

		if( strTemp.Find( ".DLL" ) != -1 )
		{
			//CDebugSet::ToLogFile( "���� ���� ����Ʈ�� [%s]�� �ֽ��ϴ�.", pNewFile->FileName );
			m_vectorServerFile.push_back( pNewFile );
		}
		else
		{
			m_vectorServerFile.push_back( pNewFile );
		}
	}
	fclose ( fp );

	if( IsForceTerminate() ) return FALSE;

	str.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::g_szClientFileList );
	fp = fopen( str.GetString(), "rb" );
	if ( !fp )
	{
		return FALSE;
	}

	//	NOTE
	//		ù��° ���� ���� [������ �ƹ��� �ϵ� ���� ����]
	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		return FALSE;
	}

	//	NOTE
	//		�ι�° ���� ����
	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		return FALSE;
	}	

	for ( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE NewFile;
		if ( 1 != fread( &NewFile, sizeof( SFILENODE ), 1, fp ) )
		{
			return FALSE;
		}
		m_ClientFileTree.Add( &NewFile ); 
	}
	fclose ( fp );

	return TRUE;
}

BOOL CHttpPatchThread::MakeNewList( const int cPatchVer, const int sPatchVer, const int cGameVer, const int sGameVer )
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

BOOL CHttpPatchThread::DownloadFilesByHttp( CHttpPatch* pHttpPatch )
{
	int DownCount = 0;

	// Temp �����н��� �����Ѵ�.
	//
	CString	DownloadDir( NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp );

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
		DownCount++;

		if ( !bAlreadyDown ) // MEMO : �̹� �ٿ���� ������ ����Ʈ�� ǥ������ �ʴ´�.
		{
			CString	strMsg;
			strMsg.LoadString( IDS_MESSAGE_047 );
			// ����ȭ ���� ������ �޼����� ������ �׻� �޸𸮸� ���� �����Ѵ�.
			CHAR * szListMessage = new CHAR[256]; 
			wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );
		}

		ULONGLONG TotalPos = 10 + (DownCount * 60) / (int)m_vectorNewFile.size();			
		NS_LOG_CONTROL::SetProcessAllPosition( TotalPos, 100 );
		
		CString FullSubPath = pNewFile->SubPath;

		if ( !bAlreadyDown ) //	Note : ���� �ٿ�ε� �ȵ� �͸� GetFile�Ѵ�.
		{
			if ( !GETFILE_USEHTTP ( pHttpPatch, FullSubPath.GetString(), pNewFile->FileName ) )
			{
				return FALSE;
			}
		}

		if( IsForceTerminate() ) return FALSE;

		//	<--	���������� �ٿ�ε� ������ ����Ʈ �ۼ��Ѵ�.	--> //
		m_vectorDownFile.push_back( pNewFile );

		Sleep( 0 ); // ���Ϲ޴ٰ� ���ߴ°� ������
	}

	if ( m_vectorNewFile.size() != m_vectorDownFile.size () )
	{
		return FALSE; // �ٿ���� ���ϰ� ��Ͽ� �ִ� ������ ������ �ٸ��ٸ� ����
	}

	LoadCopyList (); // ��ġ�ߴ� ����Ʈ�� �ε��Ѵ�.

	return TRUE;
}

BOOL CHttpPatchThread::Installing()
{
	CString strTemp;
	int InstallCount = 0;

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
			InstallCount++;

			if ( !bAlreadyCopy )
			{
				CString	strMsg;
				strMsg.LoadString( IDS_MESSAGE_048 );
				CHAR * szListMessage = new CHAR[256];
				wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
				::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );
			}

			ULONGLONG TotalPos = 70 + (InstallCount * 20) / m_vectorDownFile.size();
			NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );

			if ( !bAlreadyCopy )
			{
				CString	DownloadDir;
				DownloadDir = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp + pNewFile->FileName;		

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

BOOL CHttpPatchThread::DeleteNotFoundFile()
{
	CString	str;
	int nCnt = static_cast<int>( m_vectorDeleteFile.size() );

	for ( int i = 0; i < nCnt; i++ )
	{
		SFILENODE* pNewFile = m_vectorDeleteFile[i];

		str.Format( "%s%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), pNewFile->SubPath, pNewFile->FileName );
		str.MakeLower();
		str = str.Left( str.ReverseFind ( '.' ) );

		if( str.Find( ".dll" ) == -1 )
		{
			if( !DeleteFile( str.GetString() ) ) return FALSE;
		}
	}

	return TRUE;
}

BOOL CHttpPatchThread::DeleteDownFiles()
{
	CString	strTemp;
	int nCnt = static_cast<int>( m_vectorDownFile.size() );

	for ( int i = 0; i < nCnt; ++i )
	{
		SFILENODE* pNewFile = m_vectorDownFile[i];
		strTemp.Format( "%s%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::strDownloadTemp.GetString(), pNewFile->FileName );

		if( !DeleteFile( strTemp.GetString() ) ) return FALSE;
	}

	return TRUE;
}

BOOL CHttpPatchThread::Destroy()
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

	return TRUE;
}

BOOL CHttpPatchThread::VersionUp( int sGameVer )
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