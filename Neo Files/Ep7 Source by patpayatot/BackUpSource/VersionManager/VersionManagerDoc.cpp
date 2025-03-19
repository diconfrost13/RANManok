// VersionManagerDoc.cpp : CVersionManagerDoc Ŭ������ ����
//

#include "stdafx.h"
#include "VersionManager.h"
#include "VersionManagerDoc.h"
#include "DlgCompress.h"
#include "DlgFTP.h"
#include "CMinFtp.h"
#include "CCfg.h"
#include "DlgFileCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVersionManagerDoc

IMPLEMENT_DYNCREATE(CVersionManagerDoc, CDocument)

BEGIN_MESSAGE_MAP(CVersionManagerDoc, CDocument)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
    ON_COMMAND(ID_FTP, OnFtp)
    ON_COMMAND(ID_CHECK, OnCheck)
    ON_COMMAND(ID_FILE_SAVE, OnFileSave)
END_MESSAGE_MAP()


// CVersionManagerDoc ����/�Ҹ�

CVersionManagerDoc::CVersionManagerDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.	
}

CVersionManagerDoc::~CVersionManagerDoc()
{
}

BOOL CVersionManagerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ �ٽ� �ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CVersionManagerDoc serialization

void CVersionManagerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CVersionManagerDoc ����

#ifdef _DEBUG
void CVersionManagerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVersionManagerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CVersionManagerDoc ���

void CVersionManagerDoc::GetAppPath()
{	
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			m_strFullPath = strCommandLine.Left ( dwFind );
			
			if ( !m_strFullPath.IsEmpty() )
			if ( m_strFullPath.GetAt(0) == '"' )
				m_strFullPath = m_strFullPath.Right ( m_strFullPath.GetLength() - 1 );
		}
	}
}

void CVersionManagerDoc::OnFileOpen()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.	
	char szFilters[]=
      "Ran Version List File (*.lst)|*.lst|All Files (*.*)|*.*||";

	CFileDialog DlgFile(TRUE, "lst", "*.lst", OFN_ENABLESIZING, szFilters, NULL);
	    
    GetAppPath();
    DlgFile.m_pOFN->lpstrInitialDir = m_strFullPath;
	
	if (DlgFile.DoModal() == IDOK ) 
	{
		POSITION pos = DlgFile.GetStartPosition();
		CString strPathName = DlgFile.GetNextPathName(pos);

		CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();

        CDlgCompress DlgCompress(pFrame, strPathName);
		DlgCompress.DoModal();

        pFrame->m_pLeft->InitListData();
        pFrame->m_pRight->InitListData();
	}
}

void CVersionManagerDoc::OnFtp()
{
    // TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
    TRACE("OnFtp \n");
    CDlgFTP DlgFtp;
    DlgFtp.DoModal();   
}

void CVersionManagerDoc::OnCheck()
{
    // TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
    TRACE("OnCheck \n");
    CDlgFileCheck DlgCheck;
    DlgCheck.DoModal();
}

void CVersionManagerDoc::OnFileSave()
{
    // TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
    TRACE("OnFileSave \n");    

    // DB ���� ��ü ���ϸ���Ʈ�� �����´�    
    std::vector<FullFileList> vFullFileList; 
    COdbcManager::GetInstance()->GetAllFileList( vFullFileList );
    
    // ���ϸ���Ʈ�� �����Ѵ�.
    CFile tmpFile("filelist.bin", CFile::modeCreate | CFile::modeWrite);

    RanFileList sTemp;
    SecureZeroMemory( &sTemp, sizeof(RanFileList) );

    int nTotalCount = (int) vFullFileList.size(); // ��ü ���ϰ���
    
	// ù��° ���ڵ�� ���ϸ���Ʈ �����̴�.
#ifdef CHINAPARAM
    sTemp.nVersion = 2;
#else
	sTemp.nVersion = 1;
#endif
    tmpFile.Write(&sTemp, sizeof(RanFileList));

    // �ι�° ���ڵ�� ��ü ���ϰ����̴�.
    sTemp.nVersion = nTotalCount;
    tmpFile.Write(&sTemp, sizeof(RanFileList));

    // �ι�° ���ڵ� ���� ���������� ���ϰ�ο� ���ϸ��� �����Ѵ�.    
    for (int i=0; i<(int) vFullFileList.size(); i++)
	{	
        SecureZeroMemory( &sTemp, sizeof(RanFileList) );
        sTemp.nVersion = (&vFullFileList[i])->nVersion;
		::StringCchCopy(
			sTemp.szFileName,
			FILENAME_SIZE,
			(&vFullFileList[i])->strFileName.GetString() );

        ::StringCchCopy(
			sTemp.szSubPath,
			SUBPATH_SIZE,
			(&vFullFileList[i])->strFolder.GetString() );
#ifdef CHINAPARAM
		::StringCchCopy(
			sTemp.szMD5,
			MD5_SIZE,
			(&vFullFileList[i])->strMD5.GetString() );
#endif
        tmpFile.Write(&sTemp, sizeof(RanFileList));
    }

    tmpFile.Close();

    // ���ϸ���Ʈ�� �����ؼ� ����Ѵ�.  
     
    CString strFullPath;
    CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

    if ( !strCommandLine.IsEmpty() )
    {
        DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
        if ( dwFind != -1 )
        {
            strFullPath = strCommandLine.Left ( dwFind );

            if ( !strFullPath.IsEmpty() )
            if ( strFullPath.GetAt(0) == '"' )
            strFullPath = strFullPath.Right ( strFullPath.GetLength() - 1 );
        }
    }  

    CString ListPath;
    ListPath.Format("%s\\filelist.bin", strFullPath);

    CString TargetPath;
    TargetPath.Format("%s\\", CCfg::GetInstance()->m_szTargetPath);

    CMainFrame* pFrame = (CMainFrame *) AfxGetMainWnd();
    if (CAB_UTIL_MIN::MinMakeCab(ListPath.GetString(), TargetPath.GetString()) == TRUE)
    {
        pFrame->MessageBox("���ϸ���Ʈ ������ �Ϸ��߽��ϴ�");           
    }
    else
    {
        pFrame->MessageBox("���ϸ���Ʈ ���࿡ �����Ͽ����ϴ�");               
    }    
}