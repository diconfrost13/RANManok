///////////////////////////////////////////////////////////////////////////////
// s_CClientManager.h
//
// class CClientManager 
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
// 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_CCLIENTMANAGER_H_
#define S_CCLIENTMANAGER_H_

#include "s_NetGlobal.h"
#include "s_CLock.h"
#include "s_CNetUser.h"
#include "s_COverlapped.h"
#include "s_CBit.h"
#include "s_CCfg.h"

#include <deque>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CClientManager : public CLock
{
	typedef std::deque<int>			CLIENTS;
	typedef CLIENTS::iterator		CLIENTS_ITER;

public :
	CClientManager(int nMaxClient, COverlapped* pOverlapped, HANDLE	hIOServer);
	virtual ~CClientManager();

protected :	
	int					m_nClients;			//	현재 접속중인 클라이언트 숫자	
	int					m_nMaxClient;		//	접속가능한 최대 클라이언트수
	
	CLIENTS				m_dequeSleepCID;	//	유휴 클라이언트 ID.
	CLIENTS				m_dequePreSleepCID;	//	반환되는 클라이언트 ID.

	CNetUser*			m_pClient;

	HANDLE				m_hIOServer;
	COverlapped*		m_pIOCP;
	SERVER_UTIL::CBit	m_Bit;
    
public :
	BOOL	IsOnline			(DWORD dwClient);
	int		CloseClient			(DWORD dwClient);	// 특정클라이언트의 연결을 종료한다.
	void	Reset				(DWORD dwClient);
	void	CloseAllClient();						// 모든 클라이언트의 연결을 종료한다.
	void	ResetPreSleepCID();
	int		GetFreeClientID		(void);	

public :
	void	SetAcceptedClient	(DWORD dwClient, SOCKET sSocket);
	void	SetNetMode			(DWORD dwClient, int nMode);
	void	SetCryptKey			(DWORD dwClient, CRYPT_KEY ck);
    	
public :
	void	InsertBuffer		(DWORD dwClient, char* buff, DWORD dwSize);
	void	ResetBuffer			(DWORD dwClient);	
	char*	GetMsg				(DWORD dwClient);

public :
	int SendClient(DWORD dwClient, LPVOID pBuffer);
	int SendClient(DWORD dwClient, LPPER_IO_OPERATION_DATA PerIoData, DWORD dwSize);

public :
	int		GetNetMode			(DWORD dwClient);	
	char*	GetClientIP			(DWORD dwClient);
	SOCKET	GetSocket			(DWORD dwClient);
	char*	GetIP				(DWORD dwClient);
	CRYPT_KEY GetCryptKey		(DWORD dwClient);
	
public :
	int		GetMaxClient			(void); // Return max client number
	int		GetCurrentClientNumber	(void); // 현재 접속중인 클라이언트 숫자
	CRYPT_KEY GetNewCryptKey		(void);	

	DWORD	GetSlotAgentClient(DWORD dwClient); // Agent 서버에서의 Client 와의 통신 슬롯
	DWORD 	GetSlotAgentField (DWORD dwClient); // Agent 서버에서의 Field 서버와의 통신 슬롯
	DWORD	GetSlotFieldAgent (DWORD dwClient); // Field 서버에서의 Agent 서버와의 통신 슬롯
	DWORD	GetSlotFieldClient(DWORD dwClient); // Field 서버에서의 Client 와의 통신 슬롯
	DWORD	GetSlotType		  (DWORD dwClient);

	void	SetSlotAgentClient(DWORD dwClient, DWORD dwSlot); // Agent 서버에서의 Client 와의 통신 슬롯
	void	SetSlotAgentField (DWORD dwClient, DWORD dwSlot); // Agent 서버에서의 Field 서버와의 통신 슬롯
	void	SetSlotFieldAgent (DWORD dwClient, DWORD dwSlot); // Field 서버에서의 Agent 서버와의 통신 슬롯
	void	SetSlotFieldClient(DWORD dwClient, DWORD dwSlot); // Field 서버에서의 Client 와의 통신 슬롯
	void	SetSlotType		  (DWORD dwClient, DWORD dwType);	
};

#endif // S_CCLIENTMANAGER_H_