///////////////////////////////////////////////////////////////////////////////
// s_CServer.h
// class CServer
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_CSERVER_H_
#define S_CSERVER_H_

#include "s_NetGlobal.h"
#include "s_CLock.h"
#include "s_COverlapped.h"
#include "s_CServer.h"
#include "s_CConsoleMessage.h"
#include "s_CDbmanager.h"
#include "s_CSystemInfo.h"
#include "s_CCfg.h"
#include "s_CBit.h"
#include "s_CSMsgList.h"

// Game Logic Define
#include "GLDefine.h"
#include "GLCharDefine.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define S_HEURISTIC_NUM			2 // IOCP Worker thread number per CPU 
#define S_HEURISTIC_QUEUE_SIZE	20 // 큐사이즈는 기본 최대사용자 X S_HEURISTIC_QUEUE_SIZE 배를 형성한다.

#define S_SERVER_STOP			1
#define S_SERVER_RUNING			2
#define S_SERVER_PAUSE			3

class CServer;

class CServer : public CLock
{
public :
	CServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo);
	~CServer();

protected :	
	COverlapped*		m_pIOCP;	
	CDbmanager*			m_pDB;
	COverlapped*		m_pOverlapped;
	CSMsgManager*		m_pMsgManager;
	SERVER_UTIL::CBit	m_Bit;

	HANDLE				m_hWorkerThread[NET_MAX_WORKER_THREAD];
	HANDLE				m_hIOServer;			// IOCP Service 	
	HANDLE				m_hAcceptThread;		// Listening thread handle
	HANDLE				m_hUpdateThread;		// Game Update thread handle
	HWND				m_hEditBox;				// Main window, EditBox handle
	HWND				m_hEditBoxInfo;			// Main window, Info EditBox handle
	HWND				m_hWnd;					// Main window handle
	
	SOCKADDR_IN			m_Addr;
	SOCKET				m_sServer;				// Socket	

	BOOL				m_bIsRunning;			// Status flag
	int					m_nStatus;				// Detail status flag
	int					m_nPort;				// Server port
	int					m_nVersion;				// Server version
	int					m_nPatchVersion;		// Patch Program Version
	int					m_nServerGroup;			// Server Group
	int					m_nServerNum;			// Server Number
	int					m_nMaxClient;			// Maximum number of clients
	char				m_cAddress[20];			// Server IP Address
	char				m_pRecvBuffer[NET_DATA_BUFSIZE];

	DWORD				m_dwWorkerThreadNumber;	// Worker Thread 의 갯수
	DWORD				m_dwCompKey;

	int					m_nPacketCount;			// Packet Counter
	int					m_nPacketSize;			// Packet Size (Byte)
	DWORD				m_dwLastDropClient;		// Last Drop Client;
	
public :
	virtual int			Start()   = 0;
	virtual int			Stop()    = 0;
	virtual	int			Pause()   = 0;
	virtual	int			Resume()  = 0;
	virtual	int			ReStart() = 0;

			int			StartCfg();
			int			StartIOCPList();
			int			StartMsgManager();
			int			StartIOCP();
	virtual	int			StartClientManager() = 0;
			int			StartWorkThread();
			int			StartUpdateThread();
			int			StartListenThread();
	
			int			StopListenThread();
			int			StopIOCPList();
			int			StopWorkThread();
			int			StopIOCP();

	virtual	int			WorkProc()   = 0;		// I/O Process
	virtual int			ListenProc() = 0;		// Accept new Client connection	
	virtual int			UpdateProc() = 0;		// Message Process

			BOOL		IsRunning() { return m_bIsRunning; }
			int			GetStatus() { return m_nStatus; }
			void		SetStatus(int nStatus) { m_nStatus = nStatus; }
};

#endif // S_CSERVER_H_