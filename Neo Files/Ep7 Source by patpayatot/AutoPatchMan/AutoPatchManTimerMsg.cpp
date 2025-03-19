// AutoPatchManDlg.cpp : ���� ����
//
#include "stdafx.h"
#include "AutoPatchMan.h"
#include "AutoPatchManDlg.h"

#include "ServerLoginThread.h"
#include "FtpThread.h"
#include "GlobalVariable.h"

#include "DebugSet.h"
#include "RANPARAM.h"
#include "LogControl.h"
#include "LauncherText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CAutoPatchManDlg::UpdateLoginConnect ()
{
	if( m_pLoginThread )
	{
		if( m_pLoginThread->IsForceTerminate () ) // �������� Canceled
		{	
			KillTimer ( E_LOGIN_CONNECT );
			return ;
		}

		if( !m_pLoginThread->IsRunning() && m_pLoginThread->IsFail() ) // ������ ���
		{
			KillTimer ( E_LOGIN_CONNECT );

			//	�ִ� �õ��� ���� ���
			if ( NS_GLOBAL_VAR::nLOGIN_TRY == m_nLoginTry )
			{
				ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 13 )  ); // �ִ� ����ȸ�� �ʰ�
			#ifdef CH_PARAM
				ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 64 )  );
				m_ctrlRetry.EnableWindow( TRUE );
				m_ctrlOption.EnableWindow( TRUE );
			#endif
				return ;
			}

			EndThread( m_pLoginThread );
			BEGIN_LOGIN_THREAD (); // �ٽ� �α��� �õ�

			return ;
		}

		if( m_pLoginThread->IsRunning () ) // ���� �õ�
		{
			const UINT nLeftTime = GET_TIME ();
			if( 1 <= nLeftTime )
			{
				static	int	LeftSecBack = -1;
				static	int	LoginTryBack= -1;

				const UINT nLeftSec = (UINT)(nLeftTime / n1000_MILISEC);

				if ( nLeftSec != LeftSecBack || m_nLoginTry != LoginTryBack )
				{
					CString	strTemp, strMsg;
					strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 14 );  // ���� �õ���
					strTemp.Format ( "%s %d sec [%d/%d]", strMsg.GetString(), nLeftSec, m_nLoginTry, NS_GLOBAL_VAR::nLOGIN_TRY );
					ListAddString( strTemp );

					LeftSecBack = nLeftSec;
					LoginTryBack= m_nLoginTry;
				}
			}
			else
			{			
				//	Ÿ�̸� & ������ ����
				KillTimer( E_LOGIN_CONNECT );
				EndThread( m_pLoginThread );

				ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 15 )  ); // ���� �ð� �ʰ�
			}

			return ;
		}
	}

	// note : �������� ���� ����
	// Ÿ�̸� & ������ ����
	KillTimer( E_LOGIN_CONNECT );
	EndThread( m_pLoginThread );

	ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 16 )  ); // ���� ����, ���� ���ۿ� ����
	SetTimer ( E_CHECK_VERSION, nCHECK_VERSION_TIME_ELAPSE, NULL );

	return;
}

void CAutoPatchManDlg::UpdateCheckVersion ()
{
	switch ( CheckVersion () )
	{	
	case E_CHK_VER_NOT_READY:		//	���� ������ ���� ����
	case E_CHK_VER_SERVER_UPDATE:	//	���� ������
	case E_MSG_FAILED:				//	���� �� ����
		{
			KillTimer ( E_CHECK_VERSION );

		#ifdef CH_PARAM
			ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 64 )  );
			m_ctrlRetry.EnableWindow( TRUE );
			m_ctrlOption.EnableWindow( TRUE );
		#endif
		}
		break;

		//	���� ���� ����
	case E_MSG_SUCCEED:
		{
			KillTimer ( E_CHECK_VERSION );

			//	Patch��, Game�� ��ġ�Ǿ�� �ϴ°�?
			if ( m_emPatchState != E_VERSION_CUR || m_emGameState != E_VERSION_CUR )
			{	
				//	NOTE
				//		����� ������ Http�� ���°����� �Ƚ�
				m_bUseHttp = true;
				if ( m_bUseHttp )
				{
					BEGIN_PATCH ();
				}
			}		
			else
			{
			#ifdef CH_PARAM
				{
					static S_PATCH_THREAD_PARAM sParam;
					sParam.pHttpPatch = m_pHttpPatch;
					sParam.cPatchVer = m_cPatchVer;
					sParam.sPatchVer = m_sPatchVer;
					sParam.cGameVer = m_cGameVer;
					sParam.sGameVer = m_sGameVer;
					BeginThread( E_THREAD_HASH, m_pHashThread, &sParam );
					SetTimer( E_CHECK_HASH, 30, NULL );
				}
			#else
				END_PATCH (); // �ƹ� ��ġ�� �ʿ���� ���
			#endif
			}
		}
		break;
	}
}

void	CAutoPatchManDlg::BEGIN_LOGIN_THREAD ()
{
	++m_nLoginTry;
	BeginThread( E_THREAD_LOGIN, m_pLoginThread, m_pNetClient, RANPARAM::LoginAddress );
	SET_TIME( NS_GLOBAL_VAR::nLOGIN_CONNECT_TIMELEFT * n1000_MILISEC );
	SetTimer( E_LOGIN_CONNECT, nLOGIN_CONNECT_TIME_ELAPSE, NULL );
}

void CAutoPatchManDlg::OnTimer(UINT nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    switch ( nIDEvent )
	{
	case E_LOGIN_CONNECT:
		{
			UPDATE_TIME ( nLOGIN_CONNECT_TIME_ELAPSE );

			UpdateControls ();
			UpdateLoginConnect ();
		}
		break;

	case E_CHECK_VERSION:
		{
			UpdateControls ();
			UpdateCheckVersion ();
		}
		break;

	case E_START_PATCH:
		{
			UpdateControls();

			if( m_pPatchThread )
			{
				if( m_pPatchThread->IsForceTerminate() )
				{
					KillTimer ( E_START_PATCH );
					break;
				}
				
				if( !m_pPatchThread->IsRunning() )
				{
					KillTimer ( E_START_PATCH );
					if( m_pPatchThread->IsFail() )
					{
						if ( m_pPatchThread->IsExtractError() )
						{
							//	���� ����Ǯ�⸦ ������ ��� ó������ �ٽ� �����Ѵ�.
							//	����Ǯ�⿡ �����ߴٴ°��� Ŭ���̾�Ʈ�� �ٿ�� ������ �ջ�Ǿ����� �ǹ��Ѵ�.
							//	���� DLL ���� ��ġ�� �ȵǴ� ������ �ִٸ� BEGIN_PATCH()�� �� ���
							//	���� ������ ������. �׷��� ������ �ּ� ó���� �صξ���.
							//BEGIN_PATCH ();
						}

						#ifdef CH_PARAM
							ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 64 )  );
							m_ctrlRetry.EnableWindow( TRUE );
							m_ctrlOption.EnableWindow( TRUE );
						#endif
					}
					else // Succeed
					{
					#ifdef CH_PARAM
						{
							KillTimer ( E_START_PATCH );

							static S_PATCH_THREAD_PARAM sParam;
							sParam.pHttpPatch = m_pHttpPatch;
							sParam.cPatchVer = m_cPatchVer;
							sParam.sPatchVer = m_sPatchVer;
							sParam.cGameVer = m_cGameVer;
							sParam.sGameVer = m_sGameVer;
							BeginThread( E_THREAD_HASH, m_pHashThread, &sParam );
							SetTimer( E_CHECK_HASH, 30, NULL );
						}
					#else
						END_PATCH (); // MEMO : ���� ��ư�� ������ ������ ���ᰡ �ƴϴ�.

						if( m_ctrlCheckBox.GetCheck() == BST_CHECKED )
						{
							OnBnClickedButtonStart();
						}
					#endif
					}
				}
			}
		}
		break;

	case E_CHECK_HASH:
		{
			UpdateControls();

			if( m_pHashThread )
			{
				if( m_pHashThread->IsForceTerminate() )
				{
					KillTimer ( E_CHECK_HASH );
					break;
				}

				if( !m_pHashThread->IsRunning() )
				{
					KillTimer ( E_CHECK_HASH );
					if( m_pHashThread->IsFail() )
					{
						if ( m_pHashThread->IsExtractError() )
						{
						}

						#ifdef CH_PARAM
							ListAddString( ID2LAUNCHERTEXT("IDS_MESSAGE", 64 )  );
							m_ctrlRetry.EnableWindow( TRUE );
							m_ctrlOption.EnableWindow( TRUE );
						#endif
					}
					else // Succeed
					{
						END_PATCH ();

						if( m_ctrlCheckBox.GetCheck() == BST_CHECKED )
						{
							OnBnClickedButtonStart();
						}
					}
				}
			}
		}
		break;

	case E_WEB_BROWSER:
		{
			m_ctrlNotify.Navigate( m_strNewsURL.GetString(), NULL, NULL, NULL, NULL );
			KillTimer( E_WEB_BROWSER );
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}