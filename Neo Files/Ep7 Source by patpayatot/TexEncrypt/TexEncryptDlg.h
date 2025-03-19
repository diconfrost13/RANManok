// TexEncryptDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"


// CTexEncryptDlg ��ȭ ����
class CTexEncryptDlg : public CDialog
{
// ����
public:
	CTexEncryptDlg(CWnd* pParent = NULL);	// ǥ�� ������

// ��ȭ ���� ������
	enum { IDD = IDD_TEXENCRYPT_DIALOG };

	enum 
	{
		ENCRYPT_FILE,
		ENCRYPT_FORDER,
		DECRYPT_FILE,
		DECRYPT_FORDER,
		OPTION_SIZE,
	};

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ����


// ����
protected:
	HICON m_hIcon;

	// �޽��� �� �Լ��� �����߽��ϴ�.
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cCombo;
	CEdit m_cEdit;

	afx_msg void OnBnClickedOpenButton();
	afx_msg void OnBnClickedRunButton();


public:
	CString m_strAppPath;

	void SetAppPath();
	void Init();
	void OpenEncryptFile();
	void OpenDecryptFile();
	void OpenSelectForder();
	afx_msg void OnCbnSelchangeOptionCombo();

	BOOL RunEncryptFile( const CString& strInput, const CString& strOutput );
	BOOL RunEncryptForder( const CString& strInput, const CString& strOutput );
	BOOL RunDecryptFile( const CString& strInput, const CString& strOutput );
	BOOL RunDecryptForder( const CString& strInput, const CString& strOutput );

	void AddEditString( const char* szInfo );
};

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM dwData);


