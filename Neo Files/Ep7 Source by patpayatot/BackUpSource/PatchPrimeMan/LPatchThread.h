#pragma once

struct	S_PATCH_THREAD_PARAM;

class CLPatchThread : public CWinThread
{
	DECLARE_DYNCREATE(CLPatchThread)

public:
	CLPatchThread( LPVOID pParam, const TCHAR * szAppPath, DWORD nThreadID );

// Attributes
protected:
	S_PATCH_THREAD_PARAM * m_pPatchThreadParam; // ������ �������� ���߿� ������ �������� ���� ������?
	TCHAR m_szAppPath[_MAX_PATH];

	BOOL m_bFail;
	BOOL m_bExtractError;

	DWORD m_nDlgThreadID;

public:
	HANDLE m_hEventKill;
	HANDLE m_hEventDead;

// Operations
public:
	void KillThread();

protected:
	BOOL VersionUp( int sPatchProgramVer );
	BOOL DELETEFILE( BOOL bCab = TRUE );
	BOOL DeleteLauncherFile();
	virtual void LauncherPatch(){}

public:
	BOOL IsFail () { return m_bFail; }
	void SetFail () { m_bFail = TRUE; }

	BOOL IsRunning ();
	BOOL IsForceTerminate ();
	
	void SetExtractError ()
	{
		SetFail ();
		m_bExtractError = TRUE;
	}
	BOOL IsExtractError () { return m_bExtractError; }

protected:
	CLPatchThread(){}           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.

public:
	virtual ~CLPatchThread();
	virtual void Delete();

protected:
	virtual BOOL InitInstance();

protected:
	DECLARE_MESSAGE_MAP()
};