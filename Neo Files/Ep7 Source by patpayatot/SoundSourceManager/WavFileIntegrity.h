#pragma once


// CWavFileIntegrity ��ȭ �����Դϴ�.

class CWavFileIntegrity : public CDialog
{
	DECLARE_DYNAMIC(CWavFileIntegrity)

public:
	CWavFileIntegrity(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CWavFileIntegrity();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_WAVFILEINTEGRITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBegin();
	afx_msg void OnBnClickedButtonEnd();
	afx_msg void OnBnClickedButtonFile();
};
