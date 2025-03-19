// MMPCDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"


// CMMPCDlg ��ȭ ����
class CMMPCDlg : public CDialog
{
// ����
public:
	CMMPCDlg(CWnd* pParent = NULL);	// ǥ�� ������

// ��ȭ ���� ������
	enum { IDD = IDD_MMPC_DIALOG };

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
	afx_msg void OnBnClickedButton1();

protected:
	// ���� ���� ��ǥ
	int m_nLX;
	int m_nTY;
	int m_nRX;
	int m_nBY;

	// ���� ��� ��
	int m_nSizeX;
	int m_nSizeY;
	int m_nCalcLX;
	int m_nCalcBY;

	CString m_strDDSFileName;

public:
	CStatic m_csFileName;
	afx_msg void OnBnClickedOk();

protected:
	RECT GetBitmapPosition();
	void MiniMapPositionCalc( RECT & rect);
	BOOL MiniMapPositionSave( CString & strPathName, CString & strBUFFER );

	void InitEditView();
	void HideEditView();
	void ShowEditView( CString & strBUFFER = CString() );

public:
	afx_msg void OnBnClickedHide();
	CButton m_ctrlButtonCancel;
	CEdit m_ctrlEditView;
	CButton m_ctrlButtonHide;
	afx_msg void OnBnClickedButton2();
	CStatic m_csDDSFileName;
};
