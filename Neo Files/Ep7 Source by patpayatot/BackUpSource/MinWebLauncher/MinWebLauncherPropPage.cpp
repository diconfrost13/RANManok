// MinWebLauncherPropPage.cpp : CMinWebLauncherPropPage �Ӽ� ������ Ŭ������ �����Դϴ�.

#include "stdafx.h"
#include "MinWebLauncher.h"
#include "MinWebLauncherPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CMinWebLauncherPropPage, COlePropertyPage)



// �޽��� ���Դϴ�.

BEGIN_MESSAGE_MAP(CMinWebLauncherPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Ŭ���� ���͸��� GUID�� �ʱ�ȭ�մϴ�.

IMPLEMENT_OLECREATE_EX(CMinWebLauncherPropPage, "MINWEBLAUNCHER.MinWebLauncherPropPage.1",
	0xd18689d1, 0xf771, 0x4d49, 0xbd, 0xa9, 0x9, 0xd4, 0x8f, 0xb6, 0x28, 0xb6)



// CMinWebLauncherPropPage::CMinWebLauncherPropPageFactory::UpdateRegistry -
// CMinWebLauncherPropPage���� �ý��� ������Ʈ�� �׸��� �߰��ϰų� �����մϴ�.

BOOL CMinWebLauncherPropPage::CMinWebLauncherPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_MINWEBLAUNCHER_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CMinWebLauncherPropPage::CMinWebLauncherPropPage - �������Դϴ�.

CMinWebLauncherPropPage::CMinWebLauncherPropPage() :
	COlePropertyPage(IDD, IDS_MINWEBLAUNCHER_PPG_CAPTION)
{
}



// CMinWebLauncherPropPage::DoDataExchange - �������� �Ӽ� ���̿��� �����͸� �̵���ŵ�ϴ�.

void CMinWebLauncherPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CMinWebLauncherPropPage �޽��� ó�����Դϴ�.
