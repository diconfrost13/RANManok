#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgFileCheck ��ȭ �����Դϴ�.

class CDlgFileCheck : public CDialog
{
	DECLARE_DYNAMIC(CDlgFileCheck)

public:
	CDlgFileCheck(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgFileCheck();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_FILECHECK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    void    StartFileCheck();
    BOOL    IsExist(CString strFullPath);
    afx_msg void OnTimer(UINT nIDEvent);
protected:
    CProgressCtrl m_ProgressCheck;
public:
    CStatic m_StaticCheck;
    afx_msg void OnEnMaxtextEditCheck();
};
