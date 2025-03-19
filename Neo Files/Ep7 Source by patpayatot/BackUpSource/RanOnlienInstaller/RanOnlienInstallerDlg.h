// RanOnlienInstallerDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

class	CHttpPatch;
class	CLPatchThread;

// CRanOnlienInstallerDlg ��ȭ ����
class CRanOnlienInstallerDlg : public CDialog
{
// ����
public:
	CRanOnlienInstallerDlg(CWnd* pParent = NULL);	// ǥ�� ������

	void UpdateControls();

	void BEGIN_DOWNLOAD();
	void END_DOWNLOAD();
	void EXECUTE_INSTALL();
	BOOL IsVista();


	void OnKillThread();

// ��ȭ ���� ������
	enum { IDD = IDD_DOWNLOADDLG };

	enum 
	{
		E_START_PATCH	= 1,
		E_FORCE_EXIT	= 2,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����

public:
	CString m_DownloadURL;
	CString m_strFileName;
	CString m_strSavePath;

	BOOL m_bForceTerminate;

	CHttpPatch*	m_pHttpPatch;
	CLPatchThread * m_pThread; // ��ġ ������

	ULONGLONG m_ULBefore;
	float m_dwBps;
	DWORD m_dwBeforeTime;
	DWORD m_dwCntTime;

// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExecuteButton();
	afx_msg void OnBnClickedStartButton();
	afx_msg void OnBnClickedCloseButton();
	afx_msg void OnTimer(UINT nIDEvent);
	
	CButton m_ctrlExecute;
	CButton m_ctrlStart;
	CProgressCtrl m_ctrlProgressAll;
protected:
	virtual void PostNcDestroy();
public:
	BOOL m_bAutoExecute;
};
