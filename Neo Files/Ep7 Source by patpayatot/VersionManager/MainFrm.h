// MainFrm.h : CMainFrame Ŭ������ �������̽�
//


#pragma once

#include "FormLeft.h"
#include "FormRight.h"
#include "c_COdbcManager.h"

class CMainFrame : public CFrameWnd
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Ư��
protected:
	CSplitterWnd m_wndSplitter;
	BOOL m_bInitSplitter;
	
public:
	//COdbcManager*	m_pOdbc;
	CFormLeft*		m_pLeft;
	CFormRight*		m_pRight;

// �۾�
public:

// ������
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ����
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // ��Ʈ�� ������ ���Ե� ����Դϴ�.
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CToolBar    m_wndToolBar2;

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);	
};


