#ifndef DLG_COMPRESS_H_
#define DLG_COMPRESS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "DatabaseTable.h"
#include "afxwin.h"
#include "MainFrm.h"
#include "afxcmn.h"

// CDlgCompress ��ȭ �����Դϴ�.

class CDlgCompress : public CDialog
{
	DECLARE_DYNAMIC(CDlgCompress)

public:
	CDlgCompress(CMainFrame* pFrame, std::vector<CompressFile> &v, CWnd* pParent = NULL, int nMode=1);   // ǥ�� �������Դϴ�.
	CDlgCompress(CMainFrame* pFrame, CString strFileName, CWnd* pParent = NULL, int nMode=2);   // ǥ�� �������Դϴ�.
	
	virtual ~CDlgCompress();

	std::vector<CompressFile> m_vCompressFile;
	CString m_strFileName;
	CMainFrame* m_pFrame;
	int	m_nMode;


// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_COMPRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	
	void threadCompressStart();
	void threadCompressFromFileStart();
	
	static unsigned int WINAPI CompressStart( void* pDlg );
	static unsigned int WINAPI CompressFromFileStart( void* pDlg );

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_EditConsole;

	afx_msg void OnTimer(UINT nIDEvent);
	CProgressCtrl m_Pregress;
	CStatic m_StaticCtl;
    afx_msg void OnEnMaxtextEditConsole();
};

#endif // DLG_COMPRESS_H_