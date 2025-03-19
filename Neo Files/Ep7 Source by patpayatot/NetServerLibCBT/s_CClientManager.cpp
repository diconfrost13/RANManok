///////////////////////////////////////////////////////////////////////////////
// s_CClientManager.cpp
// 
// class CClientManager
//
// * History
// 2002.05.30 jgkim Create
// 2003.02.12 jgkim Message buffering
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note 
// 
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "s_CClientManager.h"
#include "s_CConsoleMessage.h"
#include "GLGaeaServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CClientManager::CClientManager ( int nMaxClient, COverlapped* pOverlapped, HANDLE	hIOServer ) :
	m_pClient(NULL),
	m_nClients(0),

	m_nMaxClient(nMaxClient),
	m_hIOServer(hIOServer),
	m_pIOCP(pOverlapped)
{
	srand((unsigned) time(NULL));
	m_pClient = new CNetUser[m_nMaxClient];

	for ( int i=NET_RESERVED_SLOT; i<m_nMaxClient; ++i )	m_dequeSleepCID.push_back(i);
}

CClientManager::~CClientManager()
{
	// close all client socket
	// CloseAllClient();
	// Memory Free Client information
	if ( m_pClient != NULL )
	{	
		LockOn();
		SAFE_DELETE_ARRAY(m_pClient);
		LockOff();
	}	
}

CRYPT_KEY CClientManager::GetNewCryptKey()
{
	CRYPT_KEY ck;
	ck.nKeyDirection	= rand() % 2 + 1; // Direction Left or Right	
	ck.nKey				= rand() % 5 + 2; // Shift amount
	
	return ck;
}

CRYPT_KEY CClientManager::GetCryptKey(DWORD dwClient)
{
	//assert(dwClient<(DWORD)m_nMaxClient);

	return m_pClient[dwClient].GetCryptKey();
}

void CClientManager::SetCryptKey(DWORD dwClient, CRYPT_KEY ck)
{
	//assert(dwClient<(DWORD)m_nMaxClient);

	m_pClient[dwClient].SetCryptKey(ck);
}

BOOL CClientManager::IsOnline(DWORD dwClient)
{
	// assert(dwClient<(DWORD)m_nMaxClient);

	return m_pClient[dwClient].IsOnline();
}

// GetFreeClientID
// Return the free client slot
int	CClientManager::GetFreeClientID(void)
{	
	LockOn();
	if (m_dequeSleepCID.empty())
	{
		LockOff();
		return NET_ERROR;
	}
	else
	{    
		int nClient = *m_dequeSleepCID.begin();
		m_dequeSleepCID.pop_front();		
		// assert ( !m_pClient[nClient].IsOnline() );
		m_pClient[nClient].SetOnLine();
		LockOff();
		return nClient;
	}
}

int CClientManager::GetMaxClient()
{
	return m_nMaxClient;
}

// 클라이언트가 처음 접속했을때 호출됨
// 클라이언트의 ip, port, 접속시간을 기록함.
void CClientManager::SetAcceptedClient(DWORD dwClient, SOCKET sSocket)
{
	LockOn();
	CNetUser*	pData;
	pData = (CNetUser*) (m_pClient+dwClient);	
	pData->SetAcceptedClient(sSocket);
	// 접속자수 증가
	m_nClients++;
	LockOff();

	CConsoleMessage::GetInstance()->Write("(Current User:%d) (Client:%d) (%s:%d)",
										m_nClients,
										dwClient, 
										pData->GetIP(),
										pData->GetPort());
}

void CClientManager::CloseAllClient()
{
	int nMaxClient;
	int i;
	nMaxClient = m_nMaxClient;

	for (i=0; i<nMaxClient; i++)
		CloseClient(i);
}

///////////////////////////////////////////////////////////////////////////////
// CloseClient
// Close client socket 
int CClientManager::CloseClient(DWORD dwClient)
{	
	int nResult = 0;

	LockOn();
	if (m_pClient[dwClient].IsOnline())
	{
		nResult = m_pClient[dwClient].CloseClient();
		m_dequePreSleepCID.push_back(dwClient);
	}	
	LockOff();

	return nResult;
}

void CClientManager::Reset(DWORD dwClient)
{
	LockOn();
	m_pClient[dwClient].Reset();
	LockOff();
}

//	Note : 유휴 클라이언트 ID를 모아서 한꺼번에 등록. ( 클라이언트 ID의 교차 사용을 방지. )
//
void CClientManager::ResetPreSleepCID()
{
	LockOn();
	int nTemp = -1;
	CLIENTS_ITER iter = m_dequePreSleepCID.begin();
	CLIENTS_ITER iter_end = m_dequePreSleepCID.end();
	for ( ; iter!=iter_end; ++iter )
	{
		if (nTemp != (*iter))
		{
			m_dequeSleepCID.push_back(*iter);
			nTemp = (*iter);
			--m_nClients;
		}
	}
	m_dequePreSleepCID.clear();
	if ( m_nClients < 0 )		
		m_nClients = 0;
	LockOff();
}

// Return current client number
int	CClientManager::GetCurrentClientNumber(void)
{
	return m_nMaxClient - (int) m_dequeSleepCID.size();
}

int CClientManager::GetNetMode(DWORD dwClient)
{
	return m_pClient[dwClient].GetNetMode();
}

void CClientManager::SetNetMode(DWORD dwClient, int nMode)
{
	m_pClient[dwClient].SetNetMode(nMode);
}

char* CClientManager::GetClientIP(DWORD dwClient)
{
	return m_pClient[dwClient].GetIP();
}

// 소켓에서 넘어온 데이타를 버퍼에 집어 넣는다
void CClientManager::InsertBuffer(DWORD dwClient, char* buff, DWORD dwSize)
{
	ASSERT(buff);
	m_pClient[dwClient].InsertBuffer(buff, dwSize);
}

// 클라이언트 버퍼에서 메시지를 하나 가져온다
char* CClientManager::GetMsg(DWORD dwClient)
{
    return m_pClient[dwClient].GetMsg();
}

// 메시지에 사용될 버퍼를 리셋한다
void CClientManager::ResetBuffer(DWORD dwClient)
{
	m_pClient[dwClient].ResetBuffer();
}

SOCKET CClientManager::GetSocket(DWORD dwClient)
{
	return m_pClient[dwClient].GetSocket();
}

char* CClientManager::GetIP(DWORD dwClient)
{
	return m_pClient[dwClient].GetIP();
}

int	CClientManager::SendClient(DWORD dwClient, LPVOID pBuffer)
{
	LPPER_IO_OPERATION_DATA pIoWrite;
	NET_MSG_GENERIC*		nmg;
	DWORD					dwSndBytes;
	CRYPT_KEY				ck;

	assert(pBuffer);
	nmg = (NET_MSG_GENERIC*) pBuffer;
	if (nmg == NULL)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR : SendClient nmg == NULL");
		return 0;
	}

	dwSndBytes = nmg->nSize;
	
	if (dwSndBytes > NET_DATA_BUFSIZE)
		return NET_ERROR;

	pIoWrite = (LPPER_IO_OPERATION_DATA) m_pIOCP->GetFreeOverIO(NET_SEND_POSTED);
	
	::CopyMemory(pIoWrite->Buffer, nmg, dwSndBytes);

	// Encodeing	
	ck = GetCryptKey(dwClient);
	m_Bit.buf_encode(pIoWrite->Buffer+sizeof(NET_MSG_GENERIC), 
					dwSndBytes-sizeof(NET_MSG_GENERIC),
					ck.nKeyDirection,
					ck.nKey);

	pIoWrite->dwTotalBytes	= dwSndBytes;
	SendClient(dwClient, pIoWrite, dwSndBytes);
	return NET_OK;
}

int CClientManager::SendClient(DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize)
{
	// MSG_OOB
	// MSG_DONTROUTE
	// MSG_PARTIAL
	INT		nRetCode = 0;
	DWORD	dwFlags = 0;
	DWORD	dwSndSize = dwSize;

	PerIoData->OperationType = NET_SEND_POSTED;
	PerIoData->DataBuf.len = (u_long) dwSize;
	
	SOCKET sSocket = GetSocket(dwClient);
	if (sSocket == INVALID_SOCKET)
	{
		m_pIOCP->Release(PerIoData);
		return NET_ERROR;
	}
	nRetCode = ::WSASend(sSocket,
					&(PerIoData->DataBuf),
					1,
					&dwSndSize,
					dwFlags,
					&(PerIoData->Overlapped),
					NULL);

	if (nRetCode == SOCKET_ERROR) 
	{
		nRetCode = ::WSAGetLastError();
		if (nRetCode != WSA_IO_PENDING) // WSA_IO_PENDING is not error.
		{
			// MessageId: WSA_IO_PENDING, ERROR_IO_PENDING 997L    
			// Overlapped I/O operation is in progress.

			// MessageId: WSAEMSGSIZE 10040L
			// A message sent on a datagram socket was larger than the internal message buffer or some other network limit, or the buffer used to receive a datagram into was smaller than the datagram itself.
			
			// MessageId: WSAEINVAL 10022L
			// An invalid argument was supplied.

			// MessageId: WSAENOTSOCK 10038L
			// An operation was attempted on something that is not a socket.
			
			// MessageId: WSAECONNABORTED 10053L
			// An established connection was aborted by the software in your host machine.

			// MessageId: WSAECONNRESET 10054L
			// An existing connection was forcibly closed by the remote host.

			// MessageId: WSAESHUTDOWN 10058L
			// A request to send or receive data was disallowed 
			// because the socket had already been shut down in that direction with a previous shutdown call.
				
			// 현재는 WSA_IO_PENDING 이외의 에러시에는 연결을 종료시켜버린다.

			// 로그를 통해 1회이상 검출된 에러들
			if ((nRetCode == WSAENOTSOCK) || (nRetCode == WSAECONNRESET) || 
				(nRetCode == WSAEMSGSIZE) || (nRetCode == WSAESHUTDOWN) ||
				(nRetCode == WSAECONNABORTED))
			{				
				::PostQueuedCompletionStatus(m_hIOServer,
								0,
								dwClient, 
								&PerIoData->Overlapped);
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:Send %d", nRetCode);
				return NET_ERROR;
			}
			else // 로그를 통해 검출되지 않은 에러들
			{				
				::PostQueuedCompletionStatus(m_hIOServer,
								0,
								dwClient, 
								&PerIoData->Overlapped);				
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:Send %d", nRetCode);
				return NET_ERROR;
			}
		}
	}
	return NET_OK;
}

// Agent 서버에서의 Client 와의 통신 슬롯
DWORD CClientManager::GetSlotAgentClient(DWORD dwClient) 
{
	return m_pClient[dwClient].GetSlotAgentClient();
}

// Agent 서버에서의 Field 서버와의 통신 슬롯
DWORD CClientManager::GetSlotAgentField (DWORD dwClient)
{
	return m_pClient[dwClient].GetSlotAgentField();
}

// Field 서버에서의 Agent 서버와의 통신 슬롯
DWORD CClientManager::GetSlotFieldAgent (DWORD dwClient) 
{
	return m_pClient[dwClient].GetSlotFieldAgent();
}

// Field 서버에서의 Client 와의 통신 슬롯
DWORD CClientManager::GetSlotFieldClient(DWORD dwClient) 
{
	return m_pClient[dwClient].GetSlotFieldClient();
}

// Agent 서버에서의 Client 와의 통신 슬롯
void CClientManager::SetSlotAgentClient(DWORD dwClient, DWORD dwSlot) 
{
	m_pClient[dwClient].SetSlotAgentClient(dwSlot);
}

// Agent 서버에서의 Field 서버와의 통신 슬롯
void CClientManager::SetSlotAgentField (DWORD dwClient, DWORD dwSlot) 
{
	m_pClient[dwClient].SetSlotAgentField(dwSlot);
}

// Field 서버에서의 Agent 서버와의 통신 슬롯
void CClientManager::SetSlotFieldAgent (DWORD dwClient, DWORD dwSlot) 
{
	m_pClient[dwClient].SetSlotFieldAgent(dwSlot);
}

// Field 서버에서의 Client 와의 통신 슬롯
void CClientManager::SetSlotFieldClient(DWORD dwClient, DWORD dwSlot) 
{
	m_pClient[dwClient].SetSlotFieldClient(dwSlot);
}

void CClientManager::SetSlotType(DWORD dwClient, DWORD dwType)	
{
	m_pClient[dwClient].SetSlotType(dwType);
}

DWORD CClientManager::GetSlotType(DWORD dwClient)
{
	return m_pClient[dwClient].GetSlotType();
}