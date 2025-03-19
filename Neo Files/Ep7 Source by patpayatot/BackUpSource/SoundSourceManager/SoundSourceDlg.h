////////////////////////////////////////////////////////////////////////////////
//	�̸�	:	CSoundSourceDlg
//	����	:	�� ���ڵ��� ���� ����� ���̾�α���. 
//	�ΰ�����:	���������� ����Ÿ��	�����ؼ�, �װ��� Doc�� �Ѱ� �ڷḦ ������
//	�ۼ���	:	���⿱
//	eMail	:	kysung@mincoms.co.kr
//	WWW		:	http://www.ran-online.co.kr
//	
//	�α�
//	$2002-10-22T21:07	���� �ۼ�;
//
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "afxwin.h"

enum	EDlgType
{
	eNew,
	eModify
};

// CSoundSourceDlg ��ȭ �����Դϴ�.
class CSoundSourceDlg : public CDialog
{
	DECLARE_DYNAMIC(CSoundSourceDlg)

public:
	CSoundSourceDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSoundSourceDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SOUNDSOURCEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

protected:
	EDlgType		m_eDlgType;
	WORD			m_ID;
	BOOL			m_bChangeFile;

public:
	void	SetState ( EDlgType DlgType, WORD ID = 0 );

protected:
	BOOL	ValidateWaveFile( const TCHAR* strFileName );


	DECLARE_MESSAGE_MAP()
public:
	CString m_valPathFile;
	CString	m_OriginFileName;
	CComboBox m_ctrlType;
	CString m_valComment;
	CString m_valStaticGroup;
	CButton m_ctrlButtonFindFile;
	afx_msg void OnBnClickedButtonFindfile();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	DWORD m_valBufferCount;
};
