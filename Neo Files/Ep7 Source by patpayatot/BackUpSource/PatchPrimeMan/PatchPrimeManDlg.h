// PatchPrimeManDlg.h : ��� ����
//

#pragma once
#include "afxcmn.h"

class	CHttpPatch;
class	CPatch;
class	CLPatchThread;

class CPatchPrimeManDlg : public CDialog
{
// ����
public:
	CPatchPrimeManDlg(CWnd* pParent = NULL);	// ǥ�� ������
// ��ȭ ���� ������
	enum { IDD = IDD_PATCHPRIMEMAN_DIALOG };

private:

	//	Ÿ�̸� ID
	enum
	{
		//E_LOGIN_CONNECT = 1,
		//E_CHECK_VERSION = 2,
		E_FTP_CONNECT	= 3,
		E_START_PATCH	= 4,
		E_FORCE_EXIT	= 5,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

protected:
	void	SetAppPath ();
	void	WaitForLauncherClose();

private:
	void	DeleteInvalidFile ();

private:	
	//CPatch*		m_pFtpPatch;
	CHttpPatch*	m_pHttpPatch;
	bool		m_bUseHttp;

	CLPatchThread * m_pThread; // ��ġ ������

private:
	int		m_nFtpTry;			//	FTP �õ�
	BOOL	m_bForceTerminate;	//	���� ���� �õ�

private:
	int		m_sPatchVer;
	int		m_sGameVer;

private:
	void	UpdateControls();
//	void	UpdateFtpConnect ();

//private:
//	void	BEGIN_FTP_THREAD ();

private:
	void	BEGIN_PATCH ();
	void	END_PATCH ();
	
//private:	
	//BOOL	ConnectServer ( CString strFtpAddress );
	//BOOL	DisconnectServer ();

private:
	BOOL CREATEPROCESS();
	void OnKillThread();

public:
	void ListAddString( const CString & strLog );
	void ListAddString( UINT nIDS );

// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CProgressCtrl m_ctrlProgressAll;
	afx_msg void OnTimer(UINT nIDEvent);	
	afx_msg void OnBnClickedButtonExit();

protected:
	virtual void PostNcDestroy();

public:
	CListBox m_ctrlListBox;
};
