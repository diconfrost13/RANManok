// FormRight.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "VersionManager.h"
#include "FormRight.h"
#include "MainFrm.h"
// #include "MIN_CAB_UTIL.h"
#include "DlgCompress.h"
#include "CCfg.h"

// CFormRight

IMPLEMENT_DYNCREATE(CFormRight, CFormView)

CFormRight::CFormRight()
	: CFormView(CFormRight::IDD)
{
	m_bInit = FALSE;
	nSelectedItem = 0;
	m_nDir = 0;
}

CFormRight::~CFormRight()
{
}

void CFormRight::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_File);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}

BEGIN_MESSAGE_MAP(CFormRight, CFormView)
	ON_WM_SIZE()	
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnNMClickList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnLvnItemchangedList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, OnNMRclickList1)
	ON_COMMAND(ID_RIGHT_ADD, OnRightAdd)
	ON_COMMAND(ID_RIGHT_EDIT, OnRightEdit)
	ON_COMMAND(ID_RIGHT_DELETE, OnRightDelete)
END_MESSAGE_MAP()


// CFormRight �����Դϴ�.

#ifdef _DEBUG
void CFormRight::AssertValid() const
{
	CFormView::AssertValid();
}

void CFormRight::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CFormRight �޽��� ó�����Դϴ�.

void CFormRight::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	m_File.SetExtendedStyle ( LVS_EX_FULLROWSELECT ); 
	InitListHead(); // ����Ʈ ��Ʈ�� ��� �ʱ�ȭ
	InitListData(); // ����Ʈ ��Ʈ�� Data �ʱ�ȭ

	m_bInit = TRUE;
}

// ����Ʈ ��Ʈ�� ��� �ʱ�ȭ
void CFormRight::InitListHead() 
{
	LVCOLUMN Col;
	CString strTemp;
	
	strTemp			= "File Name";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 200;
	Col.fmt			= LVCFMT_CENTER;		
	Col.pszText		= strTemp.GetBuffer();
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(0, &Col);

	strTemp			= "Ver";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 40;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(1, &Col);

	strTemp			= "Date";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 150;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(2, &Col);

#ifdef CHINAPARAM
	strTemp			= "MD5";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 270;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(3, &Col);
#else
	strTemp			= "";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 0;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(3, &Col);
#endif

    strTemp			= "FTP";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(4, &Col);

	strTemp			= "Description";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 200;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_File.InsertColumn(5, &Col);
}

// ����Ʈ ��Ʈ�� Data �ʱ�ȭ
void CFormRight::InitListData()
{
	FillData(m_nDir);
}

void CFormRight::FillData(int nFolder)
{
	// �ش� ������ ���ϸ���Ʈ�� ����Ѵ�.
	int nRetCode;
	// Erase all list items
	m_File.DeleteAllItems();
	// Erase all vector elements
	m_vFile.erase(m_vFile.begin(), m_vFile.end());

	CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();
	
    nRetCode = COdbcManager::GetInstance()->GetFileList(nFolder, m_vFile);

	if (nRetCode == DB_ERROR)
	{
		MessageBox(_T("���ϸ���Ʈ�� ������ �� �����ϴ�"), "ERROR", MB_ICONEXCLAMATION);
		return;
	}

	m_nDir = nFolder;

	for (int i=0; i<(int) m_vFile.size(); i++)
	{		
		int nCount; 
		CString strTemp;
		nCount = m_File.GetItemCount();

		LV_ITEM lvItem;
		::memset(&lvItem, 0, sizeof(LV_ITEM));
		lvItem.mask = LVIF_TEXT; // �Ӽ� ����
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;		
		lvItem.pszText = (&m_vFile[i])->strFile.GetBuffer();
		// lvItem.iImage = 0; // �̹����� ���� ��ȣ
		m_File.InsertItem(&lvItem); // ���ο� ���� ������ InsertItem�� ����Ѵ�.
		
		strTemp.Format("%d", (&m_vFile[i])->nVer);
		m_File.SetItemText(i, 1, strTemp.GetString());		

		m_File.SetItemText(i, 2, (&m_vFile[i])->strDate.GetString());

#ifdef CHINAPARAM
		m_File.SetItemText( i, 3, (&m_vFile[i])->strMD5.GetString() );
#else
		m_File.SetItemText( i, 3, "" );
#endif        
		strTemp.Format("%d", (&m_vFile[i])->nFtp);
        m_File.SetItemText(i, 4, strTemp);

		m_File.SetItemText(i, 5, (&m_vFile[i])->strDesc.GetString());
	}
}
void CFormRight::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	// ��Ʈ�� ������ ����
	if ( m_bInit && nType != SIZE_MINIMIZED )
	{		
		CRect cRect;
		GetClientRect(&cRect);
				
		CRect ctlRect;
		m_File.GetWindowRect(&ctlRect);
		m_Edit.MoveWindow(10, 5, cx-15, 25);
		m_File.MoveWindow(10, 30, cx-15, cy-30);
	}
}

void CFormRight::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// ���õ� ������ �ִ��� Ȯ���Ѵ�.
	//int nSelected = m_File.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	//
	//if (nSelected != -1) // ���õ� ������ ����
	//{	
	//	// ���õ� ������ ������ �ڽ��� ����Ѵ�.
	//	m_Edit.SetWindowText((&m_vFile[nSelected])->strFile.GetString());
	//}
	*pResult = 0;
}

void CFormRight::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSelected = m_File.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	if (nSelected != -1) // ���õ� ������ ����
	{	
		// ���õ� ������ ������ �ڽ��� ����Ѵ�.
		m_Edit.SetWindowText((&m_vFile[nSelected])->strFile.GetString());
	}
	*pResult = 0;
}

void CFormRight::OnNMRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSelected = m_File.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	
	// ���콺 ��ǥ ���
	POINT point;
	GetCursorPos(&point);
	
	// �޴� ǥ��
	CMenu menu;
	menu.LoadMenu (IDR_CONTEXT_RIGHT);
	CMenu* pContextMenu = menu.GetSubMenu (0);		
    pContextMenu->TrackPopupMenu (
				TPM_LEFTALIGN |	TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
				point.x, point.y,
				AfxGetMainWnd ());
	
	if (nSelected != -1) // ���õ� �׸��� ����
	{	
	}
	*pResult = 0;
}

// �����߰�
void CFormRight::OnRightAdd()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();
	std::vector<CompressFile> vCompressFile;
	CFileDialog DlgFile(TRUE, NULL,	NULL);
	DlgFile.m_ofn.Flags |= OFN_ALLOWMULTISELECT | OFN_ENABLESIZING;
	
	if (DlgFile.DoModal() == IDOK ) 
	{
		POSITION pos = DlgFile.GetStartPosition();
		CString strTarget(CCfg::GetInstance()->m_szTargetPath);		
		strTarget += COdbcManager::GetInstance()->GetFolderName(m_nDir);		

		while ( pos != NULL )
		{
			// �ݺ��ؼ� strPathName �۾��� �Ѵ�.
			CString strPathName = DlgFile.GetNextPathName(pos);
			CString strFileName = strPathName;
			int nPosition = strFileName.ReverseFind('\\');
			strFileName = strFileName.Mid(nPosition+1);
			strFileName += ".cab";

			// �ϼ��� src, target �� vector �� ����			
			CompressFile sTemp;
			sTemp.strSrc		= strPathName;
			sTemp.strTarget		= strTarget;
			sTemp.strFileName	= strFileName;
			sTemp.nDir			= m_nDir;
			vCompressFile.push_back(sTemp);

            
			//// �����ؼ� �ش� ���丮�� �����Ѵ�.
			//if (CAB_UTIL_MIN::MinMakeCab(strPathName.GetString(), strTarget.GetString()) == TRUE)
			//{ 
			//	// ���� �̸��� ������ �ִ��� �����Ѵ�.
			//	BOOL bExist = pFrame->m_pOdbc->IsFileExist(strFileName);
			//	if (bExist)	pFrame->m_pOdbc->UpdateFile(strFileName);
			//	else 		pFrame->m_pOdbc->InsertFile(m_nDir, strFileName);
			//}
			//else
			//{
			//	MessageBox("�������");
			//}			
		}
		// ���� ��ȭ���ڸ� ����.
		
		CDlgCompress DlgCompress(pFrame, vCompressFile);
		DlgCompress.DoModal();
	}
	// Right Refresh
	InitListData();
}

// ����
void CFormRight::OnRightEdit()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}

// ����
void CFormRight::OnRightDelete()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
    TRACE("RIGHT DELETE \n");

    int nRetCode = MessageBox(_T("������ �����Ͻðڽ��ϱ�?"), _T("���� Ȯ��"), MB_OKCANCEL);

    if (nRetCode != 1)
    {
        return;
    }

    
    UINT i, uSelectedCount = m_File.GetSelectedCount();
    int  nItem = -1;
    
    CString strTarget(CCfg::GetInstance()->m_szTargetPath);		
	strTarget += COdbcManager::GetInstance()->GetFolderName(m_nDir);

    // Check selected item
    if (uSelectedCount > 0)
    {
        for (i=0;i < uSelectedCount;i++)
        {
            nItem = m_File.GetNextItem(nItem, LVNI_SELECTED);
            ASSERT(nItem != -1);
            TRACE("SELECTED %d \n", nItem);

            // ������ �����.
            CString strDelFullPath;
            strDelFullPath.Format("%s\\%s", strTarget, (&m_vFile[nItem])->strFile);

            if (DeleteFile(strDelFullPath) == 0)
            {                
                MessageBox(_T("������ �����Ͽ����ϴ�"));
            }
            else
            {
                // DB ���� �����Ѵ�.
                if (COdbcManager::GetInstance()->DeleteFile((&m_vFile[nItem])->nIdx) == DB_ERROR)
                {
                    MessageBox(_T("������ ���� �Ǿ����� DB ������ �����Ͽ����ϴ�"));
                }
            }
        }
        // Right View �� ���÷��� �Ѵ�.
        InitListData();
    }
}