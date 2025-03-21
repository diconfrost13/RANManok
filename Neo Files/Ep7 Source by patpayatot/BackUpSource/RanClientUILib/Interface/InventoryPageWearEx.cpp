#include "StdAfx.h"
#include "./InventoryPageWearEx.h"
#include "ItemImage.h"
#include "GLGaeaClient.h"
#include "GLItemMan.h"
#include "InnerInterface.h"
#include "ItemMove.h"
#include "BasicTextButton.h"
#include "GameTextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CInventoryPageWearEx::nOUTOFRANGE = -1;

CInventoryPageWearEx::CInventoryPageWearEx () 
	: m_pMouseOver(NULL)
	, m_pAButton(NULL)
	, m_pBButton(NULL)
	, m_pARHandSlot(NULL)
	, m_pALHandSlot(NULL)
	, m_pBRHandSlot(NULL)
	, m_pBLHandSlot(NULL)
{
}

CInventoryPageWearEx::~CInventoryPageWearEx ()
{
}

void CInventoryPageWearEx::CreateSubControl ( int nClassType )
{
	CString	strInvenWearBack[GLCI_NUM_EX-GLCI_NUM] = 
	{
		"INVENTORY_PAGE_WEAR_EXTREME_M",
		"INVENTORY_PAGE_WEAR_EXTREME_W",
	};

	CreateControl ( strInvenWearBack[nClassType-GLCI_NUM].GetString () );

	{	//	아이템 이미지 ( 0 - 11 )
		CString strInvenWearItem[ITEM_IMAGE_SIZE] = 
		{
			"INVENTORY_WEAR_EX_ITEM_IMAGE0",
			"INVENTORY_WEAR_EX_ITEM_IMAGE1",
			"INVENTORY_WEAR_EX_ITEM_IMAGE2",
			"INVENTORY_WEAR_EX_ITEM_IMAGE3",
			"INVENTORY_WEAR_EX_ITEM_IMAGE4",
			"INVENTORY_WEAR_EX_ITEM_IMAGE5",
			"INVENTORY_WEAR_EX_ITEM_IMAGE6",
			"INVENTORY_WEAR_EX_ITEM_IMAGE7",
			"INVENTORY_WEAR_EX_ITEM_IMAGE8",
			"INVENTORY_WEAR_EX_ITEM_IMAGE9",
			"INVENTORY_WEAR_EX_ITEM_IMAGE10",
			"INVENTORY_WEAR_EX_ITEM_IMAGE11",
			"INVENTORY_WEAR_EX_ITEM_IMAGE12",
			"INVENTORY_WEAR_EX_ITEM_IMAGE13",
		};

		CString strInvenWearOver[ITEM_IMAGE_SIZE] = 
		{
			"INVENTORY_WEAR_EX_OVER_IMAGE0",
			"INVENTORY_WEAR_EX_OVER_IMAGE1",
			"INVENTORY_WEAR_EX_OVER_IMAGE2",
			"INVENTORY_WEAR_EX_OVER_IMAGE3",
			"INVENTORY_WEAR_EX_OVER_IMAGE4",
			"INVENTORY_WEAR_EX_OVER_IMAGE5",
			"INVENTORY_WEAR_EX_OVER_IMAGE6",
			"INVENTORY_WEAR_EX_OVER_IMAGE7",
			"INVENTORY_WEAR_EX_OVER_IMAGE8",
			"INVENTORY_WEAR_EX_OVER_IMAGE9",
			"INVENTORY_WEAR_EX_OVER_IMAGE10",
			"INVENTORY_WEAR_EX_OVER_IMAGE11",
			"INVENTORY_WEAR_EX_OVER_IMAGE12",
			"INVENTORY_WEAR_EX_OVER_IMAGE13",
		};

		for ( int i = 0; i < ITEM_IMAGE_SIZE; i++ )
		{
			m_pItemImage[i] = CreateItemImage ( strInvenWearItem[i].GetString (), ITEM_IMAGE0 + i );
			m_pSlotDisplay[i] = CreateControl ( strInvenWearOver[i].GetString () );
			m_pSlotDisplay[i]->SetVisibleSingle ( FALSE );
		}
	}

	m_pMouseOver = CreateControl ( "ITEM_MOUSE_OVER" );

	m_pAButton = new CBasicTextButton;
	m_pAButton->CreateSub ( this, "BASIC_TEXT_BUTTON161", UI_FLAG_XSIZE, TEXT_A_BUTTON );
	m_pAButton->CreateBaseButton(	"INVENTORY_TAB_BUTTON_A", 
									CBasicTextButton::SIZE16s, 
									CBasicButton::RADIO_FLIP, 
									(char*)ID2GAMEWORD("INVEN_TAB_BUTTON",0) );
	m_pAButton->SetFlip ( TRUE );
	RegisterControl ( m_pAButton );

	m_pBButton = new CBasicTextButton;
	m_pBButton->CreateSub ( this, "BASIC_TEXT_BUTTON161", UI_FLAG_XSIZE, TEXT_B_BUTTON );
	m_pBButton->CreateBaseButton(	"INVENTORY_TAB_BUTTON_B", 
									CBasicTextButton::SIZE16s, 
									CBasicButton::RADIO_FLIP, 
									(char*)ID2GAMEWORD("INVEN_TAB_BUTTON",1) );
	m_pBButton->SetFlip ( TRUE );
	RegisterControl ( m_pBButton );

	m_pARHandSlot = CreateControl("INVENTORY_USE_SLOT_RHAND_A");
	m_pALHandSlot = CreateControl("INVENTORY_USE_SLOT_LHAND_A");
	m_pBRHandSlot = CreateControl("INVENTORY_USE_SLOT_RHAND_B");
	m_pBLHandSlot = CreateControl("INVENTORY_USE_SLOT_LHAND_B");
}

CUIControl*	CInventoryPageWearEx::CreateControl ( const char* szControl )
{
	CUIControl* pControl = new CUIControl;
	pControl->CreateSub ( this, szControl );
	RegisterControl ( pControl );

	return pControl;
}

CItemImage*	CInventoryPageWearEx::CreateItemImage ( const char* szControl, UIGUID ControlID )
{
	CItemImage* pItemImage = new CItemImage;
	pItemImage->CreateSub ( this, szControl, UI_FLAG_DEFAULT, ControlID );
	pItemImage->CreateSubControl ();
	RegisterControl ( pItemImage );

	return pItemImage;
}

EMSLOT CInventoryPageWearEx::IMAGE2EMSLOT ( int nIndex )
{
	switch ( nIndex )
	{	
	case ITEM_IMAGE0:	return SLOT_HEADGEAR;	//	머리
	case ITEM_IMAGE1:	return SLOT_NECK;		//	목걸이
	case ITEM_IMAGE2:	return SLOT_UPPER;		//	상의
	case ITEM_IMAGE3:	return SLOT_LHAND;		//	왼손도구
	case ITEM_IMAGE4:	return SLOT_WRIST;		//	손목
	case ITEM_IMAGE5:	return SLOT_HAND;		//	장갑
	case ITEM_IMAGE6:	return SLOT_LOWER;		//	하의
	case ITEM_IMAGE7:	return SLOT_LFINGER;	//	왼손 손가락
	case ITEM_IMAGE8:	return SLOT_RHAND;		//	오른손도구
	case ITEM_IMAGE9:	return SLOT_FOOT;		//	신발
	case ITEM_IMAGE10:	return SLOT_RFINGER;	//	오른손 손가락	
	case ITEM_IMAGE11:	return SLOT_RHAND_S;	//	오른손도구, 극강부
	case ITEM_IMAGE12:	return SLOT_LHAND_S;	//	왼손도구, 극강부
	case ITEM_IMAGE13:	return SLOT_VEHICLE;	//  탈것
	}

	GASSERT ( 0 && "범위를 벗어납니다." );
	return SLOT_TSIZE;
}

void CInventoryPageWearEx::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	SetItemIndex ( nOUTOFRANGE );
	if ( m_pMouseOver ) m_pMouseOver->SetVisibleSingle ( FALSE );

	//	스냅, 스킬 이미지 붙이기
	CItemMove* pItemMove = CInnerInterface::GetInstance().GetItemMove ();
	SNATIVEID sHOLD_ITEM_ID(false);
	if ( pItemMove ) sHOLD_ITEM_ID = pItemMove->GetItem();

	for ( int i = 0; i < SLOT_NSIZE_S_2; i++ )
	{
		const SITEMCUSTOM& sItemCustomOld = GetItem ( i );

		EMSLOT emSlot = IMAGE2EMSLOT ( i + ITEM_IMAGE0 );
		SITEMCUSTOM sItemCustom = GLGaeaClient::GetInstance().GetCharacter()->GET_SLOT_ITEM ( emSlot );

		//	NOTE
		//		이전 프레임과 비교하여,
		//		데이타가 달라진 경우에만,
		//		로드/언로드 작업을 진행한다.
		if ( sItemCustom != sItemCustomOld )
		{		
			if ( sItemCustom.sNativeID != NATIVEID_NULL () )
			{
				LoadItem ( i, sItemCustom );
			}
			else
			{
				UnLoadItem ( i );
			}
		}

		m_pSlotDisplay[i]->SetVisibleSingle ( FALSE );

		//	NOTE
		//		장착될 위치 표시
		if ( sHOLD_ITEM_ID != NATIVEID_NULL () /*&& !bFOUND_TOWEAR*/ )
		{
			if ( GLGaeaClient::GetInstance().GetCharacter()->CHECKSLOT_ITEM ( sHOLD_ITEM_ID, emSlot ) )
			{
				const UIRECT& rcImagePos = m_pItemImage[i]->GetGlobalPos ();
				D3DXVECTOR2 vPos ( rcImagePos.left, rcImagePos.top );

				if ( GLGaeaClient::GetInstance().GetCharacter()->ACCEPT_ITEM ( sHOLD_ITEM_ID ) )
				{
					m_pSlotDisplay[i]->SetVisibleSingle ( TRUE );
				}

//				bFOUND_TOWEAR = true;
			}
		}
	}

	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CInventoryPageWearEx::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ITEM_IMAGE0:
	case ITEM_IMAGE1:
	case ITEM_IMAGE2:
	case ITEM_IMAGE3:
	case ITEM_IMAGE4:
	case ITEM_IMAGE5:
	case ITEM_IMAGE6:
	case ITEM_IMAGE7:
	case ITEM_IMAGE8:
	case ITEM_IMAGE9:
	case ITEM_IMAGE10:	
	case ITEM_IMAGE11:	
	case ITEM_IMAGE12:	
	case ITEM_IMAGE13:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				int nIndex = ControlID - ITEM_IMAGE0;
				SetItemIndex ( nIndex );

				if ( !CInnerInterface::GetInstance().IsFirstItemSlot () ) return ;

				//	스냅, 스킬 이미지 붙이기
				CItemMove* pItemMove = CInnerInterface::GetInstance().GetItemMove ();
				if ( !pItemMove )
				{
					GASSERT ( 0 && "CItemMove가 널입니다." );
					return ;
				}

				if ( pItemMove->GetItem () != NATIVEID_NULL () )
				{
					const UIRECT& rcSlotPos = m_pItemImage[nIndex]->GetGlobalPos ();
					pItemMove->SetGlobalPos ( rcSlotPos );						

					AddMessageEx ( UIMSG_MOUSEIN_WEARSLOTEX | UIMSG_TOTOPPARENT );
				}				

				//	마우스 표시 테두리
				if ( pItemMove->GetItem () == NATIVEID_NULL () && m_pItemImage[m_nIndex]->GetItem () != NATIVEID_NULL () )
				{
					const UIRECT& rcImagePos = m_pItemImage[nIndex]->GetGlobalPos ();
					m_pMouseOver->SetGlobalPos ( rcImagePos );
					m_pMouseOver->SetVisibleSingle ( TRUE );
				}


				if ( dwMsg & UIMSG_RB_UP )
				{
				//int nPosX, nPosY;
				//m_pPage->GetItemIndex ( &nPosX, &nPosY );
				//if ( nPosX < 0 || nPosY < 0 ) return;
//				WORD wPosX, wPosY;
				EMSLOT J;
				SITEM* pItemData = GLItemMan::GetInstance().GetItem ( m_ItemCustomArray[nIndex].sNativeID );
					if ( !pItemData  ) return;
		
				GLCharacter* pCharacter = GLGaeaClient::GetInstance().GetCharacter();
					if ( !pCharacter ) return;	

				EMITEM_TYPE emItemType = pItemData->sBasicOp.emItemType;

				//pInvenItem = pCharacter->m_cInventory.FindPosItem ( pInvenItem->wPosX, pInvenItem->wPosY );
					//if ( !pInvenItem ) return;
				if (pItemData)
						{
							switch ( pItemData->sSuitOp.emSuit )
							{
							case SUIT_HEADGEAR:		J = SLOT_HEADGEAR;break;
							case SUIT_UPPER:		J = SLOT_UPPER;break;
							case SUIT_LOWER:		J = SLOT_LOWER;break;
							case SUIT_HAND:			J = SLOT_HAND;break;
							case SUIT_FOOT:			J = SLOT_FOOT;break;
							case SUIT_NECK:			J = SLOT_NECK;break;
							case SUIT_WRIST:		J = SLOT_WRIST;break;
							case SUIT_VEHICLE:		J = SLOT_VEHICLE;break;
							case SUIT_HANDHELD:	
								if ( emItemType == ITEM_ARROW || emItemType == ITEM_CHARM  )
								{
									if(GLGaeaClient::GetInstance().GetCharacter()->IsUseArmSub())
									{
										J = SLOT_LHAND_S;
									}
									else{
										J = SLOT_LHAND;
									}
								}
								else
								{
									if(GLGaeaClient::GetInstance().GetCharacter()->IsUseArmSub())
									{
										J = SLOT_RHAND_S;
									}
									else{
										J = SLOT_RHAND;
									}
								}
								break;
							case SUIT_FINGER:	
								{
									
									if ( m_bRingSwap )
									{
										m_bRingSwap = FALSE;
										J = SLOT_RFINGER;
									}
									else
									{
										J = SLOT_LFINGER;
										m_bRingSwap = TRUE;
									}
								}
								break;
							};
							GLGaeaClient::GetInstance().GetCharacter()->ReqUnEquipItem ( J );
						}
				
				return ;
				}
			}
		}
		break;

	case TEXT_A_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				GLGaeaClient::GetInstance().GetCharacter()->ReqSlotChange();
			}
		}
		break;

	case TEXT_B_BUTTON:
		{
			if( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				GLGaeaClient::GetInstance().GetCharacter()->ReqSlotChange();
			}
		}
		break;
	}
}

EMSLOT CInventoryPageWearEx::GetItemSlot ()
{
	return IMAGE2EMSLOT ( GetItemIndex () + ITEM_IMAGE0 );
}

void CInventoryPageWearEx::LoadItem ( int nIndex, SITEMCUSTOM& ref_sItemCustom )
{
	m_ItemCustomArray[nIndex] = ref_sItemCustom;

	SITEM* pItemData = GLItemMan::GetInstance().GetItem ( ref_sItemCustom.sNativeID );
	m_pItemImage[nIndex]->SetItem ( pItemData->sBasicOp.sICONID, pItemData->GetInventoryFile(), pItemData->sBasicOp.sNativeID );
}

SITEMCUSTOM& CInventoryPageWearEx::GetItem ( int nIndex )
{
	return m_ItemCustomArray[nIndex];
}

void CInventoryPageWearEx::UnLoadItem ( int nIndex )
{
	m_ItemCustomArray[nIndex].sNativeID = NATIVEID_NULL ();
	m_pItemImage[nIndex]->ResetItem ();
}

void CInventoryPageWearEx::SetTabButton( BOOL bMain )
{
	if( bMain )
	{
		m_pAButton->SetFlip( TRUE );
		m_pBButton->SetFlip( FALSE );

		m_pARHandSlot->SetVisibleSingle( FALSE );
		m_pALHandSlot->SetVisibleSingle( FALSE );
		m_pBRHandSlot->SetVisibleSingle( TRUE );
		m_pBLHandSlot->SetVisibleSingle( TRUE );
	}
	else
	{
		m_pAButton->SetFlip( FALSE );
		m_pBButton->SetFlip( TRUE );

		m_pARHandSlot->SetVisibleSingle( TRUE );
		m_pALHandSlot->SetVisibleSingle( TRUE );
		m_pBRHandSlot->SetVisibleSingle( FALSE );
		m_pBLHandSlot->SetVisibleSingle( FALSE );
	}
}