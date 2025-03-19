// DlgFTP.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include <process.h>
#include "VersionManager.h"
#include "DlgFTP.h"
#include "CConsoleMessage.h"
#include "CCfg.h"
#include "CMinFtp.h"
#include <vector>
#include "DatabaseTable.h"
#include "c_COdbcManager.h"
#include ".\dlgftp.h"

// CDlgFTP ��ȭ �����Դϴ�.

static unsigned int WINAPI UploadThread(void* p)
{
	CDlgFTP* pThread = (CDlgFTP*) p;
	pThread->UploadThreadProc();
	return 0;
}

IMPLEMENT_DYNAMIC(CDlgFTP, CDialog)
CDlgFTP::CDlgFTP(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFTP::IDD, pParent)
{
    m_bStop = FALSE;
}

CDlgFTP::~CDlgFTP()
{
}

void CDlgFTP::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS_FTP, m_ProgressFTP);
    DDX_Control(pDX, IDC_STATIC_FTP, m_StaticFTP);
}


BEGIN_MESSAGE_MAP(CDlgFTP, CDialog)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
    ON_BN_CLICKED(IDC_START, OnBnClickedStart)
    ON_EN_MAXTEXT(IDC_EDIT_FTP, OnEnMaxtextEditFtp)
END_MESSAGE_MAP()


// CDlgFTP �޽��� ó�����Դϴ�.

BOOL CDlgFTP::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
    CConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_FTP)->m_hWnd);	
	m_ProgressFTP.SetRange(0, 100);	
	m_ProgressFTP.SetPos(0);

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgFTP::OnTimer(UINT nIDEvent)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    CDialog::OnTimer(nIDEvent);
}

int CDlgFTP::UploadThreadProc()
{
    // ftp �� �����Ѵ�.	
    CConsoleMessage::GetInstance()->Write("---------- FTP Upload �� �����մϴ� ---------");
    CMinFtp* pFtp = new CMinFtp();

	int nFtpCount = static_cast <int> (CCfg::GetInstance()->m_vFtpIP.size());

	for (int nFtpCrt=1; nFtpCrt<=nFtpCount; nFtpCrt++)
	{
		CString strIP = CCfg::GetInstance()->m_vFtpIP[nFtpCrt-1];
		int nRetCode = pFtp->Connect(strIP.GetString(),
									CCfg::GetInstance()->m_nFtpPort,
									CCfg::GetInstance()->m_szFtpUserName,
									CCfg::GetInstance()->m_szFtpPassword);
		if (nRetCode == NET_ERROR)
		{
			CConsoleMessage::GetInstance()->Write("-------------------------------------------------------------------");
			CConsoleMessage::GetInstance()->Write("ERROR:%d %s FTP ���ῡ �����Ͽ����ϴ�.", nFtpCrt, strIP.GetString());
			CConsoleMessage::GetInstance()->Write("-------------------------------------------------------------------");
			return -1;
		}

		// FTP �� ���� ���� ROOT ���丮 ��ġ
		CString strFirstDir;
		
		if (pFtp->m_pConnect->GetCurrentDirectory(strFirstDir) == 0)
		{
			CConsoleMessage::GetInstance()->Write("ERROR:GetCurrentDirectory %d", GetLastError());
			return -1;
		}

		// ��ü ��������Ʈ�� �����´�.
		std::vector<FolderList> vFolder;
		COdbcManager::GetInstance()->GetFolderList(vFolder);

		// FTP �� ������ ������Ų��
		// ���� : �Ʒ��� ���� ������� ���� ������...
		// Windows �迭�� Unix �迭���� ���ÿ� �������� �ʴ´�.

		CConsoleMessage::GetInstance()->Write("--FTP �� ������ üũ�մϴ�--");
		for (int i=0; i<(int) vFolder.size(); i++)
		{	
			CString strTmpFolder= (&vFolder[i])->strName;
	        
			CString resToken;
			int curPos= 0;

			resToken= strTmpFolder.Tokenize("\\",curPos);
			while (resToken != "")
			{            
				pFtp->CreateDirectory(resToken);
				pFtp->SetCurrentDirectory(resToken);
				resToken= strTmpFolder.Tokenize("\\",curPos);
			}
			pFtp->SetCurrentDirectory(strFirstDir);        
		}   

		// ��ü ���ϸ���Ʈ�� �����´�.
		CConsoleMessage::GetInstance()->Write("��ü ���ϸ���Ʈ�� �����ɴϴ�");
		std::vector<FullFileList> vFullFileList;    
		COdbcManager::GetInstance()->GetNotUploadedFileList(vFullFileList);
	    
		// ��ȸ�ϸ鼭 ������ �ϳ��� ���ε� �Ѵ�.
		CConsoleMessage::GetInstance()->Write("������ ���ε� �մϴ�");
		CString strSrcPath;
		strSrcPath.Format("%s", CCfg::GetInstance()->m_szTargetPath);

		int nTotalCount = (int) vFullFileList.size();
		int nCount = 0;


		for (int i=0; i<(int) vFullFileList.size(); i++)
		{	
			nCount++;       

			CString strMakePath;
			CString strFileName = (&vFullFileList[i])->strFileName;
			strMakePath = strSrcPath + (&vFullFileList[i])->strFolder + strFileName;

			CString strTemp;        
			strTemp.Format("%s %d/%d", strFileName.GetString(), nCount, nTotalCount);
			m_StaticFTP.SetWindowText(strTemp);

			CString strChgfolder;
			strChgfolder = strFirstDir + (&vFullFileList[i])->strFolder;
			strChgfolder.Replace('\\', '/');
			pFtp->SetCurrentDirectory(strChgfolder);
			if (pFtp->PutFile(strMakePath, strFileName) == NET_ERROR)
			{
				CConsoleMessage::GetInstance()->Write("ERROR:%s ���Ͼ��ε� ����", strFileName.GetString());
				CConsoleMessage::GetInstance()->Write("INFO:%s ��õ�", strFileName.GetString());
				if (pFtp->PutFile(strMakePath, strFileName) == NET_ERROR)
				{
					CConsoleMessage::GetInstance()->Write("ERROR:%s ���Ͼ��ε� ��õ� ����", strFileName.GetString());					
				}
				else
				{
					CConsoleMessage::GetInstance()->Write("INFO:%s ���Ͼ��ε� ��õ� ����", strFileName.GetString());
				}
			}
			else
			{
				// ���ε� ����
				// DB �� ���ε� �ߴٰ� ����Ѵ�.
				if (nFtpCrt == nFtpCount)
				{
					COdbcManager::GetInstance()->UpdateFileStateTrue((&vFullFileList[i])->nIdx);
				}
			}
			m_ProgressFTP.SetPos((int)((nCount*100)/nTotalCount));

			if (m_bStop)
			{
				CConsoleMessage::GetInstance()->Write("���ε带 �ߴ��մϴ�");
				pFtp->DisConnect();
				delete pFtp;
				return -1;
			}
			Sleep( 5 );
		}

		// ���������� ���ϸ���Ʈ�� ���ε� �Ѵ�.
		CConsoleMessage::GetInstance()->Write("���ϸ���Ʈ�� ���ε� �մϴ�");    
		if (pFtp->SetCurrentDirectory(strFirstDir) == NET_ERROR)
		{
			CConsoleMessage::GetInstance()->Write("ERROR:���ϸ���Ʈ SetCurrentDirectory ����");
		}
		else
		{
			if (pFtp->PutFile(strSrcPath + '\\' + "filelist.bin.cab", "filelist.bin.cab") == NET_ERROR)
			{
                CConsoleMessage::GetInstance()->Write("ERROR:���ϸ���Ʈ ���ε� ����");
			}
			else
			{
				CConsoleMessage::GetInstance()->Write("INFO:���ϸ���Ʈ ���ε� ����");
			}
		}
	   
		// ftp ������ �����Ѵ�.
		CConsoleMessage::GetInstance()->Write("FTP ������ �����մϴ�");
		pFtp->DisConnect();
	}
    delete pFtp;
	CConsoleMessage::GetInstance()->Write("---------- FTP Upload �� �Ϸ��߽��ϴ� ---------");
    return 0;
}

void CDlgFTP::OnBnClickedStop()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    m_bStop = TRUE;
}

void CDlgFTP::OnBnClickedStart()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    m_bStop = FALSE;
    StartUploadThread();
}

int CDlgFTP::StartUploadThread()
{	
	DWORD dwThreadID = 101;	
	
	/*m_hThread = ::CreateThread(
						NULL, 
					    0, 
					    (LPTHREAD_START_ROUTINE) UploadThread, 
					    this, 
					    0, 
					    &dwThreadID );*/
	m_hThread = (HANDLE) ::_beginthreadex(
								NULL,
								0,
								UploadThread,
								this, 
								0, 
								(unsigned int*) &dwThreadID );
	if (m_hThread == NULL)
	{
		// ������ ������ �����Ͽ����ϴ�.
        CConsoleMessage::GetInstance()->Write("������ ������ �����Ͽ����ϴ�");
		return NET_ERROR;
	}
	else
	{
		return NET_OK;
	}
}

void CDlgFTP::OnEnMaxtextEditFtp()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    GetDlgItem(IDC_EDIT_FTP)->SetWindowText("");
}
