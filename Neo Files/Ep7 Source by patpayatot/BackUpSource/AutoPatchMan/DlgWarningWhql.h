#pragma once
#include "afxwin.h"

#include "HyperlinkStatic.h"

// CDlgWarningWhql ��ȭ �����Դϴ�.
class CDlgWarningWhql : public CDialog
{
	DECLARE_DYNAMIC(CDlgWarningWhql)

public:
	BOOL m_bCheckWhql;

public:
	CDlgWarningWhql(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgWarningWhql();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_WARNING_WHQL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CHyperlinkStatic m_ctrlNVidiaLink;
	CHyperlinkStatic m_ctrlATILink;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
