#pragma once


// CNewFolderDlg ��ȭ �����Դϴ�.

class CNewFolderDlg : public CDialog
{
	DECLARE_DYNAMIC(CNewFolderDlg)

public:
	CNewFolderDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CNewFolderDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_NEWFOLDER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strFolderName;
};
