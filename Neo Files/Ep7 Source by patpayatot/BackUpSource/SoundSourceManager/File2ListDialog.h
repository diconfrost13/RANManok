#pragma once


// CFile2ListDialog ��ȭ �����Դϴ�.

class CFile2ListDialog : public CDialog
{
	DECLARE_DYNAMIC(CFile2ListDialog)

public:
	CFile2ListDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CFile2ListDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FILE2LISTDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBegin();
	afx_msg void OnBnClickedButtonFile();
	afx_msg void OnBnClickedButtonEnd();
};
