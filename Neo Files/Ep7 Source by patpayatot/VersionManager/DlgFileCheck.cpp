// DlgFileCheck.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "VersionManager.h"
#include <vector>
#include "DlgFileCheck.h"
#include "CConsoleMessage.h"
#include "CCfg.h"
#include "DatabaseTable.h"
#include "c_COdbcManager.h"
#include ".\dlgfilecheck.h"

// CDlgFileCheck ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgFileCheck, CDialog)
CDlgFileCheck::CDlgFileCheck(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFileCheck::IDD, pParent)
{
}

CDlgFileCheck::~CDlgFileCheck()
{
}

void CDlgFileCheck::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCheck);
    DDX_Control(pDX, IDC_STATIC_PG, m_StaticCheck);
}


BEGIN_MESSAGE_MAP(CDlgFileCheck, CDialog)
    ON_WM_TIMER()
    ON_EN_MAXTEXT(IDC_EDIT_CHECK, OnEnMaxtextEditCheck)
END_MESSAGE_MAP()


// CDlgFileCheck �޽��� ó�����Դϴ�.

BOOL CDlgFileCheck::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
    CConsoleMessage::GetInstance()->SetControl(GetDlgItem(IDC_EDIT_CHECK)->m_hWnd);
    m_ProgressCheck.SetRange(0, 100);	
	m_ProgressCheck.SetPos(0);
    SetTimer(100, 1000, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgFileCheck::StartFileCheck()
{
    // DB �� �Էµ� ���ϰ�, ������ �����ϴ��� �񱳰˻��Ѵ�.
    CConsoleMessage::GetInstance()->Write("------- ����üũ�� �����մϴ� ---------");

    // DB ���� ��ü ���ϸ���Ʈ�� �����´�
    CConsoleMessage::GetInstance()->Write("��ü ���ϸ���Ʈ�� �����ɴϴ�");
    std::vector<FullFileList> vFullFileList; 
    COdbcManager::GetInstance()->GetAllFileList(vFullFileList);

    // ���������� �����ϴ��� ���Ѵ�.
    CString strSrc(CCfg::GetInstance()->m_szTargetPath);

    int nTotalCount = (int) vFullFileList.size();
    int nCount = 0;
    int nErrorNum = 0;   

    CConsoleMessage::GetInstance()->Write("���� �񱳸� �����մϴ�");

    for (int i=0; i<(int) vFullFileList.size(); i++)
	{	        
        nCount++;
        CString strFullPath;
        strFullPath = strSrc + (&vFullFileList[i])->strFolder + (&vFullFileList[i])->strFileName; 

        CString strTemp;        
		strTemp.Format("%s %d/%d", (&vFullFileList[i])->strFileName, nCount, nTotalCount);
		m_StaticCheck.SetWindowText(strTemp);

        if (IsExist(strFullPath)) // �����Ѵٸ�
        {
            
        }
        else // �������� �ʴ´ٸ�
        {
            nErrorNum++;
            CConsoleMessage::GetInstance()->Write("%d %s ������ �������� �ʽ��ϴ�", nCount, strFullPath.GetString());
        }

        m_ProgressCheck.SetPos((int)((nCount*100)/nTotalCount));
    }

    CConsoleMessage::GetInstance()->Write("%d �� ���Ͽ����� �߰��߽��ϴ�", nErrorNum);
    CConsoleMessage::GetInstance()->Write("------- ����üũ�� �Ϸ�Ǿ����ϴ� ---------");
}

BOOL CDlgFileCheck::IsExist(CString strFullPath)
{
    DWORD dwReturn;
    dwReturn = GetFileAttributes(strFullPath);
    if (dwReturn == INVALID_FILE_ATTRIBUTES)
    {
        DWORD dwError = GetLastError();        
        return FALSE;
    }
    else if(dwReturn == FILE_ATTRIBUTE_DIRECTORY)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }

    
    //CFileFind find;
    //int ret = find.FindFile(m_strDesFile);
    //�����ϸ� nonzero, �������� ������ 0�� �����մϴ�.
}

void CDlgFileCheck::OnTimer(UINT nIDEvent)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    if (nIDEvent == 100)
    {
        KillTimer(100);
		StartFileCheck();
    }

    CDialog::OnTimer(nIDEvent);
}

void CDlgFileCheck::OnEnMaxtextEditCheck()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    GetDlgItem(IDC_EDIT_CHECK)->SetWindowText("");
}
