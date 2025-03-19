// FormLeft.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "VersionManager.h"
#include "FormLeft.h"
#include "MainFrm.h"
#include "DlgNewFolder.h"
#include "CCfg.h"
#include ".\formleft.h"

// CFormLeft

IMPLEMENT_DYNCREATE(CFormLeft, CFormView)

CFormLeft::CFormLeft()
	: CFormView(CFormLeft::IDD)
{
	m_bInit = FALSE;
	nSelectedItem = 0;
}

CFormLeft::~CFormLeft()
{
}

void CFormLeft::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	DDX_Control(pDX, IDC_LIST1, m_Folder);
}

BEGIN_MESSAGE_MAP(CFormLeft, CFormView)	
	ON_WM_SIZE()	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnLvnItemchangedList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnNMRclickList1)	
	ON_COMMAND(ID__MAKE, OnMakeCmd)
	ON_COMMAND(ID__DELETE, OnDeleteCmd)
    ON_WM_TIMER()
END_MESSAGE_MAP()

// CFormLeft �����Դϴ�.

#ifdef _DEBUG
void CFormLeft::AssertValid() const
{
	CFormView::AssertValid();
}

void CFormLeft::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CFormLeft �޽��� ó�����Դϴ�.
void CFormLeft::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_Folder.SetExtendedStyle ( LVS_EX_FULLROWSELECT ); 

	InitListHead(); // ����Ʈ ��Ʈ�� ��� �ʱ�ȭ
	InitListData(); // ����Ʈ ��Ʈ�� Data �ʱ�ȭ

    SetTimer(100, 1000, NULL);

	m_bInit = TRUE;
}

void CFormLeft::InitListHead()
{
	LVCOLUMN Col;
	CString strTemp;

	strTemp			= "Directory Name";	
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 400;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_Folder.InsertColumn(0, &Col);
}

void CFormLeft::InitListData()
{	
	int nRetCode;
	// Erase all list items
	m_Folder.DeleteAllItems();
	// Erase all vector elements
	m_vFolder.erase(m_vFolder.begin(), m_vFolder.end());

	CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();
	nRetCode = COdbcManager::GetInstance()->GetFolderList(m_vFolder);

	if (nRetCode == DB_ERROR)
	{
		MessageBox("��������Ʈ�� ������ �� �����ϴ�", "ERROR", MB_ICONEXCLAMATION);
		return;
	}

	for (int i=0; i<(int) m_vFolder.size(); i++)
	{		
		int nCount; 
		nCount = m_Folder.GetItemCount();

		LV_ITEM lvItem;
		::memset(&lvItem, 0, sizeof(LV_ITEM));
		lvItem.mask = LVIF_TEXT; // �Ӽ� ����
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;		
		lvItem.pszText = (&m_vFolder[i])->strName.GetBuffer();
		// lvItem.iImage = 0; // �̹����� ���� ��ȣ
		m_Folder.InsertItem(&lvItem); // ���ο� ���� ������ InsertItem�� ����Ѵ�.
	}    
}

void CFormLeft::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// ��Ʈ�� ������ ����
	if ( m_bInit && nType != SIZE_MINIMIZED )
	{		
		CRect cRect;
		GetClientRect(&cRect);
				
		// CRect ctlRect;
		// m_Folder.GetWindowRect(&ctlRect);
		m_Edit.MoveWindow(10, 5, cx-15, 25);
		m_Folder.MoveWindow(10, 30, cx-15, cy-30);
	}
}

void CFormLeft::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSelected = m_Folder.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	if (nSelected != -1)
	{
		// ���õ� ������ ������ �ڽ��� ����Ѵ�.
		m_Edit.SetWindowText((&m_vFolder[nSelected])->strName.GetString());
		// ���õ� ������ ���ϵ��� ������ ����Ʈ�� �����ش�.
		CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();
		pFrame->m_pRight->FillData((&m_vFolder[nSelected])->nIdx);		
		nSelectedItem = nSelected;
	}
	*pResult = 0;
}

void CFormLeft::OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// ���õ� ������ �ִ��� Ȯ���Ѵ�.
	int nSelected = m_Folder.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // ���õ� �׸��� ����
	{	
		// ���콺 ��ǥ ���
		POINT point;
		GetCursorPos(&point);
		
		// �޴� ǥ��
		CMenu menu;
		menu.LoadMenu (IDR_CONTEXT_LEFT);
		CMenu* pContextMenu = menu.GetSubMenu (0);		
        pContextMenu->TrackPopupMenu (
					TPM_LEFTALIGN |	TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
					point.x, point.y,
					AfxGetMainWnd ());
	}
	*pResult = 0;
}

void CFormLeft::OnMakeCmd()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	// ���ο� ������ �����Ѵ�.	

	CString strTemp;
	m_Edit.GetWindowText(strTemp);
	CDlgNewFolder dlg(strTemp);
	int nRet;
	nRet = (int) dlg.DoModal();

	if (nRet == IDOK)
	{
		strTemp = CCfg::GetInstance()->m_szTargetPath + dlg.m_strFolder;		
		if (CreateDirectory(strTemp, NULL) == 0)
		{
			// ��������
			MessageBox("������ �����Ͽ����ϴ�", "ERROR", MB_ICONEXCLAMATION);
		}
		else
		{
			// ��������
			MessageBox("������ �����Ͽ����ϴ�");
			// DB �� ������ ���� �̸� �Է�
			CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();
			if (COdbcManager::GetInstance()->CreateFolder(dlg.m_strFolder) != DB_OK)
			{
				MessageBox("���丮�� �����Ǿ����� DB �� �Էµ����� �ʾҽ��ϴ�");
			
			}
			// Left form refresh
			InitListData();
			// Right form refresh
			pFrame->m_pLeft->InitListData();
		}	
	}
}

void CFormLeft::OnDeleteCmd()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString strTemp;
	m_Edit.GetWindowText(strTemp);
	strTemp = CCfg::GetInstance()->m_szTargetPath + strTemp;

	if (RemoveDirectory(strTemp) == 0)
	{
		MessageBox("������ �����Ͽ����ϴ�", "ERROR", MB_ICONEXCLAMATION);
	}
	else	
	{
		// ���õ� ������ �ִ��� Ȯ���Ѵ�.
		int nSelected = m_Folder.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		
		if (nSelected != -1) // ���õ� �׸��� ����
		{	
			int nDir = m_vFolder[nSelected].nIdx; 
			//// ���丮 ������ DB ���� �����.
			CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();
			if (COdbcManager::GetInstance()->RemoveFolder(nDir) == DB_OK)
			{
				MessageBox("�ùٸ��� �����Ǿ����ϴ�");
			}
			else
			{
				MessageBox("���丮�� ���������� DB ���� �������� �ʾҽ��ϴ�");
			}
			// Left form refresh
			InitListData();
			// Right form refresh
			pFrame->m_pLeft->InitListData();
		}
		else
		{
			MessageBox("���õ� ���丮�� �����ϴ�");
		}
	}
}

void CFormLeft::OnTimer(UINT nIDEvent)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    if (nIDEvent == 100)
    {
        KillTimer(100);
        if (m_Folder.GetItemCount() > 0)
        {
            m_Folder.SetFocus();
            m_Folder.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);          
        }
    }

    CFormView::OnTimer(nIDEvent);
}
