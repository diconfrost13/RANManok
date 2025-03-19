#include "StdAfx.h"
#include "Resource.h"
#include "patchprime.h"

#include <afxinet.h>
#include "s_NetGlobal.h"
#include "s_NetClient.h"
#include "s_CConsoleMessage.h"
#include "s_CPatch.h"
#include "CompactFdi.h"
#include "RANPARAM.h"
#include "s_CHttpPatch.h"
#include "PatchThread.h"
#include "GlobalVariable.h"
#include "LogControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//char	g_szLauncherCabFile[] = "launcher4d.exe.cab";
extern	char*	g_szClientVerFile;
extern	char	g_szAppPath[1024];

DWORD	PatchByFTP ( S_PATCH_THREAD_PARAM* pParam )
{
	return PatchByHTTP ( pParam );
}

DWORD	PatchByHTTP ( S_PATCH_THREAD_PARAM* pParam )
{
	//CPatch*		pFtpPatch = pParam->pFtpPatch;
	CHttpPatch* pHttpPatch = pParam->pHttpPatch;
	const bool bUseHttp = pParam->bUseHttp;
	//const int cPatchVer = pParam->cPatchVer;
	const int sPatchVer = pParam->sPatchVer;
	//const int cGameVer = pParam->cGameVer;
	//const int sGameVer = pParam->sGameVer;

	CString str;

	//pFtpPatch->SetLocalDir ( g_szAppPath );

	// ������ ���� �ڵ�
	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) return 0;

	if ( bUseHttp )
	{
		NS_LOG_CONTROL::Write ( IDS_MESSAGE_011 ); // ������ �ٿ�ε�

		if ( !GETFILE_USEHTTP ( pHttpPatch, "\\", NS_GLOBAL_VAR::g_szLauncherCabFile, "" ) )
		{
			if ( !NS_PATCH_THREAD::IsForceTerminate () )
			{
				NS_LOG_CONTROL::Write ( IDS_MESSAGE_020 ); // �ٿ�ε� ����
				NS_PATCH_THREAD::SetFail (); // MEMO : ó������ �ٽ� �ٿ�޾ƾ� �Ѵ�.
			}

			goto LFail;
		}
		else NS_LOG_CONTROL::Write ( IDS_MESSAGE_013 ); // �ٿ�ε� ����
	}
	//else
	//{
	//	//	Ran Auto Patch ��������
	//	//	
	//	pFtpPatch->SetCurrentDirectory ( "\\" );	//	��Ʈ�� �̵�
	//	if ( pFtpPatch->GetFile ( NS_GLOBAL_VAR::g_szLauncherCabFile ) == NET_ERROR )
	//	{
	//		NS_LOG_CONTROL::Write ( IDS_MESSAGE_012 );
	//		NS_PATCH_THREAD::SetFail ();
	//		return 0;
	//	}
	//}

	// ������ ���� �ڵ�
	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) goto LFail;

	//	������ Ran Auto Patch ���� ( *.exe )
	//	
	DELETEFILE(FALSE); // cab Ȯ���ڰ� ������� false

	//	�� ���� ��ġ
	NS_LOG_CONTROL::Write ( IDS_MESSAGE_014 ); // ���� ��ġ

	str.Format ( "\"%s%s\"", g_szAppPath, NS_GLOBAL_VAR::g_szLauncherCabFile );
	if ( !Extract ( g_szAppPath, str.GetString() ) )
	{
		NS_PATCH_THREAD::SetExtractError(); // Cab ���� Ǯ�� ����

		if ( !NS_PATCH_THREAD::IsForceTerminate () )
		{
			NS_LOG_CONTROL::Write ( IDS_MESSAGE_012 ); // ��ġ ����
			NS_PATCH_THREAD::SetFail ();
		}

		goto LFail;
	}
	else NS_LOG_CONTROL::Write ( IDS_MESSAGE_015 ); // ��ġ ����

	// ������ ���� �ڵ�
	//
	if ( NS_PATCH_THREAD::IsForceTerminate () ) goto LFail;

	//	Ran Auto Patch ���� �ø���
	//
	if ( !VersionUp( sPatchVer ) )
	{		
		NS_LOG_CONTROL::Write ( IDS_MESSAGE_016 ); // ���� ���� ����
		goto LFail;
	}
	else NS_LOG_CONTROL::Write ( IDS_MESSAGE_017 ); // ��ġ ����

	DELETEFILE(); // �ٿ�ε� �� Ran Auto Patch ���� ( *.exe.cab )

	return 0;

LFail:
	DELETEFILE(); // �ٿ�ε� �� Ran Auto Patch ���� ( *.exe.cab )

	//str.Format ( "\"%s%s\"", g_szAppPath, NS_GLOBAL_VAR::g_szLauncherCabFile );
	//str = str.Left ( str.ReverseFind ( '.' ) );

	//CString	Error;
	//Error.Format ( " PatchError" );
	//str += Error;

	//memset ( &si, 0, sizeof ( STARTUPINFO ) );
	//si.cb = sizeof ( STARTUPINFO );
	//si.dwFlags = 0;	
	//CreateProcess ( NULL, str.GetString(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi );

	return 0;
}

BOOL	DELETEFILE(BOOL bCab)
{
	CString strTemp;
	strTemp.Format ( "%s%s", g_szAppPath, NS_GLOBAL_VAR::g_szLauncherCabFile );
	
	if( !bCab ) 
	{
		strTemp = strTemp.Left ( strTemp.ReverseFind ( '.' ) );
	}
	
	SetFileAttributes ( strTemp.GetString(), FILE_ATTRIBUTE_NORMAL );

	return DeleteFile ( strTemp.GetString() );
}

BOOL	VersionUp ( int sPatchProgramVer )
{	
	CString str;

	str.Format ( "%s%s", g_szAppPath, g_szClientVerFile );
	SetFileAttributes ( str.GetString(), FILE_ATTRIBUTE_NORMAL );	

	FILE* cfp = fopen ( str.GetString(), "rb+" );	
	if ( !cfp ) return FALSE;

	if ( 1 != fwrite ( &sPatchProgramVer, sizeof ( int ), 1, cfp ) )
	{
		fclose ( cfp );
		return FALSE;
	}
	fclose ( cfp );
	return TRUE;
}

BOOL	GETFILE_USEHTTP ( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir )
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
	replace ( strRemoteSubPath.begin(), strRemoteSubPath.end(), '\\','/' );	
	strRemoteSubPath += strFileName;
	
	std::string strLocalFullPath;
	strLocalFullPath += g_szAppPath;	
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
		if ( NS_PATCH_THREAD::IsForceTerminate () ) return FALSE;		
        
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
			NS_LOG_CONTROL::Write ( pHttpPatch->GetErrMsg() );
			nTRY++;
			nERROR_RETRY++;
			continue;
		}
		
		if ( NET_ERROR == pHttpPatch->GetFile ( strRemoteSubPath.c_str(), strLocalFullPath.c_str() ) )
		{
			NS_LOG_CONTROL::Write ( pHttpPatch->GetErrMsg() );
			nTRY ++;
			nERROR_RETRY++;
			continue;
		}

		//	���� �����
		if ( NS_PATCH_THREAD::IsForceTerminate () ) return FALSE;	

		ULONGLONG ulRECEIVED, ulTOTALSIZE;
		NS_LOG_CONTROL::GetProcessCurPosition ( &ulRECEIVED, &ulTOTALSIZE );
		
		if ( ulRECEIVED != ulTOTALSIZE )
		{
			nTRY_FILESIZE_CHECK++;
			NS_LOG_CONTROL::Write ( IDS_MESSAGE_018 );			
			continue;
		}
		else
		{
			// NET_OK, ������ ��
			return TRUE;
		}        
	}

	return FALSE;
}

BOOL MAKE_ALL_FILE_NORMAL_ATTRIBUTE ( CString strPath )
{	
	//	��� ���Ͽ� ���ؼ� �˻縦 ������.
	if ( !strPath.GetLength () ) return FALSE;
	if ( strPath[strPath.GetLength()-1] != '\\') strPath += "\\";
	strPath += "*.*";

	CFileFind finder;	
	BOOL bWorking = finder.FindFile ( strPath.GetString () );

	CString strFilePath;
	while (bWorking)
	{
		if ( NS_PATCH_THREAD::IsForceTerminate () )	return FALSE;

		bWorking = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!
		if (finder.IsDots())
			continue;

		strFilePath = finder.GetFilePath ();

		if ( !SetFileAttributes ( strFilePath.GetString(), FILE_ATTRIBUTE_NORMAL ) )
		{
		}

		if ( finder.IsDirectory () )
		{
			if ( !MAKE_ALL_FILE_NORMAL_ATTRIBUTE ( strFilePath ) )
			{
				finder.Close ();
				return FALSE;
			}
		}
		else
		{
		}
	}
	finder.Close ();

	return TRUE;
}