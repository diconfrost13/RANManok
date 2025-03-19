// VersionManagerDoc.h : CVersionManagerDoc Ŭ������ �������̽�
//


#pragma once

#include "FormLeft.h"
#include "FormRight.h"
#include "MIN_CAB_UTIL.h"

class CVersionManagerDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CVersionManagerDoc();
	DECLARE_DYNCREATE(CVersionManagerDoc)

// Ư��
public:
	CFormLeft*	m_pLeft;
	CFormRight* m_pRight;

// �۾�
public:

// ������
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
    void 	GetAppPath();
    CString m_strFullPath;

// ����
public:
	virtual ~CVersionManagerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// �޽��� �� �Լ��� �����߽��ϴ�.
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
    afx_msg void OnFtp();
    afx_msg void OnCheck();
    afx_msg void OnFileSave();
};


