///////////////////////////////////////////////////////////////////////////////
// s_CGameServer.cpp
//
// class CGameServer
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
// 
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "GLChar.h"
#include "GLContrlMsg.h"
#include "GLGaeaServer.h"
#include "DxServerInstance.h"

#include "s_CGameServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static DWORD WINAPI CServerListenProc(CGameServer *pThis)
{
	return pThis->ListenProc();
}

static DWORD WINAPI CServerWorkProc(CGameServer *pThis)
{
	return pThis->WorkProc();
}

static DWORD WINAPI CServerUpdateProc(CGameServer *pThis)
{
	return pThis->UpdateProc();
}

CGameServer::CGameServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo) : CServer(hWnd, hEditBox, hEditBoxInfo)
{	
	m_pClientManager		= NULL;	// client information	
	m_sSession				= INVALID_SOCKET;
}

CGameServer::~CGameServer()
{
	SAFE_DELETE(m_pClientManager);	
}

// 클라이언트 정보저장 클래스 생성
int	CGameServer::StartClientManager()
{	
	SAFE_DELETE(m_pClientManager);
	m_pClientManager = new CClientGame(m_nMaxClient, m_pIOCP, m_hIOServer);
	if (!m_pClientManager) 
	{
		m_bIsRunning = FALSE;
		return NET_ERROR;
	}
	else
	{
		return NET_OK;
	}
}

// Create DB manager and Open DB
int	CGameServer::StartDbManager()
{
	int nRetCode;
	SAFE_DELETE(m_pDB);
	m_pDB = new CDbmanager();
	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "User DB Pool Size %d", CCfg::GetInstance()->GetDBPoolSize());	
	nRetCode = m_pDB->OpenUserDB(CCfg::GetInstance()->GetUserDBServer(),
							CCfg::GetInstance()->GetUserDBUser(),
							CCfg::GetInstance()->GetUserDBPass(),
							CCfg::GetInstance()->GetUserDBDatabase(),
							CCfg::GetInstance()->GetUserDBPoolSize(),
							CCfg::GetInstance()->GetUserDBResponseTime());
	if (nRetCode == DB_ERROR)
	{
		DB_Shutdown();
		CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "User DB Open Error");
		m_bIsRunning = FALSE;
		return NET_ERROR;
	}

	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "Game DB Pool Size %d", CCfg::GetInstance()->GetDBPoolSize());	
	nRetCode = m_pDB->OpenGameDB(CCfg::GetInstance()->GetGameDBServer(),
							CCfg::GetInstance()->GetGameDBUser(),
							CCfg::GetInstance()->GetGameDBPass(),
							CCfg::GetInstance()->GetGameDBDatabase(),
							CCfg::GetInstance()->GetGameDBPoolSize(),
							CCfg::GetInstance()->GetGameDBResponseTime());
	if (nRetCode == DB_ERROR)
	{
		DB_Shutdown();
		CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "Game DB Open Error");
		m_bIsRunning = FALSE;
		return NET_ERROR;
	}

	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "Database Open OK");
	return NET_OK;
}

int CGameServer::Start()
{
	HRESULT hr;
	
	m_bIsRunning = TRUE;
	
	if (StartCfg()			== NET_ERROR) return NET_ERROR;	
	if (StartIOCPList()		== NET_ERROR) return NET_ERROR;    	
	if (StartMsgManager()	== NET_ERROR) return NET_ERROR;
	if (StartIOCP()			== NET_ERROR) return NET_ERROR;
	if (StartClientManager()== NET_ERROR) return NET_ERROR;	
	if (StartDbManager()    == NET_ERROR) return NET_ERROR;
	if (StartWorkThread()	== NET_ERROR) return NET_ERROR;
	if (StartUpdateThread() == NET_ERROR) return NET_ERROR;	

	///////////////////////////////////////////////////////////////////////////
	//	Note : 가이아 서버 생성.
	hr = DxFieldInstance::Create ( this, CConsoleMessage::GetInstance(), m_pDB );
	if ( FAILED(hr) )
	{
		m_bIsRunning = FALSE;
		return NET_ERROR;
	}

	if (StartListenThread() == NET_ERROR) return NET_ERROR;
	
	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "=======================================================");	
	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "Server Start OK");	
	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "=======================================================");

	m_nStatus = S_SERVER_RUNING;

	return NET_OK;
}

int CGameServer::Stop()
{
	// Stop All Thread and exit
	DWORD dwExitCode = 0;

	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "=========== Please wait until server stop =============");
	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "=========== UpdateThread 종료를 기다리고 있습니다.");

	LockOn();
	m_bIsRunning = FALSE;
	LockOff();

	while ( m_hUpdateThread )	{	Sleep(0); }
	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "=========== UpdateThread가 종료되었습니다.");


	// 클라이언트 저장 클래스 종료
	SAFE_DELETE(m_pClientManager);
	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "Client Manager closed OK");
	// IOCP 종료
	SAFE_DELETE(m_pIOCP);
	// Message Queue 종료
	SAFE_DELETE(m_pMsgManager);
	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "Message Manager closed OK");
	// DB 연결종료
	SAFE_DELETE(m_pDB);
	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "Database Manager closed OK");
	// CFG class 종료
	CCfg::GetInstance()->ReleaseInstance();

	// Put message to console
	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "=======================================================");	
	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "Server Stop OK");	
	CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "=======================================================");
	
	m_nStatus = S_SERVER_STOP;

	return NET_OK;
}

int	CGameServer::Pause()
{
	::closesocket(m_sServer);
	m_sServer = INVALID_SOCKET;
	CloseAllClient();
	m_nStatus = S_SERVER_PAUSE;
	// Put message to console
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");	
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Server Pause OK");	
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");

	return NET_OK;	
}

int CGameServer::Resume()
{
	if (StartListenThread() == NET_OK)
	{
		m_nStatus = S_SERVER_RUNING;
		// Put message to console
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");	
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Server Resume OK");	
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");

		return NET_OK;
	}
	else
	{
		// Put message to console
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");	
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Server Resume Failed");	
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "=======================================================");

		return NET_ERROR;
	}
}

int	CGameServer::ReStart()
{
	Stop();
	Start();
	return NET_OK;
}

// close all client connections
void CGameServer::CloseAllClient()
{	
	if (m_pClientManager)
	{
		for (DWORD i=0; i < (DWORD) m_pClientManager->GetMaxClient(); i++)
		{
			if (m_pClientManager->IsOnline(i))
				CloseClient(i);
		}		
	}
}

CGameServer::ListenProc(void)
{
	LPPER_IO_OPERATION_DATA IOData;
	SOCKET	Accept;
	DWORD	dwRecvNumBytes = 0;
	DWORD	Flags = 0;
	HANDLE	hRetCode = NULL;
	int		nClientNumber = 0;
	int		nRetCode;

	while (m_bIsRunning)
	{
		Accept = WSAAccept(m_sServer, NULL, NULL, NULL, 0);
		if (Accept == INVALID_SOCKET) 
		{
			nRetCode = ::WSAGetLastError();
			CConsoleMessage::GetInstance()->Write("ERROR:WSAAccept %d", nRetCode);
			if (nRetCode == WSAENOTSOCK || nRetCode == WSAEINTR)
			{	
				break;
			}
			else
			{
				continue;
			}
		}

		///////////////////////////////////////////////////////////////////////
		// Create per-handle data information structure to associate with the socket
		nClientNumber = m_pClientManager->GetFreeClientID(); // Get free client slot number
		if (nClientNumber == NET_ERROR) 
		{
			closesocket(Accept);
			CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "INFO:Reached Max Client Number!");
			continue;
		} 
		else 
		{
			// Set client information
			// 클라이언트의 ip, port, 접속시간을 기록함.
			m_pClientManager->SetAcceptedClient(nClientNumber, Accept);
		}

		// Associate the accepted socket with the completion port
		hRetCode = CreateIoCompletionPort((HANDLE) Accept, 
										m_hIOServer, 
										(DWORD) nClientNumber, 
										0);
		if (hRetCode == NULL) 
		{
			CConsoleMessage::GetInstance()->Write(C_MSG_CONSOLE, "CreateIoCompletionPort Error");
			closesocket(Accept);
			continue;
		} 
		
		// Start processing I/O on ther accepted socket
		// First WSARecv, TCP/IP Send 8 bytes (ignored byte)
		// Client 버전 정보를 받는다.
		dwRecvNumBytes = sizeof(NET_MSG_GENERIC);
        
		IOData = (LPPER_IO_OPERATION_DATA) m_pIOCP->GetFreeOverIO(NET_RECV_POSTED);		
		IOData->dwRcvBytes		= 0;
		IOData->dwTotalBytes	= dwRecvNumBytes;
		// Head receive mode
		m_pClientManager->SetNetMode(nClientNumber, NET_PACKET_HEAD);
						
		WSARecv(Accept,
			&(IOData->DataBuf), 
			1,
			&dwRecvNumBytes,
			&Flags ,
			&(IOData->Overlapped),
			NULL);
		Sleep(0);
	}
	// ServerAcceptThread Running End
	CConsoleMessage::GetInstance()->Write ( C_MSG_CONSOLE, "ListenProc End");
	return 0;	
}

int CGameServer::UpdateProc()
{
	// 프레임 측정을 위한 변수들
	DWORD nTimeS;
	float fFrame			= 0;
	float fCount			= 0;	
	float fUpdateSec		= 30000;		// FPS 표시 갱신시간 30초
	DWORD dwUsrChkTime		= 1800000;		// 30 분마다 한번씩 사용자 접속끊기 체크
	DWORD dwUsrChkStart		= timeGetTime(); // 아무것도 하지 않는 사용자 접속끊기를 위한 시간
	DWORD dwUsrSaveTime		= 60000 * 30; // 사용자 저장시간 30분
	DWORD dwUsrSaveStart	= timeGetTime(); // 사용자 저장시간 체크를 위한 시간변수
	nTimeS					= GetTickCount();

	MSG_LIST* pMsg = NULL;
	while (m_bIsRunning)
	{		
		// Flip Messge Queue
		m_pMsgManager->MsgQueueFlip();	
		// Get Messages and Processing message
		pMsg = m_pMsgManager->GetMsg();		
		while (pMsg != NULL)
		{
			// 메시지 처리
			MsgProcess(pMsg);
			pMsg = m_pMsgManager->GetMsg(); // 다음메시지 가져오기
		}	
		///////////////////////////////////////////////////////////////////////
		// Update Gaea Server
		DxFieldInstance::FrameMove();		

		///////////////////////////////////////////////////////////////////////
		// 프레임 측정
		if ( (GetTickCount() - nTimeS) >= fUpdateSec )
		{			
			fFrame = ( (float)((fCount * fUpdateSec) /(float)(GetTickCount() - nTimeS)));
			//nFrame = Count; 
			nTimeS = GetTickCount();
			fCount = 0;
			
			CConsoleMessage::GetInstance()->Write ( C_MSG_CONSOLE, 
							"UpdateProc %d FPS, avg %d msec , Player Char : %d ", 
							(int)(fFrame/((fUpdateSec)/1000)), 
							(int)(fUpdateSec/fFrame), GLGaeaServer::Instance.GetNumPC() );

			CConsoleMessage::GetInstance()->Write ( C_MSG_CONSOLE, 
							"Current User %d/%d",							
							(int) m_pClientManager->GetCurrentClientNumber(),
							(int) m_pClientManager->GetMaxClient());
		} 
		else
		{
			fCount++;
		}
		Sleep(0);
	}

	TRACE ( "[UpdateProc End]\n" );

	// 가이아 서버 스탑
	TRACE ( "[Gaea Server CleanUp]\n" );
	GLGaeaServer::Instance.CleanUp ();

	//	업데이트 스래드 사라짐.
	m_hUpdateThread = NULL;

	return 0;
}

int CGameServer::WorkProc()
{
	LPPER_IO_OPERATION_DATA PerIoData;
	DWORD dwSndBytes;
	DWORD dwRcvBytes;
	DWORD dwByteTrans;
	DWORD Flags;
	DWORD dwClient = -1;
	int	  nRetCode=0;
	PerIoData = NULL;
	DWORD dwMaxClient = m_pClientManager->GetMaxClient();

	while (m_bIsRunning)
	{
		/////////////////////////////////////////////////////////////////////////////
		// Wait for I/O to complete on any socket
		nRetCode = GetQueuedCompletionStatus(m_hIOServer,
			&dwByteTrans, 
			(LPDWORD) &dwClient,
			(LPOVERLAPPED *) &PerIoData,			
			INFINITE);
 
		/////////////////////////////////////////////////////////////////////////////
		// 서버를 종료하기 위해서 종료처리 루틴을 호출했을때...
		if (dwClient == m_dwCompKey && PerIoData == NULL && dwByteTrans == 0)
		{
			// m_dwCompKey Time to work proc exit
			// ServerWorkerThread Running End		
			return 0;
		}

		/////////////////////////////////////////////////////////////////////////////
        // Illegal Message Skip
		if ((dwClient < 0) || (dwClient >= dwMaxClient))
		{			
			m_pIOCP->Release(PerIoData);
			continue;
		}

		/////////////////////////////////////////////////////////////////////////////
		// GetQueuedCompletionStatus() nRetCode=0 
		if (nRetCode == 0)
		{	
			if (PerIoData == NULL) // Time to exit, stop thread
			{
				return 0;
			}
			else // 강제종료 PerIoData != NULL && nRetCode == 0 && dwByteTrans == 0
			{
				m_pIOCP->Release(PerIoData);
				CloseClient(dwClient);				
				continue;
			}
		}
		else // (nRetCode != 0)
		{
			if (PerIoData == NULL) // 클라이언트가 강제 종료했을때...
		    { 
				// PostQueuedCompletionStatus post an I/O packet with 
				// a NULL CompletionKey (or if we get one for any reason).  It is time to exit.
				CloseClient(dwClient);				
				continue;
			}
		}
		
		/////////////////////////////////////////////////////////////////////////////
		// 클라이언트쪽에서 정상적으로 socket 을 close 했을때...
		if (dwByteTrans == 0 &&
			(PerIoData->OperationType == NET_RECV_POSTED || 
			PerIoData->OperationType == NET_SEND_POSTED))
		{
			// Client Closed. Bytes Transferred 0
			m_pIOCP->Release(PerIoData);
			CloseClient(dwClient);			
			continue;
		}

		switch (PerIoData->OperationType)
		{
		// 읽기 완료 통보일때...
		case NET_RECV_POSTED :
			{			
				// Insert to client buffer
                m_pClientManager->InsertBuffer(dwClient, (char*) PerIoData->Buffer, dwByteTrans);
				// Get one Message
				char* pMsg = m_pClientManager->GetMsg(dwClient);

				while (pMsg)
				{
					// Insert to message queue					
					m_pMsgManager->MsgQueueInsert(dwClient, pMsg);
					pMsg = m_pClientManager->GetMsg(dwClient);
				}

				// OverlappedIO 메모리 반환
				m_pIOCP->Release(PerIoData);
				// WSARecv 요청
				Flags = 0;
				dwRcvBytes = sizeof(NET_MSG_GENERIC);
				PerIoData = (LPPER_IO_OPERATION_DATA) m_pIOCP->GetFreeOverIO(NET_RECV_POSTED);
				
				nRetCode = WSARecv(m_pClientManager->GetSocket(dwClient),
									&(PerIoData->DataBuf), 
									1, 
									&dwRcvBytes,
									&Flags,
									&(PerIoData->Overlapped),
									NULL);
				if ((nRetCode == SOCKET_ERROR) && (WSAGetLastError() != WSA_IO_PENDING))
				{
					// 치명적 에러, 로그에 기록남김
					CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "CLoginServer::WorkProc WSARecv %d ERROR", nRetCode);
				}
			}
			break;
		// 쓰기 완료 통보 일때...
		case NET_SEND_POSTED :
			// 총 보낸 바이트수
			dwSndBytes = dwByteTrans + PerIoData->dwSndBytes;
			// 쓰기 완료됨
			if (dwSndBytes >= (DWORD) PerIoData->dwTotalBytes)
			{				
				m_pIOCP->Release(PerIoData);
			}
			else // 쓰기 완료되지 않음 남은 바이트를 전송
			{
				// 보낸 바이트수 업데이트
				PerIoData->dwSndBytes = dwSndBytes;
				// 보내야할 데이타 포인터 업데이트
				PerIoData->DataBuf.buf = PerIoData->Buffer + dwSndBytes;
				// 보내야할 바이트수 업데이트
				dwSndBytes = (DWORD) PerIoData->dwTotalBytes - dwSndBytes;
				// 버퍼길이 업데이트
				PerIoData->DataBuf.len = (u_long) dwSndBytes;
				
				// 전송요청
				Send(dwClient, PerIoData, dwSndBytes);
			}
			break;
		default :			
			m_pIOCP->Release(PerIoData);			
			break;
		}
		Sleep(0);
	}
	// ServerWorkerThread Running End
	CConsoleMessage::GetInstance()->Write ( C_MSG_CONSOLE, "WorkProc End");
	return 0;
}

void CGameServer::Send(DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize)
{
}

int CGameServer::SendClient ( DWORD dwClient, LPVOID pBuffer )
{
	m_pClientManager->SendClient(dwClient, pBuffer);
	return 0;
}

void CGameServer::SendAllClient(LPVOID pBuffer)
{
	DWORD dwMaxClient;
	DWORD dwStartClient;

	dwStartClient = NET_RESERVED_SLOT;
	dwMaxClient = (DWORD) m_nMaxClient;

	CConsoleMessage::GetInstance()->Write("전체 게임중인 플레이어에 메시지를 보냅니다");

	for (dwStartClient; dwStartClient < dwMaxClient; dwStartClient++ )
	{
		// 게임중이면 메시지를 보낸다
		if (m_pClientManager->IsGaming(dwStartClient))		
			SendClient(dwStartClient, pBuffer);
	}
}

// 사용자를 로그아웃 시킨다.
void CGameServer::CloseClient(DWORD dwClient)
{
	int nUserNum;
	int	nGameTime;
	DWORD dwGaeaID;
	char* szUserID;	

	if (m_pClientManager->IsOnline(dwClient) == FALSE)
		return;


	nUserNum = m_pClientManager->GetUserNum(dwClient);
	CConsoleMessage::GetInstance()->Write("사용자가 연결을 종료했습니다");
	
	szUserID  = m_pClientManager->GetUserID(dwClient);
	

	
	if ( nUserNum > 0 )
	{		
		nGameTime = m_pClientManager->GetLoginTime(dwClient);
		
		// 사용자가 종료되었음을 알림
		CConsoleMessage::GetInstance()->Write("사용자를 로그아웃 시킵니다");
		// if ( m_pDB->UserLogout(szUserID, nUserNum, nGameTime) == DB_ERROR )	return;

		CConsoleMessage::GetInstance()->Write ( C_MSG_CONSOLE, "캐릭터를 게임에서 Drop ( ClientID %d ) 시킵니다", dwClient );
		dwGaeaID = m_pClientManager->GetGaeaID(dwClient);
		if ( dwGaeaID != GAEAID_NULL )
		{
			// 가이아서버 캐릭터 드랍 예약
			GLGaeaServer::Instance.ReserveDropOutPC ( dwGaeaID ); // lock
		}
	}
	m_pClientManager->CloseClient(dwClient); // lock	
}

int CGameServer::MsgProcess(MSG_LIST* pMsg)
{	
	NET_MSG_GENERIC* nmg;
	DWORD dwClient;

	nmg = (NET_MSG_GENERIC*) pMsg->Buffer; 
	dwClient = pMsg->dwClient;

	// 서버 네트웍 처리량 계산 변수 업데이트
	m_nPacketCount++;
	m_nPacketSize += nmg->nSize;

	switch(nmg->nType)
	{
	case NET_MSG_LOGIN : // 로그인 데이타일때...		
		MsgLogIn(pMsg);		
		break;
	case NET_MSG_VERSION_INFO : // 클라이언트 버전정보
		MsgVersion(pMsg);
		break;
	case NET_MSG_REQ_USER_INFO : // 사용자 정보 요청
		MsgSndUserInfo(pMsg);
		break;		
	case NET_MSG_CHA_NEW : // 새로운 캐릭터 생성
		MsgCreateCharacter(pMsg);
		break;	
	case NET_MSG_REQ_CHA_BAINFO : // 클라이언트->게임서버 : 사용자 (디스플레이용)  캐릭터 기초정보 요청
        MsgSndChaBasicBAInfo(pMsg);
		break;
	case NET_MSG_REQ_CHA_BINFO : // 클라이언트->게임서버 : 특정캐릭의 기초정보 요청 (디스플레이용)
		MsgSndChaBasicInfo(pMsg);
		break;
	case NET_MSG_CHA_DEL : // 클라이언트->게임서버 : 캐릭터 삭제
		MsgSndChaDelInfo(pMsg);
		break;
	case NET_MSG_LOBBY_GAME_JOIN : // 클라이언트->게임서버 : 캐릭터 선택후 조인
        MsgGameJoin(pMsg);
		break;
	case NET_MSG_PING_REQUEST : // 핑요청
		MsgSndPingAnswer(pMsg);
		break;

	default:
		{
			if ( nmg->nType > NET_MSG_GCTRL )
			{
				GLGaeaServer::Instance.MsgProcess ( (NET_MSG_GENERIC*)pMsg->Buffer, pMsg->dwClient, m_pClientManager->GetGaeaID(pMsg->dwClient) );
			}			
		}
		break;
	}
	return 0;
}

// 게임서버->클라이언트
// 일반적 채팅 메시지 전송
void CGameServer::MsgSndChatNormal(DWORD dwClient, const char* szName, const char* szMsg)
{
    NET_CHAT nc;
	nc.nmg.nSize = sizeof(NET_CHAT);
	nc.nmg.nType = NET_MSG_CHAT;
	nc.emType = NET_MSG_CHAT_NORMAL;
	StringCchCopyN(nc.szName, USR_ID_LENGTH, szName, USR_ID_LENGTH);
	StringCchCopyN(nc.szChatMsg, CHAT_MSG_SIZE, szMsg, CHAT_MSG_SIZE);

	SendClient(dwClient, (NET_MSG_GENERIC*) &nc);
}

// 클라이언트->게임서버 : 캐릭터 선택후 조인
void CGameServer::MsgGameJoin(MSG_LIST* pMsg)
{
	NET_GAME_JOIN* pNgj = (NET_GAME_JOIN*) pMsg->Buffer;
	DWORD dwClient = pMsg->dwClient;
	int nChaNum = pNgj->nChaNum;
	int nRetCode = 0;
	DWORD dwItemNum = 0;

	SCHARDATA2 CharData2;

	CharData2.m_dwUserID	= (DWORD) m_pClientManager->GetUserNum(dwClient);// 유저번호 (DB)
	CharData2.m_dwServerID	= (DWORD) m_nServerGroup; // 서버그룹
	CharData2.m_dwCharID    = (DWORD) nChaNum; // 캐릭터 번호 (DB)

	// 캐릭터 가져오기
	nRetCode = m_pDB->GetCharacterInfo(nChaNum, &CharData2);
	if (nRetCode == DB_ERROR)
	{
		CConsoleMessage::GetInstance()->Write ( C_MSG_FILE_CONSOLE, "ERROR - 캐릭터 DB 읽기에 실패하였습니다." );

		// 읽기 실패
		return;
	}

	// 캐릭터 생성
	PGLCHAR pGLChar = GLGaeaServer::Instance.CreatePC ( &CharData2, dwClient, 0 );
	if ( !pGLChar )
	{
		CConsoleMessage::GetInstance()->Write ( C_MSG_FILE_CONSOLE, "ERROR - 캐릭터 인스턴스의 초기화에 실패하였습니다." );

		//	캐릭터 생성실패 : 예외처리 추가...
		return ;
	}

	// 가이아서버에 정보 세팅
	m_pClientManager->SetGaeaID ( dwClient, pGLChar->m_dwGaeaID );

	// 클라이언트에 전송 : 캐릭터정보 + 스킬 + 아이템
	pGLChar->MsgGameJoin();

	CConsoleMessage::GetInstance()->Write("캐릭터 정보를 모두 전송하였습니다");	
}

// 클라이언트->게임서버 : 케릭터 삭제
void CGameServer::MsgSndChaDelInfo(MSG_LIST* pMsg)
{
	NET_CHA_DEL_FB ncbi;
	DWORD dwSndBytes = 0;
	DWORD dwClient = pMsg->dwClient;
	
	NET_CHA_DEL* ncd = (NET_CHA_DEL*) pMsg->Buffer;

	dwSndBytes		= sizeof(NET_CHA_BBA_INFO);
	ncbi.nmg.nSize	= dwSndBytes;
	ncbi.nChaNum	= ncd->nChaNum;

    if (ncd->nChaNum > 0)
	{
		if (m_pDB->DelCharacter(m_pClientManager->GetUserNum(dwClient), ncd->nChaNum, ncd->szPass2) == DB_OK) // 삭제성공
			ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_OK;    
		else // 삭제 에러
			ncbi.nmg.nType = NET_MSG_CHA_DEL_FB_ERROR;
	}	
	SendClient(dwClient, &ncbi);
}

// 클라이언트가 캐릭터 선택화면에서 디스플레이 해야할
// 캐릭터의 고유번호를 전송한다.
void CGameServer::MsgSndChaBasicBAInfo(MSG_LIST* pMsg)
{
	DWORD dwSndBytes = 0;
	DWORD dwClient = pMsg->dwClient;
	INT nUsrNum = 0;
	INT nSvrGrp = 0;

	NET_CHA_BBA_INFO ncbi;
	
	dwSndBytes = sizeof(NET_CHA_BBA_INFO);
	ncbi.nmg.nSize = dwSndBytes;
	ncbi.nmg.nType = NET_MSG_CHA_BAINFO;
	
	nUsrNum = m_pClientManager->GetUserNum(dwClient);
	if (nUsrNum < 1) // 치명적 에러
		return;
	nSvrGrp = m_nServerGroup;
	m_pDB->GetChaBAInfo(nUsrNum, nSvrGrp, &ncbi);

	SendClient(dwClient, &ncbi);
}

void CGameServer::MsgSndChaBasicInfo(DWORD dwClient, int nChaNum)
{
	DWORD dwSndBytes = 0;	
	GLMSG::SNETLOBBY_CHARINFO snci;
	m_pDB->GetChaBInfo(nChaNum, &snci.Data);
	
	SendClient(dwClient, &snci);
}

void CGameServer::MsgSndChaBasicInfo(MSG_LIST* pMsg)
{
	DWORD dwSndBytes = 0;
	DWORD dwClient = pMsg->dwClient;
	INT nChaNum = 0;
	INT nUsrNum = 0;
	INT nSvrGrp = 0;

	NET_CHA_BA_INFO* ncbi;
	ncbi = (NET_CHA_BA_INFO*) pMsg->Buffer;
	nChaNum = ncbi->nChaNum;

	GLMSG::SNETLOBBY_CHARINFO snci;
	if ( m_pDB->GetChaBInfo(nChaNum, &snci.Data) == DB_ERROR )
	{		
		return;
	}
	
	SendClient(dwClient, &snci);
}

void CGameServer::MsgSndChatGlobal(char* szChatMsg)
{
	DWORD dwSndBytes = 0;
	CConsoleMessage::GetInstance()->Write(szChatMsg);
	NET_CHAT_FB ncf;
	ncf.emType = NET_MSG_CHAT_GLOBAL;
	StringCchCopyN(ncf.szChatMsg, CHAT_MSG_SIZE, szChatMsg, CHAT_MSG_SIZE);
	SendAllClient(&ncf);
}

void CGameServer::MsgLogIn(MSG_LIST* pMsg)
{	
	NET_LOGIN_DATA* nml;
	DWORD dwFlags = 0;
	DWORD dwSndBytes = 0;
	DWORD dwClient;
	
	nml = (NET_LOGIN_DATA *) pMsg->Buffer;	
	dwClient = pMsg->dwClient;
	
	// 사용자 id 세팅
	m_pClientManager->SetUserID(dwClient, nml->szUserid);
	// DB 에서 사용자 확인

}

void CGameServer::MsgVersion(MSG_LIST* pMsg)
{
	DWORD					dwSndBytes = 0;
	NET_CLIENT_VERSION		ncv;

	DWORD dwClient			= pMsg->dwClient;

	ncv.nmg.nSize			= sizeof(NET_CLIENT_VERSION);
	ncv.nmg.nType			= NET_MSG_VERSION_INFO;
	ncv.nGameProgramVer		= m_nVersion;
	ncv.nPatchProgramVer	= m_nPatchVersion;
	
	SendClient(dwClient, &ncv);
}

// 새로운 캐릭터를 생성한다.
void CGameServer::MsgCreateCharacter(MSG_LIST* pMsg)
{
	DWORD			dwClient;
	NET_NEW_CHA*	nnc;
	NET_NEW_CHA_FB	nncfd;	
	DWORD			dwSndBytes = 0;
	int				nUsrNum;
	int				nIndex;	
	int				nSvrGrp;
	int				nChaNum;

	dwClient = pMsg->dwClient;	
	nnc = (NET_NEW_CHA*) pMsg->Buffer;

	// 캐릭터 인덱스
	nIndex = nnc->nIndex;
	if (nIndex < 0 || nIndex >= GLCI_NUM )	return;

	// 사용자번호, 캐릭터 명, 서버 그룹
	nUsrNum = m_pClientManager->GetUserNum(dwClient);	 	
	nSvrGrp = m_nServerGroup;

	// 캐릭터명
	CString strChaName(nnc->szChaName);
	strChaName.Trim(); // 앞뒤 공백제거

	if ((strChaName.FindOneOf(" ") != -1) || (strChaName.GetLength() < 2)) // 스페이스 찾기, 캐릭터이름 2글자 이하 에러
	{	 	
		nChaNum = -1; // 캐릭터이름 에러
	}
	else
	{
		// 캐릭터 초기 수치 설정.
		GLCHARLOGIC NewCharLogic;
		NewCharLogic.INIT_NEW_CHAR ( (EMCHARINDEX)nIndex, (DWORD)nUsrNum, (DWORD)nSvrGrp, strChaName );

		// 캐릭터 생성
		nChaNum = m_pDB->CreateNewCharacter(&NewCharLogic);
		dwSndBytes = sizeof (NET_NEW_CHA_FB);		
		nncfd.nmg.nSize = (USHORT) dwSndBytes;	
		nncfd.nmg.nType = NET_MSG_CHA_NEW_FB;
		nncfd.nChaNum = 0;
	}
	
	if (nChaNum > 0) // 캐릭터 생성 성공
	{
		nncfd.nChaNum = nChaNum;
		nncfd.nResult = NET_MSG_CHA_NEW_FB_SUB_OK;
	}	
	else if (nChaNum == DB_ERROR) // 캐릭터 생성(DB) 에러발생...
	{
		nncfd.nResult  = NET_MSG_CHA_NEW_FB_SUB_ERROR;
	}
	else if (nChaNum == DB_CHA_MAX) // 만들수 있는 캐릭터 숫자 초과
	{
		nncfd.nResult  = NET_MSG_CHA_NEW_FB_SUB_MAX;
	}
	else if (nChaNum == DB_CHA_DUF) // 같은 이름을 가진 캐릭터 존재
	{
		nncfd.nResult  = NET_MSG_CHA_NEW_FB_SUB_DUP;
	}
	else // 알수 없는 에러
	{
		nncfd.nResult  = NET_MSG_CHA_NEW_FB_SUB_ERROR;
	}

	SendClient(dwClient, &nncfd);
	
	if ( nChaNum > 0 )
	{
		MsgSndChaBasicInfo(dwClient, nChaNum);
	}
}

// 사용자 정보를 DB 에서 읽어와서 g_Client에 세팅한다.
void CGameServer::SetUserInfo(DWORD dwClient, const char* strUserId, const char* strPassword)
{
//#ifdef _DEBUG
//	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "NET_SetUserInfo %d, %s, %s", dwClient, strUserId, strPassword);
//#endif
//	LockOn();
//	memcpy((LPUSER_INFO_DETAIL)&m_pClientManager->m_pClient[dwClient].uid, 
//		(LPUSER_INFO_DETAIL)&m_pDB->GetUserInfo(strUserId, strPassword), 
//		sizeof(USER_INFO_DETAIL));
//	LockOff();
//#ifdef _DEBUG	
//	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "UserNum = %d", m_pClientManager->m_pClient[dwClient].uid.uib.nUserNum);	
//#endif
}

void CGameServer::MsgSndPingAnswer(MSG_LIST* pMsg)
{	
	NET_MSG_PING*	nmpr;	
	NET_MSG_PING	nmpa;	
	DWORD			dwSndBytes = 0;
	DWORD			dwClient = pMsg->dwClient;

	nmpr = (NET_MSG_PING*) pMsg->Buffer;
	
	nmpa.nmg.nSize	= sizeof(NET_MSG_PING);	
	nmpa.nmg.nType	= NET_MSG_PING_ANSWER;
	nmpa.stime		= nmpr->stime;
	SendClient(dwClient, &nmpa);
}

int CGameServer::ExecuteCommand(char* strCmd)
{
	// 토큰으로 분리할 분리자.
	// space
	char seps[]   = " ";
	char *token;
//	char strTemp[100];

	// Establish string and get the first token
	token = strtok(strCmd, seps);
	while (token != NULL)
	{
		// Request Ping Info
		// command : ping session
		if (strcmp(token, "ping") == 0) 
		{
			// Ping 요청
			token = strtok(NULL, seps);
			if (token) 
			{
				// 세션서버에 핑요청
				if (strcmp(token, "session") == 0) 
				{

				}			
				return 0;
			}
			else
			{
				return 0;
			}
		}
		// 서버 채팅 메시지, global
		if (strcmp(token, "chat") == 0)
		{
			token = strtok(NULL, seps);
			if (token)
			{
				MsgSndChatGlobal(token);
				return 0;
			}
			else
			{
				return 0;
			}
		}

		// Get next token
		token = strtok(NULL, seps);
	}	
	return 0;
}

//                        ,     
//                   ,   /^\     ___
//                  /^\_/   `...'  /`
//               ,__\    ,'     ~ (
//            ,___\ ,,    .,       \
//             \___ \\\ .'.'   .-.  )
//               .'.-\\\`.`.  '.-. (
//              / (==== ."".  ( o ) \
//            ,/u  `~~~'|  /   `-'   )
//           "")^u ^u^|~| `""".  ~_ /
//             /^u ^u ^\~\     ".  \\
//     _      /u^  u ^u  ~\      ". \\
//    ( \     )^ ^U ^U ^U\~\      ". \\
//   (_ (\   /^U ^ ^U ^U  ~|       ". `\
//  (_  _ \  )U ^ U^ ^U ^|~|        ". `\.
// (_  = _(\ \^ U ^U ^ U^ ~|          ".`.;
//(_ -(    _\_)U ^ ^ U^ ^|~|            ""
//(_    =   ( ^ U^ U^ ^ U ~|
//(_ -  ( ~  = ^ U ^U U ^|~/
// (_  =     (_^U^ ^ U^ U /
//  (_-   ~_(/ \^ U^ ^U^,"
//   (_ =  _/   |^ u^u."  
//    (_  (/    |u^ u.(   
//     (__/     )^u^ u/ 
//             /u^ u^(  
//            |^ u^ u/   
//            |u^ u^(       ____   
//            |^u^ u(    .-'    `-,
//             \^u ^ \  / ' .---.  \
//              \^ u^u\ |  '  `  ;  |
//               \u^u^u:` . `-'  ;  |
//                `-.^ u`._   _.'^'./
//                   "-.^.-```_=~._/
//                      `"------"'
// jgkim