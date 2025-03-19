// VerManDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"

const int nFILENAME = 64;
const int nSUBPATH = 128;

struct	SFILENODE
{
	char	FileName[nFILENAME];
	char	SubPath[nSUBPATH];	
	int		Ver;

public:
	SFILENODE()
	{
		memset ( FileName, 0, sizeof ( char ) * nFILENAME );
		memset ( SubPath, 0, sizeof ( char ) * nSUBPATH );
		Ver = 0;
	}
};

// CVerManDlg ��ȭ ����
class CVerManDlg : public CDialog
{
// ����
public:
	CVerManDlg(CWnd* pParent = NULL);	// ǥ�� ������

// ��ȭ ���� ������
	enum { IDD = IDD_VERMAN_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����


// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:	
	CString m_valPatch;
	CString m_valClient;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonDummy();
};
