// DlgCompress.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include <process.h>
#include "VersionManager.h"
#include "DlgCompress.h"
#include "MainFrm.h"

#include "MIN_CAB_UTIL.h"
#include "CConsoleMessage.h"
#include "ListLoader.h"
#include "CCfg.h"
#include ".\dlgcompress.h"

// CDlgCompress ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgCompress, CDialog)
CDlgCompress::CDlgCompress(CMainFrame* pFrame, std::vector<CompressFile> &v, CWnd* pParent /*=NULL*/, int nMode)
	: CDialog(CDlgCompress::IDD, pParent)
{
	m_vCompressFile = v;
	m_pFrame		= pFrame;
	m_nMode			= nMode;
}

CDlgCompress::CDlgCompress(CMainFrame* pFrame, CString strFileName, CWnd* pParent, int nMode)   // ǥ�� �������Դϴ�.
	: CDialog(CDlgCompress::IDD, pParent)
{
	m_nMode			= nMode;
	m_pFrame		= pFrame;
	m_strFileName	= strFileName;
}

CDlgCompress::~CDlgCompress()
{
	CConsoleMessage::GetInstance()->ReleaseInstance();
}

void CDlgCompress::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_PROGRESS1, m_Pregress);
	DDX_Control(pDX, IDC_STATIC_PG, m_StaticCtl);
}


BEGIN_MESSAGE_MAP(CDlgCompress, CDialog)
	ON_WM_TIMER()
    ON_EN_MAXTEXT(IDC_EDIT_CONSOLE, OnEnMaxtextEditConsole)
END_MESSAGE_MAP()


// CDlgCompress �޽��� ó�����Դϴ�.

BOOL CDlgCompress::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	CConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_CONSOLE)->m_hWnd);	
	m_Pregress.SetRange(0, 100);	
	m_Pregress.SetPos(0);

	if (m_nMode == 1) // �Ϲ��� �����߰�
	{
		SetTimer(100, 1000, NULL);
	}
	else if(m_nMode == 2)
	{
		SetTimer(200, 1000, NULL);
	}
	else
	{
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgCompress::OnTimer(UINT nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (nIDEvent == 100)
	{
		KillTimer(100);
		threadCompressStart();
	}
	
	if (nIDEvent == 200)
	{
		KillTimer(200);
		threadCompressFromFileStart();
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgCompress::threadCompressStart()
{
	DWORD dwThreadId;

	HANDLE hThread = (HANDLE) ::_beginthreadex(
								NULL, 
								0, 
								CompressStart,
								this,
								0,
								(unsigned int*) &dwThreadId);
}

void CDlgCompress::threadCompressFromFileStart()
{
	DWORD dwThreadId;

	HANDLE hThread = (HANDLE) ::_beginthreadex(
								NULL, 
								0, 
								CompressFromFileStart,
								this,
								0,
								(unsigned int*) &dwThreadId);
}

unsigned int WINAPI CDlgCompress::CompressFromFileStart( void* pDlg )
{
	CDlgCompress* pDLG = (CDlgCompress*) pDlg;

	CListLoader ListLoader( pDLG->m_strFileName );

	// ���丮���� �����ϰ� DB �� �Է��Ѵ�.
	std::vector<FolderList>::iterator pos;
	int nTotalCount = (int) ListLoader.m_vFolderList.size();
    int nRetCode;
	DWORD dwRetCode;
	CString strFolder;

    CConsoleMessage::GetInstance()->Write("----������ üũ�մϴ�----");

	//////////////////////////////////////////////////////////////////////////////
	// ��Ʈ���丮�� ������ �����Ѵ�. "\\"
	strFolder = CCfg::GetInstance()->m_szTargetPath;
	CreateDirectory(strFolder, NULL);
	COdbcManager::GetInstance()->CreateFolder("\\");
	//////////////////////////////////////////////////////////////////////////////

	for (pos = ListLoader.m_vFolderList.begin(); pos<ListLoader.m_vFolderList.end(); ++pos)
	{		
		// DB �� ������ �ִ��� üũ�Ѵ�.        
        nRetCode = COdbcManager::GetInstance()->IsFolderExist((*pos).strName);
        strFolder = CCfg::GetInstance()->m_szTargetPath + (*pos).strName;
        strFolder.Replace('/', '\\');

        if (nRetCode == TRUE)
        {
			// DB �� �ְ� ���� �����Ѵٸ� ��ŵ
            dwRetCode = GetFileAttributes(strFolder);
            if (dwRetCode == INVALID_FILE_ATTRIBUTES) // DB ���� ������ �������� �ʴ� ���
            {
                // ���丮�� �����Ѵ�.
                if (CreateDirectory(strFolder, NULL) == 0)
		        {
			        // ��������
                    CConsoleMessage::GetInstance()->Write("%s ���丮�� �������� �ʾҽ��ϴ�", strFolder.GetString());			        
		        }
		        else
		        {
			        // ��������
		        }
            } 
            else // DB �� �ԷµǾ� �ְ� ������ �����ϴ� ��� 
            {                                   
            }
        }
        else // DB �� ���°��
        {
            // ���丮�� �����Ѵ�.
            if (CreateDirectory(strFolder, NULL) == 0)
		    {
			    // ��������
				CConsoleMessage::GetInstance()->Write("%s ���丮�� üũ�Ͻʽÿ�", strFolder.GetString());
		    }
		    else
		    {
			    // ��������
			    // DB �� ������ ���� �̸� �Է�
                CString strXXX = (*pos).strName;
                strXXX.Replace('/', '\\');
			    if (COdbcManager::GetInstance()->CreateFolder(strXXX) != DB_OK)
			    {
				    // �������� ���� DB ���� �Էµ��� �ʾ���
                    CConsoleMessage::GetInstance()->Write("%s ���丮�� DB �� �Էµ��� �ʾҽ��ϴ�.", strFolder.GetString());    			
			    }
                else
                {
                    // ����
                }
		    }            
        }	
	}
    CConsoleMessage::GetInstance()->Write("----���� üũ �Ϸ�----");
	// ������ �ϳ��� �����ؼ� �ش� ������ �ű��.
	std::vector<CompressFile>::iterator posFile;

    CConsoleMessage::GetInstance()->Write("----������ üũ�մϴ�----");
    
    // ���丮 ������ȣ�� �־ �����Ѵ�.
	for (posFile = ListLoader.m_vFile.begin(); posFile<ListLoader.m_vFile.end(); ++posFile)
	{
		// ���丮�� ������ȣ�� ������ ���� �ִ´�.
        int nDidx = COdbcManager::GetInstance()->GetFolderNum((*posFile).strPurePath);

        if (nDidx == DB_ERROR)
        {
            CConsoleMessage::GetInstance()->Write("%s �� �������� ã�� ���߽��ϴ�", (*posFile).strTarget.GetString());
        }
        else
        {
            (*posFile).nDir = nDidx;
            CConsoleMessage::GetInstance()->Write("%s �� �������� %d �Դϴ�", (*posFile).strTarget.GetString(), nDidx);
        }

        // ������ ����Ѵ�.
	}

    nTotalCount = (int) ListLoader.m_vFile.size();
    int nCount = 0;

    // ������ �����ؼ� �ִ´�.
    for (posFile = ListLoader.m_vFile.begin(); posFile<ListLoader.m_vFile.end(); ++posFile)
	{
		nCount++;
		CString strTemp;
        CompressFile sTemp;
        sTemp = (CompressFile) *posFile;
		strTemp.Format("%s %d/%d", sTemp.strFileName.GetString(), nCount, nTotalCount);
		pDLG->m_StaticCtl.SetWindowText(strTemp);
		sTemp.strSrc.GetString();
		// �����ؼ� �ش� ���丮�� �����Ѵ�.
		if (CAB_UTIL_MIN::MinMakeCab(sTemp.strSrc.GetString(), sTemp.strTarget.GetString()) == TRUE)
		{ 
			// ���� �̸��� ������ �ִ��� �����Ѵ�.
			BOOL bExist = COdbcManager::GetInstance()->IsFileExist(sTemp.strFileName);
			if (bExist)
			{
                // ���� ���丮�� ���� �������� �����Ѵ�.
                int nDirNum = COdbcManager::GetInstance()->GetFileFolderNum(sTemp.strFileName);
                
                // ���� ���丮�� ���� �����̸� ������Ʈ �Ѵ�.
                if (nDirNum == sTemp.nDir) 
                {               
				    COdbcManager::GetInstance()->UpdateFile( sTemp.strFileName, sTemp.strMD5 );
                    COdbcManager::GetInstance()->UpdateFileStateFalse( sTemp.strFileName );
				    // CConsoleMessage::GetInstance()->Write("%s ���� ������ ��ü�Ͽ����ϴ�", sTemp.strFileName.GetString());
                }                
                else // �ٸ� ���丮�� ���� ���ϸ��̸� ������ ����Ѵ�.
                {
                    CConsoleMessage::GetInstance()->Write("%s ������ �̹� �����մϴ�", sTemp.strFileName.GetString());
                }
            }
			else
			{
				// �ű�����
				COdbcManager::GetInstance()->InsertFile( (*posFile).nDir, (*posFile).strFileName, (*posFile).strMD5 );
				// CConsoleMessage::GetInstance()->Write("%s ���ο� ������ ����߽��ϴ�", sTemp.strFileName.GetString());
			}
		}
		else
		{
			CConsoleMessage::GetInstance()->Write("%s ���� ���࿡ �����Ͽ����ϴ�", sTemp.strFileName.GetString());			
		}		
		
		pDLG->m_Pregress.SetPos((int)((nCount*100)/nTotalCount));
	}

    CConsoleMessage::GetInstance()->Write("----���� üũ �Ϸ�----");

	return 0;
}

unsigned int WINAPI CDlgCompress::CompressStart( void* pDlg )
{
	CDlgCompress* pDLG = (CDlgCompress*) pDlg;

	std::vector<CompressFile>::iterator pos;
	int nTotalCount = (int) pDLG->m_vCompressFile.size();
	int nCount = 0;

	for (pos = pDLG->m_vCompressFile.begin(); pos<pDLG->m_vCompressFile.end(); ++pos)
	{
		nCount++;
		CString strTemp;
        CompressFile sTemp;
        sTemp = (CompressFile) *pos;
		strTemp.Format("%s %d/%d", sTemp.strFileName.GetString(), nCount, nTotalCount);
		pDLG->m_StaticCtl.SetWindowText(strTemp);
        
		sTemp.strSrc.GetString();
		// �����ؼ� �ش� ���丮�� �����Ѵ�.
		if (CAB_UTIL_MIN::MinMakeCab(sTemp.strSrc.GetString(), sTemp.strTarget.GetString()) == TRUE)
		{ 
			// ���� �̸��� ������ �ִ��� �����Ѵ�.
			BOOL bExist = COdbcManager::GetInstance()->IsFileExist(sTemp.strFileName);
			if (bExist)
			{
				COdbcManager::GetInstance()->UpdateFile( sTemp.strFileName, sTemp.strMD5 );
                COdbcManager::GetInstance()->UpdateFileStateFalse(sTemp.strFileName); 
				CConsoleMessage::GetInstance()->Write("%s ���� ������ ��ü�Ͽ����ϴ�", sTemp.strFileName.GetString());
			}
			else
			{
				COdbcManager::GetInstance()->InsertFile( sTemp.nDir, sTemp.strFileName, sTemp.strMD5 );
				CConsoleMessage::GetInstance()->Write("%s ���ο� ������ ����߽��ϴ�", sTemp.strFileName.GetString());
			}
		}
		else
		{
			CConsoleMessage::GetInstance()->Write("%s ���� ���࿡ �����Ͽ����ϴ�", sTemp.strFileName.GetString());			
		}
		pDLG->m_Pregress.SetPos((int)((nCount*100)/nTotalCount));
	}

	return 0;
}

void CDlgCompress::OnEnMaxtextEditConsole()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    GetDlgItem(IDC_EDIT_CONSOLE)->SetWindowText("");
}
