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
	int					m_nClients;			//	���� �������� Ŭ���̾�Ʈ ����	
	int					m_nMaxClient;		//	���Ӱ����� �ִ� Ŭ���̾�Ʈ��
	
	CLIENTS				m_dequeSleepCID;	//	���� Ŭ���̾�Ʈ ID.
	CLIENTS				m_dequePreSleepCID;	//	��ȯ�Ǵ� Ŭ���̾�Ʈ ID.

	CNetUser*			m_pClient;

	HANDLE				m_hIOServer;
	COverlapped*		m_pIOCP;
	SERVER_UTIL::CBit	m_Bit;
    
public :
	BOOL	IsOnline			(DWORD dwClient);
	int		CloseClient			(DWORD dwClient);	// Ư��Ŭ���̾�Ʈ�� ������ �����Ѵ�.
	void	Reset				(DWORD dwClient);
	void	CloseAllClient();						// ��� Ŭ���̾�Ʈ�� ������ �����Ѵ�.
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
	int		GetCurrentClientNumber	(void); // ���� �������� Ŭ���̾�Ʈ ����
	CRYPT_KEY GetNewCryptKey		(void);	

	DWORD	GetSlotAgentClient(DWORD dwClient); // Agent ���������� Client ���� ��� ����
	DWORD 	GetSlotAgentField (DWORD dwClient); // Agent ���������� Field �������� ��� ����
	DWORD	GetSlotFieldAgent (DWORD dwClient); // Field ���������� Agent �������� ��� ����
	DWORD	GetSlotFieldClient(DWORD dwClient); // Field ���������� Client ���� ��� ����
	DWORD	GetSlotType		  (DWORD dwClient);

	void	SetSlotAgentClient(DWORD dwClient, DWORD dwSlot); // Agent ���������� Client ���� ��� ����
	void	SetSlotAgentField (DWORD dwClient, DWORD dwSlot); // Agent ���������� Field �������� ��� ����
	void	SetSlotFieldAgent (DWORD dwClient, DWORD dwSlot); // Field ���������� Agent �������� ��� ����
	void	SetSlotFieldClient(DWORD dwClient, DWORD dwSlot); // Field ���������� Client ���� ��� ����
	void	SetSlotType		  (DWORD dwClient, DWORD dwType);	
};

#endif // S_CCLIENTMANAGER_H_