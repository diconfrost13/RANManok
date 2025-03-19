// GameSetDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "AutoPatchMan.h"
#include "GameSetDlg.h"

#include "DebugSet.h"
#include "../EngineSoundLib/DxSound/DxSoundMan.h"

#include "RanParam.h"
#include "EtcFunction.h"

#include "GlobalVariable.h"
#include "LauncherText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGameSetDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CGameSetDlg, CDialog)
CGameSetDlg::CGameSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGameSetDlg::IDD, pParent)
	, m_nDownloadArea(0)
{
	InitString();
}

CGameSetDlg::~CGameSetDlg()
{
}

void CGameSetDlg::InitString()
{
	CString strMsg;
	
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 31 );
	szCharShadow[0] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 32 ) ;
	szCharShadow[1] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 33 ) ;
	szCharShadow[2] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 34 ) ;
	szCharShadow[3] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 35 ) ;
	szCharShadow[4] = strMsg.GetString();

	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 36 ) ;
	szCharDetail[0] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 37 ) ;
	szCharDetail[1] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 38 ) ;
	szCharDetail[2] = strMsg.GetString();

	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 39 ) ;
	sz3DAlgorithm[0] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 40 ) ;
	sz3DAlgorithm[1] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 41 ) ;
	sz3DAlgorithm[2] = strMsg.GetString();

	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 42 ) ;
	szFogRange[0] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 43 ) ;
	szFogRange[1] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 44 ) ;
	szFogRange[2] = strMsg.GetString();

	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 73 ) ;
	szDefaultOption[0] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 36 ) ;
	szDefaultOption[1] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 37 ) ;
	szDefaultOption[2] = strMsg.GetString();
	strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 38 ) ;
	szDefaultOption[3] = strMsg.GetString();


}

void CGameSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_WINDOWMODE, m_buttonWindowMode);

#ifdef CH_PARAM
	DDX_Radio(pDX, IDC_RADIO1, m_nDownloadArea);
#endif
}


BEGIN_MESSAGE_MAP(CGameSetDlg, CDialog)	
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_COMPATIBLE, OnBnClickedButtonCompatible)
	ON_CBN_SELCHANGE(IDC_COMBO_RESOLUTION, OnCbnSelchangeComboResolution)
	ON_CBN_SELCHANGE(IDC_COMBO_HZ, OnCbnSelchangeComboHz)
	ON_CBN_SELCHANGE(IDC_COMBO_DEFAULTOPTION, OnCbnSelchangeComboDefaultoption)
	ON_BN_CLICKED(IDC_CHECK_SHADOWLAND, OnBnClickedCheckButton)
	ON_BN_CLICKED(IDC_CHECK_REALREFLECT, OnBnClickedCheckButton)
	ON_BN_CLICKED(IDC_CHECK_REFRACT, OnBnClickedCheckButton)
	ON_BN_CLICKED(IDC_CHECK_GLOW, OnBnClickedCheckButton)
	ON_BN_CLICKED(IDC_CHECK_POSTPROCESSING, OnBnClickedCheckButton)
	ON_BN_CLICKED(IDC_CHECK_BUFFEFFECT, OnBnClickedCheckButton)
	ON_CBN_SELCHANGE(IDC_COMBO_FOGRANGE, OnCbnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_SHADOWCHAR, OnCbnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_SKINDETAIL, OnCbnSelchangeCombo)
END_MESSAGE_MAP()


// CGameSetDlg �޽��� ó�����Դϴ�.
BOOL CGameSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.	
	m_D3DApp.BuildDeviceListSet ();

	D3DAdapterInfo *pAdapter = m_D3DApp.GetCurAdapterInfo();
	
	BOOL bWHQL = ( pAdapter->d3dAdapterIdentifier.WHQLLevel != 0 );

	CString strWHQLCommant;
	if ( bWHQL )	strWHQLCommant = ID2LAUNCHERTEXT("IDS_WHQL_CVD", 0 );
	else			strWHQLCommant = ID2LAUNCHERTEXT("IDS_WHQL_UCVD", 0 );

//	CWnd *pWnd = GetDlgItem(IDC_STATIC_WHQL);
//	pWnd->SetWindowText ( strWHQLCommant );

	InitCtrls ();
	UpdateCtrls ();
	InitDlgText ();

#ifdef CH_PARAM
	// Note : �߱� ������ ��� â ��� ���� üũ �ڽ��� �����.
	::ShowWindow( m_buttonWindowMode.GetSafeHwnd(), SW_HIDE );
#endif

#ifndef	CH_PARAM	// �߱����� �߰�
	CStatic* pStatic = (CStatic*) GetDlgItem(IDC_DOWNAREA_STATIC);	
	pStatic->ShowWindow(SW_HIDE);
	CButton* pButton = (CButton*) GetDlgItem(IDC_RADIO1);	
	pButton->ShowWindow(SW_HIDE);
	pButton = (CButton*) GetDlgItem(IDC_RADIO2);	
	pButton->ShowWindow(SW_HIDE);
	pButton = (CButton*) GetDlgItem(IDC_RADIO3);	
	pButton->ShowWindow(SW_HIDE);
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CGameSetDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	InverseUpdateCtrls ();

	RANPARAM::SAVE ();	
	
	OnOK();
}

void CGameSetDlg::OnBnClickedButtonCompatible()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	BOOL bResult = DxSoundMan::GetInstance().OneTimeSceneInit ( m_hWnd, "Nothing" );
	if ( !bResult )
	{
		CString strMsg;
		strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 30 ) ;
		MessageBox ( strMsg.GetString(), "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return ;
	}

	DSCAPS DSCaps;
	LPDIRECTSOUND8 pDirectSound = DxSoundMan::GetInstance().GetSoundManager()->GetDirectSound ();
	if ( !pDirectSound )
	{		
		DxSoundMan::GetInstance().FinalCleanup ();
		CString strMsg;
		strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 30 ) ;
		MessageBox ( strMsg.GetString(), "ERROR", MB_OK | MB_ICONEXCLAMATION );
		return;
	}

	SecureZeroMemory ( &DSCaps, sizeof ( DSCAPS ) );
	DSCaps.dwSize = sizeof ( DSCAPS );
	pDirectSound->GetCaps ( &DSCaps );

	BOOL bCompatible = TRUE;
	if ( (!( DSCaps.dwFlags & DSCAPS_PRIMARY8BIT )) ||
		 (!( DSCaps.dwFlags & DSCAPS_SECONDARY8BIT)) )
	{
		bCompatible = FALSE;
	}

	if (( DSCaps.dwFreeHwMixingAllBuffers <= 0 )		||
		( DSCaps.dwFreeHwMixingStaticBuffers <= 0 )		||
		( DSCaps.dwFreeHwMixingStreamingBuffers <= 0 )	||
		( DSCaps.dwMaxHw3DAllBuffers <= 0 )				||
		( DSCaps.dwMaxHw3DStaticBuffers <= 0 )			||
		( DSCaps.dwMaxHw3DStreamingBuffers <= 0 )		||
		( DSCaps.dwFreeHw3DAllBuffers <= 0 )			||
		( DSCaps.dwFreeHw3DStaticBuffers <= 0 )			||
		( DSCaps.dwFreeHw3DStreamingBuffers <= 0 ))
	{
		bCompatible = FALSE;
	}

	if ( bCompatible )
	{
		((CComboBox*)GetDlgItem ( IDC_COMBO_3DALGORITHM ))->SetCurSel ( DxSoundMan::HRTF_LIGHT );
	}
	else
	{
		((CComboBox*)GetDlgItem ( IDC_COMBO_3DALGORITHM ))->SetCurSel ( DxSoundMan::DEFAULT );
	}

	DxSoundMan::GetInstance().FinalCleanup ();
}

void CGameSetDlg::OnCbnSelchangeComboResolution()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_HZ);
	pComboBox->ResetContent();

	D3DDeviceInfo* pDeviceInfo = m_D3DApp.GetCurDeviceInfo ();
	DWORD dwNumModes = pDeviceInfo->dwNumModes;	
	D3DModeInfo* pModeInfo = pDeviceInfo->modes;

	if ( !dwNumModes ) return ;

	BOOL	bCompatible = FALSE;	
	BOOL	bFirstHz = TRUE;
	UINT	uFirstHz;

	DWORD	CurSel = ((CComboBox*)GetDlgItem(IDC_COMBO_RESOLUTION))->GetCurSel ();
	if ( CurSel==LB_ERR )	return;

	CMList<UINT> &listRefreshRate = pModeInfo[CurSel].RefreshRate;

	CMList<UINT> listTemp;

	listRefreshRate.SetHead();
	while( !listRefreshRate.IsEnd() )
	{
		UINT uHz1, uHz2;
		listRefreshRate.GetCurrent( uHz1 );

		listTemp.GetTail( uHz2 );
		if( uHz1 != uHz2 )
		{
			listTemp.AddTail( uHz1 );
		}

		listRefreshRate.GoNext();
	}

	CString str;
	listTemp.SetHead ();
	while ( !listTemp.IsEnd () )
	{
		UINT uHz;
		listTemp.GetCurrent ( uHz );

		if( RANPARAM::uScrRefreshHz == uHz ) //	�츣���� ���� �������� �����Ҽ� �ִ��� Ȯ��
		{
			bCompatible = TRUE;
		}

		if( bFirstHz ) // �ּ� �츣���� ���
		{
			uFirstHz = uHz;
			bFirstHz = FALSE;
		}
	
		str.Format ( "%3d", uHz );
		int nIndex = pComboBox->AddString ( str );
		pComboBox->SetItemData ( nIndex, (DWORD_PTR) uHz );

		listTemp.GoNext ();
	}

	UINT uScrRefreshHz;
	if ( bCompatible )
	{
		uScrRefreshHz = RANPARAM::uScrRefreshHz;		
	}
	else
	{
		uScrRefreshHz = uFirstHz;
	}

	str.Format ( "%3d", uScrRefreshHz );
	SetWin_Combo_SelMfc ( this, IDC_COMBO_HZ, str );

	//	�ǵ��� ����
	OnCbnSelchangeComboHz ();


	//	���� ���� ����
	DWORD	bpp = 16;
	D3DFORMAT &d3dformat = pModeInfo[CurSel].Format;
	if ((d3dformat == D3DFMT_R8G8B8)	||
		(d3dformat == D3DFMT_A8R8G8B8)	||
		(d3dformat == D3DFMT_X8R8G8B8) )
	{
		bpp = 32;
	}

	m_dwScrWidth = pModeInfo[CurSel].Width;
	m_dwScrHeight = pModeInfo[CurSel].Height;
	m_emScrFormat = (bpp==16)?EMSCREEN_F16:EMSCREEN_F32;	
}

void CGameSetDlg::OnCbnSelchangeComboHz()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_HZ);
	int nIndex = pComboBox->GetCurSel ();
	m_uScrRefreshHz = (UINT)(pComboBox->GetItemData ( nIndex ));
}
void CGameSetDlg::OnCbnSelchangeComboDefaultoption()
{
	CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_DEFAULTOPTION);
	int nIndex = pComboBox->GetCurSel ();

	if( nIndex == 1)
	{
		SetLowLevel();
	}
	else if( nIndex == 2)
	{
		SetMediumLevel();
	}
	else if( nIndex == 3)
	{
		SetHighLevel();
	}

	pComboBox->SetCurSel(nIndex);

}

void CGameSetDlg::OnBnClickedCheckButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetWin_Combo_Sel ( this, IDC_COMBO_DEFAULTOPTION, szDefaultOption[0] );
}

void CGameSetDlg::OnCbnSelchangeCombo()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetWin_Combo_Sel ( this, IDC_COMBO_DEFAULTOPTION, szDefaultOption[0] );
}

void CGameSetDlg::SetHighLevel()
{
	SetWin_Combo_Sel ( this, IDC_COMBO_SHADOWCHAR, szCharShadow[4] );
	SetWin_Combo_Sel ( this, IDC_COMBO_SKINDETAIL, szCharDetail[2] );
	SetWin_Combo_Sel ( this, IDC_COMBO_FOGRANGE, szFogRange[2] );	
	
	SetWin_Check ( this, IDC_CHECK_SHADOWLAND, true );
	SetWin_Check ( this, IDC_CHECK_REALREFLECT, true );	
	SetWin_Check ( this, IDC_CHECK_REFRACT, true );	
	SetWin_Check ( this, IDC_CHECK_GLOW, true );	
	SetWin_Check ( this, IDC_CHECK_BUFFEFFECT, true );	
	SetWin_Check ( this, IDC_CHECK_POSTPROCESSING, true );		
}

void CGameSetDlg::SetMediumLevel()
{
	SetWin_Combo_Sel ( this, IDC_COMBO_SHADOWCHAR, szCharShadow[2] );
	SetWin_Combo_Sel ( this, IDC_COMBO_SKINDETAIL, szCharDetail[1] );
	SetWin_Combo_Sel ( this, IDC_COMBO_FOGRANGE, szFogRange[1] );	
	
	SetWin_Check ( this, IDC_CHECK_SHADOWLAND, true );
	SetWin_Check ( this, IDC_CHECK_REALREFLECT,false );	
	SetWin_Check ( this, IDC_CHECK_REFRACT, false );	
	SetWin_Check ( this, IDC_CHECK_GLOW, false );	
	SetWin_Check ( this, IDC_CHECK_BUFFEFFECT, true );	
	SetWin_Check ( this, IDC_CHECK_POSTPROCESSING, false );		
}

void CGameSetDlg::SetLowLevel()
{
	SetWin_Combo_Sel ( this, IDC_COMBO_SHADOWCHAR, szCharShadow[1] );
	SetWin_Combo_Sel ( this, IDC_COMBO_SKINDETAIL, szCharDetail[0] );
	SetWin_Combo_Sel ( this, IDC_COMBO_FOGRANGE, szFogRange[0] );	
	
	SetWin_Check ( this, IDC_CHECK_SHADOWLAND, false );
	SetWin_Check ( this, IDC_CHECK_REALREFLECT, false );	
	SetWin_Check ( this, IDC_CHECK_REFRACT, false );	
	SetWin_Check ( this, IDC_CHECK_GLOW, false );	
	SetWin_Check ( this, IDC_CHECK_BUFFEFFECT, false );	
	SetWin_Check ( this, IDC_CHECK_POSTPROCESSING, false );		
}