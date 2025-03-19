#include "stdafx.h"
#include "AutoPatchMan.h"
#include "AutoPatchManDlg.h"

#include <afxinet.h>
#include "s_CPatch.h"
#include "RANPARAM.h"
#include "DebugSet.h"

#include "GlobalVariable.h"
#include "LogControl.h"
#include "LauncherText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//BOOL	CAutoPatchManDlg::ConnectFtpServer ( CString strFtpAddress )
//{
//	int nRetCode;	
//
//	Sleep( 500 );
//	return FALSE;
//	
//    
//#ifdef	__ALPHA__
//	nRetCode = m_pFtpPatch->Connect( strFtpAddress.GetString (),
//						21,
//						g_szAlphaFtpID,
//						g_szAlphaFtpPW,
//						RANPARAM::bUsePassiveDN );
//#else
//	nRetCode = m_pFtpPatch->Connect( strFtpAddress.GetString (),
//						21,
//						NS_GLOBAL_VAR::g_szBetaFtpID,
//						NS_GLOBAL_VAR::g_szBetaFtpPW,
//						RANPARAM::bUsePassiveDN );
//#endif
//    	
//	if (nRetCode == NET_ERROR)
//	{
//		return FALSE;
//	}
//	return TRUE;
//}
//
//BOOL	CAutoPatchManDlg::DisconnectFtpServer ()
//{
//	if ( m_pFtpPatch )
//	{
//		m_pFtpPatch->DisConnect();		
//	}
//
//	return TRUE;
//}

void	CAutoPatchManDlg::InitDlgText()
{
	SetDlgItemText(IDC_TITLE_STATIC,ID2LAUNCHERTEXT("IDC_TITLE_STATIC"));
	SetDlgItemText(IDC_BUTTON_START,ID2LAUNCHERTEXT("IDC_BUTTON_START"));
	SetDlgItemText(IDC_BUTTON_RETRY,ID2LAUNCHERTEXT("IDC_BUTTON_RETRY"));
	SetDlgItemText(IDC_READYTOSTART,ID2LAUNCHERTEXT("IDC_READYTOSTART"));
	SetDlgItemText(IDC_BUTTON_PATCH,ID2LAUNCHERTEXT("IDC_BUTTON_PATCH"));
	SetDlgItemText(IDC_CHECK_USE_HTTP,ID2LAUNCHERTEXT("IDC_CHECK_USE_HTTP"));
	SetDlgItemText(IDC_BUTTON_OPTION,ID2LAUNCHERTEXT("IDC_BUTTON_OPTION"));
	SetDlgItemText(IDC_BUTTON_EXIT,ID2LAUNCHERTEXT("IDC_BUTTON_EXIT"));
}

void	CAutoPatchManDlg::SetAppPath ()
{
	// Note : ���������� ��θ� ã�Ƽ� �����Ѵ�.
	//
	CString strAppPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		strAppPath = strCommandLine.Left ( strCommandLine.ReverseFind ( '\\' ) );
		
		if ( !strAppPath.IsEmpty() )
		if ( strAppPath.GetAt(0) == '"' )
			strAppPath = strAppPath.Right ( strAppPath.GetLength() - 1 );

        strAppPath += '\\';
		NS_GLOBAL_VAR::strAppPath = strAppPath.GetString();		
	}
	else 
	{
		MessageBox ( "SetAppPath Error", "Error", MB_OK );
		return;
	}

	TCHAR szPROFILE[MAX_PATH] = {0};
	SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );	
	NS_GLOBAL_VAR::strProFile = szPROFILE;
}

int	CAutoPatchManDlg::CheckVersion ()
{
	m_sPatchVer= m_pNetClient->GetPatchVer ();
	m_sGameVer = m_pNetClient->GetGameVer ();
	
	if (m_sPatchVer == E_CHK_VER_NOT_READY)
	{
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 27 ) );
		return E_CHK_VER_NOT_READY; // ���� ���� ����
	}

	//	NOTE
	//		������ Ȯ���ϰ�
	//		�ٷ� ��ü�� ������
	//		�α��μ���(?)�� ��� �پ��ְԵǴ°� ����
	SAFE_DELETE ( m_pNetClient );

	if ( m_sPatchVer == E_CHK_VER_SERVER_UPDATE )
	{
		// ���� ������
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 8 ) );
		return E_CHK_VER_SERVER_UPDATE;
	}

	//	NOTE
	//		�ܺ� �������̽����� ������ �ε��ϴ� �κ��� �ֽ��ϴ�.
	//		����� ��� �ݵ�� üũ �ؾ��մϴ�.
	CString strTemp;
	strTemp.Format ( "%s%s", NS_GLOBAL_VAR::strAppPath, g_szClientVerFile );
	FILE* cfp = fopen ( strTemp.GetString(), "rb" );
	if( !cfp )
	{
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 9 ) );
		return E_MSG_FAILED;
	}

	if ( 1 != fread ( &m_cPatchVer, sizeof ( int ), 1, cfp ) )
	{
		fclose ( cfp );	
		
		// ���� ���� �б� ����
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 9 ) );
		return E_MSG_FAILED;
	}

	if ( 1 != fread ( &m_cGameVer, sizeof ( int ), 1, cfp ) )
	{
		fclose ( cfp );	
		
		ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 9 ) );
		return E_MSG_FAILED;
	}

	fclose ( cfp );

	////////////////////////////////////////////////////////////////////
	//	Launcher ��ġ
	if ( m_sPatchVer <= m_cPatchVer )	m_emPatchState = E_VERSION_CUR;
	else								m_emPatchState = E_VERSION_UP;
	////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////
	//	���� ����Ÿ ��ġ
	if ( m_sGameVer <= m_cGameVer )		m_emGameState = E_VERSION_CUR;
	else								m_emGameState = E_VERSION_UP;
	///////////////////////////////////////////////////////////////////

	return E_MSG_SUCCEED;	
}