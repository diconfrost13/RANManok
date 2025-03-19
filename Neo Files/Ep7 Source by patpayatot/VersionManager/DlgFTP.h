#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgFTP ��ȭ �����Դϴ�.

class CDlgFTP : public CDialog
{
	DECLARE_DYNAMIC(CDlgFTP)

public:
	CDlgFTP(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgFTP();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_FTP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

    void UploadStart();

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    CProgressCtrl m_ProgressFTP;
    afx_msg void OnTimer(UINT nIDEvent);
    CStatic m_StaticFTP;
    afx_msg void OnBnClickedStop();
    BOOL m_bStop;
    HANDLE m_hThread;
    afx_msg void OnBnClickedStart();
    int StartUploadThread();
    int UploadThreadProc();
    afx_msg void OnEnMaxtextEditFtp();
};
