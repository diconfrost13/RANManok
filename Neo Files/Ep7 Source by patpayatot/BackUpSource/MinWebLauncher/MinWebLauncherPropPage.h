#pragma once

// MinWebLauncherPropPage.h : CMinWebLauncherPropPage �Ӽ� ������ Ŭ������ �����Դϴ�.


// CMinWebLauncherPropPage : ������ ������ MinWebLauncherPropPage.cpp�� �����Ͻʽÿ�.

class CMinWebLauncherPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CMinWebLauncherPropPage)
	DECLARE_OLECREATE_EX(CMinWebLauncherPropPage)

// �������Դϴ�.
public:
	CMinWebLauncherPropPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_PROPPAGE_MINWEBLAUNCHER };

// ����
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �޽��� ���Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

