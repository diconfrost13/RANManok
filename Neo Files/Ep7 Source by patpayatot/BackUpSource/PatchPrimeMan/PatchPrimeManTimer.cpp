#include "stdafx.h"
#include "PatchPrimeMan.h"
#include "PatchPrimeManDlg.h"

#include <afxinet.h>
#include "s_NetGlobal.h"
#include "s_NetClient.h"
#include "s_CConsoleMessage.h"
#include "s_CPatch.h"
#include "RANPARAM.h"
#include ".\patchprimemandlg.h"
#include "s_CHttpPatch.h"
#include "LogControl.h"
#include "GlobalVariable.h"
#include "LPatchThread.h"
#include "HttpPatchThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CPatchPrimeManDlg::OnTimer(UINT nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	switch ( nIDEvent )
	{
	//case E_FTP_CONNECT:
	//	{
	//		UPDATE_TIME ( nFTP_CONNECT_TIME_ELAPSE );

	//		UpdateControls ();
	//		UpdateFtpConnect ();
	//	}	
	//	break;

	case E_START_PATCH:
		{
			UpdateControls(); // ���α׷����� ���� ������Ʈ
			
			if ( m_pThread->IsForceTerminate() )	//	Canceled
			{
				KillTimer ( E_START_PATCH );
			}
			else if ( !m_pThread->IsRunning() )	//	Not Running
			{
				KillTimer ( E_START_PATCH );

				if ( m_pThread->IsFail () )	//	Failed
				{
					//	���� ����Ǯ�⸦ ������ ���, ó������ �ٽ� �����Ѵ�.
					//	����Ǯ�⿡ �����ߴٴ°���, Ŭ���̾�Ʈ�� �ٿ�� ������ �ջ�Ǿ����� �ǹ��Ѵ�.					
					if ( m_pThread->IsExtractError() )
					{
						OnKillThread(); // �����带 ���̰�...
						BEGIN_PATCH(); // �ٽ� ��ġ ����... 
						// MEMO : ������ �����ϸ� ��� �ݺ��ȴ�. ������ �ھ߰ڴ�.
					}
				}
				else //	Succeed
				{
					END_PATCH ();
					PostMessage( WM_QUIT ); // OnOK, SendMessage, WM_CLOSE ���� ���� ī�尡 0�� �ƴϴ�.
				}
			}			
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

//void	CPatchPrimeManDlg::BEGIN_FTP_THREAD ()
//{
//	const int nIndex = m_nFtpTry % RANPARAM::MAX_FTP;
//
//	static S_FTP_THREAD_PARAM sParam;
//	sParam.pFtpPatch = m_pFtpPatch;
//	sParam.strFtpAddress = RANPARAM::FtpAddressTable[nIndex];
//	
//	m_nFtpTry++;
//	NS_FTP_THREAD::BEGIN ( &sParam );
//	SET_TIME ( NS_GLOBAL_VAR::nFTP_CONNECT_TIMELEFT * n1000_MILISEC );
//	SetTimer ( E_FTP_CONNECT, nFTP_CONNECT_TIME_ELAPSE, NULL );
//}

//void CPatchPrimeManDlg::UpdateFtpConnect ()
//{
//	//	NOTE
//	//		��������
//	if ( NS_FTP_THREAD::IsForceTerminate () )	//	Canceled
//	{	
//		//	NOTE
//		//		���⿡�� �ϴ°��� ������,
//		//		����, �����尡 �������ε� �ɷȴٸ�,
//		//		���α׷��� �����ó�� �������� ������,
//		//		���� ������ �������� END�۾��� �̷�д�.
//		//NS_FTP_THREAD::END (); 
//		KillTimer ( E_FTP_CONNECT );
//		return ;
//	}
//
//	//	NOTE
//	//		������ ���
//	if ( NS_FTP_THREAD::IsFail () )
//	{
//		//	Ÿ�̸� & ������ ����
//		NS_FTP_THREAD::STOP ();
//		NS_FTP_THREAD::END ();
//		KillTimer ( E_FTP_CONNECT );	
//
//		//	�ִ� �õ��� ���� ���
//		if ( NS_GLOBAL_VAR::nFTP_TRY == m_nFtpTry )
//		{
//			NS_LOG_CONTROL::Write ( IDS_MESSAGE_005 );
//			return ;
//		}
//
//		BEGIN_FTP_THREAD ();
//
//		return ;
//	}
//
//	//	NOTE
//	//		���� �õ�
//	if ( NS_FTP_THREAD::IsRunning () )
//	{
//		const UINT nLeftTime = GET_TIME ();
//		if ( 1 <= nLeftTime )
//		{
//			static	int	LeftSecBack = -1;
//			static	int	FtpTryBack= -1;
//
//			const UINT nLeftSec = (UINT)(nLeftTime / n1000_MILISEC);
//
//			if ( nLeftSec != LeftSecBack || m_nFtpTry != FtpTryBack )
//			{
//				CString	strTemp;
//				CString strMsg;
//				strMsg.LoadString( IDS_MESSAGE_006 );
//				strTemp.Format ( "%s %d sec [%d/%d]", strMsg.GetString(),
//					nLeftSec, m_nFtpTry, NS_GLOBAL_VAR::nFTP_TRY );
//				NS_LOG_CONTROL::Write ( strTemp );
//				
//				LeftSecBack = nLeftSec;
//				FtpTryBack= m_nFtpTry;
//			}
//		}
//		else
//		{			
//			//	Ÿ�̸� & ������ ����
//			NS_FTP_THREAD::STOP ();				 
//			NS_FTP_THREAD::END ();
//			KillTimer ( E_FTP_CONNECT );		
//
//			NS_LOG_CONTROL::Write ( IDS_MESSAGE_007 );
//		}
//
//		return ;
//	}
//
//	//	Ÿ�̸� & ������ ����
//	NS_FTP_THREAD::END ();
//	KillTimer ( E_FTP_CONNECT );		
//
//	NS_LOG_CONTROL::Write ( IDS_MESSAGE_008 );
//	BEGIN_PATCH ();
//}