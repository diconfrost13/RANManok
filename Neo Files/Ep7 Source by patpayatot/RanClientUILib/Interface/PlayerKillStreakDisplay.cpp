#include "StdAfx.h"
#include "PlayerKillStreakDisplay.h"
#include "PlayerKillType.h"
#include "InnerInterface.h"
#include "GLGaeaClient.h"
#include "DxViewPort.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPlayerKillStreakDisplay::CPlayerKillStreakDisplay ()
{
}

CPlayerKillStreakDisplay::~CPlayerKillStreakDisplay ()
{
}

void CPlayerKillStreakDisplay::CreateSubControl ()
{
	CString strKeyword[MAXTYPE] = 
	{
		"PK_FIRST_BLOOD",
		"PK_DOUBLE_KILL",
		"PK_TRIPLE_KILL",
		"PK_ULTRA_KILL",
		"PK_RAMPAGE",
		"PK_KILLING_SPREE",
		"PK_DOMINATING",
		"PK_MEGA_KILL",
		"PK_UNSTOPPABLE",
		"PK_WICKED_SICK",
		"PK_MONSTER_KILL",
		"PK_GODLIKE",
		"PK_HOLY_SHIT",
		"PK_OWNAGE"
	};

	for ( int i = 0; i < MAXTYPE; ++i )
	{
		m_pPK_TYPE[i] = new CPlayerKillType;
		m_pPK_TYPE[i]->CreateSub ( this, strKeyword[i].GetString(), UI_FLAG_DEFAULT, FIRST_BLOOD + i );
		m_pPK_TYPE[i]->CreateSubControl ( strKeyword[i] );
		m_pPK_TYPE[i]->SetVisibleSingle ( FALSE );
		m_pPK_TYPE[i]->SetUseRender ( FALSE );
		m_pPK_TYPE[i]->STOP ();
		m_pPK_TYPE[i]->RESET ();		
		RegisterControl ( m_pPK_TYPE[i] );

        m_PK_TYPE_KEEP[i] = new CUIControl;
		m_PK_TYPE_KEEP[i]->CreateSub ( this, strKeyword[i].GetString() );
		m_PK_TYPE_KEEP[i]->SetVisibleSingle ( FALSE );
		RegisterControl ( m_PK_TYPE_KEEP[i] );
	}

	m_pPositionControl = new CUIControl;
	m_pPositionControl->CreateSub ( this, "QUESTION_ITEM_POSITION" );
	m_pPositionControl->SetVisibleSingle ( FALSE );
	RegisterControl ( m_pPositionControl );
}

bool	CPlayerKillStreakDisplay::START ( UIGUID cID )
{
	if ( cID < FIRST_BLOOD || OWNAGE < cID ) return false;
	
	int nIndex = cID - FIRST_BLOOD;
	m_pPK_TYPE[nIndex]->SetVisibleSingle ( TRUE );
	m_pPK_TYPE[nIndex]->START ();

	return true;
}

bool	CPlayerKillStreakDisplay::RESET ( UIGUID cID )
{
	if ( cID < FIRST_BLOOD || OWNAGE < cID ) return false;

	int nIndex = cID - FIRST_BLOOD;	
	m_pPK_TYPE[nIndex]->RESET ();

    return true;
}

bool	CPlayerKillStreakDisplay::STOP ( UIGUID cID )
{
	if ( cID < FIRST_BLOOD || OWNAGE < cID ) return false;

	int nIndex = cID - FIRST_BLOOD;	
	m_pPK_TYPE[nIndex]->STOP ();
	m_pPK_TYPE[nIndex]->SetVisibleSingle ( FALSE );

	return true;
}

void CPlayerKillStreakDisplay::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( !IsVisible () ) return ;

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	D3DXVECTOR3 vPos = GLGaeaClient::GetInstance().GetCharacter()->GetPosBodyHeight();

	static D3DXVECTOR3 vScreenBack;
	D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project ( &vPos, NULL );

	// 마우스 움직임에 흔들림을 보정한다.
	if( abs( vScreenBack.x - vScreen.x ) < 1.0f )
	{
		vScreen.x = vScreenBack.x;
	}
	
	bool bPLAYING( false );
	bool bKEEPING( false );

	for ( int i = 0; i < MAXTYPE; ++i )
	{
		const UIRECT& rcOriginPos = m_pPK_TYPE[i]->GetGlobalPos ();

		D3DXVECTOR2 vPos;
		vPos.x = floor(vScreen.x - ( rcOriginPos.sizeX * 0.5f ));
		vPos.y = m_pPositionControl->GetGlobalPos().top;

		if ( m_pPK_TYPE[i]->ISPLAYING () )
		{
			m_pPK_TYPE[i]->SetGlobalPos ( vPos );

			bPLAYING = true;
		}
		else
		{
			STOP ( FIRST_BLOOD + i );
		}

		if ( m_PK_TYPE_KEEP[i]->IsVisible () )
		{
			m_PK_TYPE_KEEP[i]->SetGlobalPos ( vPos );

			bKEEPING = true;
		}
	}

	vScreenBack = vScreen;

	if ( !bPLAYING && !bKEEPING ) CInnerInterface::GetInstance().HideGroup ( GetWndID () );
}

void CPlayerKillStreakDisplay::ALLSTOP ()
{
	for ( int i = 0; i < MAXTYPE; ++i )
	{
		m_pPK_TYPE[i]->STOP ();
		m_pPK_TYPE[i]->RESET ();
		m_pPK_TYPE[i]->SetVisibleSingle ( FALSE );

		m_PK_TYPE_KEEP[i]->SetVisibleSingle ( FALSE );
	}

	CInnerInterface::GetInstance().HideGroup ( GetWndID () );
}

bool CPlayerKillStreakDisplay::KEEP_START ( UIGUID cID )
{
	if ( cID < FIRST_BLOOD || OWNAGE < cID ) return false;

	int nIndex = cID - FIRST_BLOOD;	

	m_PK_TYPE_KEEP[nIndex]->SetVisibleSingle ( TRUE );
	m_PK_TYPE_KEEP[nIndex]->SetDiffuseAlpha ( 50 );

	return true;
}

void CPlayerKillStreakDisplay::KEEP_STOP ()
{
	for ( int i = 0; i < MAXTYPE; ++i )
	{
		m_PK_TYPE_KEEP[i]->SetVisibleSingle ( FALSE );
	}
}