#pragma once

#include "afxwin.h"
#include "RANPARAM.h"

// CDlgDownloadArea ��ȭ �����Դϴ�.

class CDlgDownloadArea : public CDialog
{
	DECLARE_DYNAMIC(CDlgDownloadArea)

public:
	CDlgDownloadArea(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgDownloadArea();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_DOWNLOAD_AREA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	int m_nDownloadArea;

	CString m_strAreaName[RANPARAM::MAX_CHINA_REGION];
	virtual BOOL OnInitDialog();
};
