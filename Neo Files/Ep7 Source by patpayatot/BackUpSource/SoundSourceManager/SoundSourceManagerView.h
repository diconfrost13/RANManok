////////////////////////////////////////////////////////////////////////////////
//	�̸�	:	CSoundSourceManagerView
//	����	:	ȭ�鿡 �ѷ���. ����Ÿ�� Doc���� �� ���ڵ庰�� ������.
//	�ΰ�����:
//	�ۼ���	:	���⿱
//	eMail	:	kysung@mincoms.co.kr
//	WWW		:	http://www.ran-online.co.kr
//	
//	�α�
//	$2.	2002-10-23T17:22	DirectSound ����Ŭ���� �ٷ� ���� ��� �ۼ�;
//	$1.	2002-10-22T21:07	���� �ۼ�;
//
////////////////////////////////////////////////////////////////////////////////

// SoundSourceManagerView.h : iCSoundSourceManagerView Ŭ������ �������̽�
//


#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class	CSoundManager;	//Directsound ��ü
class	CSound;			//DirectSoundBuffer
class	CSoundSourceManagerDoc;
class	CSoundSourceMan;
class CSoundSourceManagerView : public CFormView
{
protected: // serialization������ ��������ϴ�.
	CSoundSourceManagerView();
	DECLARE_DYNCREATE(CSoundSourceManagerView)

public:
	enum{ IDD = IDD_SOUNDSOURCEMANAGER_FORM };

// Ư��
public:
	CSoundSourceManagerDoc* GetDocument() const;

private:
	CSoundSourceMan	*m_pSoundSourceMan;

public:
	CSoundSourceMan*	GetSSM ( void ) { return m_pSoundSourceMan; }

protected:
	CSoundManager*			m_pSoundManager;	//Directsound ��ü
	CSound*					m_pSound;			//DirectSoundBuffer
	BOOL					m_bNormal;			//DirectSound ��ü�� ���� �ʱ�ȭ�Ǿ� �ִ�.

// ������
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����
	virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.

// ����
public:
	virtual ~CSoundSourceManagerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CImageList	m_ImageList;

protected:
	void	ReloadAllItems();
	void	DeleteSelectItems();
	void	CopySelectItems ( const CString& strOriginDir, const CString& strTargetDir );
	void	ChangeState( BOOL State );
	BOOL	CompareAllToken ( CString Left, CString Right );	

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctrlSoundSourceTable;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_ctrlButtonDelete;
	CButton m_ctrlButtonModify;
	CButton m_ctrlButtonFind;
	CButton m_ctrlButtonWrite;
	CEdit m_ctrlEditKeyword;
	afx_msg void OnBnClickedButtonWrite();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnNMDblclkListSoundsourceTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListSoundsourceTable(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListSoundsourceTable(NMHDR *pNMHDR, LRESULT *pResult);	
	CString m_valKeyword;
	afx_msg void OnBnClickedButtonFind();
	CComboBox m_ctrlCategory;
	CStatic m_ctrlStaticCategory;
	afx_msg void OnCbnSelchangeComboCategory();
	
public:
	afx_msg void OnMenuitemTextout();
	afx_msg void OnMenuitemLoad();
	afx_msg void OnMenuitemSave();
	afx_msg void OnMenuitemIntegrity();
	afx_msg void OnMenuitemFile2listTrace();
};

#ifndef _DEBUG  // SoundSourceManagerView.cpp�� ����� ����
inline CSoundSourceManagerDoc* CSoundSourceManagerView::GetDocument() const
   { return reinterpret_cast<CSoundSourceManagerDoc*>(m_pDocument); }
#endif

