#include "StdAfx.h"
#include ".\minedit.h"

CMinEdit::CMinEdit(void)
{
	m_clrText	= RGB(255, 255,   0); // Text color
	m_clrBkgnd	= RGB(  0,   0,   0); // Background color
	m_brBkgnd.CreateSolidBrush(m_clrBkgnd);
}

CMinEdit::~CMinEdit(void)
{
}

BEGIN_MESSAGE_MAP(CMinEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

HBRUSH CMinEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  ���⼭ DC�� Ư���� �����մϴ�.	
	pDC->SetTextColor(m_clrText);   // text
	pDC->SetBkColor  (m_clrBkgnd);  // text bkgnd
	return m_brBkgnd;  

	// TODO:  �θ� ó���Ⱑ ȣ����� ���� ��� Null�� �ƴ� �귯�ø� ��ȯ�մϴ�.
	return NULL;
}
