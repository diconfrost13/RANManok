#include "stdafx.h"
#include "Resource.h"
#include "PatchThread.h"

#include "PatchPrime.h"
#include "LogControl.h"

#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace	NS_PATCH_THREAD
{
	HANDLE	hThread = NULL;
	HANDLE	hTerminateEvent = NULL;
	//DWORD	dwThreadID = 0;
	unsigned threadID;
	BOOL	bExtractError = FALSE;
	BOOL	bFail = FALSE;

	BOOL	BEGIN ( S_PATCH_THREAD_PARAM* pParam )
	{			
		if ( hTerminateEvent || hThread )
		{
			return FALSE; // �̹� �̺�Ʈ�� �ڵ��� �����Ѵٸ�...
		}

        hTerminateEvent = CreateEvent  ( NULL, TRUE, TRUE, NULL );
		if( hTerminateEvent == NULL ) return FALSE;

		hThread = (HANDLE)_beginthreadex( NULL, 0, PatchThread, pParam, 0, &threadID );
		if( hThread == NULL )
		{
			CloseHandle( hTerminateEvent );
			return FALSE;
		}

		return TRUE;
	}

	void	INIT ()
	{
		if( hTerminateEvent )
		{
			CloseHandle ( hTerminateEvent );
			hTerminateEvent = NULL;
		}

		if ( hThread )
		{
			CloseHandle ( hThread );
			hThread = NULL;
		}

		bExtractError = FALSE;
		bFail = FALSE;
	}

	void	STOP ()
	{
		ResetEvent ( hTerminateEvent );
	}

	void	END ()
	{
		if ( hThread )
		{
			const DWORD	miliSec = 1000L;
			if ( WaitForSingleObject ( hThread, 1 * miliSec ) == WAIT_TIMEOUT )
			{
				//NS_LOG_CONTROL::Write ( IDS_MESSAGE_019 ); // ���� ����
				TerminateThread ( hThread, 1L ); // MEMO : �̰� �´°ɱ�...?
			}

			CloseHandle ( hThread );
			hThread = NULL;
		}

		if( hTerminateEvent )
		{
			CloseHandle ( hTerminateEvent );
			hTerminateEvent = NULL;
		}
	}

	BOOL	IsRunning ()
	{
		if ( hThread )
		{
			BOOL bRun = FALSE;
			bRun = WaitForSingleObject ( hThread, 0 ) != WAIT_OBJECT_0;
			return bRun;
		}
		return FALSE;
	}

	BOOL	IsForceTerminate ()
	{
		return WaitForSingleObject ( hTerminateEvent, 0 ) == WAIT_TIMEOUT;
	}

	void	SetExtractError ()
	{
		SetFail ();
		bExtractError = TRUE;
	}

	BOOL	IsExtractError ()
	{
		return bExtractError;
	}

	BOOL	IsFail ()
	{
		return bFail;
	}

	void	SetFail ()
	{
		bFail = TRUE;
	}
};