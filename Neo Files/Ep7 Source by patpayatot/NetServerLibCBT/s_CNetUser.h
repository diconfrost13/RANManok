///////////////////////////////////////////////////////////////////////////////
// s_CNetUser.h
//
// * History
// 2003.02.28 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.
// 
// * Note 
// 
///////////////////////////////////////////////////////////////////////////////
#ifndef S_NETUSER_H_
#define S_NETUSER_H_

#include "s_NetGlobal.h"
#include "s_CBit.h"
#include "s_CLock.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNetUser : public CLock
{
public:
	CNetUser();
	~CNetUser();

protected:
	USER_INFO_BASIC		m_uib;
	SOCKET				m_Socket;
	int					m_nOnline;	
	char				m_szIp[IP_LENGTH];
	USHORT				m_usPort;
	DWORD				m_dwLastMsgTime;	// 마지막으로 읽기 통신이 된 시간
	DWORD				m_dwLastSaveTime;   // 마지막으로 캐릭터를 저장한 시간
	int					m_nPing;			// 클라이언트와 통신에 걸린 시간
	int					m_nNetMode;			// Packet Head 를 받을것인가 Body 를 받을것인가 결정	
	CTime				m_LoginTime;		// 로그인을 성공한 시각

	DWORD				m_dwGaeaID;			// 게임참여시 '가이아'에서의 고유ID.	
	DWORD				m_dwSlotAgentClient;// Agent 서버에서의 Client 와의 통신 슬롯
	DWORD				m_dwSlotAgentField;	// Agent 서버에서의 Field 서버와의 통신 슬롯
	DWORD				m_dwSlotFieldAgent;	// Field 서버에서의 Agent 서버와의 통신 슬롯
	DWORD				m_dwSlotFieldClient;// Field 서버에서의 Client 와의 통신 슬롯
	DWORD				m_dwSlotType;

	char				m_RcvBuffer[NET_DATA_MSG_BUFSIZE]; // 사용자가 보내는 패킷을 임시저장할 버퍼
	char				m_MsgBuffer[NET_DATA_BUFSIZE];
	int					m_nRcvByte;
	CRYPT_KEY			m_ck;
	
	SERVER_UTIL::CBit	m_Bit;

public:	
	int		CloseClient();

	BOOL	IsOnline();	
	void	SetOnLine();
	void	SetOffLine();

public:	
	void	Reset();
	void	ResetBuffer();
	void	InsertBuffer(char* buff, DWORD dwSize);	
		
public:	
	void	SetAcceptedClient(SOCKET s);
	void	SetSocket(SOCKET s);
	void	SetIP(char* szIP);
	void	SetLoginTime();	

	void	SetPort(USHORT usPort);
	void	SetNetMode(int nMode);
	void	SetUserID(const char* szUserID);
	void	SetUserNum(int nUserNum);
	void	SetCryptKey(CRYPT_KEY ck);
	void	SetLastMsgTime();
	void	SetLastMsgTime(DWORD dwTime);
	void	SetLastSaveTime();
	void	SetLastSaveTime(DWORD dwTime);

	void	SetGaeaID(DWORD dwGaeaID);
	void	SetSlotAgentClient(DWORD dwSlot); // Agent 서버에서의 Client 와의 통신 슬롯
	void	SetSlotAgentField (DWORD dwSlot); // Agent 서버에서의 Field 서버와의 통신 슬롯
	void	SetSlotFieldAgent (DWORD dwSlot); // Field 서버에서의 Agent 서버와의 통신 슬롯
	void	SetSlotFieldClient(DWORD dwSlot); // Field 서버에서의 Client 와의 통신 슬롯	
	void	SetSlotType		  (DWORD dwType);	

public:	
	SOCKET	GetSocket();
	char*	GetIP();	
	USHORT	GetPort();	
	int		GetNetMode();	
	char*	GetUserID();	
	int		GetUserNum();	
	char*	GetMsg();

	DWORD	GetGaeaID();
	DWORD	GetSlotAgentClient(); // Agent 서버에서의 Client 와의 통신 슬롯
	DWORD 	GetSlotAgentField (); // Agent 서버에서의 Field 서버와의 통신 슬롯
	DWORD	GetSlotFieldAgent (); // Field 서버에서의 Agent 서버와의 통신 슬롯
	DWORD	GetSlotFieldClient(); // Field 서버에서의 Client 와의 통신 슬롯	
	DWORD	GetSlotType();
	BOOL	IsClientSlot();
	BOOL	IsFieldSlot();
	BOOL	IsAgentSlot();

	CTime	GetLoginTime();	
	DWORD	GetLastMsgTime();
	DWORD	GetLastSaveTime();

	CRYPT_KEY GetCryptKey();
};

#endif // S_NETUSER_H_