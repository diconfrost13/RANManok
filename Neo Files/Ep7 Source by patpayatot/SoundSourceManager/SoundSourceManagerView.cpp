// SoundSourceManagerView.cpp : CSoundSourceManagerView Ŭ������ ����
//

#include "stdafx.h"
#include "SoundSourceManager.h"

#include "SoundSourceManagerDoc.h"
#include "SoundSourceManagerView.h"

#include "SoundSourceMan.h"
#include "SoundSourceDlg.h"

#include "dsutil.h"
#include "dxutil.h"
#include ".\soundsourcemanagerview.h"
#include "WavFileIntegrity.h"
#include "File2ListDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSoundSourceManagerView

static	char	g_szDataDir[]		= "Sounds\\Sfx\\";

IMPLEMENT_DYNCREATE(CSoundSourceManagerView, CFormView)

BEGIN_MESSAGE_MAP(CSoundSourceManagerView, CFormView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_WRITE, OnBnClickedButtonWrite)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBnClickedButtonModify)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SOUNDSOURCE_TABLE, OnNMDblclkListSoundsourceTable)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SOUNDSOURCE_TABLE, OnNMClickListSoundsourceTable)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SOUNDSOURCE_TABLE, OnNMRclickListSoundsourceTable)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
	ON_CBN_SELCHANGE(IDC_COMBO_CATEGORY, OnCbnSelchangeComboCategory)
	ON_COMMAND(ID_MENUITEM_TEXTOUT, OnMenuitemTextout)
	ON_COMMAND(ID_MENUITEM_LOAD, OnMenuitemLoad)
	ON_COMMAND(ID_MENUITEM_SAVE, OnMenuitemSave)
	ON_COMMAND(ID_MENUITEM_INTEGRITY, OnMenuitemIntegrity)
	ON_COMMAND(ID_MENUITEM_FILE2LIST_TRACE, OnMenuitemFile2listTrace)
END_MESSAGE_MAP()

// CSoundSourceManagerView ����/�Ҹ�

CSoundSourceManagerView::CSoundSourceManagerView()
	: CFormView(CSoundSourceManagerView::IDD)
	, m_valKeyword(_T(""))
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	m_pSoundManager = NULL;
	m_pSound = NULL;
	m_bNormal = FALSE;
}

CSoundSourceManagerView::~CSoundSourceManagerView()
{
	if ( m_pSound != NULL )
	{
		m_pSound->Stop();
		SAFE_DELETE ( m_pSound );
	}	
	SAFE_DELETE ( m_pSoundManager );
	SAFE_DELETE ( m_pSoundSourceMan );
}

void CSoundSourceManagerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SOUNDSOURCE_TABLE, m_ctrlSoundSourceTable);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_ctrlButtonDelete);
	DDX_Control(pDX, IDC_BUTTON_MODIFY, m_ctrlButtonModify);
	DDX_Control(pDX, IDC_BUTTON_FIND, m_ctrlButtonFind);
	DDX_Control(pDX, IDC_BUTTON_WRITE, m_ctrlButtonWrite);
	DDX_Control(pDX, IDC_EDIT_KEYWORD, m_ctrlEditKeyword);
	DDX_Text(pDX, IDC_EDIT_KEYWORD, m_valKeyword);
	DDX_Control(pDX, IDC_COMBO_CATEGORY, m_ctrlCategory);
	DDX_Control(pDX, IDC_STATIC_CATEGORY, m_ctrlStaticCategory);
}

BOOL CSoundSourceManagerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	// Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CFormView::PreCreateWindow(cs);
}

void CSoundSourceManagerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	//	����Ÿ
	m_pSoundSourceMan = new CSoundSourceMan;

	CSoundSourceManagerApp	*pApp = (CSoundSourceManagerApp*)AfxGetApp();	
	CString DataDirectory = pApp->m_strAppPath + g_szDataDir;

	m_pSoundSourceMan->SetDataDirectory ( DataDirectory );
	CreateDirectory ( DataDirectory.GetString(), NULL );

	m_ctrlSoundSourceTable.SetExtendedStyle (
		m_ctrlSoundSourceTable.GetExtendedStyle() |
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	m_ImageList.Create ( IDB_BITMAP_STATE, 16, 1, (COLORREF)-1 );
	m_ctrlSoundSourceTable.SetImageList ( &m_ImageList, LVSIL_SMALL );

	RECT	rect;
	m_ctrlSoundSourceTable.GetClientRect ( &rect );
	LONG	Width = rect.right - rect.left;

	LV_COLUMN	lvcolumn;
	int			ColumnCount = 7;	
	char		*ColumnName[7] = { "!", "ID", "����", "���ۼ�", "����", "��¥", "�ڸ�Ʈ" };
	int			ColumnWidthPercent[7] = { 2, 10, 10, 10, 15, 15, 38 };

	for ( int i = 0; i < ColumnCount; i++ )
	{		
		lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM
			| LVCF_TEXT | LVCF_WIDTH;// | LVCF_IMAGE;
		lvcolumn.fmt = LVCFMT_LEFT;
		lvcolumn.pszText = ColumnName[i];
		lvcolumn.iSubItem = i;
		lvcolumn.cx = ( Width * ColumnWidthPercent[i] ) / 100;
		m_ctrlSoundSourceTable.InsertColumn (i, &lvcolumn);		
	}

	for ( i = 0; i < NTypeDesc::DescCount; i++ )
	{
		int nIndex = m_ctrlCategory.AddString ( NTypeDesc::Desc[i] );
		m_ctrlCategory.SetItemData ( nIndex, i );
	}
    m_ctrlCategory.SetCurSel ( m_ctrlCategory.GetCount () - 1 );

	//	<--	�ε嵥��Ÿ�� ȭ�鿡 �Ѹ�	-->	//
	ReloadAllItems();

	m_pSoundManager = new CSoundManager();
    
	if( FAILED( m_pSoundManager->Initialize( m_hWnd, DSSCL_PRIORITY, 1, 22050, 16 ) ) )
	{
		MessageBox( "DirectSound �ʱ�ȭ�� �����߽��ϴ�.", 
					"DirectSound", MB_OK | MB_ICONERROR );
		return;
	}

	m_bNormal = TRUE;
}




// CSoundSourceManagerView ����

#ifdef _DEBUG
void CSoundSourceManagerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSoundSourceManagerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CSoundSourceManagerDoc* CSoundSourceManagerView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSoundSourceManagerDoc)));
	return (CSoundSourceManagerDoc*)m_pDocument;
}
#endif //_DEBUG


// CSoundSourceManagerView �޽��� ó����

void CSoundSourceManagerView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if( m_ctrlSoundSourceTable.m_hWnd )
	{
		m_ctrlStaticCategory.MoveWindow ( 20, 13, 70, 17, FALSE );
		m_ctrlCategory.MoveWindow ( 90, 10, 180, 20, FALSE );		

		RECT	rect;
		GetClientRect ( &rect );

		LONG	Width = rect.right - rect.left;
		LONG	Height = rect.bottom - rect.top;
        
		m_ctrlSoundSourceTable.MoveWindow( 20, 40, Width - 40, Height - 80, FALSE);

		((CEdit*)GetDlgItem(IDC_EDIT_SRCDIRECTORY))->MoveWindow ( Width-320, 10, 300, 20, FALSE );

		//	<--	�� �� ���� ����	-->	//
		int	ColumnCount = 7;
		LONG	TotalColumnWidth = 0;
		LONG	ColumnWidth = 0;
		int		ColumnPercent = 0;
		for ( int i = 0; i < ColumnCount; i++ )
		{
			ColumnWidth = m_ctrlSoundSourceTable.GetColumnWidth ( i );
			TotalColumnWidth += ColumnWidth;
		}

		//	<--	���� % ����	-->	//
		int	ColumnWidthPercent[7];
		for ( int i = 0; i < ColumnCount; i++ )
		{
			ColumnWidth = m_ctrlSoundSourceTable.GetColumnWidth ( i );
			ColumnWidthPercent[i] = (int)((float)( ColumnWidth * 100 ) / (float)TotalColumnWidth + 0.5f);
		}

        //	<--	���̺� ���� ���� ũ�⸦ ����	-->	//
		RECT	TableRect;
		m_ctrlSoundSourceTable.GetClientRect ( &TableRect );
		LONG	TableWidth = TableRect.right - TableRect.left;

		//	<--	���̺��� ���� ���� ���̿� %��ŭ Column���̸� �����Ŵ	-->	//
		for ( int i = 0; i < ColumnCount; i++ )
		{
			m_ctrlSoundSourceTable.SetColumnWidth ( i, ( TableWidth * ColumnWidthPercent[i] ) / 100 );
		}

		//	<--	��ư �� ����Ʈ�ڽ� ��Ʈ�� �̵�	-->	//
		RECT	ButtonRect;
		m_ctrlButtonDelete.GetClientRect ( &ButtonRect );
		LONG	ButtonWidth = ButtonRect.right - ButtonRect.left;
		LONG	ButtonHeight = ButtonRect.bottom - ButtonRect.top;

//	<--	�۵����� ����, �̻���
//		RECT	EditRect;
//		m_ctrlEditKeyword.GetClientRect ( &EditRect );
//		LONG	EditWidth = EditRect.right - EditRect.left;
//	-->
		LONG	EditWidth = 200;

		LONG	ButtonInterval = 10;
		LONG	ButtonCx = (Width / 2) - ((ButtonWidth * 4 + EditWidth + ButtonInterval * 4) / 2);
		m_ctrlButtonDelete.MoveWindow	( ButtonCx + ButtonWidth * 0 + ButtonInterval * 0 + EditWidth * 0, Height - 30, ButtonWidth, ButtonHeight, FALSE );
		m_ctrlButtonModify.MoveWindow	( ButtonCx + ButtonWidth * 1 + ButtonInterval * 1 + EditWidth * 0, Height - 30, ButtonWidth, ButtonHeight, FALSE );
		m_ctrlEditKeyword.MoveWindow	( ButtonCx + ButtonWidth * 2 + ButtonInterval * 2 + EditWidth * 0, Height - 30, EditWidth,   ButtonHeight, FALSE );
		m_ctrlButtonFind.MoveWindow		( ButtonCx + ButtonWidth * 2 + ButtonInterval * 3 + EditWidth * 1, Height - 30, ButtonWidth, ButtonHeight, FALSE );
		m_ctrlButtonWrite.MoveWindow	( ButtonCx + ButtonWidth * 3 + ButtonInterval * 4 + EditWidth * 1, Height - 30, ButtonWidth, ButtonHeight, FALSE );

		InvalidateRect ( NULL );			 
	}
}

void CSoundSourceManagerView::OnBnClickedButtonWrite()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CSoundSourceDlg	dlg;
	
	dlg.SetState ( eNew );
	if ( dlg.DoModal() == IDOK )
	{	
		//	<--	���� ���� ��ü ���̱�	-->	//
		if( m_pSound )
		{
			m_pSound->Stop();
			m_pSound->Reset();		
			SAFE_DELETE( m_pSound );
		}

		ReloadAllItems ();
	}
}

void CSoundSourceManagerView::OnBnClickedButtonDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	DeleteSelectItems();
}

void CSoundSourceManagerView::OnBnClickedButtonModify()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION	pos = m_ctrlSoundSourceTable.GetFirstSelectedItemPosition ();
	if ( pos )
	{
		//	<--	���� ���� ��ü ���̱�	-->	//
		if( m_pSound )
		{
			m_pSound->Stop();
			m_pSound->Reset();		
			SAFE_DELETE( m_pSound );
		}

		int nItem = m_ctrlSoundSourceTable.GetNextSelectedItem ( pos );
		if ( nItem == -1 )
		{
			return ;
		}

		SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );		

		CSoundSourceDlg	dlg;
		dlg.SetState ( eModify, pRecord->ID );
		if ( dlg.DoModal () )
		{
			ReloadAllItems ();
		}
	}
}

void CSoundSourceManagerView::OnNMDblclkListSoundsourceTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	//	<--	���� �Ŵ��� ��ü �ʱ�ȭ�� ������ ���
	if ( !m_bNormal )
	{
		return;
	}

	//	<--	���� ���� ��ü ���̱�	-->	//
	if( m_pSound )
	{
		m_pSound->Stop();
		m_pSound->Reset();		
		SAFE_DELETE( m_pSound );
	}
	
	POSITION	pos = m_ctrlSoundSourceTable.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlSoundSourceTable.GetNextSelectedItem ( pos );
	if ( nItem == -1 )
	{
		return ;
	}

	SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );

	//	<--	��� ���� �̸�(FullPath) �����	-->	//
	CString	TargetFile = GetSSM()->GetDataDirectory() + pRecord->FileName;
	
	//	<--	Wav���� ���ۿ� ��� �ø���	-->	//
	if( FAILED( m_pSoundManager->Create( &m_pSound, TargetFile.GetBuffer(), 0, GUID_NULL ) ) )
	{
		MessageBox ( "���� ������ �����Ͽ����ϴ�." );
		return; 
	}

	if( FAILED( m_pSound->Play( 0, 0 ) ) )
	{
		MessageBox ( "����� �����Ͽ����ϴ�." );
		return;
	}
}

void CSoundSourceManagerView::OnNMClickListSoundsourceTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	CSoundSourceManagerDoc *pDoc = (CSoundSourceManagerDoc *) GetDocument ();
	POSITION	pos = m_ctrlSoundSourceTable.GetFirstSelectedItemPosition ();
	int nItem = m_ctrlSoundSourceTable.GetNextSelectedItem ( pos );
	if ( nItem == -1 )
	{
		return ;
	}

	SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );

	m_ctrlButtonDelete.EnableWindow ( pRecord->State );
	m_ctrlButtonModify.EnableWindow ( pRecord->State );
}

void CSoundSourceManagerView::OnNMRclickListSoundsourceTable(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	int pMenuID = 0;
	CMenu Menu;

	DWORD SelectionMade;
	VERIFY ( Menu.LoadMenu ( IDR_MENU_POPUP ) );

	CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
	ASSERT ( pPopup != NULL );	

	POINT pp;
	GetCursorPos (&pp);
	SelectionMade = pPopup->TrackPopupMenu (
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
		pp.x, pp.y, this );

	pPopup->DestroyMenu ();

	switch ( SelectionMade )
	{
	case ID_COMPLETEDATA_LOCK:
		{			
			//	<--	TRUE�� LOCK�� �Ŵ� ����, ���� ���۵ȴٴ� �̾߱�	-->	//
			ChangeState ( TRUE );
		}
		break;

	case ID_COMPLETEDATA_UNLOCK:
		{	
			//	<--	FALSE�� UNLOCK�� �Ŵ� ����, ���� �����ٴ� �̾߱�	-->	//
			ChangeState ( FALSE );
		}
		break;

	case ID_COMPLETEDATA_DELETE:
		{
			if ( MessageBox ( "���� �����Ͻðڽ��ϱ�", "����", MB_YESNO|MB_ICONQUESTION ) == IDYES )
			{
				DeleteSelectItems();
			}
			return;
		}
		break;

	case ID_COMPLETEDATA_COPY:
		{
			ITEMIDLIST *pidBrowse;
			TCHAR pszPathname[MAX_PATH] = {0};

			CString strTargetDir;

			BROWSEINFO Brinfo;
			memset( &Brinfo, 0 , sizeof(Brinfo));
			Brinfo.pszDisplayName = pszPathname;
			Brinfo.lpszTitle = _T("������ �����ϼ���");
			Brinfo.ulFlags = BIF_RETURNONLYFSDIRS;

			pidBrowse = ::SHBrowseForFolder(&Brinfo);
			if ( pidBrowse != NULL )
			{
				SHGetPathFromIDList(pidBrowse , pszPathname );		
				strTargetDir = pszPathname;
				strTargetDir += "\\";

				CopySelectItems ( GetSSM()->GetDataDirectory(), strTargetDir );
			}
		}
		break;

	default:
		return;		
	};
}

void	CSoundSourceManagerView::CopySelectItems ( const CString& strOriginDir, const CString& strTargetDir )
{
	POSITION	pos = m_ctrlSoundSourceTable.GetFirstSelectedItemPosition ();
	if ( !pos )	return;

	int nCount = 0;
	while ( pos )
	{        
		int nItem = m_ctrlSoundSourceTable.GetNextSelectedItem ( pos );
		SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );
		if ( !pRecord )
		{
			GASSERT ( 0 && "���ڵ尡 �������� �ʽ��ϴ�." );
			continue;
		}

		CString strSrcFile = strOriginDir + pRecord->FileName;
		CString strTarFile = strTargetDir + pRecord->FileName;

		if ( !CopyFile ( strSrcFile, strTarFile, TRUE ) )
		{
			DWORD dwError = GetLastError ();
			switch ( dwError )
			{
			case ERROR_FILE_EXISTS:
				CString strTryAgain;
				strTryAgain.Format ( "%s�� �����մϴ�. ����ðڽ��ϱ�?", strTarFile );
				if ( MessageBox ( strTryAgain, "����", MB_YESNO|MB_ICONQUESTION ) == IDYES )
				{
					CopyFile ( strSrcFile, strTarFile, FALSE );
				}
				break;
			}
		}

		nCount++;
	} 

	CString strResult;
	strResult.Format ( "�ҽ� : %s\nŸ�� : %s\n"
		"%d���� ���� ���翡 �����Ͽ����ϴ�.", strOriginDir, strTargetDir, nCount );
	MessageBox ( strResult );
}

void	CSoundSourceManagerView::DeleteSelectItems()
{
	POSITION	pos = m_ctrlSoundSourceTable.GetFirstSelectedItemPosition ();

	if ( !pos )
	{
		return;
	}

	//	<--	���� ���� ��ü ���̱�	-->	//
	if( m_pSound )
	{
		m_pSound->Stop();
		m_pSound->Reset();		
		SAFE_DELETE( m_pSound );
	}

	int	TotalCount = 0;
	int CurCount = 0;
	while ( pos )
	{        
		int nItem = m_ctrlSoundSourceTable.GetNextSelectedItem ( pos );
		SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );
		if ( pRecord->State )
		{
			TotalCount++;
		}
	}

	if ( !TotalCount )
	{
		MessageBox ( "������ ���� ���� �������� �ʽ��ϴ�." );
		return ;
	}

	int *pItems = new int [ TotalCount ];
	pos = m_ctrlSoundSourceTable.GetFirstSelectedItemPosition ();
	while ( pos )
	{
		int nItem = m_ctrlSoundSourceTable.GetNextSelectedItem ( pos );
		SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );
		if ( pRecord->State )
		{
			pItems[CurCount] = nItem;
			CurCount++;
		}
	}

	if ( TotalCount != CurCount )
	{
		MessageBox ( "�����ӵ�, ���� ������ ���� �ֽ���" );
		return ;
	}

    CurCount = TotalCount;
	while ( CurCount )
	{
		int nItem = pItems[CurCount - 1];
		SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );
		GetSSM()->DelRecord ( pRecord->ID );
		m_ctrlSoundSourceTable.DeleteItem ( nItem );

		CurCount--;
	}

	delete [] pItems;

	ReloadAllItems();
}

void	CSoundSourceManagerView::ChangeState( BOOL State )
{
	CSoundSourceManagerDoc *pDoc = (CSoundSourceManagerDoc *) GetDocument ();
	POSITION	pos = m_ctrlSoundSourceTable.GetFirstSelectedItemPosition ();
	
	while ( pos )	//	���콺�� ���ڵ忡 ����� Ŭ���� ��츸 ó���Ѵ�.
	{
		int nItem = m_ctrlSoundSourceTable.GetNextSelectedItem ( pos );
		if ( nItem == -1 )
		{
			MessageBox ( "�����߻� ChangeState" );
			return ;
		}

		SSoundSource* pRecord = (SSoundSource*)m_ctrlSoundSourceTable.GetItemData ( nItem );
		pRecord->State = State;
	}

	ReloadAllItems();
}

void CSoundSourceManagerView::OnBnClickedButtonFind()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	ReloadAllItems();
}

void	CSoundSourceManagerView::ReloadAllItems()
{
	m_ctrlSoundSourceTable.DeleteAllItems ();

	SSoundSource*	pReloadSource;
	CString			str;

	UpdateData ( TRUE );

	int nIndex = m_ctrlCategory.GetCurSel ();
	int	nCategory = (int)m_ctrlCategory.GetItemData ( nIndex );

    int Amount = 0;
	GetSSM()->GoTail ();
	for ( int i = 0; i < GetSSM()->GetCount(); i++ )
	{
		pReloadSource = GetSSM()->GetRecord ();

		if ( pReloadSource->TYPE == nCategory || nCategory == 7 )
		{
			if ( CompareAllToken ( pReloadSource->Comment, m_valKeyword ) )
			{
				str.Format ( "%d", pReloadSource->ID );
				int nItem = m_ctrlSoundSourceTable.InsertItem ( Amount, NULL, pReloadSource->State );		
				m_ctrlSoundSourceTable.SetItemText ( Amount, 1, str );	//	ID
				m_ctrlSoundSourceTable.SetItemText ( Amount, 2, NTypeDesc::Desc[pReloadSource->TYPE] );
				str.Format ( "%d", pReloadSource->BufferCount );
				m_ctrlSoundSourceTable.SetItemText ( Amount, 3, str );
				m_ctrlSoundSourceTable.SetItemText ( Amount, 4, pReloadSource->FileName );
				m_ctrlSoundSourceTable.SetItemText ( Amount, 5, pReloadSource->LastUpdated );
				m_ctrlSoundSourceTable.SetItemText ( Amount, 6, pReloadSource->Comment );

				m_ctrlSoundSourceTable.SetItemData ( nItem, (DWORD_PTR)pReloadSource );

				Amount++;
			}
		}

		GetSSM()->GoPrev ();
	}
}

BOOL CSoundSourceManagerView::CompareAllToken ( CString Left, CString Right )
{	
	char szRight[1024];
	int	LengthRight = 0;
	CString Token[32];
	int TokenCount = 0;
	
	strcpy ( szRight, Right.GetString () );
	LengthRight = Right.GetLength ();

	for ( int i = 0; i < LengthRight; i++ )
	{
		//	<--	ù��°���� �����϶�	-->	//
		if ( i == 0 && szRight[i] == ' ' )
		{			
		}
		//	<--	�ι�°���� �����϶� -->	//
		else if ( szRight[i] == ' ' && szRight[i - 1] != ' ' )
		{			
			TokenCount++;			
		}
		else
		{
			if ( !Token[TokenCount].GetLength () )
			{
				Token[TokenCount] = szRight[i];
			}
			else
			{
				Token[TokenCount] += szRight[i];
			}
		}
	}

	BOOL	bResult = TRUE;
    for ( int j = 0; j <= TokenCount; j++ )
	{
		 if ( !strstr ( Left.GetString (), Token[j].GetString () ) )
		 {
			 bResult = FALSE;
		 }
	}

	return bResult;
}

void CSoundSourceManagerView::OnCbnSelchangeComboCategory()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_valKeyword = "";
	UpdateData ( FALSE );
	ReloadAllItems();
}

void CSoundSourceManagerView::OnMenuitemTextout()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "���� ��� �ؽ�Ʈ ���� (*.txt)|*.txt|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,".txt",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter);

//	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{

		FILE* fp = fopen ( dlg.GetFileName ().GetString(), "wt" );
		if ( !fp ) return ;

		CString strTemp;
		GetSSM ()->GoHead ();
		while ( !GetSSM()->IsEnd () )
		{
			SSoundSource* pSoundSource = GetSSM()->GetRecord();		
			fprintf ( fp, "%s | %-64.64s | %s\n",
				pSoundSource->FileName, pSoundSource->Comment,
				pSoundSource->LastUpdated );
			GetSSM()->GoNext ();
		}
		fclose ( fp );

		MessageBox ( "������µǾ����ϴ�.", "Ȯ��", MB_OK );
	}
}

void CSoundSourceManagerView::OnMenuitemLoad()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "���̺� ���̺� (*.sg)|*.sg|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".sg",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter);

//	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		if ( !m_pSoundSourceMan->LoadFile ( dlg.GetPathName().GetString() ) )
		{
			MessageBox ( "���� �б⿡ �����Ͽ����ϴ�.", "���", MB_OK );
			return ;
		}
		CString strDirectory = dlg.GetPathName ();
		strDirectory = strDirectory.Left ( strDirectory.ReverseFind ( '\\' ) + 1 );
		m_pSoundSourceMan->SetDataDirectory ( strDirectory );
		
		ReloadAllItems();

		((CEdit*)GetDlgItem ( IDC_EDIT_SRCDIRECTORY ))->SetWindowText ( strDirectory );		
	}
}

void CSoundSourceManagerView::OnMenuitemSave()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "���̺� ���̺� (*.sg)|*.sg|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,".sg",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter);

//	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		if ( !m_pSoundSourceMan->SaveFile ( dlg.GetPathName().GetString() ) )
			MessageBox ( "���� ���⿡ �����Ͽ����ϴ�.", "���", MB_OK );
	}
}

void CSoundSourceManagerView::OnMenuitemIntegrity()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CWavFileIntegrity dlg;
	dlg.DoModal ();
}

void CSoundSourceManagerView::OnMenuitemFile2listTrace()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CFile2ListDialog dlg;
	dlg.DoModal ();
}
