#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// CDlgNewFolder ��ȭ �����Դϴ�.

class CDlgNewFolder : public CDialog
{
	DECLARE_DYNAMIC(CDlgNewFolder)

public:
	CDlgNewFolder(CString strTemp, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgNewFolder();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_NEW_FOLDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strFolder;
	CEdit m_EditFolder;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
