#include "stdafx.h"

#include "DxInputDevice.h"
#include "DXInputString.h"

#include "DxViewPort.h"
#include "DxWeatherMan.h"
#include "DxEffectMan.h"
#include "StringFile.h"
#include "GLGaeaClient.h"
#include "DxResponseMan.h"
#include "GLLandMan.h"
#include "GLMobSchedule.h"
#include "GLogicData.h"

#include "GLQuestMan.h"
#include "GLPartyClient.h"
#include "GLFriendClient.h"

#include "GLBusStation.h"
#include "GLTaxiStation.h"
#include "GLItemMixMan.h"
#include "DxGlobalStage.h"
#include "DxReplaceContainer.h"
#include "DxBoneCollector.h"
#include "DxSkinPieceContainer.h"
#include "DxSkinAniMan.h"
#include "NpcDialogueSet.h"

#include "DxSkinMeshMan.h"
#include "GLStringTable.h"
#include "GLItemMan.h"

#include "D3DFont.h"
#include "Emoticon.h"
#include "RanFilter.h"
#include "Unzipper.h"
#include "ListXML.h"

#include "../Lib_Engine/Utils/RANPARAM.h"
#include "../Lib_Engine/Common/SUBPATH.h"
#include "../Lib_Engine/GUInterface/Cursor.h"
#include "../Lib_Engine/G-Logic/GLOGIC.h"
#include "../Lib_ClientUI/Interface/GameTextControl.h"
#include "../Lib_Engine/GUInterface/interfacecfg.h"
#include "../Lib_ClientUI/Interface/LoadingThread.h"
#include "../Lib_Engine/DxCommon/DxFontMan.h"

/*pvp tyranny, Juver, 2017/08/24 */
#include "GLPVPTyrannyClient.h"

/*activity system, Juver, 2017/10/21 */
#include "GLActivity.h"

/*school wars, Juver, 2018/01/19 */
#include "GLPVPSchoolWarsClient.h"

/*pvp capture the flag, Juver, 2018/01/24 */
#include "GLPVPCaptureTheFlagClient.h"


#if defined( CH_PARAM ) || defined ( HK_PARAM ) || defined ( TW_PARAM ) //|| defined ( _RELEASED ) // Apex 적용
#include "./ClientApex.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	struct CEMULMSG
	{
		float	fDelay;

		DWORD	dwSize;
		PBYTE	pBuffer;

		CEMULMSG () :
			pBuffer(NULL),
			
			dwSize(0),
			fDelay(0.0f)
		{
		}

		CEMULMSG ( float _fdelay, DWORD _dwsize, PBYTE _pbuffer )
		{
			fDelay = _fdelay;
			dwSize = _dwsize;
			if ( _dwsize==0 )	return;

			pBuffer = new BYTE [ _dwsize ];
			memcpy ( pBuffer, _pbuffer, _dwsize );
		}

		~CEMULMSG()
		{
			SAFE_DELETE_ARRAY(pBuffer);
		}

		CEMULMSG& operator= ( const CEMULMSG &cMsg )
		{
			SAFE_DELETE_ARRAY(pBuffer);
			
			fDelay = cMsg.fDelay;
			dwSize = cMsg.dwSize;
			if ( cMsg.dwSize==0 )	return *this;

			pBuffer = new BYTE [ cMsg.dwSize ];
			memcpy ( pBuffer, cMsg.pBuffer, cMsg.dwSize );

			return *this;
		}
	};

	std::deque<CEMULMSG*>	g_vecMsg;

	void _insert_emul_msg ( NET_MSG_GENERIC *nmg )
	{
		CEMULMSG* pMsg = new CEMULMSG;
		pMsg->fDelay = 0.1f;
		pMsg->dwSize = nmg->dwSize;
		pMsg->pBuffer = new BYTE [ pMsg->dwSize ];
		memcpy ( pMsg->pBuffer, nmg, pMsg->dwSize );

		g_vecMsg.push_back ( pMsg );
	}

	void _clear_emul_msg ()
	{
		std::for_each ( g_vecMsg.begin(), g_vecMsg.end(), std_afunc::DeleteObject() );
		g_vecMsg.clear();
	}

	void _update_emul_msg ( float fElapsTime )
	{
		while(1)
		{
			if ( g_vecMsg.empty() )		return;

			CEMULMSG &sMsg = *(*g_vecMsg.begin());
			
			sMsg.fDelay -= fElapsTime;
			if ( sMsg.fDelay > 0.0f )	return;

			GLGaeaServer::GetInstance().MsgProcess ( (NET_MSG_GENERIC*)sMsg.pBuffer, GETMYCLIENTID(), GETMYGAEAID() );

			delete (*g_vecMsg.begin());
			g_vecMsg.pop_front();
		}
	}
};

/*hackshield implementation, Juver, 2018/06/18 */
namespace HS_GLOBAL_DATA
{
	int hs_start_ret = 0;
	int hs_service_ret = 0;
	int hs_monitor_ret = 0;
	int hs_detect_ret = 0;
	DWORD hs_sdk_version = 0xffff; //wont check if value is default
	std::string hs_detect_string = "";
};

void DxGlobalStage::NetSend ( NET_MSG_GENERIC *nmg )
{
	if ( !m_bEmulate )	m_NetClient.Send ( (char*)nmg, nmg->dwSize );
	else				_insert_emul_msg ( nmg );
}

void DxGlobalStage::NetSendToField ( NET_MSG_GENERIC *nmg )
{
	if ( !m_bEmulate )	m_NetClient.SendToField ( (char*)nmg, nmg->dwSize );
	else				_insert_emul_msg ( nmg );
}

void DxGlobalStage::NetSendEventLottery ( const char* szLotteryName )
{
	if ( !m_bEmulate )
	{
		m_NetClient.SndEventLottery ( szLotteryName );
	}
}

DxGlobalStage& DxGlobalStage::GetInstance()
{
	static DxGlobalStage Instance;
	return Instance;
}

CRITICAL_SECTION	DxGlobalStage::m_CSMsgProcLock;

DxGlobalStage::DxGlobalStage (void) :
	m_hWnd(NULL),
	m_dwState(NULL),
	m_bEmulate(FALSE),
	m_dwBLOCK_PROG_COUNT(0),
	m_dwBLOCK_PROG_IMAGE(0),
	m_bBLOCK_PROG_DETECTED(false),
	m_bDETECTED_REPORT(false),
	m_fBLOCK_PROG_TIMER(0.0f),

	m_emThisStage(EM_STAGE_NULL),
	m_emChangeStage(EM_STAGE_NULL),

	m_pD3DApp(NULL),
	m_pd3dDevice(NULL),
	m_pActiveStage(NULL),
	
	m_pMsgActive(NULL),
	m_pMsgReceive(NULL),
	m_nChannel(-1), // 서버의 채널은 0부터 시작한다.

	/*hackshield implementation, Juver, 2018/06/21 */
	hs_close_game(FALSE),
	hs_close_timer(0.0f)
{
	InitializeCriticalSection(&m_CSMsgProcLock);

	m_MsgBufferA.reserve ( 1000 );
	m_MsgBufferB.reserve ( 1000 );

	m_pMsgActive = &m_MsgBufferA;
	m_pMsgReceive = &m_MsgBufferB;
}

DxGlobalStage::~DxGlobalStage (void)
{
	DeleteCriticalSection(&m_CSMsgProcLock);

	_clear_emul_msg();

	{
		int nSize = (int) m_GGAuthBuffer.size();
		for( int i=0; i<nSize; ++i)
		{
			P_AUTH_DATA pData = m_GGAuthBuffer.front();
			SAFE_DELETE( pData );
			m_GGAuthBuffer.pop();
		}
	}
}

void DxGlobalStage::OnInitLogin ()
{
	m_LobyStage.OnInitLogin ();
	m_GameStage.OnInitLogin ();

	GLGaeaClient::GetInstance().GetCharacter()->ResetData();
	GLPartyClient::GetInstance().ResetParty();

#if defined ( CH_PARAM ) || defined ( HK_PARAM ) || defined ( TW_PARAM ) //|| defined ( _RELEASED ) // Apex 적용
	int nReturn = APEX_CLIENT::StartApexClient();
#endif

#if defined ( HK_PARAM ) || defined ( TW_PARAM ) // Apex 적용( 홍콩 )
	NET_APEX_RETURN nmg;
	nmg.nReturn = nReturn;

	NetSend( (NET_MSG_GENERIC*) &nmg );
#endif


}

HRESULT DxGlobalStage::ReSizeWindow ( WORD wWidth, WORD wHeight )
{
	HRESULT hr;
	if ( m_pActiveStage )
	{
		hr = m_pActiveStage->ReSizeWindow ( wWidth, wHeight );
		if ( FAILED(hr) )		return E_FAIL;
	}

	return S_OK;
}

void DxGlobalStage::SetProgDetected ( DWORD dwID )
{
	if ( dwID!=UINT_MAX )
	{
		m_dwBLOCK_PROG_IMAGE = dwID;
		m_bBLOCK_PROG_DETECTED = true;
	}
}

HRESULT DxGlobalStage::OneTimeSceneInit ( const char* szAppPath, HWND hWnd, BOOL bEmulate, WORD wWidth, WORD wHeight, const char* szMapList )
{
	m_hWnd = hWnd;
	HRESULT hr = S_OK;
	StringCchCopy(m_szFullPath,MAX_PATH,szAppPath);

	char szFullPath[MAX_PATH] = {0};

	m_bEmulate = bEmulate;

	if ( wWidth==0 || wHeight==0 )
	{
		RECT rtWin;
		GetClientRect ( m_hWnd, &rtWin );

		wWidth = static_cast<WORD> ( rtWin.right - rtWin.left );
		wHeight = static_cast<WORD> ( rtWin.bottom - rtWin.top );
	}
	
	if ( !bEmulate )
	{
		//	Note : GLLandMan 경로 지정.
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLMAP_FILE );
		GLLandMan::SetPath ( szFullPath );

		//	Note : Npc 대화파일
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::NTK_FILE_ROOT );
		CNpcDialogueSet::SetPath ( szFullPath );

		//	Note : Quest 파일
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::QST_FILE_ROOT );
		GLQuestMan::GetInstance().SetPath ( szFullPath );

		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::LEVEL_FILE_ROOT );
		GLLevelFile::SetLevelPath ( szFullPath );

		//	Note : 버스 파일. 택시 파일
		//
		StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
		StringCchCat ( szFullPath, MAX_PATH, SUBPATH::GLOGIC_FILE );
		GLBusStation::GetInstance().SetPath ( szFullPath );
		GLTaxiStation::GetInstance().SetPath ( szFullPath );
		GLItemMixMan::GetInstance().SetPath ( szFullPath );

		/*activity system, Juver, 2017/10/21 */
		GLActivity::GetInstance().SetPath( szFullPath );

		// Note : GLogic zip 파일 패스 셋팅
		// zip 파일 패스는 계속 추가
		GLOGIC::SetFullPath( GLOGIC::bGLOGIC_PACKFILE );
		GLOGIC::SetEngFullPath();
		GLOGIC::OpenPackFile( szAppPath );

		//	Note : 로직 대이타 초기화.
		//
		hr = GLogicData::GetInstance().LoadData ( FALSE );
		if ( FAILED(hr) )	return hr;
	}

	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, SUBPATH::MAP_FILE );
	GLMapAxisInfo::SetPath ( szFullPath );

	if ( !szMapList )	GLGaeaClient::GetInstance().LoadMapsListFile ( "mapslist.mst" );
	else				GLGaeaClient::GetInstance().LoadMapsListFile ( szMapList );

	if ( !m_bEmulate )
	{
		//	Note : Initilaize Winsock2
		//
		if ( NET_InitializeSocket() != NET_OK )
		{
			CDebugSet::ToLogFile ( "NET_InitializeSocket NET_ERROR" );
			return E_FAIL;	
		}
		
		m_NetClient.SetWndHandle ( m_hWnd, this );
	}

	//	Note : 이모티콘 설정 데이터 파일 로딩
	//
	CEmoticon::GetInstance().LOAD ();

	// Note : 인터페이스 설정 파일 로딩
	//
	StringCchCopy ( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, SUBPATH::GUI_FILE_ROOT );
	CInterfaceCfg::GetInstance().SetPath(szFullPath);
	CListXML::GetInstance().LOAD( szFullPath );

	CListXML::LISTXMLFILE listXML = CListXML::GetInstance().GetXmlList();
	CListXML::LISTXMLFILE_ITER iterXML = listXML.begin();

	for( ; iterXML != listXML.end(); ++iterXML )
	{
		const SXMLFILE sXMLFILE = (*iterXML);
		CInterfaceCfg::GetInstance().LoadText( sXMLFILE.strFileName.c_str(), sXMLFILE.bEncrypted );
	}

	if( !CRanFilter::GetInstance().Init(GLOGIC::bGLOGIC_ZIPFILE,
										GLOGIC::bGLOGIC_PACKFILE,
										GLOGIC::strGLOGIC_ZIPFILE.c_str(), 
										GLOGIC::GetPath() ) ) 
	{
		GASSERT( 0 && "[ERROR] : Ran Filter Initialize." );
		return E_FAIL;
	}

	//m_GameStage.SetD3DApp(m_pD3DApp);

	//	Note :활성화된 stage 초기화.
	//
	if ( !m_bEmulate )
	{
		m_emThisStage = EM_STAGE_LOBY;
		m_pActiveStage = &m_LobyStage;
		if ( m_pActiveStage)
		{
			hr = m_pActiveStage->OneTimeSceneInit ( m_hWnd, wWidth, wHeight, m_szFullPath );
			if ( FAILED(hr) )		return E_FAIL;
		}
	}
	else
	{
		m_emThisStage = EM_STAGE_GAME;
		m_pActiveStage = &m_GameStage;
		if ( m_pActiveStage)
		{
			hr = m_pActiveStage->OneTimeSceneInit ( m_hWnd, wWidth, wHeight, m_szFullPath );
			if ( FAILED(hr) )		return E_FAIL;
		}
	}

	//	Note : 게임 커서 사용 여부 결정.
	//
	CCursor::GetInstance().SetGameCursor ( RANPARAM::bGameCursor );

	return S_OK;
}

HRESULT DxGlobalStage::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);

	HRESULT hr=S_OK;
	m_pd3dDevice = pd3dDevice;

	hr = m_pd3dDevice->GetDeviceCaps ( &m_d3dCaps );
	if ( FAILED(hr) )	return hr;

	//	Note : 로직 data 초기화.
	//
	hr = GLogicData::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	if ( FAILED(hr) )	return hr;
	LOADINGSTEP::SETSTEP ( 4 );
	
	DxBoneCollector::GetInstance().PreLoad ( _PRELOAD_BONE, m_pd3dDevice );
	LOADINGSTEP::SETSTEP ( 5 );

	DxSkinAniMan::GetInstance().PreLoad ( _PRELOAD_ANI, m_pd3dDevice );
	LOADINGSTEP::SETSTEP ( 7 );

	DxSkinMeshMan::GetInstance().PreLoad( _PRELOAD_SKIN, m_pd3dDevice, FALSE );
	LOADINGSTEP::SETSTEP ( 8 );

	for ( int i=0; i<GLCI_NUM_8CLASS; ++i )
		DxSkinCharDataContainer::GetInstance().LoadData( GLCONST_CHAR::szCharSkin[i], m_pd3dDevice, FALSE );

	LOADINGSTEP::SETSTEP ( 9 );

	DxFontMan::GetInstance().FindFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW|D3DFONT_ASCII );
	DxFontMan::GetInstance().FindFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

	if ( m_pActiveStage )
	{
		hr = m_pActiveStage->InitDeviceObjects ( m_pd3dDevice );
		if ( FAILED(hr) )		return E_FAIL;
	}

	LOADINGSTEP::SETSTEP ( 10 );

	//	Note : Connect Login Server
	//
	if ( !m_bEmulate )
	{
		/*login port, Juver, 2017/11/16 */
		if ( m_NetClient.ConnectLoginServer(RANPARAM::LoginAddress, RANPARAM::nLoginPort) <= NET_ERROR )
			CDebugSet::ToListView ( "DxGlobalStage::InitDeviceObjects ConnectLoginServer NET_ERROR" );
		else
			CDebugSet::ToListView ( "DxGlobalStage::InitDeviceObjects ConnectLoginServer NET_OK" );

		//	Note : Send version information
		//
		m_NetClient.SndVersion ();
		m_NetClient.SndReqServerInfo ();
	}

	return S_OK;
}

HRESULT DxGlobalStage::RestoreDeviceObjects ()
{
	HRESULT hr=S_OK;
	if ( !m_pd3dDevice )	return S_FALSE;

	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,		TRUE );
	}

	float fFogStart=720.0f, fFogEnd=790.0f, fFongDensity=0.0f;

	m_pd3dDevice->SetRenderState ( D3DRS_FOGSTART,		*((DWORD *)(&fFogStart)) );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGEND,		*((DWORD *)(&fFogEnd)) );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGDENSITY,	*((DWORD *)(&fFongDensity)) );

	m_pd3dDevice->SetRenderState( D3DRS_FOGVERTEXMODE,	D3DFOG_LINEAR );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGTABLEMODE,	D3DFOG_NONE );

	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGRANGE )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_RANGEFOGENABLE,	TRUE );
	}

	if ( m_pActiveStage)
	{
		hr = m_pActiveStage->RestoreDeviceObjects ();
		if ( FAILED(hr) )		return E_FAIL;
	}

	return S_OK;
}

HRESULT DxGlobalStage::ChangeStage ( WORD wWidth, WORD wHeight )
{
	DWORD dwColorArg1(NULL),dwOldMod(NULL);
	m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG1,	&dwColorArg1 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TEXTURE );

	m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,		&dwOldMod );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,		D3DTOP_SELECTARG1 );

	std::string strLoadingImageName = "";
	std::string strLoadingMapName = "";

	switch ( m_emChangeStage )
	{
	case EM_STAGE_LOBY:
		strLoadingImageName = _T("loading_002.dds");
		break;

	case EM_STAGE_GAME:
		{
			DxLobyStage * pLobbyStage = GetLobyStage();
			if( !pLobbyStage ) break;
			
			SCHARINFO_LOBBY * pCharInfo_Lobby = pLobbyStage->GetSelectCharInfo();
			if( !pCharInfo_Lobby) break;
			
			GLMapList::FIELDMAP MapsList = GLGaeaClient::GetInstance().GetMapList();
			GLMapList::FIELDMAP_ITER iter = MapsList.find( pCharInfo_Lobby->m_sSaveMapID.dwID );
			if( iter == MapsList.end() ) break;

			const SMAPNODE *pMapNode = &(*iter).second;
			if( !pMapNode ) break;

			strLoadingImageName = pMapNode->strLoadingImageName;
			strLoadingMapName = pMapNode->strMapName;
		}
		break;
	};

	NLOADINGTHREAD::StartThreadLOAD( &m_pd3dDevice, m_hWnd, m_szFullPath, strLoadingImageName.c_str(), strLoadingMapName.c_str(), TRUE );
	NLOADINGTHREAD::WaitThread();

	LOADINGSTEP::SETSTEP ( 1 );

	//	Note : 액티브 stage 비활성화.
	//
	if ( m_pActiveStage )	m_pActiveStage->DeActive ();
	LOADINGSTEP::SETSTEP ( 2 );

	DxResponseMan::GetInstance().DoInterimClean ( m_pd3dDevice );
	LOADINGSTEP::SETSTEP ( 3 );

	int nstep = 0;
	//	Note : 케릭터 기본 스킨 다시 로드.
	for ( int i=0; i<GLCI_NUM_8CLASS; ++i )
	{
		DxSkinCharDataContainer::GetInstance().LoadData( GLCONST_CHAR::szCharSkin[i], m_pd3dDevice, FALSE );
		nstep = i / 2;
		LOADINGSTEP::SETSTEP ( nstep + 4 );
	}

	//	Note : 활성화 stage 변경.
	//
	switch ( m_emChangeStage )
	{
	case EM_STAGE_LOBY:	m_pActiveStage = &m_LobyStage;	break;
	case EM_STAGE_GAME:	m_pActiveStage = &m_GameStage;	break;
	};

	//	Note : 현제 stage 활성화.
	//
	if ( m_pActiveStage )
	{
		m_pActiveStage->SetActive ( m_pd3dDevice, m_hWnd, wWidth, wHeight );
	}

	LOADINGSTEP::SETSTEP ( 10 );

	ReSetSTATE(EM_CHANGE);
	m_emThisStage = m_emChangeStage;

	LOADINGSTEP::SETSTEP ( 11 );

	NLOADINGTHREAD::EndThread();

	DxResponseMan::GetInstance().SetRenderState ();

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,		dwOldMod );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	dwColorArg1 );

	return S_OK;
}

HRESULT DxGlobalStage::GameToLobbyStage ()
{
	if ( m_bEmulate )	return S_FALSE;

#if defined ( CH_PARAM ) || defined ( HK_PARAM ) || defined ( TW_PARAM )//|| defined ( _RELEASED ) // Apex 적용
	APEX_CLIENT::StopApexClient();
#endif

	//	Note : 네트웍 연결 종료.
	if ( m_NetClient.IsOnline() == TRUE )
		m_NetClient.CloseConnect ();

	//	Note : 메세지 버퍼 초기화.
	//
	EnterCriticalSection(&m_CSMsgProcLock);
	{
		std::for_each ( m_MsgBufferA.begin(), m_MsgBufferA.end(), std_afunc::DeleteArray() );
		m_MsgBufferA.clear ();

		std::for_each ( m_MsgBufferB.begin(), m_MsgBufferB.end(), std_afunc::DeleteArray() );
		m_MsgBufferB.clear ();
	}
	LeaveCriticalSection(&m_CSMsgProcLock);

	m_GameStage.ResetCharJoinData();

	//	Note : 스테이지 변경 요청.
	//
	ToChangeStage ( EM_STAGE_LOBY );
	SetSTATE(EM_REQCONNECT_LOGINSVR);

	return S_OK;
}

HRESULT DxGlobalStage::ToChangeStage ( EMSTAGE emChangeStage )
{
	if ( m_emThisStage==emChangeStage )	return S_FALSE;

	SetSTATE(EM_CHANGE);
	m_emChangeStage = emChangeStage;

	return S_OK;
}

void DxGlobalStage::CloseGame ( LPCTSTR lpszMsg )
{
	SetSTATE(EM_CLOSE);

	if (lpszMsg)
	{
		MessageBox( m_hWnd, lpszMsg, "Error Message", MB_OK ); // or use game’s UI
	}

    ::PostMessage ( m_hWnd, WM_CLOSE, 0, 0 );	
}

HRESULT DxGlobalStage::FrameMove ( float m_fTime, float m_fElapsedTime )
{
	HRESULT hr=S_OK;
	if ( IsSTATE(EM_CLOSE) )	return S_FALSE;

	//	Note : 에뮬레이터에서 메세지 지연을 강제로 만들기 위해서.
	//
	if ( m_bEmulate )
	{
		_update_emul_msg ( m_fElapsedTime );
	}

	//	Note : 수신된 메시지처리.
	//
	PROFILE_BEGIN("net msg");
	MsgProcessFrame ();
	PROFILE_END("net msg");

	//	Note : 스테이지변경시에 처리.
	//
	if ( IsSTATE(EM_CHANGE) )
	{
		hr = ChangeStage ( m_wWidth, m_wHeight );
		if ( FAILED(hr) )	return hr;
	}
    
	//	Note : Connect Login Server
	//
	if ( !m_bEmulate && IsSTATE(EM_REQCONNECT_LOGINSVR) )
	{
		/*login port, Juver, 2017/11/16 */
		if ( m_NetClient.ConnectLoginServer(RANPARAM::LoginAddress, RANPARAM::nLoginPort) <= NET_ERROR )
		{
			CloseGame ();
			CDebugSet::ToListView ( "DxGlobalStage::FrameMove ConnectLoginServer NET_ERROR" );
		}
		else
		{
			CDebugSet::ToListView ( "DxGlobalStage::FrameMove ConnectLoginServer NET_OK" );

			//	Note : Send version information
			//
			m_NetClient.SndVersion ();
			m_NetClient.SndReqServerInfo ();
		}

		ReSetSTATE(EM_REQCONNECT_LOGINSVR);
	}

	if ( m_pActiveStage )
	{
		hr = m_pActiveStage->FrameMove ( m_fTime, m_fElapsedTime );
		if ( FAILED(hr) )	return E_FAIL;
	}

	/*hackshield implementation, Juver, 2018/06/21 */
	if ( hs_close_game )
	{
		hs_close_timer += m_fElapsedTime;

		if ( hs_close_timer >= 60.0f )
		{
			hs_close_timer = 0.0f;
			hs_close_game = FALSE;
			CloseGame();
		}
	}

	return S_OK;
}

HRESULT DxGlobalStage::Render ()
{
	HRESULT hr=S_OK;
	if ( IsSTATE(EM_CLOSE) )	return S_FALSE;

	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX )		
		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );

	if ( m_pActiveStage)
	{
		hr = m_pActiveStage->Render ();
		if ( FAILED(hr) )		return E_FAIL;
	}

	return S_OK;
}

HRESULT DxGlobalStage::InvalidateDeviceObjects ()
{
	HRESULT hr=S_OK;

	if ( m_pActiveStage)
	{
		hr = m_pActiveStage->InvalidateDeviceObjects ();
		if ( FAILED(hr) )		return E_FAIL;
	}

	return S_OK;
}

HRESULT DxGlobalStage::DeleteDeviceObjects ()
{
	HRESULT hr=S_OK;

	//	Note : 로직 data 삭제
	//
	hr = GLogicData::GetInstance().DeleteDeviceObjects ();
	if ( FAILED(hr) )	return hr;

	if ( m_pActiveStage)
	{
		hr = m_pActiveStage->DeleteDeviceObjects ();
		if ( FAILED(hr) )		return E_FAIL;
	}

	return S_OK;
}

HRESULT DxGlobalStage::FinalCleanup ()
{
	HRESULT hr=S_OK;

	hr = GLogicData::GetInstance().ClearData ();
	if ( FAILED(hr) )	return hr;

	if ( m_pActiveStage)
	{
		hr = m_pActiveStage->FinalCleanup ();
		if ( FAILED(hr) )		return E_FAIL;
	}

	//	Note : 에뮬레이터가 아니면 네트웍 연결 종료.
	if ( !m_bEmulate )
	{
		if ( m_NetClient.IsOnline() == true )
			m_NetClient.CloseConnect ();

		NET_CloseSocket();
	}

	NLOADINGTIP::Clear();
	//	Note : 메세지 버퍼 초기화.
	//
	EnterCriticalSection(&m_CSMsgProcLock);
	{
		std::for_each ( m_MsgBufferA.begin(), m_MsgBufferA.end(), std_afunc::DeleteArray() );
		m_MsgBufferA.clear ();

		std::for_each ( m_MsgBufferB.begin(), m_MsgBufferB.end(), std_afunc::DeleteArray() );
		m_MsgBufferB.clear ();
	}
	LeaveCriticalSection(&m_CSMsgProcLock);

	return S_OK;
}

LRESULT DxGlobalStage::MsgProc ( MSG* pMsg )
{
	DxResponseMan::GetInstance().MsgProc ( pMsg );

	return 0L;
}

LRESULT DxGlobalStage::OnNetNotify(WPARAM wParam, LPARAM lParam)
{
	m_NetClient.ClientProcess (wParam, lParam);

	return 0L;
}

void DxGlobalStage::MsgQueueFlip ()
{
	EnterCriticalSection(&m_CSMsgProcLock);
	{
		MSGBUFFERLIST* pMsgTemp = m_pMsgActive;

		m_pMsgActive = m_pMsgReceive;
		m_pMsgReceive = pMsgTemp;
	}
	LeaveCriticalSection(&m_CSMsgProcLock);
}

void DxGlobalStage::MsgProcess ( NET_MSG_GENERIC* nmg )
{
	EnterCriticalSection(&m_CSMsgProcLock);
	{
		MSGBUFFERLIST &MsgBufferList = GetActiveMsgList();

		PBYTE pBuffer = new BYTE[nmg->dwSize];
		memcpy ( pBuffer, nmg, sizeof(BYTE)*nmg->dwSize );

		MsgBufferList.push_back ( pBuffer );
	}
	LeaveCriticalSection(&m_CSMsgProcLock);
}

void DxGlobalStage::MsgProcessFrame ()
{
	//	Note : Active Msg Buffer 플립 시킴.
	//
	MsgQueueFlip ();

	// 현제 가져올 메세지 버퍼 리스트를 가져옴.
	MSGBUFFERLIST &MsgReceivedMsg = GetReceivedMsgList();

	MSGBUFFERLIST_ITER iter = MsgReceivedMsg.begin();
	MSGBUFFERLIST_ITER iter_end = MsgReceivedMsg.end();
	for ( ; iter!=iter_end; ++iter )
	{
		NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) (*iter);

		if ( nmg->nType==NET_MSG_COMBINE )
		{
			GLMSG::SNET_COMBINE &sMSG_COMBINE = *((GLMSG::SNET_COMBINE*)nmg);
			GASSERT(sMSG_COMBINE.IsVALID());

			sMSG_COMBINE.RESET_CUR();

			WORD wNUM(0);
			NET_MSG_GENERIC* pMsg(NULL);
			while(sMSG_COMBINE.POPMSG(pMsg))
			{
				wNUM++;
				GASSERT(wNUM<NET_DATA_BUFSIZE&&"sMSG_COMBINE.POPMSG()");
				if ( wNUM>NET_DATA_BUFSIZE )	continue;

				MsgProcessFrame ( pMsg );
			}
		}
		else
		{
			MsgProcessFrame ( nmg );
		}
	}

	std::for_each ( MsgReceivedMsg.begin(), MsgReceivedMsg.end(), std_afunc::DeleteArray() );
	MsgReceivedMsg.clear ();
}

void DxGlobalStage::MsgProcessFrame ( NET_MSG_GENERIC *nmg )
{
	int test = 0;
	if( nmg->nType == NET_MSG_PET_PETSKINPACKOPEN_BRD )
	{
		test = 10;
	}

	//CDebugSet::ToListView ( "msg : %d", nmg->nType );
	switch ( nmg->nType )
	{
	case NET_MSG_LOGIN_FB:
	case NET_MSG_PASSCHECK_FB:
	case NET_MSG_CHA_BAINFO:
	case NET_MSG_LOBBY_CHAR_SEL:
	case NET_MSG_CHA_DEL_FB_OK:
	case NET_MSG_CHA_DEL_FB_ERROR:
	case NET_MSG_CHA_NEW_FB:
	case JAPAN_NET_MSG_UUID:
	//case CHINA_NET_MSG_REGISTER_FB:
	case CHINA_NET_MSG_PASS_FB: //userpanel
	case CHINA_NET_MSG_PIN_FB:
	case CHINA_NET_MSG_EMAIL_FB:
	case CHINA_NET_MSG_GT2VP_FB:
	case CHINA_NET_MSG_TOPUP_FB:
	case CHINA_NET_MSG_PP2VP_FB:
	case CHINA_NET_MSG_RESETPASS_FB:
	case CHINA_NET_MSG_RESETPASS2_FB:
	case CHINA_NET_MSG_RESETPIN_FB:
	case NET_MSG_LOBBY_CHAR_JOIN_FB:
	case NET_MSG_LOBBY_CHINA_ERROR:
	case NET_MSG_REGISTER_ACTION_FB: /*register page, Juver, 2017/11/18 */
		{
			m_LobyStage.MsgProcess ( nmg );
		}
		break;

	case NET_MSG_CHAT_FB:
	case NET_MSG_CHAT_CTRL_FB:			// 관리자용 채팅메시지
	case NET_MSG_SERVER_GENERALCHAT:

	case NET_MSG_ALLIANCE_BATTLE_BEGIN:
	case NET_MSG_ALLIANCE_BATTLE_BEGIN_REFUSE:
	case NET_MSG_ALLIANCE_BATTLE_OVER_ARMISTICE:
	case NET_MSG_ALLIANCE_BATTLE_OVER_ARMISTICE_RESULT:
	case NET_MSG_ALLIANCE_BATTLE_OVER_DRAW:
	case NET_MSG_ALLIANCE_BATTLE_OVER_SUBMISSION:
	case NET_MSG_ALLIANCE_BATTLE_OVER_WIN:
	case NET_MSG_ALLIANCE_BATTLE_RESULT:
	case NET_MSG_CLUB_BATTLE_BEGIN:
	case NET_MSG_CLUB_BATTLE_BEGIN_REFUSE:
	case NET_MSG_CLUB_BATTLE_OVER_ARMISTICE:
	case NET_MSG_CLUB_BATTLE_OVER_ARMISTICE_RESULT:
	case NET_MSG_CLUB_BATTLE_OVER_DRAW:
	case NET_MSG_CLUB_BATTLE_OVER_SUBMISSION:
	case NET_MSG_CLUB_BATTLE_OVER_WIN:
	case NET_MSG_CLUB_BATTLE_RESULT:
	case NET_MSG_CLUB_DEATHMATCH_END_RANKING:
	case NET_MSG_EMCONFRONT_END_CDRAWN:
	case NET_MSG_EMCONFRONT_END_CWIN:
	case NET_MSG_EMCONFRONT_END_PDRAWN:
	case NET_MSG_EMCONFRONT_END_PWIN:
	case NET_MSG_EMCONFRONT_START_PARTY:
	case NET_MSG_EMCROWACT_KNOCK:
	case NET_MSG_EMCROWACT_REPULSE:
	case NET_MSG_EMGUIDCLUB_CERTIFIED:
	case NET_MSG_BRIGHT_EVENT_MSG:
	case NET_MSG_CHAT_PRIVATE_FAIL:
	case NET_MSG_CHAT_BLOCK:

	case NET_MSG_LOBBY_CHAR_JOIN:
	case NET_MSG_LOBBY_CHAR_PUTON:
	case NET_MSG_LOBBY_CHAR_PUTON_EX:
	case NET_MSG_LOBBY_CHAR_ITEM:
	case NET_MSG_LOBBY_CHAR_SKILL:
	case NET_MSG_LOBBY_QUEST_END:
	case NET_MSG_LOBBY_QUEST_PROG:
	case NET_MSG_LOBBY_CHARGE_ITEM:
	case NET_MSG_LOBBY_ITEM_COOLTIME:

		 /*itemfood system, Juver, 2017/05/25 */
	case NET_MSG_LOBBY_CHAR_ITEMFOOD:

		/*activity system, Juver, 2017/10/30 */
	case NET_MSG_LOBBY_CHAR_ACTIVITY_PROG: 
	case NET_MSG_LOBBY_CHAR_ACTIVITY_DONE: 
	
	case NET_MSG_LOBBY_CLUB_INFO:
	case NET_MSG_LOBBY_CLUB_MEMBER:
	case NET_MSG_LOBBY_CLUB_ALLIANCE:
	case NET_MSG_LOBBY_CLUB_BATTLE:
	case NET_MSG_LOBBY_CHAR_VIETNAM_ITEM:

	case NET_MSG_GCTRL_DROP_OUT_FORCED:
	case NET_MSG_GCTRL_REQ_TAKE_FB:

	case NET_MSG_GCTRL_PARTY_LURE_TAR:
	case NET_MSG_GCTRL_TRADE_TAR:
	case NET_MSG_GCTRL_PASSENGER_TAR:

	case NET_MSG_GCTRL_CONFRONT_TAR:

	case NET_MSG_EVENT_LOTTERY_FB:

	case NET_MSG_APEX_ANSWER:

		{
			m_GameStage.MsgProcess ( nmg );
		}
		break;

	case NET_MSG_GCTRL_PERIOD:

	case NET_MSG_GCTRL_DROP_ITEM:
	case NET_MSG_GCTRL_DROP_MONEY:

	case NET_MSG_GCTRL_DROP_PC:
	case NET_MSG_GCTRL_DROP_CROW:
	case NET_MSG_GCTRL_DROP_MATERIAL:
	
	case NET_MSG_GCTRL_DROP_OUT:

	case NET_MSG_GCTRL_REQ_GATEOUT_FB:
	case NET_MSG_GCTRL_CREATE_INSTANT_MAP_FB:


	case NET_MSG_GCTRL_ACTION_BRD:
	case NET_MSG_GCTRL_MOVESTATE_BRD:
	case NET_MSG_GCTRL_JUMP_POS_BRD:

	case NET_MSG_GCTRL_GOTO_BRD:
	case NET_MSG_GCTRL_ATTACK_BRD:
	case NET_MSG_GCTRL_ATTACK_CANCEL_BRD:
	case NET_MSG_GCTRL_ATTACK_AVOID:
	case NET_MSG_GCTRL_ATTACK_AVOID_BRD:
	case NET_MSG_GCTRL_ATTACK_DAMAGE:
	case NET_MSG_GCTRL_ATTACK_DAMAGE_BRD:
	case NET_MSG_GCTRL_DEFENSE_SKILL_ACTIVE:

	case NET_MSG_GCTRL_SUMMON_ATTACK_AVOID:
	case NET_MSG_GCTRL_SUMMON_ATTACK_DAMAGE:

	case NET_MSG_GCTRL_UPDATE_STATE:
	case NET_MSG_GCTRL_UPDATE_EXP:
	case NET_MSG_GCTRL_UPDATE_MONEY:
	case NET_MSG_GCTRL_UPDATE_SP:
	case NET_MSG_GCTRL_UPDATE_SKP:
	case NET_MSG_GCTRL_UPDATE_BRIGHT:
	case NET_MSG_GCTRL_UPDATE_BRIGHT_BRD:
	case NET_MSG_GCTRL_UPDATE_STATS:

	case NET_MSG_GCTRL_PUSHPULL_BRD:
	case NET_MSG_GCTRL_UPDATE_PASSIVE_BRD:
	case NET_MSG_GCTRL_POSITIONCHK_BRD:
	case NET_MSG_GCTRL_PASSENGER_FB:
	case NET_MSG_GCTRL_PASSENGER_BRD:
	case NET_MSG_GCTRL_PASSENGER_CANCEL_TAR:

	case NET_MSG_GCTRL_PICKUP_MONEY:
	case NET_MSG_GCTRL_PICKUP_ITEM:

	case NET_MSG_GCTRL_REQ_HOLD_FB:

	case NET_MSG_GCTRL_INVEN_INSERT:
 	//itemmall
	case NET_MSG_GCTRL_BUY_ITEMSHOP_ITEM:
	case NET_MSG_GCTRL_BUY_ITEMSHOP:
	case NET_MSG_GCTRL_INVEN_DELETE:
	case NET_MSG_GCTRL_INVEN_DEL_INSERT:
	case NET_MSG_GCTRL_REQ_VNINVEN_TO_INVEN_FB:

	case NET_MSG_GCTRL_ITEM_COOLTIME_UPDATE:
	case NET_MSG_GCTRL_ITEM_COOLTIME_ERROR:

	case NET_MSG_GCTRL_PUTON_RELEASE:
	case NET_MSG_GCTRL_PUTON_RELEASE_BRD:
	case NET_MSG_GCTRL_PUTON_UPDATE:
	case NET_MSG_GCTRL_PUTON_UPDATE_BRD:
	case NET_MSG_GCTRL_PUTON_CHANGE:
	case NET_MSG_GCTRL_PUTON_CHANGE_BRD:

	case NET_MSG_GCTRL_REQ_SKILLQ_FB:
	case NET_MSG_GCTRL_REQ_ACTIONQ_FB:

	case NET_MSG_GCTRL_REQ_REBIRTH_FB:
	case NET_MSG_GCTRL_REQ_LEVELUP_FB:
	case NET_MSG_GCTRL_REQ_LEVELUP_BRD:
	case NET_MSG_GCTRL_REQ_STATSUP_FB:
	case NET_MSG_GCTRL_REQ_STATSUPCMD_FB:	//add addstats cmd
	case NET_MSG_GCTRL_REQ_LEARNSKILL_FB:
	case NET_MSG_GCTRL_REQ_SKILLUP_FB:

	case NET_MSG_GCTRL_REQ_SKILL_FB:
	case NET_MSG_REQ_SKILL_REVIVEL_FAILED:
	case NET_MSG_GCTRL_SKILLCONSUME_FB:

	case NET_QBOX_OPTION_MEMBER:

	case NET_MSG_GCTRL_SKILLFACT_BRD:
	case NET_MSG_GCTRL_SKILLHOLD_BRD:
	case NET_MSG_GCTRL_STATEBLOW_BRD:
	case NET_MSG_GCTRL_CURESTATEBLOW_BRD:
	case NET_MSG_GCTRL_LANDEFFECT:

	case NET_MSG_GCTRL_REQ_SKILL_BRD:
	case NET_MSG_GCTRL_SKILL_CANCEL_BRD:

	case NET_MSG_GCTRL_SKILLHOLD_RS_BRD:
	case NET_MSG_GCTRL_SKILLHOLDEX_BRD:

	case NET_MSG_GCTRL_INVEN_DRUG_UPDATE:
	case NET_MSG_GCTRL_PUTON_DRUG_UPDATE:

	case NET_MSG_GCTRL_REQ_GETSTORAGE_FB:
	case NET_MSG_GCTRL_REQ_GETSTORAGE_ITEM:

	case NET_MSG_GCTRL_STORAGE_INSERT:
	case NET_MSG_GCTRL_STORAGE_DELETE:
	case NET_MSG_GCTRL_STORAGE_ITEM_UPDATE:

	case NET_MSG_GCTRL_STORAGE_DEL_INSERT:
	case NET_MSG_GCTRL_STORAGE_DRUG_UPDATE:

	case NET_MSG_GCTRL_STORAGE_UPDATE_MONEY:

	case NET_MSG_GCTRL_TRADE_FB:
	case NET_MSG_GCTRL_TRADE_AGREE_TAR:
	case NET_MSG_GCTRL_TRADE_LOCK_TAR: /*trade lock, Juver, 2018/01/02 */
	case NET_MSG_GCTRL_TRADE_ITEM_REGIST_TAR:
	case NET_MSG_GCTRL_TRADE_ITEM_REMOVE_TAR:
	case NET_MSG_GCTRL_TRADE_MONEY_TAR:
	case NET_MSG_GCTRL_TRADE_COMPLETE_TAR:
	case NET_MSG_GCTRL_TRADE_CANCEL_TAR:

	case NET_MSG_GCTRL_INVEN_ITEM_UPDATE:
	case NET_MSG_GCTRL_INVEN_GRINDING_FB:
	case NET_MSG_GCTRL_INVEN_BOXOPEN_FB:
	case NET_MSG_GCTRL_INVEN_DISGUISE_FB:
	case NET_MSG_GCTRL_INVEN_CLEANSER_FB:
	case NET_MSG_GCTRL_INVEN_DEL_ITEM_TIMELMT:
	case NET_MSG_CHAT_LOUDSPEAKER_FB:

	case NET_MSG_GCTRL_MAPWEATHER:

	case NET_MSG_GCTRL_WEATHER:
	case NET_MSG_GCTRL_WHIMSICAL:
	case NET_MSG_GCTRL_MAPWHIMSICAL:

	case NET_MSG_GCTRL_CROW_MOVETO:
	case NET_MSG_GCTRL_CROW_ATTACK:
	case NET_MSG_GCTRL_CROW_AVOID:
	case NET_MSG_GCTRL_CROW_DAMAGE:

	case NET_MSG_GCTRL_CROW_SKILL:

		/*pvp tyranny, Juver, 2017/08/24 */
	case NET_MSG_GCTRL_TYRANNY_CROW_OWNER:
	case NET_MSG_GCTRL_TYRANNY_CROW_DAMAGE:

		/*pvp capture the flag, Juver, 2018/02/06 */
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_CROW_HIDE:

	case NET_MSG_GCTRL_REGEN_GATE_FB:
	case NET_MSG_GCTRL_ETNRY_FAILED:
	case NET_MSG_GCTRL_LIMITTIME_OVER:

	case NET_MSG_GCTRL_CHARRESET_FB:

	case NET_MSG_GCTRL_CONFRONT_FB:
	case NET_MSG_GCTRL_CONFRONT_START2_CLT:
	case NET_MSG_GCTRL_CONFRONT_FIGHT2_CLT:
	case NET_MSG_GCTRL_CONFRONT_END2_CLT:
	case NET_MSG_GCTRL_CONFRONT_END2_CLT_BRD:
	case NET_MSG_GCTRL_CONFRONT_END2_CLT_MBR:

	case NET_MSG_GCTRL_CONFRONTPTY_START2_CLT:
	case NET_MSG_GCTRL_CONFRONTPTY_END2_CLT:

	case NET_MSG_GCTRL_CONFRONTCLB_START2_CLT:
	case NET_MSG_GCTRL_CONFRONTCLB_END2_CLT:

	case NET_MSG_GCTRL_CONFRONT_RECOVE:

	case NET_MSG_GCTRL_UPDATE_LP:

	case NET_MSG_GCTRL_UPDATE_STATE_BRD:
	case NET_MSG_GCTRL_UPDATE_FLAGS:

	case NET_MSG_GM_MOVE2GATE_FB:
	case NET_MSG_GCTRL_PARTY_BRD:
	case NET_MSG_GCTRL_2_FRIEND_FB:
	case NET_MSG_GM_SHOP_INFO_FB:

	case NET_MSG_GCTRL_CURE_FB:
	case NET_MSG_GCTRL_INVEN_CHARCARD_FB:
	case NET_MSG_GCTRL_INVEN_STORAGECARD_FB:
	case NET_MSG_GCTRL_INVEN_INVENLINE_FB:
	case NET_MSG_GCTRL_INVEN_STORAGEOPEN_FB:
	case NET_MSG_GCTRL_INVEN_REMODELOPEN_FB:
	case NET_MSG_GCTRL_INVEN_GARBAGEOPEN_FB:
	case NET_MSG_GCTRL_GARBAGE_RESULT_FB:
	case NET_MSG_GCTRL_INVEN_PREMIUMSET_FB:
	case NET_MSG_GCTRL_PREMIUM_STATE:
	case NET_MSG_GCTRL_INVEN_RESET_SKST_FB:
	case NET_MSG_GCTRL_STORAGE_STATE:
	case NET_MSG_GCTRL_INVEN_RANDOMBOXOPEN_FB:
	case NET_MSG_GCTRL_INVEN_GMITEM_FB: //add itemcmd
	case NET_MSG_GCTRL_INVEN_DISJUNCTION_FB:

	case NET_MSG_GCTRL_INVEN_HAIR_CHANGE_FB:
	case NET_MSG_GCTRL_INVEN_FACE_CHANGE_FB:
	case NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_FB:
	case NET_MSG_GCTRL_INVEN_GENDER_CHANGE_FB:
	case NET_MSG_GCTRL_INVEN_RENAME_FB:

	case NET_MSG_GCTRL_INVEN_HAIR_CHANGE_BRD:
	case NET_MSG_GCTRL_INVEN_FACE_CHANGE_BRD:
	case NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_BRD:
	case NET_MSG_GCTRL_INVEN_RENAME_BRD:
	case NET_MSG_GCTRL_CHANGE_NAMEMAP:

	case NET_MSG_GCTRL_NPC_ITEM_TRADE_FB:

	case NET_MSG_GCTRL_REQ_RECALL_FB:
	case NET_MSG_GCTRL_REQ_BUS_FB:
	case NET_MSG_GCTRL_REQ_TAXI_FB:
	case NET_MSG_GCTRL_REQ_TELEPORT_FB:

	case NET_MSG_REQ_MUST_LEAVE_MAP_FB:

	case NET_MSG_GCTRL_FIRECRACKER_FB:
	case NET_MSG_GCTRL_FIRECRACKER_BRD:
	case NET_MSG_GCTRL_CHARGED_ITEM_GET_FB:
	case NET_MSG_GCTRL_CHARGED_ITEM_DEL:
	case NET_MSG_GCTRL_REVIVE_FB:
	case NET_MSG_GCTRL_GETEXP_RECOVERY_FB:
	case NET_MSG_GCTRL_RECOVERY_FB:
	case NET_MSG_GCTRL_GETEXP_RECOVERY_NPC_FB:
	case NET_MSG_GCTRL_RECOVERY_NPC_FB:
	case NET_MSG_GCTRL_GET_CHARGEDITEM_FROMDB_FB:
	case NET_MSG_GCTRL_GET_ITEMSHOP_FROMDB_FB: //itemmall

	case NET_MSG_GCTRL_REQ_GESTURE_BRD:

	case NET_MSG_GCTRL_REQ_QUEST_START_FB:

	case NET_MSG_GCTRL_QUEST_PROG_STREAM:
	case NET_MSG_GCTRL_QUEST_PROG_STEP_STREAM:
	case NET_MSG_GCTRL_QUEST_PROG_INVEN:
	case NET_MSG_GCTRL_QUEST_PROG_DEL:

	case NET_MSG_GCTRL_QUEST_END_STREAM:
	case NET_MSG_GCTRL_QUEST_END_DEL:
	
	case NET_MSG_GCTRL_QUEST_PROG_NPCTALK_FB:
	case NET_MSG_GCTRL_QUEST_PROG_MOBKILL:
	case NET_MSG_GCTRL_QUEST_PARTY_PROG_MOBKILL:
	case NET_MSG_GCTRL_QUEST_PROG_QITEM:
	case NET_MSG_GCTRL_QUEST_PARTY_PROG_QITEM:
	case NET_MSG_GCTRL_QUEST_PROG_REACHZONE:
	case NET_MSG_GCTRL_QUEST_PROG_TIMEOVER:
	case NET_MSG_GCTRL_QUEST_PROG_TIME:
	case NET_MSG_GCTRL_QUEST_PROG_NONDIE:
	case NET_MSG_GCTRL_QUEST_PROG_LEAVEMAP:

	case NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT:
	case NET_MSG_GCTRL_QUEST_PROG_INVEN_DELETE:
	case NET_MSG_GCTRL_QUEST_PROG_INVEN_TURN:
	case NET_MSG_GCTRL_QUEST_PROG_INVEN_PICKUP:
	case NET_MSG_GCTRL_QUEST_COMPLETE_FB:

	case NET_MSG_GM_EVENT_EXP_FB:
	case NET_MSG_GM_EVENT_EXP_END_FB:
	case NET_MSG_GM_EVENT_ITEM_GEN_FB:
	case NET_MSG_GM_EVENT_ITEM_GEN_END_FB:
	case NET_MSG_GM_EVENT_MONEY_GEN_FB:
	case NET_MSG_GM_EVENT_MONEY_GEN_END_FB:

	case NET_MSG_CYBERCAFECLASS_UPDATE:

	case NET_MSG_GM_BIGHEAD_BRD:
	case NET_MSG_GM_BIGHAND_BRD:
	case NET_MSG_GM_CHAT_BLOCK_FB:
	case NET_MSG_GM_CHAR_INFO_AGT_FB:
	case NET_MSG_GM_CHAR_INFO_FLD_FB:

	case NET_MSG_USER_CHAR_INFO_AGT_FB:
	case NET_MSG_USER_CHAR_INFO_FLD_FB:

	case NET_MSG_GM_FREEPK_BRD:
	case NET_MSG_GM_VIEWALLPLAYER_FLD_FB:
	case NET_MSG_GM_SHOWMETHEMONEY:
	case NET_MSG_GM_SET_PRIVATE_MARKET_BRD: /*private market set, Juver, 2018/01/02 */
	case NET_MSG_GM_SET_MEGAPHONE_BRD:		/*megaphone set, Juver, 2018/01/02 */

	case NET_MSG_GM_VIEWWORKEVENT_FB:

	case NET_MSG_GCTRL_PMARKET_TITLE_FB:
	case NET_MSG_GCTRL_PMARKET_REGITEM_FB:
	case NET_MSG_GCTRL_PMARKET_DISITEM_FB:
	case NET_MSG_GCTRL_PMARKET_OPEN_FB:
	case NET_MSG_GCTRL_PMARKET_OPEN_BRD:
	case NET_MSG_GCTRL_PMARKET_CLOSE_BRD:
	case NET_MSG_GCTRL_PMARKET_ITEM_INFO_BRD:
	case NET_MSG_GCTRL_PMARKET_BUY_FB:
	case NET_MSG_GCTRL_PMARKET_ITEM_UPDATE_BRD:
	case NET_MSG_GCTRL_PMARKET_SEARCH_ITEM_RESULT:

	case NET_MSG_GCTRL_CLUB_INFO_2CLT:
	case NET_MSG_GCTRL_CLUB_DEL_2CLT:
	case NET_MSG_GCTRL_CLUB_INFO_DISSOLUTION:
	case NET_MSG_GCTRL_CLUB_INFO_BRD:
	case NET_MSG_GCTRL_CLUB_INFO_NICK_BRD:
	case NET_MSG_GCTRL_CLUB_DEL_BRD:
	case NET_MSG_GCTRL_CLUB_INFO_MARK_BRD:
	case NET_MSG_GCTRL_CLUB_MEMBER_2CLT:
	case NET_MSG_GCTRL_CLUB_NEW_FB:
	case NET_MSG_GCTRL_CLUB_DISSOLUTION_FB:
	case NET_MSG_GCTRL_CLUB_MEMBER_REQ_ASK:
	case NET_MSG_GCTRL_CLUB_MEMBER_REQ_FB:
	case NET_MSG_GCTRL_CLUB_MEMBER_DEL_2CLT:
	case NET_MSG_GCTRL_CLUB_MEMBER_DEL_FB:
	case NET_MSG_GCTRL_CLUB_MEMBER_SECEDE_FB:
	case NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_ASK:
	case NET_MSG_GCTRL_CLUB_AUTHORITY_REQ_FB:
	case NET_MSG_GCTRL_CLUB_AUTHORITY_CLT:
	case NET_MSG_GCTRL_CLUB_AUTHORITY_BRD:

	case NET_MSG_GCTRL_CLUB_MARK_CHANGE_2CLT:
	case NET_MSG_GCTRL_CLUB_MARK_INFO_FB:
	case NET_MSG_GCTRL_CLUB_RANK_2CLT:
	case NET_MSG_GCTRL_CLUB_RANK_FB:
	case NET_MSG_GCTRL_CLUB_MEMBER_NICK_FB:
	case NET_MSG_GCTRL_CLUB_MEMBER_STATE:
	case NET_MSG_GCTRL_CLUB_MEMBER_POS:

	case NET_MSG_GCTRL_CLUB_NOTICE_FB:
	case NET_MSG_GCTRL_CLUB_NOTICE_CLT:
	case NET_MSG_GCTRL_CLUB_SUBMASTER_FB:
	case NET_MSG_GCTRL_CLUB_SUBMASTER_BRD:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_ADD_CLT:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_CLT:
	case NET_MSG_GCTRL_CLUB_MBR_RENAME_CLT:

	case NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_ASK:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_CLT:

	case NET_MSG_GCTRL_CLUB_ALLIANCE_REQ_FB:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_DEL_FB:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_SEC_FB:
	case NET_MSG_GCTRL_CLUB_ALLIANCE_DIS_FB:

	case NET_MSG_GCTRL_CLUB_BATTLE_REQ_FB:
	case NET_MSG_GCTRL_CLUB_BATTLE_REQ_ASK:
	case NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT:
	case NET_MSG_GCTRL_CLUB_BATTLE_BEGIN_CLT2:
	case NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_FB:
	case NET_MSG_GCTRL_CLUB_BATTLE_ARMISTICE_REQ_ASK:
	case NET_MSG_GCTRL_CLUB_BATTLE_OVER_CLT:
	case NET_MSG_GCTRL_CLUB_BATTLE_SUBMISSION_REQ_FB:
	case NET_MSG_GCTRL_CLUB_BATTLE_KILL_UPDATE:
	case NET_MSG_GCTRL_CLUB_BATTLE_POINT_UPDATE:

	case NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_FB:
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_REQ_ASK:
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_FB:
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_ARMISTICE_REQ_ASK:
	case NET_MSG_GCTRL_ALLIANCE_BATTLE_SUBMISSION_REQ_FB:

	case NET_MSG_GCTRL_PLAYERKILLING_ADD:
	case NET_MSG_GCTRL_PLAYERKILLING_DEL:

	case NET_MSG_GCTRL_QITEMFACT_BRD:
	case NET_MSG_GCTRL_QITEMFACT_END_BRD:
	case NET_MSG_GCTRL_PKCOMBO_BRD:
	case NET_MSG_GCTRL_PKCOMBO_END_BRD:
	case NET_MSG_GCTRL_EVENTFACT_BRD:
	case NET_MSG_GCTRL_EVENTFACT_END_BRD:
	case NET_MSG_GCTRL_EVENTFACT_INFO:

	case NET_MSG_GCTRL_CLUB_BATTLE_START_BRD:
	case NET_MSG_GCTRL_CLUB_BATTLE_END_BRD:
	case NET_MSG_GCTRL_CLUB_BATTLE_REMAIN_BRD:

	case NET_MSG_GCTRL_CLUB_DEATHMATCH_START_BRD:
	case NET_MSG_GCTRL_CLUB_DEATHMATCH_END_BRD:
	case NET_MSG_GCTRL_CLUB_DEATHMATCH_REMAIN_BRD:
	case NET_MSG_GCTRL_CLUB_DEATHMATCH_POINT_UPDATE:
	case NET_MSG_GCTRL_CLUB_DEATHMATCH_MYRANK_UPDATE:
	case NET_MSG_GCTRL_CLUB_DEATHMATCH_RANKING_UPDATE:

	case NET_MSG_GCTRL_CLUB_CD_CERTIFY_FB:
	case NET_MSG_GCTRL_CLUB_CD_CERTIFY_BRD:
	case NET_MSG_GCTRL_CLUB_CD_CERTIFY_ING_BRD:
	case NET_MSG_GCTRL_CLUB_COMMISSION_FB:
	case NET_MSG_GCTRL_CLUB_COMMISSION_BRD:
	case NET_MSG_GCTRL_CLUB_COMMISSION_RV_BRD:

	case NET_MSG_GCTRL_CLUB_STORAGE_RESET:
	case NET_MSG_GCTRL_CLUB_GETSTORAGE_ITEM:
	case NET_MSG_GCTRL_CLUB_STORAGE_INSERT:
	case NET_MSG_GCTRL_CLUB_STORAGE_DELETE:
	case NET_MSG_GCTRL_CLUB_STORAGE_DEL_INS:
	case NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_ITEM:
	case NET_MSG_GCTRL_CLUB_STORAGE_UPDATE_MONEY:

	case NET_MSG_GCTRL_LAND_INFO:
	case NET_MSG_GCTRL_SERVER_INFO:
	case NET_MSG_GCTRL_SERVER_BRIGHTEVENT_INFO:
	case NET_MSG_GCTRL_SERVER_CLUB_BATTLE_INFO:
	case NET_MSG_GCTRL_SERVER_CLUB_DEATHMATCH_INFO:

	case NET_MSG_GM_WHERE_NPC_FB:
	case NET_MSG_GM_WHERE_PC_MAP_FB:
	case NET_MSG_GM_WHERE_PC_POS_FB:
	case NET_MSG_GM_PRINT_CROWLIST_FB:

	case NET_MSG_GM_WARNING_MSG_BRD:
	case NET_MSG_GM_COUNTDOWN_MSG_BRD:
	case NET_MSG_GM_MOVE2CHAR_FB:
	case NET_MSG_GCTRL_SERVERTEST_FB:

	case NET_MSG_REBUILD_RESULT:	// ITEMREBUILD_MARK
	case NET_MSG_REBUILD_MOVE_SEAL:
	case NET_MSG_REBUILD_MOVE_ITEM:
	case NET_MSG_REBUILD_COST_MONEY:
	case NET_MSG_REBUILD_INPUT_MONEY:

	case NET_MSG_GCTRL_UPDATE_LASTCALL:
	case NET_MSG_GCTRL_UPDATE_STARTCALL:

	// -------- Messagese about PET (START) -------- //
	case NET_MSG_PET_REQ_USECARD_FB:
	case NET_MSG_CREATE_ANYPET:
	case NET_MSG_PET_REQ_UNUSECARD_FB:
	case NET_MSG_PET_REQ_UPDATE_MOVE_STATE_FB:
	case NET_MSG_PET_DROPPET:
	case NET_MSG_PET_GOTO_BRD:
	case NET_MSG_PET_PETSKINPACKOPEN_BRD:
	case NET_MSG_PET_REQ_UPDATE_MOVE_STATE_BRD:
	case NET_MSG_PET_STOP_BRD:

		/*dual pet skill, Juver, 2017/12/28 */
	case NET_MSG_PET_GETRIGHTOFITEM_A_FB:
	case NET_MSG_PET_GETRIGHTOFITEM_B_FB:

	case NET_MSG_PET_REQ_RENAME_FB:
	case NET_MSG_PET_REQ_RENAME_BRD:
	case NET_MSG_PET_REQ_CHANGE_COLOR_FB:
	case NET_MSG_PET_REQ_CHANGE_COLOR_BRD:
	case NET_MSG_PET_REQ_CHANGE_STYLE_FB:
	case NET_MSG_PET_REQ_CHANGE_STYLE_BRD:
	case NET_MSG_PET_UPDATECLIENT_FULL:
	case NET_MSG_PET_REQ_SLOT_EX_HOLD_FB:
	case NET_MSG_PET_REQ_HOLD_TO_SLOT_FB:
	case NET_MSG_PET_REQ_SLOT_TO_HOLD_FB:
	case NET_MSG_PET_REQ_SLOT_EX_HOLD_BRD:

		/*dual pet skill, Juver, 2017/12/27 */
	case NET_MSG_PET_REQ_SKILLCHANGE_A_FB:
	case NET_MSG_PET_REQ_SKILLCHANGE_A_BRD:
	case NET_MSG_PET_ADD_SKILLFACT_A:
	case NET_MSG_PET_REMOVE_SKILLFACT_A:
	case NET_MSG_PET_REQ_SKILLCHANGE_B_FB:
	case NET_MSG_PET_REQ_SKILLCHANGE_B_BRD:
	case NET_MSG_PET_ADD_SKILLFACT_B:
	case NET_MSG_PET_REMOVE_SKILLFACT_B:

	case NET_MSG_PET_REQ_LEARNSKILL_FB:
	case NET_MSG_PET_REQ_LEARNSKILL_BRD:
	case NET_MSG_PET_REQ_FUNNY_BRD:
	case NET_MSG_PET_REMOVE_SLOTITEM_FB:
	case NET_MSG_PET_REMOVE_SLOTITEM_BRD:
	case NET_MSG_PET_ATTACK:
	case NET_MSG_PET_ATTACK_BRD:
	case NET_MSG_PET_SAD:
	case NET_MSG_PET_SAD_BRD:
	case NET_MSG_PET_REQ_GIVEFOOD_FB:
	case NET_MSG_PET_REQ_PETCARDINFO_FB:
	case NET_MSG_PET_REQ_PETREVIVEINFO_FB:
	case NET_MSG_PET_NOTENOUGHINVEN:
	case NET_MSG_PET_REQ_REVIVE_FB:
	case NET_MSG_PET_PETSKINPACKOPEN_FB:

		/*dual pet skill, Juver, 2017/12/29 */
	case NET_MSG_PET_REQ_ENABLE_DUAL_SKILL_FB:

	// -------- Messagese about PET (END) -------- //

	// -------- Messagese about SUMMON (START) -------- //
	case NET_MSG_REQ_USE_SUMMON_FB:
	case NET_MSG_REQ_USE_SUMMON_DEL:
	case NET_MSG_DROP_ANYSUMMON:
	case NET_MSG_CREATE_ANYSUMMON:
	case NET_MSG_SUMMON_ATTACK:

	case NET_MSG_SUMMON_GOTO_BRD:
	case NET_MSG_SUMMON_ATTACK_BRD:
	case NET_MSG_SUMMON_REQ_STOP_BRD:
	case NET_MSG_SUMMON_REQ_UPDATE_MOVE_STATE_BRD:

		/*skill summon, Juver, 2017/10/09 */
	case NET_MSG_SUMMON_END_REST:
	case NET_MSG_SUMMON_END_REST_BRD:
	case NET_MSG_SUMMON_END_LIFE:
	case NET_MSG_SUMMON_END_LIFE_BRD:
	case NET_MSG_SUMMON_RESET_TARGET:
	case NET_MSG_SUMMON_RESET_TARGET_BRD:

	// -------- Messagese about SUMMON (END) -------- //

	case NET_MSG_SMS_PHONE_NUMBER_FB:
	case NET_MSG_SMS_SEND_FB:

	case NET_MSG_MGAME_ODDEVEN_FB:	// 미니게임 - 홀짝
	case NET_MSG_SERVERTIME_BRD:

	case NET_MSG_CHINA_GAINTYPE:
	case NET_MSG_VIETNAM_GAINTYPE:
	case NET_MSG_VIETNAM_TIME_REQ_FB:
	case NET_MSG_VIETNAM_GAIN_EXP:
	case NET_MSG_VIETNAM_GAIN_MONEY:
	case NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGETNUM_UPDATE:
	case NET_MSG_GCTRL_INVEN_VIETNAM_EXPGET_FB:
	case NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGET_FB:
	case NET_MSG_VIETNAM_ALLINITTIME:

	case NET_MSG_GM_LIMIT_EVENT_APPLY_START:
	case NET_MSG_GM_LIMIT_EVENT_APPLY_END:

	case NET_MSG_GM_LIMIT_EVENT_BEGIN_FB:
	case NET_MSG_GM_LIMIT_EVENT_END_FB:
	case NET_MSG_GM_LIMIT_EVENT_TIME_REQ_FB:


	case NET_MSG_GCTRL_ACTIVE_VEHICLE_FB:
	case NET_MSG_GCTRL_ACTIVE_VEHICLE_BRD:
	case NET_MSG_GCTRL_GET_VEHICLE_FB:
	case NET_MSG_GCTRL_GET_VEHICLE_BRD:
	case NET_MSG_GCTRL_UNGET_VEHICLE_FB:
	case NET_MSG_GCTRL_UNGET_VEHICLE_BRD:
	case NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_FB:
	case NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_BRD:
	case NET_MSG_VEHICLE_REQ_HOLD_TO_SLOT_FB:
	case NET_MSG_VEHICLE_REQ_SLOT_TO_HOLD_FB:
	case NET_MSG_VEHICLE_REMOVE_SLOTITEM_FB:
	case NET_MSG_VEHICLE_REMOVE_SLOTITEM_BRD:
	case NET_MSG_VEHICLE_ACCESSORY_DELETE:
	case NET_MSG_VEHICLE_ACCESSORY_DELETE_BRD:
	case NET_MSG_VEHICLE_UPDATE_CLIENT_BATTERY:
	case NET_MSG_VEHICLE_REQ_GIVE_BATTERY_FB:
	case NET_MSG_VEHICLE_REQ_ITEM_INFO_FB:
	case NET_MSG_GCTRL_ITEMSHOPOPEN_BRD:
	case NET_MSG_UPDATE_TRACING_CHARACTER:
	case NET_MSG_REQ_ATTENDLIST_FB:
	case NET_MSG_REQ_ATTENDANCE_FB:
	case NET_MSG_REQ_ATTEND_REWARD_CLT:
	case NET_MSG_GCTRL_NPC_RECALL_FB:
	case NET_MSG_GCTRL_INVEN_ITEM_MIX_FB:
	case NET_MSG_REQ_GATHERING_FB:
	case NET_MSG_REQ_GATHERING_BRD:
	case NET_MSG_REQ_GATHERING_RESULT:
	case NET_MSG_REQ_GATHERING_RESULT_BRD:
	case NET_MSG_REQ_GATHERING_CANCEL_BRD:
	case NET_MSG_DROPCHAR_TOCHEATER:
	case NET_MSG_PK_RANK_HISTORY_UPDATE:
	case NET_MSG_GCTRL_DEATHTIMER_FB_AG:
	case NET_MSG_GCTRL_DEATHTIMER_FB_FLD:
	case NET_MSG_GCTRL_REQ_AUTOSYSTEM_FB:
	//dmk14 | 11-4-16 | pk ranking
	case NET_MSG_GCTRL_REQ_PLAYERRANKING_UPDATE:
	case NET_MSG_GCTRL_REQ_PLAYERRANKING_UPDATE_EX:

	//itemmall
	case NET_MSG_RETRIEVE_POINTS_FB:
		{
			GLGaeaClient::GetInstance().MsgProcess ( nmg );
		}
		break;

	/*get process command, Juver, 2017/06/08 */
	case NET_MSG_GM_CHAR_GETPROC_AGT_FB: 
	case NET_MSG_GM_CHAR_GETPROC_AGT_START:
	case NET_MSG_GM_CHAR_GETPROC_UPDATE:
	case NET_MSG_GM_CHAR_GETPROC_UPDATE_COMPLETE:
		/*itemfood system, Juver, 2017/05/25 */
	case NET_MSG_GCTRL_INVEN_CONSUME_FOOD_FB:  
	case NET_MSG_GCTRL_INVEN_UNLOCK_FOOD_FB:
	case NET_MSG_GCTRL_FITEMFACT_BRD:

		/*combatpoint logic, Juver, 2017/05/28 */
	case NET_MSG_GCTRL_UPDATE_CP:	

		/*game stats, Juver, 2017/06/21 */
	case NET_MSG_GCTRL_PING_PACKET_FB: 

		/*npc shop, Juver, 2017/07/27 */
	case NET_MSG_GCTRL_NPCSHOP_PURCHASE_MONEY_FB: 

		/*vehicle booster system, Juver, 2017/08/10 */
	case NET_MSG_VEHICLE_REQ_ENABLE_BOOSTER_FB: 
	case NET_MSG_GCTRL_VEHICLE_BOOSTER_STATE_CHARGE:
	case NET_MSG_GCTRL_VEHICLE_BOOSTER_STATE_START:
	case NET_MSG_GCTRL_VEHICLE_BOOSTER_STATE_RESET:
	case NET_MSG_GCTRL_VEHICLE_BOOSTER_STATE_START_BRD:
	case NET_MSG_GCTRL_VEHICLE_BOOSTER_STATE_RESET_BRD:

	case NET_MSG_GCTRL_REQ_LEARNSKILL_NONINVEN_FB: 
	case NET_MSG_GCTRL_UPDATE_MONEYTYPE:
	
		/*contribution point, Juver, 2017/08/23 */
	case NET_MSG_GCTRL_UPDATE_CONTRIBUTION_POINT: 

		/*activity point, Juver, 2017/08/23 */
	case NET_MSG_GCTRL_UPDATE_ACTIVITY_POINT:

		/*event map move, Juver, 2017/08/25 */
	case NET_MSG_GCTRL_REQ_EVENT_MOVEMAP_FB: 

		/*system buffs, Juver, 2017/09/05 */
	case NET_MSG_GCTRL_SYSTEM_BUFF_BRD:		

		/*item exchange, Juver, 2017/10/13 */
	case NET_MSG_GCTRL_NPC_ITEM_EXCHANGE_TRADE_FB:

		/*product item, Juver, 2017/10/18 */
	case NET_MSG_GCTRL_ITEM_COMPOUND_START_FB:
	case NET_MSG_GCTRL_ITEM_COMPOUND_PROCESS_FB:

		/*activity system, Juver, 2017/10/30 */
	case NET_MSG_GCTRL_ACTIVITY_COMPLETE_BRD:
	case NET_MSG_GCTRL_ACTIVITY_UPDATE:
	case NET_MSG_GCTRL_ACTIVITY_COMPLETE:
	case NET_MSG_GCTRL_ACTIVITY_NOTIFY_CLIENT:

		/*activity system, Juver, 2017/11/05 */
	case NET_MSG_GCTRL_CHARACTER_BADGE_CHANGE_FB:
	case NET_MSG_GCTRL_CHARACTER_BADGE_CHANGE_BRD:

		/*charinfoview , Juver, 2017/11/11 */
	case NET_MSG_GCTRL_REQ_CHARINFO_FB:

		/*bike color , Juver, 2017/11/13 */
	case NET_MSG_VEHICLE_REQ_CHANGE_COLOR_FB:
	case NET_MSG_VEHICLE_REQ_CHANGE_COLOR_BRD:

		/*pk info, Juver, 2017/11/17 */
	case NET_MSG_GCTRL_UPDATE_PK_SCORE:
	case NET_MSG_GCTRL_UPDATE_PK_DEATH:

		/*rv card, Juver, 2017/11/26 */
	case NET_MSG_GCTRL_INVEN_RANDOM_OPTION_CHANGE_FB:

		/*nondrop card, Juver, 2017/11/26 */
	case NET_MSG_GCTRL_INVEN_NONDROP_CARD_FB:

		/*change scale card, Juver, 2018/01/04 */
	case NET_MSG_GCTRL_INVEN_SCALE_CHANGE_FB:
	case NET_MSG_GCTRL_INVEN_SCALE_CHANGE_BRD:
		
		/*item color, Juver, 2018/01/10 */
	case NET_MSG_GCTRL_INVEN_ITEMCOLOR_CHANGE_FB:
	case NET_MSG_GCTRL_INVEN_ITEMCOLOR_CHANGE_BRD:

		/*item wrapper, Juver, 2018/01/12 */
	case NET_MSG_GCTRL_INVEN_WRAP_FB:
	case NET_MSG_GCTRL_INVEN_UNWRAP_FB:

		/*change school card, Juver, 2018/01/12 */
	case NET_MSG_GCTRL_INVEN_CHANGE_SCHOOL_FB:

		/*equipment lock, Juver, 2018/01/14 */
	case NET_MSG_GCTRL_INVEN_EQUIPMENT_LOCK_ENABLE_FB:
	case NET_MSG_GCTRL_INVEN_EQUIPMENT_LOCK_INPUT_FB:
	case NET_MSG_GCTRL_INVEN_EQUIPMENT_LOCK_RECOVER_FB:
	case NET_MSG_GCTRL_INVEN_EQUIPMENT_LOCK_RECOVER_CHANGE_FB:
	case NET_MSG_GCTRL_INVEN_EQUIPMENT_LOCK_RECOVER_DELETE_FB:

		/*item transfer card, Juver, 2018/01/18 */
	case NET_MSG_GCTRL_INVEN_ITEM_TRANSFER_FB:

		/*pvp capture the flag, Juver, 2018/01/30 */
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_F2C_PLAYER_TEAM_BRD:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_F2C_FLAG_HOLD_BRD:

		/* car, cart color, Juver, 2018/02/14 */
	case NET_MSG_VEHICLE_REQ_CHANGE_CAR_COLOR_FB:
	case NET_MSG_VEHICLE_REQ_CHANGE_CAR_COLOR_BRD:

	case NET_MSG_ALLVEHICLE_REQ_ENABLE_BOOSTER_FB:

		{
			GLGaeaClient::GetInstance().MsgProcess ( nmg );
		}
		break;

	case NET_MSG_GCTRL_PARTY_LURE_FB:
	case NET_MSG_GCTRL_PARTY_FNEW:
	case NET_MSG_GCTRL_PARTY_ADD:
	case NET_MSG_GCTRL_PARTY_DEL:
	case NET_MSG_GCTRL_PARTY_DISSOLVE:
	case NET_MSG_GCTRL_PARTY_AUTHORITY:
	case NET_MSG_GCTRL_PARTY_MBR_MOVEMAP:
	case NET_MSG_GCTRL_PARTY_MBR_DETAIL:
	case NET_MSG_GCTRL_PARTY_MBR_POINT:
	case NET_MSG_GCTRL_PARTY_MBR_POS:

	case NET_MSG_GCTRL_PARTY_MBR_RENAME_CLT:

	case NET_MSG_GCTRL_PARTY_MBR_PICKUP_BRD:
	case NET_MSG_GCTRL_PARTY_MASTER_RENEW:
		{
			GLPartyClient::GetInstance().MsgProcess ( nmg );
		}
		break;

	case NET_MSG_REQ_FRIENDADD_FB:
	case NET_MSG_REQ_FRIENDADD_LURE:
	case NET_MSG_REQ_FRIENDDEL_FB:
	case NET_MSG_REQ_FRIENDBLOCK_FB:
	case NET_MSG_FRIENDINFO:
	case NET_MSG_FRIENDSTATE:
	case NET_MSG_GCTRL_FRIEND_RENAME_CLT:
	case NET_MSG_GCTRL_FRIEND_PHONENUMBER_CLT:
		{
			GLFriendClient::GetInstance().MsgProcess ( nmg );
		}
		break;

	case NET_MSG_GM_KICK_USER_PROC:
		{
			m_NetClient.CloseConnect();
		}
		break;

	case NET_MSG_DROPCHAR_TOCLIENT:
		{
			m_NetClient.CloseConnect();
		}break;

	case NET_MSG_GAMEGUARD_AUTH:
		{
			const NET_GAMEGUARD_AUTH * pNGA;
			pNGA = reinterpret_cast<NET_GAMEGUARD_AUTH*>(nmg);

			P_AUTH_DATA pAD = new AUTH_DATA;
			memmove( &(pAD->gg_Auth_Data), &(pNGA->ggad), sizeof(GG_AUTH_DATA) );
			pAD->Auth_Msg = NET_MSG_GAMEGUARD_AUTH;
			m_GGAuthBuffer.push( pAD );
		}
		break;

	case NET_MSG_GAMEGUARD_AUTH_1:
		{
			const NET_GAMEGUARD_AUTH_1 * pNGA;
			pNGA = reinterpret_cast<NET_GAMEGUARD_AUTH_1*>(nmg);

			P_AUTH_DATA pAD = new AUTH_DATA;
			memmove( &(pAD->gg_Auth_Data), &(pNGA->ggad), sizeof(GG_AUTH_DATA) );
			pAD->Auth_Msg = NET_MSG_GAMEGUARD_AUTH_1;
			m_GGAuthBuffer.push( pAD );
		}
		break;

	case NET_MSG_GAMEGUARD_AUTH_2:
		{
			const NET_GAMEGUARD_AUTH_2 * pNGA;
			pNGA = reinterpret_cast<NET_GAMEGUARD_AUTH_2*>(nmg);

			P_AUTH_DATA pAD = new AUTH_DATA;
			memmove( &(pAD->gg_Auth_Data), &(pNGA->ggad), sizeof(GG_AUTH_DATA) );
			pAD->Auth_Msg = NET_MSG_GAMEGUARD_AUTH_2;
			m_GGAuthBuffer.push( pAD );
		}
		break;

#if defined ( CH_PARAM ) || defined ( HK_PARAM ) || defined ( TW_PARAM )//|| defined ( _RELEASED ) // Apex 적용
	case NET_MSG_APEX_DATA:
		{
			const NET_APEX_DATA * pNAD;
			pNAD = reinterpret_cast<NET_APEX_DATA*> (nmg);
			int nLength = pNAD->nmg.dwSize - sizeof(NET_MSG_GENERIC);
			APEX_CLIENT::NoticeApex_UserData( pNAD->szData, nLength );
		}
		break;
#endif

		/*pvp tyranny, Juver, 2017/08/24 */
	case NET_MSG_GCTRL_TYRANNY_A2FC_STATE_REGISTER:
	case NET_MSG_GCTRL_TYRANNY_A2FC_STATE_BATTLE:
	case NET_MSG_GCTRL_TYRANNY_A2FC_STATE_REWARD:
	case NET_MSG_GCTRL_TYRANNY_A2FC_STATE_ENDED:
	case NET_MSG_GCTRL_TYRANNY_A2C_TOBATTLE_TIME:
	case NET_MSG_GCTRL_TYRANNY_A2FC_NEXTSCHED:
	case NET_MSG_GCTRL_TYRANNY_A2C_BATTLEINFO_PC:
	
	case NET_MSG_GCTRL_TYRANNY_F2C_TOWER_CAPTURE:
	case NET_MSG_GCTRL_TYRANNY_F2C_TOWER_WINNER:
	case NET_MSG_GCTRL_TYRANNY_F2C_RANKING_UPDATE:
	case NET_MSG_GCTRL_TYRANNY_F2C_RANKING_END:
	case NET_MSG_GCTRL_TYRANNY_A2C_RANKINFO_PC:

	case NET_MSG_GCTRL_TYRANNY_A2C_REGISTER_FB:
	case NET_MSG_GCTRL_TYRANNY_A2C_QUEUE_MOVED:
	case NET_MSG_GCTRL_TYRANNY_A2C_QUEUE_UPDATE:
	case NET_MSG_GCTRL_TYRANNY_A2C_REJOIN_FB:
	case NET_MSG_GCTRL_TYRANNY_A2C_TOWER_INFO:
		{
			GLPVPTyrannyClient::GetInstance().MsgProcess( nmg );
		}break;
		
	/*hackshield implementation, Juver, 2018/06/21 */
	case NET_MSG_GCTRL_HS_CLOSE_CLIENT:
		{
			GLMSG::SNETPC_HS_CLOSE_CLIENT* pnetmsg = ( GLMSG::SNETPC_HS_CLOSE_CLIENT* ) nmg;

			GetNetClient()->CloseConnect();
			CloseGame();
		}break;
		/*school wars, Juver, 2018/01/19 */
	case NET_MSG_GCTRL_SCHOOLWARS_A2FC_STATE_REGISTER:
	case NET_MSG_GCTRL_SCHOOLWARS_A2FC_STATE_BATTLE:
	case NET_MSG_GCTRL_SCHOOLWARS_A2FC_STATE_REWARD:
	case NET_MSG_GCTRL_SCHOOLWARS_A2FC_STATE_ENDED:
	case NET_MSG_GCTRL_SCHOOLWARS_A2C_TOBATTLE_TIME:
	case NET_MSG_GCTRL_SCHOOLWARS_A2FC_NEXTSCHED:
	case NET_MSG_GCTRL_SCHOOLWARS_A2C_BATTLEINFO_PC:

	case NET_MSG_GCTRL_SCHOOLWARS_A2C_REGISTER_FB:
	case NET_MSG_GCTRL_SCHOOLWARS_A2C_QUEUE_MOVED:
	case NET_MSG_GCTRL_SCHOOLWARS_A2C_QUEUE_UPDATE:
	case NET_MSG_GCTRL_SCHOOLWARS_A2C_REJOIN_FB:
	case NET_MSG_GCTRL_SCHOOLWARS_A2C_SCORE_INFO:

	case NET_MSG_GCTRL_SCHOOLWARS_F2C_SCORE_UPDATE:
	case NET_MSG_GCTRL_SCHOOLWARS_F2C_SCORE_WINNER:
	case NET_MSG_GCTRL_SCHOOLWARS_F2C_RANKING_UPDATE:
	case NET_MSG_GCTRL_SCHOOLWARS_F2C_RANKING_END:
	case NET_MSG_GCTRL_SCHOOLWARS_A2C_RANKINFO_PC:
	case NET_MSG_GCTRL_SCHOOLWARS_A2C_SCORE_UPDATE:
		{
			GLPVPSchoolWarsClient::GetInstance().MsgProcess( nmg );
		}break;


		/*pvp capture the flag, Juver, 2018/01/24 */
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_A2FC_STATE_REGISTER:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_A2FC_STATE_BATTLE:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_A2FC_STATE_REWARD:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_A2FC_STATE_ENDED:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_A2C_TOBATTLE_TIME:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_A2FC_NEXTSCHED:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_A2C_BATTLEINFO_PC:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_A2C_PLAYER_NUM:

	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_A2C_REGISTER_FB:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_A2C_REJOIN_FB:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_A2C_SCORE_INFO:

	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_F2C_CAPTURE_UPDATE:

	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_F2C_FLAG_HOLD:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_F2C_WINNER:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_F2C_RANKING_UPDATE:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_F2C_RANKING_END:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_A2C_RANKINFO_PC:
	case NET_MSG_GCTRL_CAPTURE_THE_FLAG_F2C_FLAG_POSITION:
		{
			GLPVPCaptureTheFlagClient::GetInstance().MsgProcess( nmg );
		}break;

	default:
		CDebugSet::ToListView ( "DxGlobalStage::MsgProcessFrame() not classified message : TYPE[%d]", nmg->nType );
		break;
	};
}

UINT DxGlobalStage::GetGGAuthData( PGG_AUTH_DATA pGG )
{
	if( m_GGAuthBuffer.empty() ) return 0;

	P_AUTH_DATA pData = m_GGAuthBuffer.front();

	memmove( pGG, &(pData->gg_Auth_Data), sizeof(GG_AUTH_DATA) );
	UINT nRet = pData->Auth_Msg;

	SAFE_DELETE( pData );
	m_GGAuthBuffer.pop();

	return nRet;
}

HRESULT DxGlobalStage::ChangeWndMode ()
{
	if ( RANPARAM::bScrWndFullSize )	ToRestoreSize ();
	else								ToFullSize ();

	return S_OK;
}

HRESULT DxGlobalStage::ToFullSize ()
{
	if ( !RANPARAM::bScrWindowed )	return S_FALSE;

	RANPARAM::bScrWndFullSize = TRUE;
	
	LONG nWindowStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	nWindowStyle = WS_POPUP | WS_VISIBLE;
	SetWindowLong ( m_hWnd, GWL_STYLE, nWindowStyle );

	int nScreenX = (int) GetSystemMetrics(SM_CXSCREEN);
	int nScreenY = (int) GetSystemMetrics(SM_CYSCREEN);

	//	Note : 와이드 스크린을 넘어서는 비율일 경우
	//			2 스크린으로 인식.
	/*float fScreen = nScreenX / float(nScreenY);
	if ( fScreen > (16.0f/9.0f) )
	{
	nScreenX = nScreenX/2;
	}*/

	MoveWindow ( m_hWnd, 0, 0, nScreenX, nScreenY, TRUE );

	return S_OK;
}

HRESULT DxGlobalStage::ToRestoreSize ()
{
	if ( !RANPARAM::bScrWndHalfSize )	return S_FALSE;
	if ( !RANPARAM::bScrWindowed )		return S_FALSE;

	RANPARAM::bScrWndFullSize = FALSE;

	LONG nWindowStyle = WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION |
						WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_CLIPCHILDREN;
	SetWindowLong ( m_hWnd, GWL_STYLE, nWindowStyle );

	int wndSizeX = (int) RANPARAM::dwScrWidth;
	int wndSizeY = (int) RANPARAM::dwScrHeight;

	int nScreenX = (int) GetSystemMetrics(SM_CXSCREEN);
	int nScreenY = (int) GetSystemMetrics(SM_CYSCREEN);

	if ( wndSizeX >= nScreenX || wndSizeY >= nScreenY )
	{
		wndSizeX = 1024;
		wndSizeY = 768;

		if ( wndSizeX >= nScreenX || wndSizeY >= nScreenY )
		{
			wndSizeX = nScreenX-60;
			wndSizeY = nScreenY-80;
		}
	}

	RECT rt = {0, 0, wndSizeX, wndSizeY};
	AdjustWindowRectEx(&rt, nWindowStyle, FALSE, WS_EX_APPWINDOW);

	wndSizeX = rt.right - rt.left;
	wndSizeY = rt.bottom - rt.top;
	INT nX = (nScreenX - wndSizeX) / 2;
	INT nY = (nScreenY - wndSizeY) / 2;

	MoveWindow ( m_hWnd, nX, nY, wndSizeX, wndSizeY, TRUE );
	
	return S_OK;
}

void DxGlobalStage::SetD3DApp( CD3DApplication * pD3DApp )
{
	// 이 함수가 헤더에 존재하면 대입 연산이 이루어 지지 않는다
	// 갑자기 왜 그런걸까...? 싱글톤? 컴파일 옵션? 최적화?
	// 정확한 이유를 모르겠다.
	m_pD3DApp = pD3DApp;
}

CD3DApplication* DxGlobalStage::GetD3DApp ()
{
	return m_pD3DApp;
}


/*hackshield implementation, Juver, 2018/06/19 */
void DxGlobalStage::hs_send_callback( int type, std::string info )
{
	if ( m_emThisStage != EM_STAGE_GAME )	return;

	GLMSG::SNETPC_HS_CALLBACK msg;
	msg.type = type;

	if ( info.size() )
	{
		std::string file = info;
		if ( file.size() ){
			const size_t idx = file.find_last_of("\\/");
			if (std::string::npos != idx)
				file.erase(0, idx + 1);
		}

		StringCchCopy ( msg.info, MAX_PATH, info.c_str() );
		StringCchCopy ( msg.file, MAX_PATH, file.c_str() );
	}
	else
	{
		StringCchCopy ( msg.info, MAX_PATH, "none" );
		StringCchCopy ( msg.file, MAX_PATH, "none" );
	}
	
	NETSENDTOFIELD ( &msg );

	hs_close_game = TRUE;
}

/*hackshield implementation, Juver, 2018/06/19 */
void DxGlobalStage::hs_send_heartbeat( DWORD hs_sdk_version )
{
	if ( m_emThisStage != EM_STAGE_GAME )	return;

	GLMSG::SNETPC_HS_HEARTBEAT msg;
	msg.hs_sdk_version = hs_sdk_version;
	NETSENDTOFIELD ( &msg );
}