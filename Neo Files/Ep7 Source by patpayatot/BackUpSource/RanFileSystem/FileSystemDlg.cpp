// FileSystemDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "FileSystem.h"
#include "FileSystemDlg.h"
#include "NewFolderDlg.h"
#include ".\filesystemdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CFileSystemDlg ��ȭ ����

static int CALLBACK FileListItemSortFunc(LPARAM lParam1,LPARAM lParam2,LPARAM /*lParamSort*/)
{
	SFileHandle* pFH1 = (SFileHandle*)lParam1;
	SFileHandle* pFH2 = (SFileHandle*)lParam2;

	CString strItem1;
	if(pFH1->GetFileContext()->iType == FT_DIR)
	{
		strItem1.Format("!%s",pFH1->GetFileContext()->strName);
	}
	else
	{
		strItem1 = pFH1->GetFileContext()->strName;
	}

	CString strItem2;
	if(pFH2->GetFileContext()->iType == FT_DIR)
	{
		strItem2.Format("!%s",pFH2->GetFileContext()->strName);
	}
	else
	{
		strItem2 = pFH2->GetFileContext()->strName;
	}

	return strcmp(strItem1,strItem2);
}

void CFileSystemDlg::UpdateFileList()
{
	m_FileList.DeleteAllItems();

	int iCount = 0;
	CString strFileSize;
	SFileHandleList* pFHL = m_FileSystem.GetFileHandleList();
	for(SFileHandleList_i it=pFHL->begin();it!=pFHL->end();++it)
	{
		m_FileList.InsertItem(iCount,(*it)->GetFileContext()->strName,(*it)->GetFileContext()->iType);

		m_FileList.SetItemData(iCount,(DWORD)(*it));

		if((*it)->GetFileContext()->iType == FT_FILE)
		{
			strFileSize.Format("%dKB",((*it)->GetFileContext()->lCompressSize+999)/1000);
			m_FileList.SetItemText(iCount,1,LPCTSTR(strFileSize));
		}

		iCount++;
	}

	m_FileList.SortItems(FileListItemSortFunc,NULL);
}

void CFileSystemDlg::DropFile(HDROP hDrop)
{
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox("�������Ͻý����� �����Ǿ� ���� �ʽ��ϴ�.");
		return;
	}

	int iCount = DragQueryFile(hDrop,0xFFFFFFFF,NULL,0);

	TCHAR strPath[_MAX_PATH];
	for(int i=0;i<iCount;i++)
	{
		DragQueryFile(hDrop,i,strPath,_MAX_PATH);

		FILE* bFile = fopen(strPath,"rb");
		if(bFile) fclose(bFile);

		TCHAR strName[_MAX_FNAME];
		m_FileSystem.GetNameOnly(strName,strPath);

		if(bFile)
		{
			if(m_FileSystem.CheckNameExist(strName))
			{
				if(m_iOverwriteType < 0) m_iOverwriteType = AfxMessageBox("������ �̸��� ������ �̹� �����մϴ�.\n\n��� ������ ���ο� ���Ϸ� ����÷��� '��' ��\n\n��� ������ ������ �����Ͻ÷��� '�ƴϿ�' �� �����ֽʽÿ�!   ",MB_YESNO);
				if(m_iOverwriteType == IDNO) continue;

				m_FileSystem.Remove(strName);
			}
			if(!m_FileSystem.AddFile(strPath))
			{
				CString strError;
				strError.Format("������ �߰��ϴµ� �����Ͽ����ϴ�.\n\n%s",strPath);
				AfxMessageBox(strError);
			}
		}
		else
		{
			if(!DropFolder(strPath))
			{
				CString strError;
				strError.Format("������ �߰��ϴµ� �����Ͽ����ϴ�.\n\n%s",strPath);
				AfxMessageBox(strError);
			}
		}
	}

	m_iOverwriteType = -1;

	UpdateFileList();

	DragFinish(hDrop);
}

BOOL CFileSystemDlg::DropFolder(const TCHAR* strPath)
{
	TCHAR strOldPath[_MAX_PATH];
	GetCurrentDirectory(_MAX_PATH,strOldPath);
	SetCurrentDirectory(strPath);

	TCHAR strName[_MAX_FNAME];
	m_FileSystem.GetNameOnly(strName,strPath);

	if(!m_FileSystem.CheckNameExist(strName))
	{
		m_FileSystem.AddDir(strName);
	}
	if(!m_FileSystem.ChangeDir(strName)) return FALSE;

	CFileFind FileFind;
	if(FileFind.FindFile())
	{
		for(BOOL bNext=TRUE;bNext;)
		{
			bNext = FileFind.FindNextFile();
			CString strP = FileFind.GetFilePath();
			CString strN = FileFind.GetFileName();
			if(strN == "." || strN == "..") continue;

			if(FileFind.IsDirectory())
			{
				DropFolder(LPCTSTR(strP));
			}
			else
			{
				if(m_FileSystem.CheckNameExist(strN))
				{
					if(m_iOverwriteType < 0) m_iOverwriteType = AfxMessageBox("������ �̸��� ������ �̹� �����մϴ�.\n\n��� ������ ���ο� ���Ϸ� ����÷��� '��' ��\n\n��� ������ ������ �����Ͻ÷��� '�ƴϿ�' �� �����ֽʽÿ�!",MB_YESNO);
					if(m_iOverwriteType == IDNO) continue;

					m_FileSystem.Remove(strN);
				}
				if(!m_FileSystem.AddFile(strP))
				{
					CString strError;
					strError.Format("������ �߰��ϴµ� �����Ͽ����ϴ�.\n\n%s",strPath);
					AfxMessageBox(strError);
				}
			}
		}
	}

	FileFind.Close();

	m_FileSystem.ChangeDir("..");

	SetCurrentDirectory(strOldPath);

	return TRUE;
}

CFileSystemDlg::CFileSystemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileSystemDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_iOverwriteType = -1;
}

void CFileSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILELIST, m_FileList);
}

BEGIN_MESSAGE_MAP(CFileSystemDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(IDM_FILE_NEW, OnFileNew)
	ON_COMMAND(IDM_FILE_OPEN, OnFileOpen)
	ON_COMMAND(IDM_FILE_CLOSE, OnFileClose)
	ON_BN_CLICKED(IDC_NEWFOLDER, OnBnClickedNewfolder)
	ON_BN_CLICKED(IDC_INSERTFILE, OnBnClickedInsertfile)
	ON_BN_CLICKED(IDC_REMOVE, OnBnClickedRemove)
	ON_BN_CLICKED(IDC_OPTIMIZE, OnBnClickedOptimize)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_FILELIST, OnLvnEndlabeleditFilelist)
	ON_NOTIFY(NM_DBLCLK, IDC_FILELIST, OnNMDblclkFilelist)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnBnClickedButtonOpen)
END_MESSAGE_MAP()

// CFileSystemDlg �޽��� ó����

BOOL CFileSystemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	// �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_FileList.DragAcceptFiles();
	m_FileList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	RECT Rect;
	m_FileList.GetWindowRect(&Rect);

	const int COLUMN_WIDTH_GAB = 4;
	const int SIZECOLUMN_WIDTH = 80;
	m_FileList.InsertColumn(0,"�̸�",LVCFMT_LEFT,(Rect.right-Rect.left)-(SIZECOLUMN_WIDTH+COLUMN_WIDTH_GAB));
	m_FileList.InsertColumn(1,"ũ��",LVCFMT_RIGHT,SIZECOLUMN_WIDTH);

	m_ImageList.Create(IDB_FILELISTICON,16,2,RGB(0,255,0));
	m_FileList.SetImageList(&m_ImageList,LVSIL_SMALL);

	return TRUE;  // ��Ʈ�ѿ� ���� ��Ŀ���� �������� ���� ��� TRUE�� ��ȯ�մϴ�.
}

void CFileSystemDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸����� 
// �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
// �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CFileSystemDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�. 
HCURSOR CFileSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFileSystemDlg::OnFileNew()
{
	CFileDialog Dlg(FALSE,".rpf","filename.rpf",OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"����(*.rpf)|*.rpf||",NULL);
	if(Dlg.DoModal() == IDCANCEL) return;

	if(!m_FileSystem.NewFileSystem(LPCTSTR(Dlg.GetPathName()))) return;
	if(!m_FileSystem.OpenFileSystem(LPCTSTR(Dlg.GetPathName()))) return;

	m_strPath = Dlg.GetPathName();

	UpdateFileList();
}

void CFileSystemDlg::OnFileOpen()
{
	CFileDialog Dlg(TRUE,".rpf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"����(*.rpf)|*.rpf||",NULL);
	if(Dlg.DoModal() == IDCANCEL) return;

	if(!m_FileSystem.OpenFileSystem(LPCTSTR(Dlg.GetPathName()))) return;

	m_strPath = Dlg.GetPathName();

	UpdateFileList();
}

void CFileSystemDlg::OnFileClose()
{
	m_FileSystem.CloseFileSystem();

	m_strPath.Empty();

	UpdateFileList();
}

void CFileSystemDlg::OnBnClickedNewfolder()
{
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox("�������Ͻý����� �����Ǿ� ���� �ʽ��ϴ�.");
		return;
	}

	CNewFolderDlg Dlg;
	if(Dlg.DoModal() == IDCANCEL) return;
	if(Dlg.m_strFolderName.IsEmpty()) return;

	m_FileSystem.AddDir(LPCTSTR(Dlg.m_strFolderName));

	UpdateFileList();
}

void CFileSystemDlg::OnBnClickedInsertfile()
{
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox("�������Ͻý����� �����Ǿ� ���� �ʽ��ϴ�.");
		return;
	}

	CFileDialog Dlg(TRUE);
	if(Dlg.DoModal() == IDCANCEL) return;

	TCHAR strName[_MAX_FNAME];
	m_FileSystem.GetNameOnly(strName,LPCTSTR(Dlg.GetPathName()));
	if(m_FileSystem.CheckNameExist(strName))
	{
		if(AfxMessageBox("������ �̸��� ������ �̹� �����մϴ�.\n\n��� ������ ���ο� ���Ϸ� ����÷��� '��' ��\n\n��� ������ ������ �����Ͻ÷��� '�ƴϿ�' �� �����ֽʽÿ�!   ",MB_YESNO) == IDNO) return;

		m_FileSystem.Remove(strName);
	}

	if(!m_FileSystem.AddFile(LPCTSTR(Dlg.GetPathName())))
	{
		CString strError;
		strError.Format("������ �߰��ϴµ� �����Ͽ����ϴ�.\n\n%s",LPCTSTR(Dlg.GetPathName()));
		AfxMessageBox(strError);
	}

	UpdateFileList();
}

void CFileSystemDlg::OnBnClickedRemove()
{
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox("�������Ͻý����� �����Ǿ� ���� �ʽ��ϴ�.");
		return;
	}

	if(!m_FileList.GetSelectedCount()) return;

	if(AfxMessageBox("���õ� ���� Ȥ�� ������ �����Ͻðڽ��ϱ�?", MB_YESNO) == IDYES)
	{
		CStringArray strRemove;

		POSITION Pos = m_FileList.GetFirstSelectedItemPosition();
		while(Pos)
		{
			int iItem = m_FileList.GetNextSelectedItem(Pos);
			SFileHandle* pFH = (SFileHandle*)m_FileList.GetItemData(iItem);

			strRemove.Add(pFH->GetFileContext()->strName);
		}

		for(int i=0;i<strRemove.GetSize();i++) m_FileSystem.Remove(strRemove[i]);

		UpdateFileList();
	}
}

void CFileSystemDlg::OnBnClickedOptimize()
{
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox("�������Ͻý����� �����Ǿ� ���� �ʽ��ϴ�.");
		return;
	}

	CString strPath = m_strPath;
	strPath = strPath.Left(strPath.ReverseFind('\\'));
	strPath = strPath + "\\Optimized.rpf";

	if(!m_FileSystem.OptimizeFileSystem(strPath))
	{
		AfxMessageBox("����ȭ�۾��� �����Ͽ����ϴ�!");
		return;
	}

	m_FileSystem.CloseFileSystem();
	remove(m_strPath);
	rename(strPath,m_strPath);
	m_FileSystem.OpenFileSystem(m_strPath);

	UpdateFileList();

	AfxMessageBox("����ȭ�۾��� ���������� �����Ͽ����ϴ�!");
}

void CFileSystemDlg::OnLvnEndlabeleditFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	if(!pDispInfo->item.pszText) return;

	SFileHandle* pFH = (SFileHandle*)m_FileList.GetItemData(pDispInfo->item.iItem);
	if(!m_FileSystem.Rename(pFH->GetFileContext()->strName,pDispInfo->item.pszText))
	{
		AfxMessageBox("���õ� ���� Ȥ�� ������ �̸��� �����Ҽ� �����ϴ�!");
		return;
	}

	m_FileList.SetItemText(pDispInfo->item.iItem,0,pDispInfo->item.pszText);

	*pResult = 0;
}

void CFileSystemDlg::OnNMDblclkFilelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(m_FileList.GetSelectedCount() != 1) return;

	NMITEMACTIVATE* pNMItemActivate = (NMITEMACTIVATE*)pNMHDR;
	if(pNMItemActivate->iItem >= 0)
	{
		SFileHandle* pFH = (SFileHandle*)m_FileList.GetItemData(pNMItemActivate->iItem);

		if(pFH->GetFileContext()->iType == FT_DIR)
		{
			m_FileSystem.ChangeDir(pFH->GetFileContext()->strName);

			UpdateFileList();
		}
	}

	*pResult = 0;
}

BOOL CFileSystemDlg::PreTranslateMessage(MSG* pMsg)
{
	switch(pMsg->message)
	{
	case WM_DROPFILES:
		if(pMsg->hwnd == m_FileList.GetSafeHwnd()) DropFile((HDROP)pMsg->wParam);
		break;
	case WM_KEYDOWN:
		if(pMsg->hwnd == m_FileList.GetSafeHwnd() && pMsg->wParam == VK_DELETE) OnBnClickedRemove();
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CFileSystemDlg::OnBnClickedButtonOpen()
{
	if(!m_FileList.GetItemCount())
	{
		AfxMessageBox("�������Ͻý����� �����Ǿ� ���� �ʽ��ϴ�.");
		return;
	}

	if(!m_FileList.GetSelectedCount()) return;

	CStringArray strOpenFile;

	POSITION Pos = m_FileList.GetFirstSelectedItemPosition();
	while(Pos)
	{
		int iItem = m_FileList.GetNextSelectedItem(Pos);
		SFileHandle* pFH = (SFileHandle*)m_FileList.GetItemData(iItem);

		strOpenFile.Add(pFH->GetFileContext()->strName);
	}

	for(int i=0;i<strOpenFile.GetSize();i++) 
	{
		SFileHandle * pFileHandle = m_FileSystem.OpenFile(strOpenFile[i]);
		if( pFileHandle )
		{
			TCHAR szFileName[256] = {0};
			m_FileSystem.GetNameOnly( szFileName, strOpenFile[i] );

			CFile cFile;
			cFile.Open( szFileName, CFile::modeCreate | CFile::modeWrite );

			BYTE buffer[4096];
			DWORD dwRead;

			do
			{
				dwRead = pFileHandle->Read( buffer, 4096 );
				cFile.Write(buffer, dwRead);
			}
			while(dwRead > 0);

			cFile.Close();

			m_FileSystem.CloseFile( pFileHandle );
		}
	}

	UpdateFileList();
}