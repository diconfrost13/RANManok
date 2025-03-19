///////////////////////////////////////////////////////////////////////////////
// s_CNetUser.cpp
// 
// class CNetUser
//
// * History
// 2003.02.18 jgkim 
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note 
// 
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "s_CNetUser.h"
#include "s_CConsoleMessage.h"
#include "GLGaeaServer.h"
#include "s_CDbmanager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CNetUser::CNetUser()
{
	Reset();
}

CNetUser::~CNetUser()
{
}

void CNetUser::SetAcceptedClient(SOCKET s)
{	
	INT			nSize;
	sockaddr_in	sAddrIn;
	m_Socket = s;
	
	nSize = sizeof(sockaddr_in);
	// Get client ip address and port
	::getpeername(m_Socket, (sockaddr *) &sAddrIn, &nSize);
	::StringCchCopyN(m_szIp, IP_LENGTH, ::inet_ntoa(sAddrIn.sin_addr), IP_LENGTH);
	m_usPort			= ::ntohs(sAddrIn.sin_port);
	m_dwLastMsgTime		= ::timeGetTime();
	m_dwLastSaveTime	= m_dwLastMsgTime;
	m_nOnline			= NET_ONLINE;
}

int CNetUser::CloseClient()
{
	int nRetCode;
	LockOn();
	if (m_nOnline == NET_ONLINE)
	{
		nRetCode = ::shutdown(m_Socket, SD_SEND);
		if (nRetCode == SOCKET_ERROR)
		{
			nRetCode = ::WSAGetLastError();
			if (nRetCode == WSANOTINITIALISED) // A successful WSAStartup call must occur before using this function. 
			{
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CNetUser::CloseClient shutdown WSANOTINITIALISED ERROR");
			}
			else if (nRetCode == WSAENETDOWN) // The network subsystem has failed. 
			{
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CNetUser::CloseClient shutdown WSAENETDOWN ERROR");
			}
			else if (nRetCode == WSAEINVAL) //  The how parameter is not valid, or is not consistent with the socket type. For example, SD_SEND is used with a UNI_RECV socket type. 
			{
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CNetUser::CloseClient shutdown WSAEINVAL ERROR");
			}
			else if (nRetCode == WSAEINPROGRESS) // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. 
			{
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CNetUser::CloseClient shutdown WSAEINPROGRESS ERROR");
			}
			else if (nRetCode == WSAENOTCONN) // The socket is not connected (connection-oriented sockets only). 
			{
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CNetUser::CloseClient shutdown WSAENOTCONN ERROR");
			}
			else if (nRetCode == WSAENOTSOCK) // The descriptor is not a socket. 
			{
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CNetUser::CloseClient shutdown WSAENOTSOCK ERROR");
			}
			else
			{
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CNetUser::CloseClient shutdown unknown ERROR");
			}
		}
		nRetCode = ::closesocket(m_Socket);
		if (nRetCode == SOCKET_ERROR)
		{
			if (nRetCode == WSANOTINITIALISED) // A successful WSAStartup call must occur before using this function. 
			{
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CNetUser::CloseClient closesocket WSANOTINITIALISED ERROR");
			}
			else if (nRetCode == WSAENETDOWN) // The network subsystem has failed. 
			{
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CNetUser::CloseClient closesocket WSAENETDOWN ERROR");
			}
			else if (nRetCode == WSAENOTSOCK) // The descriptor is not a socket. 
			{
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CNetUser::CloseClient closesocket WSAENOTSOCK ERROR");
			}
			else if (nRetCode == WSAEINPROGRESS) // A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. 
			{
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CNetUser::CloseClient closesocket WSAEINPROGRESS ERROR");
			}
			else if (nRetCode == WSAEINTR) // The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall. 
			{
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CNetUser::CloseClient closesocket WSAEINTR ERROR");
			}
			else if (nRetCode == WSAEWOULDBLOCK) // The socket is marked as nonblocking and SO_LINGER is set to a nonzero time-out value. 
			{
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CNetUser::CloseClient closesocket WSAEWOULDBLOCK ERROR");
			}
			else
			{
				CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:CNetUser::CloseClient closesocket unknown ERROR");
			}
		}
		Reset();
	}
	LockOff();
	return NET_OK;
}

void CNetUser::Reset()
{	
	m_Socket			= INVALID_SOCKET;
	m_nNetMode			= NET_PACKET_HEAD; // Packet Head �� �������ΰ� Body �� �������ΰ� ����	
	m_nOnline			= NET_OFFLINE;	
	m_usPort			= 0;
	m_dwLastMsgTime		= 0; // ���������� �б� ����� �� �ð�
	m_dwLastSaveTime	= 0; // ���������� ĳ���͸� ������ �ð�
	m_nPing				= 0; // Ŭ���̾�Ʈ�� ��ſ� �ɸ� �ð�		

	m_dwGaeaID			= GAEAID_NULL;
	m_dwSlotAgentClient	= 0; // Agent ���������� Client ���� ��� ����
	m_dwSlotAgentField	= 0; // Agent ���������� Field �������� ��� ����
	m_dwSlotFieldAgent	= 0; // Field ���������� Agent �������� ��� ����
	m_dwSlotFieldClient	= 0; // Field ���������� Client ���� ��� ����
	m_dwSlotType		= 0; // ������ Ÿ��

	m_uib.nKey			= 0;
	m_uib.nKeyDirection = 0;
	m_uib.nUserNum		= -1;
	m_ck.nKey			= 0;
	m_ck.nKeyDirection  = 0;

	::memset(m_uib.szUserID,	0, USR_ID_LENGTH);
	::memset(m_szIp,			0, IP_LENGTH);
	ResetBuffer();
}

char* CNetUser::GetMsg()
{
	int nSize;

	LockOn();
	
	if (!IsOnline())
	{
		Reset();
		LockOff();
		return NULL;
	}

	if (m_nRcvByte >= sizeof(NET_MSG_GENERIC))
	{
		NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) m_RcvBuffer;
		nSize = (int) nmg->nSize;
        
		// �߸��� ũ���� �޽��� ��� return ��Ų��.
		// ����ڰ� �Ҽ��� �������� ���� ��Ŷ�� ���ɼ��� �ִ�.
		if ((nSize > NET_DATA_BUFSIZE) || (nSize < 0))
		{
			// ����� ������ ���´�.

			// ��� ���۸� ���½��� ������
			ResetBuffer(); 
			LockOff();
			return NULL;
		}		
		if ((m_nRcvByte >= (int) nSize) && (nSize >= sizeof(NET_MSG_GENERIC)))
		{
			::ZeroMemory(m_MsgBuffer, NET_DATA_BUFSIZE);			
			::CopyMemory(m_MsgBuffer, m_RcvBuffer, min(nSize, NET_DATA_BUFSIZE));			
			::MoveMemory(m_RcvBuffer, m_RcvBuffer+nSize, m_nRcvByte-nSize);
			
			m_nRcvByte -= nSize;
			LockOff();
			return m_MsgBuffer;
		}
		else
		{
			LockOff();
			return NULL;
		}
	}
	LockOff();
	return NULL;
}

// ���Ͽ��� �Ѿ�� ����Ÿ�� ���ۿ� ���� �ִ´�
void CNetUser::InsertBuffer(char* buff, DWORD dwSize)
{
	ASSERT(buff);	
	if (dwSize <= 0) return;
	LockOn();	
	if (!IsOnline())
	{
		Reset();
		LockOff();
		return;
	}
	// ������� ����Ÿ ����� ���� ���ۻ����� ���� �۾ƾ� �ϰ�
	// �� ���� ����Ÿ ����� �ִ� ���ۻ������ �۾ƾ� �Ѵ�.
	if ((dwSize > NET_DATA_BUFSIZE) || ((m_nRcvByte+dwSize) > NET_DATA_MSG_BUFSIZE)) 
	{
		CConsoleMessage::GetInstance()->Write("ERROR:%d %s Buffer Over", m_nRcvByte+dwSize, m_szIp);
        ResetBuffer();
	}
	else
	{		
		::CopyMemory(m_RcvBuffer+m_nRcvByte, buff, (int) dwSize);
		m_nRcvByte += (int) dwSize;
	}
	LockOff();
}

// �޽����� ���� ���۸� �����Ѵ�
void CNetUser::ResetBuffer()
{		
	::ZeroMemory(m_RcvBuffer, NET_DATA_MSG_BUFSIZE);
	::ZeroMemory(m_MsgBuffer, NET_DATA_BUFSIZE);
	m_nRcvByte = 0;
}

BOOL CNetUser::IsOnline()
{	
	return m_nOnline == NET_ONLINE ? 1 : 0;
}

void CNetUser::SetOnLine()
{
	m_nOnline = NET_ONLINE;
}

void CNetUser::SetOffLine()
{
	m_nOnline = NET_OFFLINE;
}

void CNetUser::SetSocket(SOCKET s)
{
	m_Socket = s;
}

void CNetUser::SetCryptKey(CRYPT_KEY ck)
{
	LockOn();
	m_ck = ck;
	LockOff();
}

CRYPT_KEY CNetUser::GetCryptKey()
{
	return m_ck;
}

SOCKET CNetUser::GetSocket()
{
	return m_Socket;
}

void CNetUser::SetIP(char* szIP)
{
	::StringCchCopyN(m_szIp, IP_LENGTH, szIP, IP_LENGTH);
}

char* CNetUser::GetIP()
{
	return m_szIp;
}

void CNetUser::SetPort(USHORT usPort)
{
	m_usPort = usPort;
}

USHORT CNetUser::GetPort()
{
	return m_usPort;
}

void CNetUser::SetUserID(const char* szUserID)
{
	::StringCchCopyN(m_uib.szUserID, USR_ID_LENGTH, szUserID, USR_ID_LENGTH);	
}

char* CNetUser::GetUserID()
{
	return m_uib.szUserID;
}

void CNetUser::SetUserNum(int nUserNum)
{
	m_uib.nUserNum = nUserNum;
}

int CNetUser::GetUserNum()
{
	return m_uib.nUserNum;
}

void CNetUser::SetLoginTime()
{
	 m_LoginTime = CTime::GetCurrentTime();
}

void CNetUser::SetNetMode(int nMode)
{
	m_nNetMode = nMode;
}

int	CNetUser::GetNetMode()
{
	return m_nNetMode;
}

void CNetUser::SetGaeaID(DWORD dwGaeaID)
{
	m_dwGaeaID = dwGaeaID;
}

DWORD CNetUser::GetGaeaID()
{
	return m_dwGaeaID;
}

CTime CNetUser::GetLoginTime()
{
	return m_LoginTime;
}

void CNetUser::SetLastMsgTime()
{
	m_dwLastMsgTime	= ::timeGetTime();
}

void CNetUser::SetLastMsgTime(DWORD dwTime)
{
	m_dwLastMsgTime	= dwTime;
}

DWORD CNetUser::GetLastMsgTime()
{
	return m_dwLastMsgTime;
}

void CNetUser::SetLastSaveTime()
{
	m_dwLastSaveTime = ::timeGetTime();
}

void CNetUser::SetLastSaveTime(DWORD dwTime)
{
	m_dwLastSaveTime = dwTime;
}

DWORD CNetUser::GetLastSaveTime()
{
	return m_dwLastSaveTime;
}

void CNetUser::SetSlotAgentClient(DWORD dwSlot) // Agent ���������� Client ���� ��� ����
{
	m_dwSlotAgentClient	= dwSlot;
}
void CNetUser::SetSlotAgentField (DWORD dwSlot) // Agent ���������� Field �������� ��� ����
{
	m_dwSlotAgentField	= dwSlot;
}

void CNetUser::SetSlotFieldAgent (DWORD dwSlot) // Field ���������� Agent �������� ��� ����
{
	m_dwSlotFieldAgent	= dwSlot;
}

void CNetUser::SetSlotFieldClient(DWORD dwSlot) // Field ���������� Client ���� ��� ����
{
	m_dwSlotFieldClient	= dwSlot;
}

DWORD CNetUser::GetSlotAgentClient() // Agent ���������� Client ���� ��� ����
{
	return m_dwSlotAgentClient;
}

DWORD CNetUser::GetSlotAgentField () // Agent ���������� Field �������� ��� ����
{
	return m_dwSlotAgentField;
}

DWORD CNetUser::GetSlotFieldAgent () // Field ���������� Agent �������� ��� ����
{
	return m_dwSlotFieldAgent;
}

DWORD CNetUser::GetSlotFieldClient() // Field ���������� Client ���� ��� ����
{
	return m_dwSlotFieldClient;
}

void CNetUser::SetSlotType(DWORD dwType)
{
	m_dwSlotType = dwType;
}

DWORD CNetUser::GetSlotType()
{
	return m_dwSlotType;
}

BOOL CNetUser::IsClientSlot()
{
	return m_dwSlotType == NET_SLOT_CLIENT ? 1 : 0;
}

BOOL CNetUser::IsFieldSlot()
{
	return m_dwSlotType == NET_SLOT_FIELD ? 1 : 0;
}

BOOL CNetUser::IsAgentSlot()
{
	return m_dwSlotType == NET_SLOT_AGENT ? 1 : 0;
}