#include "stdafx.h"
#include "./GLCharData.h"
#include "./GLItemMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD SCHARSKILL::VERSION	= 0x0100;
DWORD SCHARSKILL::SIZE		= sizeof(SCHARSKILL);

bool SCHARDATA::IsKEEP_STORAGE ( DWORD dwCHANNEL )
{
	//	기본 창고일 경우 참.
	if ( EMSTORAGE_CHANNEL_DEF <= dwCHANNEL && dwCHANNEL < EMSTORAGE_CHANNEL_DEF+EMSTORAGE_CHANNEL_DEF_SIZE )				return true;

	//	추가 창고일 경우.
	if ( EMSTORAGE_CHANNEL_SPAN <= dwCHANNEL && dwCHANNEL < EMSTORAGE_CHANNEL_SPAN+EMSTORAGE_CHANNEL_SPAN_SIZE )
	{
		return m_bSTORAGE[dwCHANNEL-EMSTORAGE_CHANNEL_SPAN];
	}

	//	추가 창고일 경우.
	if ( EMSTORAGE_CHANNEL_PREMIUM <= dwCHANNEL && dwCHANNEL < EMSTORAGE_CHANNEL_PREMIUM+EMSTORAGE_CHANNEL_PREMIUM_SIZE )
	{
		return m_bPREMIUM;
	}

	return false;
}

CTime SCHARDATA::GetStorageTime (  DWORD dwCHANNEL )
{
	//	기본 창고일 경우 참.
	if ( EMSTORAGE_CHANNEL_DEF <= dwCHANNEL && dwCHANNEL < EMSTORAGE_CHANNEL_DEF+EMSTORAGE_CHANNEL_DEF_SIZE )				return CTime(0);

	//	추가 창고일 경우.
	if ( EMSTORAGE_CHANNEL_SPAN <= dwCHANNEL && dwCHANNEL < EMSTORAGE_CHANNEL_SPAN+EMSTORAGE_CHANNEL_SPAN_SIZE )
	{
		return CTime(m_tSTORAGE[dwCHANNEL-EMSTORAGE_CHANNEL_SPAN]);
	}

	//	추가 창고일 경우.
	if ( EMSTORAGE_CHANNEL_PREMIUM <= dwCHANNEL && dwCHANNEL < EMSTORAGE_CHANNEL_PREMIUM+EMSTORAGE_CHANNEL_PREMIUM_SIZE )
	{
		return CTime(0);
	}

	return CTime(0);
}

WORD SCHARDATA::GetOnINVENLINE ()
{
	if ( m_bPREMIUM )	return (m_wINVENLINE+EM_INVEN_PREMIUM_SIZE);
	return m_wINVENLINE;
}

void SCHARDATA::CalcPREMIUM ()
{
	const CTime cTIME_CUR = CTime::GetCurrentTime();

	bool bPREMIUM(true);
	CTime tPREMIUM(m_tPREMIUM);
	if ( m_tPREMIUM==0 || tPREMIUM<cTIME_CUR )	bPREMIUM = false;
	m_bPREMIUM = bPREMIUM;
}

SCHARDATA2::SCHARDATA2() :
	m_lnStorageMoney(0),
	m_bServerStorage(FALSE),

	m_wSKILLQUICK_ACT(0),

	m_dwStartGate(0),
	m_vStartPos(0,0,0),

	m_sSaveMapID(false),
	m_vSavePos(0,0,0),

	m_sLastCallMapID(false),
	m_vLastCallPos(0,0,0),

	m_dwThaiCCafeClass(0),
	m_nMyCCafeClass(0),

	m_bVietnamLevelUp(false),

	m_bTracingUser(FALSE)
{

	for ( int i=0; i<EMSKILLQUICK_SIZE; ++i )	m_sSKILLQUICK[i] = NATIVEID_NULL();

	for ( int i=0; i<EMSTORAGE_CHANNEL; ++i )	m_bStorage[i] = FALSE;
	for ( int i=0; i<EMSTORAGE_CHANNEL; ++i )	m_cStorage[i].SetState ( STORAGE_INVEN_X, STORAGE_INVEN_Y );

	m_cInvenCharged.SetState ( CHARGED_INVEN_X, CHARGED_INVEN_Y );

	//itemmall
	for( int i=0;i<13; i++)
	{
		m_cInvenItemShop[i].SetState ( ITEMSHOP_INVEN_X-2, 200 );
		m_cInvenItemShopVote[i].SetState ( ITEMSHOP_INVEN_X-2, 200 );
		m_cInvenItemShopPremium[i].SetState ( ITEMSHOP_INVEN_X-2, 200 );
	}
	InitRebuildData();	// ITEMREBUILD_MARK
	CloseRebuild();


	/*itemfood system, Juver, 2017/05/24 */
	for ( int i=0; i<FITEMFACT_SIZE; ++i )
		m_sFITEMFACT[i] = SFITEMFACT();

	for ( int i=0; i<FITEMFACT_DEFAULT_SIZE; ++i )
		m_sFITEMFACT[i].bENABLE = TRUE;

	/*system buffs, Juver, 2017/09/04 */
	for ( int i=0; i<SYSTEM_BUFF_SIZE; ++i )
		m_sSYSTEM_BUFF[i] = SSYSTEM_BUFF();
}

void SCHARDATA2::Assign ( SCHARDATA2 &sDATA )
{
	GETCHARDATA() = sDATA.GETCHARDATA();

	StringCchCopy( m_szUID, USR_ID_LENGTH+1, sDATA.m_szUID );

	m_ExpSkills = sDATA.m_ExpSkills;

	memcpy ( m_PutOnItems, sDATA.m_PutOnItems, sizeof(SITEMCUSTOM)*SLOT_TSIZE );

	m_wSKILLQUICK_ACT = sDATA.m_wSKILLQUICK_ACT;
	memcpy ( m_sSKILLQUICK, sDATA.m_sSKILLQUICK, sizeof(SNATIVEID)*EMSKILLQUICK_SIZE );
	memcpy ( m_sACTIONQUICK, sDATA.m_sACTIONQUICK, sizeof(SACTION_SLOT)*EMACTIONQUICK_SIZE );

	m_cInventory.Assign ( sDATA.m_cInventory );

#if defined(VN_PARAM) //vietnamtest%%%
	m_cVietnamInventory.Assign( sDATA.m_cVietnamInventory );
#endif
	
	m_bServerStorage = sDATA.m_bServerStorage;
	m_lnStorageMoney = sDATA.m_lnStorageMoney;

	for ( int i=0; i<EMSTORAGE_CHANNEL; ++i )
	{
		m_bStorage[i] = sDATA.m_bStorage[i];
		m_cStorage[i].Assign ( sDATA.m_cStorage[i] );
	}

	m_cQuestPlay = sDATA.m_cQuestPlay;

	m_mapCharged = sDATA.m_mapCharged;
	m_cInvenCharged.Assign ( sDATA.m_cInvenCharged );
	m_mapChargedKey = sDATA.m_mapChargedKey;
	//itemmall
	for ( int i=0; i<13; i++ )
	{	

		m_cInvenItemShop[i].Assign ( sDATA.m_cInvenItemShop[i] );
		m_mapItemShopKey[i] = sDATA.m_mapItemShopKey[i];

		m_cInvenItemShopVote[i].Assign ( sDATA.m_cInvenItemShopVote[i] );
		m_mapItemShopVoteKey[i] = sDATA.m_mapItemShopVoteKey[i];

		
		m_cInvenItemShopPremium[i].Assign ( sDATA.m_cInvenItemShopPremium[i] );
		m_mapItemShopPremiumKey[i] = sDATA.m_mapItemShopPremiumKey[i];

	}
	m_mapItemShop = sDATA.m_mapItemShop;
	m_sStartMapID = sDATA.m_sStartMapID;
	m_dwStartGate = sDATA.m_dwStartGate;
	m_vStartPos = sDATA.m_vStartPos;

	m_sSaveMapID = sDATA.m_sSaveMapID;
	m_vSavePos = sDATA.m_vSavePos;

	m_sLastCallMapID = sDATA.m_sLastCallMapID;
	m_vLastCallPos = sDATA.m_vLastCallPos;

	m_mapCoolTimeType = sDATA.m_mapCoolTimeType;
	m_mapCoolTimeID = sDATA.m_mapCoolTimeID;

#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
	m_bTracingUser = sDATA.m_bTracingUser;
#endif

	m_dwThaiCCafeClass	= sDATA.m_dwThaiCCafeClass;
	m_nMyCCafeClass		= sDATA.m_nMyCCafeClass;					// 말레이시아 PC방 이벤트

	/*itemfood system, Juver, 2017/05/24 */
	for ( int i=0; i<FITEMFACT_SIZE; ++i )
		m_sFITEMFACT[i] = sDATA.m_sFITEMFACT[i];

	/*system buffs, Juver, 2017/09/04 */
	for ( int i=0; i<SYSTEM_BUFF_SIZE; ++i )
		m_sSYSTEM_BUFF[i] = sDATA.m_sSYSTEM_BUFF[i];

	/*activity system, Juver, 2017/10/23 */
	m_mapActivityProg = sDATA.m_mapActivityProg;
	m_mapActivityDone = sDATA.m_mapActivityDone;
}

BOOL SCHARDATA2::SETEXPSKILLS_BYBUF ( CByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
	{
		//CONSOLEMSG_WRITE ( "SETITEM_BYBUFFER() CByteStream::IsEmpty()" );
		return TRUE;
	}

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + EXPSKILLS_ARRAY[SIZE]
	//
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	GASSERT ( (dwSize==sizeof(SCHARSKILL)) && (SCHARSKILL::VERSION==dwVersion) );

	SCHARSKILL sCharSkill;
	for ( DWORD i=0; i<dwNum; i++ )
	{
		ByteStream.ReadBuffer ( (LPBYTE)&sCharSkill, sizeof(SCHARSKILL) );
		m_ExpSkills.insert ( std::make_pair(sCharSkill.sNativeID.dwID,sCharSkill) );
	}

	return TRUE;
}

BOOL SCHARDATA2::GETEXPSKILLS_BYBUF ( CByteStream &ByteStream ) const
{
	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + EXPSKILLS_ARRAY[SIZE]
	//
	ByteStream.ClearBuffer ();

	ByteStream << SCHARSKILL::VERSION;
	ByteStream << SCHARSKILL::SIZE;
	
	DWORD dwSize = (DWORD) m_ExpSkills.size();
	ByteStream << dwSize;

	SKILL_MAP_CITER iter = m_ExpSkills.begin ();
	SKILL_MAP_CITER iter_end = m_ExpSkills.end ();

	for ( ; iter!=iter_end; ++iter )
	{
		const SCHARSKILL &sSkill = (*iter).second;
		ByteStream.WriteBuffer ( &sSkill, (DWORD)sizeof(SCHARSKILL) );
	}

	return TRUE;
}

BOOL SCHARDATA2::GETPUTONITEMS_BYBUF ( CByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << SITEMCUSTOM::VERSION;
	ByteStream << (DWORD)sizeof(SITEMCUSTOM);
	
	DWORD dwSize = (DWORD) SLOT_TSIZE;
	ByteStream << dwSize;

	ByteStream.WriteBuffer ( (LPBYTE)&m_PutOnItems[0], (DWORD)sizeof(SITEMCUSTOM)*dwSize );

	return TRUE;
}

BOOL SCHARDATA2::SETSKILL_QUICKSLOT ( CByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
	{
		//CONSOLEMSG_WRITE ( "SETSKILL_QUICKSLOT() CByteStream::IsEmpty()" );
		return TRUE;
	}

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + m_sSKILLQUICK[SIZE]
	//
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;

	switch ( dwVersion )
	{
	case 0x0100:
		{
			ByteStream >> dwSize;
			ByteStream >> dwNum;

			SNATIVEID sSKILL_ID;
			for ( DWORD i=0; i<dwNum; i++ )
			{
				if ( i>=EMSKILLQUICK_SIZE_101 )		break;

				GASSERT ( dwSize==sizeof(SNATIVEID) );
				ByteStream.ReadBuffer ( (LPBYTE)&sSKILL_ID, sizeof(SNATIVEID) );

				m_sSKILLQUICK[i] = sSKILL_ID;
			}
		}break;

	case 0x0101:
		{
			ByteStream >> dwSize;
			ByteStream >> dwNum;

			SNATIVEID sSKILL_ID;
			for ( DWORD i=0; i<dwNum; i++ )
			{
				if ( i>=EMSKILLQUICK_SIZE_101 )		break;

				GASSERT ( dwSize==sizeof(SNATIVEID) );
				ByteStream.ReadBuffer ( (LPBYTE)&sSKILL_ID, sizeof(SNATIVEID) );

				m_sSKILLQUICK[i] = sSKILL_ID;
			}

			ByteStream >> m_wSKILLQUICK_ACT;
		}break;

	case EMSKILLQUICK_VERSION:
		{
			ByteStream >> dwSize;
			ByteStream >> dwNum;

			SNATIVEID sSKILL_ID;
			for ( DWORD i=0; i<dwNum; i++ )
			{
				if ( i>=EMSKILLQUICK_SIZE )		break;

				GASSERT ( dwSize==sizeof(SNATIVEID) );
				ByteStream.ReadBuffer ( (LPBYTE)&sSKILL_ID, sizeof(SNATIVEID) );

				m_sSKILLQUICK[i] = sSKILL_ID;
			}

			ByteStream >> m_wSKILLQUICK_ACT;
		}
		break;
	};

	return TRUE;
}

BOOL SCHARDATA2::GETSKILL_QUICKSLOT ( CByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << EMSKILLQUICK_VERSION;
	ByteStream << (DWORD)sizeof(SNATIVEID);
	
	DWORD dwSize = (DWORD) EMSKILLQUICK_SIZE;
	ByteStream << dwSize;

	ByteStream.WriteBuffer ( (LPBYTE)m_sSKILLQUICK, (DWORD)sizeof(SNATIVEID)*dwSize );

	ByteStream << m_wSKILLQUICK_ACT;

	return TRUE;
}

BOOL SCHARDATA2::SETACTION_QUICKSLOT ( CByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
	{
		//CONSOLEMSG_WRITE ( "SETACTION_QUICKSLOT() CByteStream::IsEmpty()" );
		return TRUE;
	}

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + m_sActionSLOT[SIZE]
	//
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	GASSERT ( (dwSize==sizeof(SACTION_SLOT)) && (SACTION_SLOT::VERSION==dwVersion) );

	SACTION_SLOT sACTION_SLOT;
	for ( DWORD i=0; i<dwNum; i++ )
	{
		if ( i>=EMACTIONQUICK_SIZE )		break;

		ByteStream.ReadBuffer ( (LPBYTE)&sACTION_SLOT, sizeof(SACTION_SLOT) );

		m_sACTIONQUICK[i] = sACTION_SLOT;
	}

	return TRUE;
}

BOOL SCHARDATA2::GETACTION_QUICKSLOT ( CByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << SACTION_SLOT::VERSION;
	ByteStream << (DWORD)sizeof(SACTION_SLOT);
	
	DWORD dwSize = (DWORD) EMACTIONQUICK_SIZE;
	ByteStream << dwSize;

	ByteStream.WriteBuffer ( (LPBYTE)m_sACTIONQUICK, (DWORD)sizeof(SACTION_SLOT)*dwSize );
	
	return TRUE;
}

BOOL SCHARDATA2::SETINVENTORY_BYBUF ( CByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )		return TRUE;

	GLInventory cInven;
	BOOL bOk = cInven.SETITEM_BYBUFFER ( ByteStream );
	if ( !bOk )		return TRUE;

	// 프리미엄 계산.
	CalcPREMIUM();
	m_cInventory.SetAddLine ( GetOnINVENLINE(), true );

	WORD wSIZEX = cInven.GetSizeX();
	WORD wSIZEY = cInven.GetSizeY();
	if ( wSIZEX==EM_INVENSIZE_X && wSIZEY==EM_INVENSIZE_Y )
	{
		m_cInventory.Assign ( cInven );
	}
	else
	{
		m_cInventory.CleanUp ();

		m_cInventory.SetState ( EM_INVENSIZE_X, EM_INVENSIZE_Y );

		GLInventory::CELL_MAP* pmapITEM = cInven.GetItemList();
		GLInventory::CELL_MAP_CITER pos = pmapITEM->begin();
		GLInventory::CELL_MAP_CITER end = pmapITEM->end();
		for ( ; pos!=end; ++pos )
		{
			m_cInventory.InsertItem ( (*pos).second->sItemCustom );
		}
	}

	return TRUE;
}

BOOL SCHARDATA2::GETINVENTORYE_BYBUF ( CByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();
	m_cInventory.GETITEM_BYBUFFER ( ByteStream );

	return TRUE;
}

BOOL SCHARDATA2::SETSTORAGE_BYBUF ( CByteStream &ByteStream )
{
	m_bServerStorage = TRUE;

	if ( ByteStream.IsEmpty() )		return TRUE;

	DWORD dwChannel = 0;
	ByteStream >> dwChannel;
	if ( dwChannel > EMSTORAGE_CHANNEL )	dwChannel = EMSTORAGE_CHANNEL;
	
	for ( DWORD i=0; i<dwChannel; ++i )
	{
		BOOL bOk = m_cStorage[i].SETITEM_BYBUFFER ( ByteStream );
		if ( !bOk )		return TRUE;	//ERROR
	}

	return TRUE;
}

BOOL SCHARDATA2::GETSTORAGE_BYBUF ( CByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << DWORD(EMSTORAGE_CHANNEL);
	for ( DWORD i=0; i<EMSTORAGE_CHANNEL; ++i )	m_cStorage[i].GETITEM_BYBUFFER_FORSTORAGE ( ByteStream );

	return TRUE;
}

// 베트남 탐닉방지 추가에 따른 추가 인벤토리 셋팅
BOOL SCHARDATA2::SETVTADDINVENTORY_BYBUF ( CByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )		return TRUE;

	GLInventory cInven;
	BOOL bOk = cInven.SETITEM_BYBUFFER ( ByteStream );
	if ( !bOk )		return TRUE;

	m_cVietnamInventory.SetAddLine ( 6, true );

	// 베트남 탐닉 방지 아이템 표시 한다.
	GLInventory::CELL_MAP* pmapITEM = cInven.GetItemList();
	GLInventory::CELL_MAP_CITER pos = pmapITEM->begin();
	GLInventory::CELL_MAP_CITER end = pmapITEM->end();
	for ( ; pos!=end; ++pos )
	{
		// 베트남 탐닉 아이템 표시
		(*pos).second->sItemCustom.bVietnamGainItem = true;		
	}

	WORD wSIZEX = cInven.GetSizeX();
	WORD wSIZEY = cInven.GetSizeY();
	if ( wSIZEX==EM_INVENSIZE_X && wSIZEY==EM_INVENSIZE_Y )
	{
		m_cVietnamInventory.Assign ( cInven );
	}
	else
	{
		m_cVietnamInventory.CleanUp ();

		m_cVietnamInventory.SetState ( EM_INVENSIZE_X, EM_INVENSIZE_Y );

		GLInventory::CELL_MAP* pmapITEM = cInven.GetItemList();
		GLInventory::CELL_MAP_CITER pos = pmapITEM->begin();
		GLInventory::CELL_MAP_CITER end = pmapITEM->end();
		for ( ; pos!=end; ++pos )
		{
			m_cVietnamInventory.InsertItem ( (*pos).second->sItemCustom );
		}
	}

	return TRUE;
}


BOOL SCHARDATA2::GETITEMCOOLTIME_BYBUF( CByteStream &ByteStream ) const
{

	ByteStream.ClearBuffer ();

	ByteStream << ITEM_COOLTIME::VERSION;
	ByteStream << (DWORD)sizeof(ITEM_COOLTIME);
	
	DWORD dwSize = (DWORD) m_mapCoolTimeID.size();
	ByteStream << dwSize;


	const ITEM_COOLTIME* pCoolTime;
	
	COOLTIME_MAP_CITER	pos = m_mapCoolTimeID.begin();
	COOLTIME_MAP_CITER	end = m_mapCoolTimeID.end();

	for ( ; pos != end; ++pos )
	{
		pCoolTime = &pos->second;
		ByteStream.WriteBuffer ( (LPBYTE)pCoolTime, sizeof(ITEM_COOLTIME) );
	}

	dwSize = (DWORD) m_mapCoolTimeType.size();
	ByteStream << dwSize;

	pos = m_mapCoolTimeType.begin();
	end = m_mapCoolTimeType.end();

	for ( ; pos != end; ++pos )
	{
		pCoolTime = &pos->second;
		ByteStream.WriteBuffer ( (LPBYTE)pCoolTime, sizeof(ITEM_COOLTIME) );
	}

	return TRUE;
}

BOOL SCHARDATA2::SETITEMCOOLTIME_BYBUF( CByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
	{
		return TRUE;
	}

	//	DWORD[VERSION] + DWORD[SIZE] + DWORD[NUMBER] + ITEM_COOLTIME[SIZE]
	//
	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	GASSERT ( (dwSize==sizeof(ITEM_COOLTIME)) && (ITEM_COOLTIME::VERSION==dwVersion) );

	ITEM_COOLTIME sCoolTime;
	
	for ( DWORD i=0; i<dwNum; i++ )
	{
		ByteStream.ReadBuffer ( (LPBYTE)&sCoolTime, sizeof(ITEM_COOLTIME) );
		m_mapCoolTimeID[sCoolTime.dwCoolID] = sCoolTime;		
	}

	ByteStream >> dwNum;

	for ( DWORD i=0; i<dwNum; i++ )
	{
		ByteStream.ReadBuffer ( (LPBYTE)&sCoolTime, sizeof(ITEM_COOLTIME) );
		m_mapCoolTimeType[sCoolTime.dwCoolID] = sCoolTime;
	}

	return TRUE;
}

// 베트남 탐직 방지 추가에 따른 추가 인벤토리 정보 가져오기
BOOL SCHARDATA2::GETVTADDINVENTORYE_BYBUF ( CByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();
	m_cVietnamInventory.GETITEM_BYBUFFER ( ByteStream );

	return TRUE;
}


BOOL SCHARDATA2::SETQUESTPLAY ( CByteStream &ByteStream )
{
	m_cQuestPlay.GET_BYBUFFER ( ByteStream );
	return TRUE;
}

BOOL SCHARDATA2::GETQUESTPLAY ( CByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();
	m_cQuestPlay.SET_BYBUFFER ( ByteStream );

	return TRUE;
}

BOOL SCHARDATA2::SETSHOPPURCHASE ( VECSHOP &vecSHOP )
{
	m_mapCharged.clear();
	DWORD dwSIZE = static_cast<DWORD>(vecSHOP.size());
	for ( DWORD i=0; i<dwSIZE; ++i )
	{
		const SHOPPURCHASE &sPURCHASE = vecSHOP[i];
		m_mapCharged.insert ( std::make_pair(sPURCHASE.strPurKey,sPURCHASE) );
	}

	return TRUE;
}
//itemmall
BOOL SCHARDATA2::SETITEMSHOP ( VECITEMSHOP &vecSHOP )
{
	m_mapItemShop.clear();
	DWORD dwSIZE = static_cast<DWORD>(vecSHOP.size());
	for ( DWORD i=0; i<dwSIZE; ++i )
	{
		const ITEMSHOP &sITEMSHOP = vecSHOP[i];
		m_mapItemShop.insert ( std::make_pair(sITEMSHOP.strItemNum,sITEMSHOP) );
	}
	return TRUE;
}

BOOL SCHARDATA2::ADDITEMSHOP ( const char* szPurKey, SNATIVEID nidITEM , const WORD wPrice , const WORD wStock , WORD wItemCtg , WORD wCurrency )
{
	SITEMCUSTOM sCUSTOM(nidITEM);

	SITEM *pITEM_DATA = GLItemMan::GetInstance().GetItem ( nidITEM );
	if ( !pITEM_DATA )		
	{
		return FALSE;
	}
	sCUSTOM.wTurnNum = pITEM_DATA->GETAPPLYNUM();

	sCUSTOM.cDAMAGE = (BYTE)pITEM_DATA->sBasicOp.wGradeAttack;
	sCUSTOM.cDEFENSE = (BYTE)pITEM_DATA->sBasicOp.wGradeDefense;

	
	if ( wCurrency == 1 )
	{
	WORD wPosX(0), wPosY(0);
		m_cInvenItemShopVote[wItemCtg].FindInsrtable ( pITEM_DATA->sBasicOp.wInvenSizeX, pITEM_DATA->sBasicOp.wInvenSizeY, wPosX, wPosY );
			BOOL bOK = m_cInvenItemShopVote[wItemCtg].InsertItem2Shop ( sCUSTOM, wPosX, wPosY ,false, wPrice , wStock , wItemCtg , wCurrency , szPurKey );
			if ( !bOK ) 
			{
				CString strText;
				strText.Format("%s %s %d",pITEM_DATA->GetName(),szPurKey,wItemCtg);
				//MessageBox(NULL,strText,"Vote",NULL);
				return FALSE;
			}
			
			SNATIVEID nid(wPosX,wPosY);
			m_mapItemShopVoteKey[wItemCtg].insert ( std::make_pair(nid.dwID,szPurKey) );
		wPosX = 0, wPosY = 0;
		m_cInvenItemShopVote[0].FindInsrtable ( pITEM_DATA->sBasicOp.wInvenSizeX, pITEM_DATA->sBasicOp.wInvenSizeY, wPosX, wPosY );
			BOOL bOK3 = m_cInvenItemShopVote[0].InsertItem2Shop ( sCUSTOM, wPosX, wPosY ,false, wPrice , wStock , 0 , wCurrency , szPurKey );
			if ( !bOK3 ) 
			{
				CString strText;
				strText.Format("%s %s %d",pITEM_DATA->GetName(),szPurKey,0);
				//MessageBox(NULL,strText,"Vote",NULL);
				return FALSE; 
			}
			m_mapItemShopVoteKey[0].insert ( std::make_pair(nid.dwID,szPurKey) );

wPosX = 0, wPosY = 0;
		m_cInvenItemShop[wItemCtg].FindInsrtable ( pITEM_DATA->sBasicOp.wInvenSizeX, pITEM_DATA->sBasicOp.wInvenSizeY, wPosX, wPosY );
			BOOL bOK2 = m_cInvenItemShop[wItemCtg].InsertItem2Shop ( sCUSTOM, wPosX, wPosY ,false, wPrice , wStock , wItemCtg , wCurrency , szPurKey );
			if ( !bOK2 )
			{
				
				CString strText;
				strText.Format("%s %s %d",pITEM_DATA->GetName(),szPurKey,wItemCtg);
				return FALSE;
			}
				
			m_mapItemShopKey[wItemCtg].insert ( std::make_pair(nid.dwID,szPurKey) );

	wPosX = 0, wPosY = 0;
		m_cInvenItemShop[0].FindInsrtable ( pITEM_DATA->sBasicOp.wInvenSizeX, pITEM_DATA->sBasicOp.wInvenSizeY, wPosX, wPosY );
			BOOL bOKALL = m_cInvenItemShop[0].InsertItem2Shop ( sCUSTOM, wPosX, wPosY ,false, wPrice , wStock , 0 , wCurrency , szPurKey );
			if ( !bOKALL )
			{
				CString strText;
				strText.Format("%s %s %d",pITEM_DATA->GetName(),szPurKey,0);
				return FALSE;	
			}
				
			m_mapItemShopKey[0].insert ( std::make_pair(nid.dwID,szPurKey) );

		

	}
	if ( wCurrency == 2 )
	{
	WORD wPosX(0), wPosY(0);
		m_cInvenItemShopPremium[wItemCtg].FindInsrtable ( pITEM_DATA->sBasicOp.wInvenSizeX, pITEM_DATA->sBasicOp.wInvenSizeY, wPosX, wPosY );
			BOOL bOK = m_cInvenItemShopPremium[wItemCtg].InsertItem2Shop ( sCUSTOM, wPosX, wPosY ,false, wPrice , wStock , wItemCtg , wCurrency , szPurKey );
			if ( !bOK )
			{
				CString strText;
				strText.Format("%s %s %d",pITEM_DATA->GetName(),szPurKey,wItemCtg);
				return FALSE;
			} 
			
			SNATIVEID nid(wPosX,wPosY);
			m_mapItemShopPremiumKey[wItemCtg].insert ( std::make_pair(nid.dwID,szPurKey) );

		wPosX = 0, wPosY = 0;
		m_cInvenItemShopPremium[0].FindInsrtable ( pITEM_DATA->sBasicOp.wInvenSizeX, pITEM_DATA->sBasicOp.wInvenSizeY, wPosX, wPosY );
			BOOL bOK3 = m_cInvenItemShopPremium[0].InsertItem2Shop ( sCUSTOM, wPosX, wPosY ,false, wPrice , wStock , 0 , wCurrency , szPurKey );
			if ( !bOK3 ) 
			{
				CString strText;
				strText.Format("%s %s %d",pITEM_DATA->GetName(),szPurKey,0);
				return FALSE;
			}
			m_mapItemShopPremiumKey[0].insert ( std::make_pair(nid.dwID,szPurKey) );

		wPosX = 0, wPosY = 0;
		m_cInvenItemShop[wItemCtg].FindInsrtable ( pITEM_DATA->sBasicOp.wInvenSizeX, pITEM_DATA->sBasicOp.wInvenSizeY, wPosX, wPosY );
			BOOL bOK2 = m_cInvenItemShop[wItemCtg].InsertItem2Shop ( sCUSTOM, wPosX, wPosY ,false, wPrice , wStock , wItemCtg , wCurrency , szPurKey );
			if ( !bOK2 )
			{	
				CString strText;
				strText.Format("%s %s %d",pITEM_DATA->GetName(),szPurKey,wItemCtg);
				return FALSE;	
			}
				
			m_mapItemShopKey[wItemCtg].insert ( std::make_pair(nid.dwID,szPurKey) );

		wPosX = 0, wPosY = 0;
		m_cInvenItemShop[0].FindInsrtable ( pITEM_DATA->sBasicOp.wInvenSizeX, pITEM_DATA->sBasicOp.wInvenSizeY, wPosX, wPosY );
			BOOL bOKALL = m_cInvenItemShop[0].InsertItem2Shop ( sCUSTOM, wPosX, wPosY ,false, wPrice , wStock , 0 , wCurrency , szPurKey );
			if ( !bOKALL )
			{
				CString strText;
				strText.Format("%s %s %d",pITEM_DATA->GetName(),szPurKey,0);
				return FALSE;	
			}
			m_mapItemShopKey[0].insert ( std::make_pair(nid.dwID,szPurKey) );
	}

	return TRUE;
//itemmmall end
}
BOOL SCHARDATA2::ADDSHOPPURCHASE ( const char* szPurKey, SNATIVEID nidITEM )
{
	SITEMCUSTOM sCUSTOM(nidITEM);

	SITEM *pITEM_DATA = GLItemMan::GetInstance().GetItem ( nidITEM );
	if ( !pITEM_DATA )		return FALSE;

	sCUSTOM.wTurnNum = pITEM_DATA->GETAPPLYNUM();

	// 아이템 에디트에서 입력한 개조 등급 적용 ( 준혁 )
	sCUSTOM.cDAMAGE = (BYTE)pITEM_DATA->sBasicOp.wGradeAttack;
	sCUSTOM.cDEFENSE = (BYTE)pITEM_DATA->sBasicOp.wGradeDefense;

	WORD wPosX(0), wPosY(0);
	m_cInvenCharged.FindInsrtable ( pITEM_DATA->sBasicOp.wInvenSizeX, pITEM_DATA->sBasicOp.wInvenSizeY, wPosX, wPosY );
	BOOL bOK = m_cInvenCharged.InsertItem ( sCUSTOM, wPosX, wPosY );
	if ( !bOK )
	{
		return FALSE;
	}

	SNATIVEID nid(wPosX,wPosY);
	m_mapChargedKey.insert ( std::make_pair(nid.dwID,szPurKey) );

	return TRUE;
}

BOOL SCHARDATA2::DELSHOPPURCHASE ( const DWORD dwID )
{
	SNATIVEID nidPOS(dwID);
	BOOL bOK = m_cInvenCharged.DeleteItem ( nidPOS.wMainID, nidPOS.wSubID );
	if ( !bOK )		return FALSE;

	MAPSHOP_KEY_ITER iter = m_mapChargedKey.find ( dwID );
	if( iter==m_mapChargedKey.end() )	return FALSE;
	m_mapChargedKey.erase ( iter );

	return TRUE;
}

const SITEMCUSTOM& SCHARDATA2::GET_REBUILD_ITEM()	// ITEMREBUILD_MARK
{
	static SITEMCUSTOM sItemCustom;
	sItemCustom.sNativeID = NATIVEID_NULL();

	if( !m_sRebuildItem.VALID() )
		return sItemCustom;

	SINVENITEM* pResistItem = m_cInventory.GetItem( m_sRebuildItem.wPosX, m_sRebuildItem.wPosY );
	if( !pResistItem )
		return sItemCustom;

	sItemCustom = pResistItem->sItemCustom;

	return sItemCustom;
}

const SITEMCUSTOM& SCHARDATA2::GET_REBUILD_SEAL() //sealed card 
{
	static SITEMCUSTOM sItemCustom;
	sItemCustom.sNativeID = NATIVEID_NULL();

	if( !m_sRebuildSeal.VALID() )
		return sItemCustom;

	SINVENITEM* pResistItem = m_cInventory.GetItem( m_sRebuildSeal.wPosX, m_sRebuildSeal.wPosY );
	if( !pResistItem )
		return sItemCustom;

	sItemCustom = pResistItem->sItemCustom;

	return sItemCustom;
}

const SITEMCUSTOM& SCHARDATA2::GET_PREHOLD_ITEM()
{
	static SITEMCUSTOM sItemCustom;
	sItemCustom.sNativeID = NATIVEID_NULL();

	if( !m_sPreInventoryItem.VALID() )
		return sItemCustom;

	SINVENITEM* pResistItem = m_cInventory.GetItem( m_sPreInventoryItem.wPosX, m_sPreInventoryItem.wPosY );
	if( !pResistItem )
		return sItemCustom;

	sItemCustom = pResistItem->sItemCustom;

	return sItemCustom;
}

VOID SCHARDATA2::InitRebuildData() //sealed card 
{
	CloseRebuild();
	m_sRebuildItem.RESET();
	m_sRebuildCardPos.RESET();
	m_sPreInventoryItem.RESET();
	m_i64RebuildCost = 0;
	m_i64RebuildInput = 0;
	m_sRebuildSeal.RESET();
}

/*itemfood system, Juver, 2017/05/25 */
BOOL SCHARDATA2::GETITEMFOOD_BYBUF( CByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << (DWORD)SFITEMFACT::VERSION;
	ByteStream << (DWORD)sizeof(SFITEMFACT);
	ByteStream << (DWORD)FITEMFACT_SIZE;

	for( int i=0; i<FITEMFACT_SIZE; ++i )
		ByteStream.WriteBuffer ( (LPBYTE)&m_sFITEMFACT[i], sizeof(SFITEMFACT) );
	
	return TRUE;
}

/*itemfood system, Juver, 2017/05/25 */
BOOL SCHARDATA2::SETITEMFOOD_BYBUF( CByteStream &ByteStream )
{
	if ( ByteStream.IsEmpty() )
		return TRUE;

	DWORD dwVersion, dwSize, dwNum;

	ByteStream >> dwVersion;
	ByteStream >> dwSize;
	ByteStream >> dwNum;

	GASSERT ( (dwSize==sizeof(SFITEMFACT)) && (SFITEMFACT::VERSION==dwVersion) );

	for ( DWORD i=0; i<dwNum; i++ )
	{
		SFITEMFACT sFITEMFACT;
		ByteStream.ReadBuffer ( (LPBYTE)&sFITEMFACT, sizeof(SFITEMFACT) );
		
		if ( i >= FITEMFACT_SIZE )	continue;
		m_sFITEMFACT[i] = sFITEMFACT;
	}

	return TRUE;
}