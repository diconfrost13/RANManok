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
	DWORD				m_dwLastMsgTime;	// ���������� �б� ����� �� �ð�
	DWORD				m_dwLastSaveTime;   // ���������� ĳ���͸� ������ �ð�
	int					m_nPing;			// Ŭ���̾�Ʈ�� ��ſ� �ɸ� �ð�
	int					m_nNetMode;			// Packet Head �� �������ΰ� Body �� �������ΰ� ����	
	CTime				m_LoginTime;		// �α����� ������ �ð�

	DWORD				m_dwGaeaID;			// ���������� '���̾�'������ ����ID.	
	DWORD				m_dwSlotAgentClient;// Agent ���������� Client ���� ��� ����
	DWORD				m_dwSlotAgentField;	// Agent ���������� Field �������� ��� ����
	DWORD				m_dwSlotFieldAgent;	// Field ���������� Agent �������� ��� ����
	DWORD				m_dwSlotFieldClient;// Field ���������� Client ���� ��� ����
	DWORD				m_dwSlotType;

	char				m_RcvBuffer[NET_DATA_MSG_BUFSIZE]; // ����ڰ� ������ ��Ŷ�� �ӽ������� ����
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
	void	SetSlotAgentClient(DWORD dwSlot); // Agent ���������� Client ���� ��� ����
	void	SetSlotAgentField (DWORD dwSlot); // Agent ���������� Field �������� ��� ����
	void	SetSlotFieldAgent (DWORD dwSlot); // Field ���������� Agent �������� ��� ����
	void	SetSlotFieldClient(DWORD dwSlot); // Field ���������� Client ���� ��� ����	
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
	DWORD	GetSlotAgentClient(); // Agent ���������� Client ���� ��� ����
	DWORD 	GetSlotAgentField (); // Agent ���������� Field �������� ��� ����
	DWORD	GetSlotFieldAgent (); // Field ���������� Agent �������� ��� ����
	DWORD	GetSlotFieldClient(); // Field ���������� Client ���� ��� ����	
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