// FileSystemDlg.h : ��� ����
//
#pragma once

#include "../enginelib/Common/SFileSystem.h"

// CFileSystemDlg ��ȭ ����
class CFileSystemDlg : public CDialog
{
public:
	CString		m_strPath;
	CImageList	m_ImageList;
	SFileSystem	m_FileSystem;
	int			m_iOverwriteType;

public:
	void UpdateFileList();
	void DropFile(HDROP hDrop);
	BOOL DropFolder(const char* strPath);

// ����
public:
	CFileSystemDlg(CWnd* pParent = NULL);	// ǥ�� ������

// ��ȭ ���� ������
	enum { IDD = IDD_FILESYSTEM_DIALOG };

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
	CListCtrl m_FileList;
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileClose();
	afx_msg void OnBnClickedNewfolder();
	afx_msg void OnBnClickedInsertfile();
	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedOptimize();
	afx_msg void OnLvnEndlabeleditFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkFilelist(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonOpen();
};
