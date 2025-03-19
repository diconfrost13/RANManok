// VersionManagerView.h : iCVersionManagerView Ŭ������ �������̽�
//


#pragma once


class CVersionManagerView : public CFormView
{
protected: // serialization������ ��������ϴ�.
	CVersionManagerView();
	DECLARE_DYNCREATE(CVersionManagerView)

public:
	enum{ IDD = IDD_VERSIONMANAGER_FORM };

// Ư��
public:
	CVersionManagerDoc* GetDocument() const;

// �۾�
public:

// ������
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����
//	virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.

// ����
public:
	virtual ~CVersionManagerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnInitialUpdate();
protected:
	virtual void OnDraw(CDC* /*pDC*/);
};

#ifndef _DEBUG  // VersionManagerView.cpp�� ����� ����
inline CVersionManagerDoc* CVersionManagerView::GetDocument() const
   { return reinterpret_cast<CVersionManagerDoc*>(m_pDocument); }
#endif

