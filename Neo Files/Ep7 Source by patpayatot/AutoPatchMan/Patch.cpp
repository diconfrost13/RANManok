#include "stdafx.h"
#include "Resource.h"
#include "Patch.h"

#include "PatchThread.h"

#include "dxutil.h"
#include <afxinet.h>
#include "s_NetClient.h"
#include "s_CPatch.h"
#include "CompactFdi.h"
#include "StringUtils.h"
#include "patchset.h"

#include "GlobalVariable.h"
#include "RANPARAM.h"
#include "LogControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define		_MAKE_PATCH_STATUS // MEMO : ����� ����

FILEVECTOR		g_vectorServerFile;
CPatchSet		g_ClientFileTree;
FILEVECTOR		g_vectorNewFile;
FILEVECTOR		g_vectorDeleteFile;
FILEVECTOR		g_vectorDownFile;
FILEMAP			g_mapDownedFile;

FILEVECTOR		g_vectorCopyFile;
FILEMAP			g_mapCopiedFile;

int		g_sFailGameVer = 0;


BOOL	MakeNewList ( const int cPatchVer, const int sPatchVer,
					 const int cGameVer, const int sGameVer )
{	
	NS_LOG_CONTROL::SetProcessCurPosition ( 0, 1 );

#ifdef	_MAKE_PATCH_STATUS
	CDebugSet::ClearFile ("_STATUS.txt");
	CDebugSet::ClearFile ("_ALL_STATUS.txt");
	CDebugSet::ClearFile ("_TEST_filelist.lst");
	static	CString strDIRBACK;
	strDIRBACK.Empty ();
	int	nVER_DOWN_COUNT = 0;
	int nVER_NORMAL = 0;

	CDebugSet::ToFileWithTime ( "_ALL_STATUS.txt", "BEGIN-----------------------------------" );
	CDebugSet::ToFileWithTime ( "_ALL_STATUS.txt", "PatchVer - c:%d/s:%d", cPatchVer, sPatchVer );
	CDebugSet::ToFileWithTime ( "_ALL_STATUS.txt", "GameVer - c:%d/s:%d", cGameVer, sGameVer );

	CDebugSet::ToFileWithTime ( "_STATUS.txt", "PatchVer - c:%d/s:%d", cPatchVer, sPatchVer );
	CDebugSet::ToFileWithTime ( "_STATUS.txt", "GameVer - c:%d/s:%d", cGameVer, sGameVer );

	CDebugSet::ToFileWithTime ( "_TEST_filelist.lst", "PatchVer - c:%d/s:%d", cPatchVer, sPatchVer );
	CDebugSet::ToFileWithTime ( "_TEST_filelist.lst", "GameVer - c:%d/s:%d", cGameVer, sGameVer );
#endif	//	_MAKE_PATCH_STATUS

	FILEVECTOR::size_type nServerFileSize = g_vectorServerFile.size ();
	for ( FILEVECTOR::size_type i = 0; i < nServerFileSize; i++ )
	{	
		SFILENODE* pServerGetFile = g_vectorServerFile[i];
#ifdef	_MAKE_PATCH_STATUS
		if ( strDIRBACK != pServerGetFile->SubPath )
		{
			CDebugSet::ToFile ( "_TEST_filelist.lst", "\n%s", pServerGetFile->SubPath );
			strDIRBACK = pServerGetFile->SubPath;
		}
#endif	//	_MAKE_PATCH_STATUS

        CPatchSetNode* pNode = g_ClientFileTree.Find ( pServerGetFile );
		if ( pNode )	//	���� ����
		{
			SFILENODE* pClientFile = pNode->GetFile ();
			pNode->SetUseFlag ();

			if ( pClientFile->Ver < pServerGetFile->Ver )
			{
				SFILENODE* pNewFile = new SFILENODE;
				*pNewFile = *pServerGetFile;
				g_vectorNewFile.push_back ( pNewFile );

#ifdef	_MAKE_PATCH_STATUS
				CDebugSet::ToFile ( "_ALL_STATUS.txt", "[VersinoUp]%s {c:%d<s:%d}",
					pServerGetFile->FileName, pClientFile->Ver, pServerGetFile->Ver );

				CString strFileName = pServerGetFile->FileName;
				strFileName = strFileName.Left ( strFileName.ReverseFind ( '.' ) );
				CDebugSet::ToFile ( "_TEST_filelist.lst", "\t%s", strFileName.GetString() );
#endif	//	_MAKE_PATCH_STATUS
			}
#ifdef	_MAKE_PATCH_STATUS
			else
			{
				if ( pClientFile->Ver == pServerGetFile->Ver )
				{
					nVER_NORMAL++;
					CDebugSet::ToFile ( "_ALL_STATUS.txt", "[��ȭ����]%s {c:%d==s:%d}",
					pClientFile->FileName, pClientFile->Ver, pServerGetFile->Ver );
				}
				else
				{
					nVER_DOWN_COUNT++;
					CDebugSet::ToFile ( "_ALL_STATUS.txt", "[��������]%s {c:%d>s:%d}",
					pClientFile->FileName, pClientFile->Ver, pServerGetFile->Ver );
				}
			}
#endif	//	_MAKE_PATCH_STATUS
		}
		else	//	�߰��Ǵ� ����
		{			
			SFILENODE* pNewFile = new SFILENODE;
			*pNewFile = *pServerGetFile;
			g_vectorNewFile.push_back ( pNewFile );
#ifdef	_MAKE_PATCH_STATUS
			CDebugSet::ToFile ( "_ALL_STATUS.txt", "[Addition]%s {%d}",
				pServerGetFile->FileName, pServerGetFile->Ver );

			CString strFileName = pServerGetFile->FileName;
			strFileName = strFileName.Left ( strFileName.ReverseFind ( '.' ) );
			CDebugSet::ToFile ( "_TEST_filelist.lst", "\t%s", strFileName );
#endif	//	_MAKE_PATCH_STATUS
		}

		NS_LOG_CONTROL::SetProcessCurPosition ( i, nServerFileSize );

		//	��������
		if ( NS_PATCH_THREAD::IsForceTerminate () )	return FALSE;
	}
	NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );

	//	<--	������ ���� ������ Ŭ���̾�Ʈ�� ������ ��� ����	-->	//
	g_ClientFileTree.GetNotUseItem ( &g_vectorDeleteFile );

#ifdef	_MAKE_PATCH_STATUS
	for ( int i = 0; i < (int)g_vectorDeleteFile.size (); i++ )
	{
		SFILENODE* pFileNode = g_vectorDeleteFile[i];
		CDebugSet::ToFile ( "_ALL_STATUS.txt", "[����]%s", pFileNode->FileName );
	}
	
	//CDebugSet::ToFile ( "_STATUS.txt", "��ϵ� ���� ����\t: c:%d-s:%d", g_ClientFileTree.GetCount (), g_vectorServerFile.size() );
	CDebugSet::ToFile ( "_STATUS.txt", "---------------" );
	CDebugSet::ToFile ( "_STATUS.txt", "������Ʈ[������/�߰�]\t: %d", g_vectorNewFile.size() );
	CDebugSet::ToFile ( "_STATUS.txt", "����\t\t\t: %d", g_vectorDeleteFile.size () );
	CDebugSet::ToFile ( "_STATUS.txt", "��������\t\t: %d", nVER_NORMAL );
	CDebugSet::ToFile ( "_STATUS.txt", "�����ٿ�\t\t: %d", nVER_DOWN_COUNT );
	CDebugSet::ToFile ( "_STATUS.txt", "---------------" );
	CDebugSet::ToFile ( "_STATUS.txt", "��ü[%d]\n = ������Ʈ[%d] + ����[%d] + ��������[%d] + �����ٿ�[%d]",
		g_vectorServerFile.size (), g_vectorNewFile.size(), g_vectorDeleteFile.size (), nVER_NORMAL, nVER_DOWN_COUNT );	
	CDebugSet::ToFile ( "_STATUS.txt", "-----------------------------------" );

	int nSUM = (int)g_vectorNewFile.size() + (int)g_vectorDeleteFile.size () + nVER_NORMAL + nVER_DOWN_COUNT;
	if ( (int)g_vectorServerFile.size () == nSUM )
		CDebugSet::ToFile ( "_STATUS.txt", "����");
	else
		CDebugSet::ToFile ( "_STATUS.txt", "���� - ���� ���� �ٸ�");

#endif	//	_MAKE_PATCH_STATUS

	LoadDownList ();	

	return TRUE;
}

BOOL DownloadFilesByHttp ( CHttpPatch* pHttpPatch )
{
	int DownCount = 0;

	BOOL	bFirst = TRUE;
	SFILENODE* pOldFile = NULL;

	// Temp �����н��� �����Ѵ�.
	//
	CString	DownloadDir;
	DownloadDir = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;

	int nErrorCount = 0;
	const int nMaxError = 5;

	for ( int i = 0; i < (int) g_vectorNewFile.size (); ++i )
	{
		nErrorCount = 0;
		SFILENODE* pNewFile = g_vectorNewFile[i];		
		
		//	Note : �̹� ���������� �ٿ�ε� �Ϸ�� ��.
        BOOL bAlreadyDown = FALSE;
		FILEMAP_ITER found = g_mapDownedFile.find ( std::string ( pNewFile->FileName ) );
		FILEMAP_ITER iter_end = g_mapDownedFile.end ();
		if ( found != iter_end ) bAlreadyDown = TRUE;

		//	<--	'��������'	-->	//
		if ( NS_PATCH_THREAD::IsForceTerminate () )
		{
			return FALSE;
		}
		
		{
			//	<--	ī��Ʈ�� ���� �����ϰ� �ִ°��� �����Ѵ�.	-->	//
			//	<**	���� ù��°���� �ް� ������, ī��Ʈ�� 1�� �Ǵ� ���̴�.
			//		�Ϸ� ������ �ƴ϶�, ���� �����̴�.			
			DownCount++;
			//	**>

			if ( !bAlreadyDown ) // MEMO : �̹� �ٿ���� ������ ����Ʈ�� ǥ������ �ʴ´�.
			{
				CString	strTemp, strMsg;
				strMsg.LoadString( IDS_MESSAGE_047 );
				strTemp.Format ( "%s %s", strMsg.GetString(), pNewFile->FileName );

				NS_LOG_CONTROL::Write ( strTemp );	
			}

			ULONGLONG TotalPos = 10 + (DownCount * 60)/(int)g_vectorNewFile.size();			
			NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );
		}

		CString FullSubPath = pNewFile->SubPath;

		//	Note : ���� �ٿ�ε� �ȵ� �͸� GetFile�Ѵ�.
		//
		if ( !bAlreadyDown )
		{
#ifdef	_MAKE_PATCH_STATUS
			CDebugSet::ToFile ( "_ALL_STATUS.txt", "[���۽���]%s", pNewFile->FileName );
#endif	//	_MAKE_PATCH_STATUS
			if ( !GETFILE_USEHTTP ( pHttpPatch, FullSubPath.GetString(), pNewFile->FileName ) )
			{
#ifdef	_MAKE_PATCH_STATUS
				if ( !NS_PATCH_THREAD::IsForceTerminate () )
				{
					CDebugSet::ToFile ( "_ALL_STATUS.txt", "[���ۿ���]" );
				}
#endif	//	_MAKE_PATCH_STATUS
				return FALSE;
			}
#ifdef	_MAKE_PATCH_STATUS
			CDebugSet::ToFile ( "_ALL_STATUS.txt", "[���ۿϷ�]" );
#endif	//	_MAKE_PATCH_STATUS
		}
		else
		{
#ifdef	_MAKE_PATCH_STATUS
            CDebugSet::ToFile ( "_ALL_STATUS.txt", "[�̹̹�������]%s", pNewFile->FileName );
#endif	//	_MAKE_PATCH_STATUS
		}	

		if ( NS_PATCH_THREAD::IsForceTerminate () )
		{
			return FALSE;
		}

		//	<--	���������� �ٿ�ε� ������
		//		����Ʈ �ۼ��Ѵ�.
		//	-->
		g_vectorDownFile.push_back ( pNewFile );
		pOldFile = pNewFile;	

		//	NOTE
		//		���Ϲ޴ٰ� ���ߴ°� ������
		Sleep( 0 );
	}

    if ( g_vectorNewFile.size() != g_vectorDownFile.size () )
	{
		return FALSE;
	}

	LoadCopyList (); // ��ġ�ߴ� ����Ʈ�� �ε��Ѵ�.

	return TRUE;
}

//BOOL DownloadFilesByFtp ( CPatch* pFtpPatch )
//{
//	int DownCount = 0;
//
//	BOOL	bFirst = TRUE;
//	SFILENODE* pOldFile = NULL;
//
//	CString	DownloadDir;
//	DownloadDir = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
//	pFtpPatch->SetLocalDir ( DownloadDir.GetString () );
//
//	int nErrorCount = 0;
//	const int nMaxError = 5;
//
//	for ( int i = 0; i < (int) g_vectorNewFile.size (); ++i )
//	{
//		nErrorCount = 0;
//		SFILENODE* pNewFile = g_vectorNewFile[i];		
//		
//		//	Note : �̹� ���������� �ٿ�ε� �Ϸ�� ��.
//        BOOL bAlreadyDown = FALSE;
//		FILEMAP_ITER found = g_mapDownedFile.find ( std::string ( pNewFile->FileName ) );
//		FILEMAP_ITER iter_end = g_mapDownedFile.end ();
//		if ( found != iter_end ) bAlreadyDown = TRUE;
//
//		//	<--	'��������'	-->	//
//		if ( NS_PATCH_THREAD::IsForceTerminate () )
//		{
//			return FALSE;
//		}
//		else
//		{
//			//	<--	ī��Ʈ�� ���� �����ϰ� �ִ°��� �����Ѵ�.	-->	//
//			//	<**	���� ù��°���� �ް� ������, ī��Ʈ�� 1�� �Ǵ� ���̴�.
//			//		�Ϸ� ������ �ƴ϶�, ���� �����̴�.			
//			DownCount++;
//			//	**>
//
//			CString	strTemp, strMsg;
//			strMsg.LoadString( IDS_MESSAGE_047 );
//			strTemp.Format ( "%s %s", strMsg.GetString(), pNewFile->FileName );			
//			ULONGLONG TotalPos = 10 + (DownCount * 60)/(int)g_vectorNewFile.size();			
//			
//			NS_LOG_CONTROL::Write ( strTemp );	
//			NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );
//		}
//
//		if ( bFirst || strcmp ( pOldFile->SubPath, pNewFile->SubPath ) )
//		{
//			CString FullSubPath = pNewFile->SubPath;
//
//			//	��Ʈ�� �̵�
//			pFtpPatch->SetCurrentDirectory ( "\\" );
//			pFtpPatch->SetCurrentDirectory ( FullSubPath.GetString() );
//
//			bFirst = FALSE;
//		}
//		
//		//	Note : ���� �ٿ�ε� �ȵ� �͸� GetFile�Ѵ�.
//		//
//		if ( !bAlreadyDown )
//		{
//			//	��õ�
//			while ( nErrorCount != nMaxError )
//			{	
//				if ( pFtpPatch->GetFile ( pNewFile->FileName ) == NET_ERROR )
//				{
//					//	<--	'��������'	-->	//
//					if ( NS_PATCH_THREAD::IsForceTerminate () )
//					{
//						return FALSE;
//					}						
//
//					if ( nMaxError == nErrorCount )
//					{
//						return FALSE;
//					}
//
//					{	//	�� ����							
//						pFtpPatch->DisConnect ();
//
//						CString strFtpAddress;
//						int nIndex = nErrorCount % RANPARAM::MAX_FTP;
//						strFtpAddress = RANPARAM::FtpAddressTable[nIndex];
//
//						int nRetCode = pFtpPatch->Connect( strFtpAddress.GetString (),
//											21,
//											NS_GLOBAL_VAR::g_szBetaFtpID,
//											NS_GLOBAL_VAR::g_szBetaFtpPW,
//											RANPARAM::bUsePassiveDN );
//
//						if ( nRetCode == NET_ERROR )
//						{
//							return FALSE;
//						}
//
//
//						CString FullSubPath = pNewFile->SubPath;
//
//						//	��Ʈ�� �̵�
//						pFtpPatch->SetCurrentDirectory ( "\\" );
//						pFtpPatch->SetCurrentDirectory ( FullSubPath.GetString() );
//					}
//
//					nErrorCount++;
//
//					continue ;
//				}
//				break;
//			}				
//		}
//
//		//	<--	���������� �ٿ�ε� ������
//		//		����Ʈ �ۼ��Ѵ�.
//		//	-->
//		g_vectorDownFile.push_back ( pNewFile );
//		pOldFile = pNewFile;		
//	}
//
//    if ( g_vectorNewFile.size() != g_vectorDownFile.size () )
//	{
//		return FALSE;
//	}
//
//	return TRUE;
//}

BOOL	Installing ()
{
	int InstallCount = 0;

	for ( int i = 0; i < (int) g_vectorDownFile.size (); ++i )
	{
		SFILENODE* pNewFile = g_vectorDownFile[i];	

		//	Note : �̹� ���������� �ٿ�ε� �Ϸ�� ��.
		BOOL bAlreadyCopy(FALSE);
		FILEMAP_ITER found = g_mapCopiedFile.find ( std::string ( pNewFile->FileName ) );
		FILEMAP_ITER iter_end = g_mapCopiedFile.end ();
		if ( found != iter_end ) bAlreadyCopy = TRUE;

		//	<--	'��������'	-->	//
		if ( NS_PATCH_THREAD::IsForceTerminate () )
		{
			return FALSE;
		}

		CString	Seperator = "\\";
		CString FullSubPath = pNewFile->SubPath;
		CStringArray SubPathArray;

		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( Seperator );
		STRUTIL::StringSeparate ( FullSubPath, SubPathArray );		

		//	<--	���� ���丮 �����	-->	//
		CString FullPath;
		FullPath.Format ( "%s", NS_GLOBAL_VAR::strAppPath.GetString () );
		for ( int i = 0; i < SubPathArray.GetCount (); i++ )
		{
			FullPath += SubPathArray[i];			
			CreateDirectory ( FullPath.GetString (), NULL );
			FullPath += "\\";
		}

		//	<--	���� Ǯ������ ��ü ��� ���� �� ���� Ǯ��&��ġ	-->	//
		//NS_PATCH_THREAD::SleepThread ();
		//NS_PATCH_THREAD::SetProcessCurPosition ( 0, 1 );
		NS_LOG_CONTROL::SetProcessCurPosition ( 0, 1 );
		{
			InstallCount++;

			if ( !bAlreadyCopy )
			{
				CString	strTemp, strMsg;
				strMsg.LoadString( IDS_MESSAGE_048 );
				strTemp.Format ( "%s %s", strMsg.GetString(), pNewFile->FileName );
				NS_LOG_CONTROL::Write ( strTemp );
			}
			
			ULONGLONG TotalPos = 70 + (InstallCount * 20) / g_vectorDownFile.size();
			NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );

			if ( !bAlreadyCopy )
			{
				CString	DownloadDir;
				DownloadDir = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp + pNewFile->FileName;		

				CString	DestFile;
				DestFile.Format ( "%s%s", FullPath.GetString(), pNewFile->FileName );
				DestFile = DestFile.Left ( DestFile.ReverseFind ( '.' ) );

				if ( !SetFileAttributes ( DestFile.GetString(), FILE_ATTRIBUTE_NORMAL ) )
				{
				}
				if ( !DeleteFile ( DestFile.GetString() ) )
				{
				}

	#ifdef	_MAKE_PATCH_STATUS
				CDebugSet::ToFileWithTime ( "_ALL_STATUS.txt", "[��ġ]%s", DestFile.GetString () );
	#endif	//	_MAKE_PATCH_STATUS

				if ( !Extract ( FullPath.GetString (), DownloadDir.GetString() ) )
				{
					if ( !NS_PATCH_THREAD::IsForceTerminate () )
					{

	#ifdef	_MAKE_PATCH_STATUS
						CDebugSet::ToFileWithTime ( "_ALL_STATUS.txt", "[��ġ����]%s{%s}", DestFile.GetString (), GetErrorMsg () );
	#endif	//	_MAKE_PATCH_STATUS

						NS_LOG_CONTROL::Write ( GetErrorMsg () );
						//	�����޽���				
						NS_PATCH_THREAD::SetExtractError ();
					}
					return FALSE;
				}

				if ( !SetFileAttributes ( DestFile.GetString(), FILE_ATTRIBUTE_NORMAL ) )
				{
				}
			}
		}
		NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );

		//	<--	���������� ��ġ������
		//		����Ʈ �ۼ��Ѵ�.
		//	-->
		g_vectorCopyFile.push_back ( pNewFile );

		//	<--	'��������'	-->	//
		if ( NS_PATCH_THREAD::IsForceTerminate () ) return FALSE;
	}

#ifdef	_MAKE_PATCH_STATUS
	CDebugSet::ToFileWithTime ( "_ALL_STATUS.txt", "END---------------" );
#endif	//	_MAKE_PATCH_STATUS

	if ( g_vectorNewFile.size() != g_vectorCopyFile.size () )
	{
		return FALSE; // MEMO : ������Ʈ�� ���� ������ ��ġ�� ���� ������ ��ġ���� ������...
	}

	return TRUE;
}


//DWORD	PatchByFTP ( S_PATCH_THREAD_PARAM* pParam )
//{
//	CPatch*		pFtpPatch = pParam->pFtpPatch;
////	CHttpPatch* pHttpPatch = pParam->pHttpPatch;
//	const bool bUseHttp = pParam->bUseHttp;
//	const int cPatchVer = pParam->cPatchVer;
//	const int sPatchVer = pParam->sPatchVer;
//	const int cGameVer = pParam->cGameVer;
//	const int sGameVer = pParam->sGameVer;
//	
//	NS_LOG_CONTROL::SetProcessAllPosition ( 0, 100 );
//
//	CString cFileList, sFileList, str, strTemp;
//
//	if ( NS_PATCH_THREAD::IsForceTerminate () )
//	{
//		return 0;
//	}
//	
//	//	���� ����Ʈ �ٿ�ε�	
//	pFtpPatch->SetLocalDir ( NS_GLOBAL_VAR::strAppPath.GetString () );
//	NS_LOG_CONTROL::Write ( IDS_MESSAGE_049 );	
//	pFtpPatch->SetCurrentDirectory ( "\\" );
//	if ( pFtpPatch->GetFile ( NS_GLOBAL_VAR::strServerCabFileList ) == NET_ERROR )
//	{
//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_050 );
//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_056 );
//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_063 );
//		goto LFail;
//	}
//
//	if ( NS_PATCH_THREAD::IsForceTerminate () )
//	{
//		goto LFail;
//	}
//
//	//	���� Ǯ������ ��ü ��� ���� �� ���� Ǯ��&��ġ
//	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strServerCabFileList;
//	NS_LOG_CONTROL::Write ( IDS_MESSAGE_051 );	
//	if ( !Extract ( NS_GLOBAL_VAR::strAppPath.GetString (), str.GetString() ) )
//	{
//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_052 );		
//		goto LFail;
//	}
//	NS_LOG_CONTROL::SetProcessAllPosition ( 4, 100 );
//
//	Initialize ();
//
//	if ( NS_PATCH_THREAD::IsForceTerminate () )
//	{
//		goto LFail;
//	}
//
//	//	���� ����Ʈ �� �� �� ��� �ۼ�
//	NS_LOG_CONTROL::Write ( IDS_MESSAGE_053 );
//	if ( !LoadList () )
//	{
//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_054 );	
//		goto LFail;
//	}
//
//	if ( NS_PATCH_THREAD::IsForceTerminate () )
//	{
//		goto LFail;
//	}
//	
//	NS_LOG_CONTROL::Write ( IDS_MESSAGE_057 );	
//	if ( !MakeNewList ( cPatchVer, sPatchVer, cGameVer, sGameVer ) )
//	{
//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_058 );		
//		goto LFail;
//	}    
//
//	NS_LOG_CONTROL::SetProcessAllPosition ( 10, 100 );
//
//	if ( NS_PATCH_THREAD::IsForceTerminate () )
//	{
//		goto LFail;
//	}
//
//	//	�� ��Ͽ� ���� ���� �ٿ� �ε�
//	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
//	CreateDirectory ( str.GetString (), NULL );
//	if ( !DownloadFilesByFtp ( pFtpPatch ) )
//	{			
//		if ( !NS_PATCH_THREAD::IsForceTerminate () )
//		{
//			NS_LOG_CONTROL::Write ( IDS_MESSAGE_055 );
//		}
//		goto LFail;
//	}
//	NS_LOG_CONTROL::SetProcessAllPosition ( 70, 100 );
//
//	if ( NS_PATCH_THREAD::IsForceTerminate () )
//	{
//		goto LFail;
//	}
//
//	//	���� ����
//	if ( !Installing() )
//	{	
//		//	���� ���ᰡ �ƴ� ��¥ ����
//		//
//		if ( NS_PATCH_THREAD::IsExtractError() )
//		{
//			NS_LOG_CONTROL::Write ( IDS_MESSAGE_059 );
//
//			//	Note : DS list ����
//			//
//			strTemp.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::strDownList );
//			DeleteFile ( strTemp.GetString () );			
//		}
//
//		goto LFail;
//	}
//	NS_LOG_CONTROL::SetProcessAllPosition ( 90, 100 );
//
//    //	<--	Ŭ���̾�Ʈ ��ϸ� ����������� ��ü	-->	//
//	cFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::g_szClientFileList );
//	sFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::g_szServerFileList );	
//	
//	DeleteFile ( cFileList.GetString () );
//	MoveFile ( sFileList.GetString (), cFileList.GetString () );
//
//	//	Note : DS list ����
//	//	
//	strTemp.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::strDownList );
//	DeleteFile ( strTemp.GetString () );
//	
//	sFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::strServerCabFileList );
//	DeleteFile ( sFileList.GetString () );
//
//	DeleteDownFiles();
//	DeleteNotFoundFile();
//	
//	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
//	RemoveDirectory ( str.GetString () );
//	
//    Destroy ();	
//	
//	NS_LOG_CONTROL::Write ( IDS_MESSAGE_060 );
//	if ( !VersionUp ( sGameVer ) )
//	{
//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_061 );
//		goto LFail;
//	}
//	
//	NS_LOG_CONTROL::Write ( IDS_MESSAGE_062 );
//	NS_LOG_CONTROL::SetProcessAllPosition ( 100, 100 );
//
//	return 0;
//
//LFail:
//	//	�ٿ�ε��� ���ϵ��� �ջ�Ǿ� ������ ��쿡��
//	//	����Ʈ�� ó������ �ٽ� �ۼ��ؾ��ϱ⶧����
//	//	����Ʈ�� �������� �ʴ´�.
//	if ( NS_PATCH_THREAD::IsExtractError () )
//	{
//		if ( NS_PATCH_THREAD::IsForceTerminate () )
//		{
//			SaveDownList ( sGameVer );
//		}
//	}
//	else
//	{
//		SaveDownList ( sGameVer );
//	}
//
//	sFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::g_szServerFileList );
//	DeleteFile ( sFileList.GetString () );
//	sFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::strServerCabFileList );
//	DeleteFile ( sFileList.GetString () );	
//
////	DeleteDownFiles ();
//    Destroy ();
//
////	str.Format ( "%s%s", strAppPath.GetString (), g_szDownloadTemp );
////	RemoveDirectory ( str.GetString () );
//
//	sFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::g_szServerFileList );	
//	DeleteFile ( sFileList.GetString () );
//	
//	NS_PATCH_THREAD::SetFail ();
//
//	return 0;
//}

DWORD	PatchByHTTP ( S_PATCH_THREAD_PARAM* pParam )
{
	CPatch*		pFtpPatch = pParam->pFtpPatch;
	CHttpPatch* pHttpPatch = pParam->pHttpPatch;
	const bool bUseHttp = pParam->bUseHttp;
	const int cPatchVer = pParam->cPatchVer;
	const int sPatchVer = pParam->sPatchVer;
	const int cGameVer = pParam->cGameVer;
	const int sGameVer = pParam->sGameVer;	
	
	CString cFileList, sFileList, str, strTemp;
	BOOL bInstalling(FALSE);

	NS_LOG_CONTROL::SetProcessAllPosition ( 0, 100 );

	//	<--	'��������'	-->	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) return 0;
	
	//	���� ����Ʈ �ٿ�ε�
	NS_LOG_CONTROL::Write ( IDS_MESSAGE_049 ); // ����Ʈ ������
	if ( !GETFILE_USEHTTP ( pHttpPatch, "\\", NS_GLOBAL_VAR::strServerCabFileList.GetString(), "" ) )
	{	
		if ( !NS_PATCH_THREAD::IsForceTerminate () )
		{
			NS_LOG_CONTROL::Write ( IDS_MESSAGE_050 ); // ����Ʈ ������ ����
		}

		goto LFail;
	}

	//	<--	'��������'	-->	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) goto LFail;
	
	//	���� Ǯ������ ��ü ��� ���� �� ���� Ǯ��&��ġ
	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strServerCabFileList;
	NS_LOG_CONTROL::Write ( IDS_MESSAGE_051 ); // ����Ʈ ���� Ǯ�� ����
	if ( !Extract ( NS_GLOBAL_VAR::strAppPath.GetString (), str.GetString() ) )
	{
		NS_LOG_CONTROL::Write ( IDS_MESSAGE_052 ); // ����Ʈ ����Ǯ�� ����
		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 4, 100 );

	//Initialize (); // MEMO : �Լ� ���� �ȵ�.

	//	<--	'��������'	-->	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) goto LFail;

	//	���� ����Ʈ �� �� �� ��� �ۼ�
	NS_LOG_CONTROL::Write ( IDS_MESSAGE_053 ); // ��ġ ���� ����Ʈ �ε�
	if ( !LoadList () )
	{
		NS_LOG_CONTROL::Write ( IDS_MESSAGE_054 ); // ����Ʈ �ε忡 ����
		goto LFail;
	}

	//	<--	'��������'	-->	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) goto LFail;
	
	NS_LOG_CONTROL::Write ( IDS_MESSAGE_057 ); // ��ġ ���� ����Ʈ �� ����
	if ( !MakeNewList ( cPatchVer, sPatchVer, cGameVer, sGameVer ) )
	{
		NS_LOG_CONTROL::Write ( IDS_MESSAGE_058 ); // ����Ʈ �񱳿� ����
		goto LFail;
	}    

	NS_LOG_CONTROL::SetProcessAllPosition ( 10, 100 );

	//	<--	'��������'	-->	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) goto LFail;
	
	//	�� ��Ͽ� ���� ���� �ٿ� �ε�
	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
	CreateDirectory ( str.GetString (), NULL );
	if ( !DownloadFilesByHttp ( pHttpPatch ) )
	{
		if ( !NS_PATCH_THREAD::IsForceTerminate () )
		{
			NS_LOG_CONTROL::Write ( IDS_MESSAGE_055 ); // ��ġ ���� ������ ����
		}

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 70, 100 );

	//	<--	'��������'	-->	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) goto LFail;

	//	���� ����
	if ( !Installing() )
	{	
		//	���� ���ᰡ �ƴ� ��¥ ����
		//
		if ( NS_PATCH_THREAD::IsExtractError() )
		{
			NS_LOG_CONTROL::Write ( IDS_MESSAGE_059 ); // ��ġ ������ �Ϻ� �ջ�

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

	//CDebugSet::ToLogFile ( "����" );
	//NS_LOG_CONTROL::Write ( "���� ���࿡ �ʿ��� ���� ���Ἲ�� �˻��մϴ�." );
	//ULONGLONG ulServerFile = (ULONGLONG)g_vectorServerFile.size ();
	//NS_LOG_CONTROL::SetProcessCurPosition ( 0, ulServerFile );
	//if ( !CheckIntegrity ( NS_GLOBAL_VAR::strAppPath ) )
	//{
	//	if ( !NS_PATCH_THREAD::IsForceTerminate () )
	//	{
	//		NS_LOG_CONTROL::Write ( "���� ���࿡ �ʿ��� ������ �Ϻΰ� �ջ�Ǿ����ϴ�." );
	//	}

	//	goto LFail;
	//}
	//NS_LOG_CONTROL::SetProcessCurPosition ( ulServerFile, ulServerFile );
	//CDebugSet::ToLogFile ( "��" );

    //	<--	Ŭ���̾�Ʈ ��ϸ� ����������� ��ü	-->	//
	cFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szClientFileList;
	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList;
	
	DeleteFile ( cFileList.GetString () );
	MoveFile ( sFileList.GetString (), cFileList.GetString () );

	//	Note : DS list ����
	//	
	strTemp = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownList;
	DeleteFile ( strTemp.GetString () );

	// Note : ī�� ����Ʈ ����
	//
	strTemp = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strCopyList;
	DeleteFile ( strTemp.GetString () );
	
	sFileList.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString (), NS_GLOBAL_VAR::strServerCabFileList );
	DeleteFile ( sFileList.GetString () );

	DeleteDownFiles();
	DeleteNotFoundFile();
	
	str = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strDownloadTemp;
	RemoveDirectory ( str.GetString () );
	
    Destroy ();	
	
	NS_LOG_CONTROL::Write ( IDS_MESSAGE_060 ); // Ŭ���̾�Ʈ ���� ����
	if ( !VersionUp ( sGameVer ) )
	{
		NS_LOG_CONTROL::Write ( IDS_MESSAGE_061 ); // Ŭ���̾�Ʈ ���� ���� ����
		goto LFail;
	}
	
	NS_LOG_CONTROL::Write ( IDS_MESSAGE_062 ); // ��� ��ġ ����
	NS_LOG_CONTROL::SetProcessAllPosition ( 100, 100 );

	return 0;

LFail:
	//	�ٿ�ε��� ���ϵ��� �ջ�Ǿ� ������ ��쿡��
	//	����Ʈ�� ó������ �ٽ� �ۼ��ؾ��ϱ⶧����
	//	����Ʈ�� �������� �ʴ´�.
	if ( NS_PATCH_THREAD::IsExtractError () )
	{
		if ( NS_PATCH_THREAD::IsForceTerminate () )
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
	DeleteFile ( sFileList.GetString () );
	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::strServerCabFileList;
	DeleteFile ( sFileList.GetString () );	

//	DeleteDownFiles ();
    Destroy ();

//	str.Format ( "%s%s", strAppPath.GetString (), g_szDownloadTemp );
//	RemoveDirectory ( str.GetString () );

	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList;
	DeleteFile ( sFileList.GetString () );
	
	NS_PATCH_THREAD::SetFail ();

	return 0;
}