// VersionManagerView.cpp : CVersionManagerView Ŭ������ ����
//

#include "stdafx.h"
#include "VersionManager.h"

#include "MainFrm.h"
#include "VersionManagerDoc.h"
#include "VersionManagerView.h"
#include ".\versionmanagerview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVersionManagerView

IMPLEMENT_DYNCREATE(CVersionManagerView, CFormView)

BEGIN_MESSAGE_MAP(CVersionManagerView, CFormView)
END_MESSAGE_MAP()

// CVersionManagerView ����/�Ҹ�

CVersionManagerView::CVersionManagerView()
	: CFormView(CVersionManagerView::IDD)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	

}

CVersionManagerView::~CVersionManagerView()
{
}

void CVersionManagerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CVersionManagerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	// Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.	
	return CFormView::PreCreateWindow(cs);
}

//void CVersionManagerView::OnInitialUpdate()
//{
//	CFormView::OnInitialUpdate();
//	GetParentFrame()->RecalcLayout();
//	ResizeParentToFit();
//
//	
//}


// CVersionManagerView ����

#ifdef _DEBUG
void CVersionManagerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CVersionManagerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CVersionManagerDoc* CVersionManagerView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVersionManagerDoc)));
	return (CVersionManagerDoc*)m_pDocument;
}
#endif //_DEBUG


// CVersionManagerView �޽��� ó����

BOOL CVersionManagerView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CVersionManagerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.	
}

void CVersionManagerView::OnDraw(CDC* /*pDC*/)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.	
}
