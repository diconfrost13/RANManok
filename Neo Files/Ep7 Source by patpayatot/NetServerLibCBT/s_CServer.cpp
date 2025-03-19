///////////////////////////////////////////////////////////////////////////////
// s_CServer.cpp
//
// class CServer
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note :
// 
///////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include <Mmsystem.h>
#include <stdio.h>
#include "s_CServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static DWORD WINAPI CServerListenProc(CServer *pThis)
{
	return pThis->ListenProc();
}

static DWORD WINAPI CServerWorkProc(CServer *pThis)
{
	return pThis->WorkProc();
}

static DWORD WINAPI CServerUpdateProc(CServer *pThis)
{
	return pThis->UpdateProc();
}

CServer::CServer(HWND hWnd, HWND hEditBox, HWND hEditBoxInfo)
{
	// 초기화
	m_hWnd					= hWnd;	
	m_hEditBox				= hEditBox;	
	m_hEditBoxInfo			= hEditBoxInfo;
	m_hIOServer				= NULL;
	m_hAcceptThread			= NULL;
	m_hUpdateThread			= NULL;
	
	m_pOverlapped			= NULL;
	m_pDB					= NULL;	
	m_pIOCP					= NULL;
	m_pDB					= NULL;
	m_pMsgManager			= NULL;

	m_sServer				= INVALID_SOCKET; // Server Socket	
	
	m_bIsRunning			= FALSE;
	m_nStatus				= S_SERVER_STOP;
	m_dwWorkerThreadNumber	= 0; // Worker Thread 의 갯수
	m_dwCompKey				= 0;
	m_nPort					= 0; // Server port
	m_nVersion				= 0; // Server version
	m_nServerGroup			= 0; // Server group

	m_nPacketCount          = 0; // 서버 처리량 계산을 위한 패킷 카운터 변수
	m_nPacketSize           = 0; // 서버 처리량 계산을 위한 패킷 사이즈 변수
	m_dwLastDropClient		= 0;

	// Create Console Manager
	CConsoleMessage::GetInstance()->SetControl(hEditBox, hEditBoxInfo);
	
	// Set work thread handle to invaild 
	for (int i=0; i < NET_MAX_WORKER_THREAD; i++)
		m_hWorkerThread[i] = INVALID_HANDLE_VALUE;

	// Initialize Database	
	DB_Initialize();
	NET_InitializeSocket();
}

CServer::~CServer()
{
	CCfg::GetInstance()->ReleaseInstance();

	SAFE_DELETE(m_pIOCP);
	// Message Queue 종료
	SAFE_DELETE(m_pMsgManager);
	SAFE_DELETE(m_pDB);
	CConsoleMessage::GetInstance()->ReleaseInstance();	
	// Clear Winsock2
	NET_CloseSocket();
	// Clear Database
	DB_Shutdown();	
	// Releases all resources
}

// Create CFG file loader and open cfg file
int CServer::StartCfg()
{	
	int	nRetCode;
	nRetCode = CCfg::GetInstance()->Load("server.cfg");	
	if (nRetCode != 0)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "CFG file load error");		
		m_bIsRunning = FALSE;
		return NET_ERROR;
	}

	m_nPort			= CCfg::GetInstance()->GetServicePort(); // 포트 저장	
	m_nVersion		= CCfg::GetInstance()->GetServerVersion(); // 버전저장
	m_nPatchVersion = CCfg::GetInstance()->GetPatchVersion(); // 패치프로그램 버전
	m_nServerGroup	= CCfg::GetInstance()->GetServerGroup(); // 서버 그룹
	m_nMaxClient	= CCfg::GetInstance()->GetMaxClient(); // 최대 접속자수
	m_dwCompKey		= (DWORD) ((m_nMaxClient * 2) + 1); // Completion Key	
	::CopyMemory(m_cAddress, CCfg::GetInstance()->GetServerIP(), 20); // 서버 IP

	return NET_OK;
}

// IOCP 리스트를 생성한다.
int CServer::StartIOCPList()
{	
	if (m_pIOCP == NULL)
	{
		m_pIOCP = new COverlapped(m_nMaxClient * 4);
		return NET_OK;
	}
	else
	{
		m_bIsRunning = FALSE;
		return NET_ERROR;
	}
}

// Create message queue
int CServer::StartMsgManager()
{
	if (m_pMsgManager == NULL)
	{
		m_pMsgManager = new CSMsgManager(m_nMaxClient * 4);
		return NET_OK;
	}
	else
	{
		m_bIsRunning = FALSE;
		return NET_ERROR;
	}
}

// Create I/O completion port
int CServer::StartIOCP(void)
{
	if (m_hIOServer == NULL)
		m_hIOServer = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	
	if(m_hIOServer == NULL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "CreateIoCompletionPort Error");
		m_bIsRunning = FALSE;
		return NET_ERROR;
	} 
	else 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "CreateIoCompletionPort OK");
		return NET_OK;
	}
}


// Create IOCP Work Thread
int CServer::StartWorkThread(void)
{
	SERVER_UTIL::CSystemInfo pSInfo;
	char lpPname[100];
	char lpSname[100];
	
	// Determine how many processors are on the system
	// Calcurate number of threads	
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "--------------- System Information -------------------");
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "%s", pSInfo.GetOSName(lpSname));
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "%s %u Mhz", pSInfo.GetProcessorName(lpPname), pSInfo.GetCpuSpeed());
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "%u System Processor Detected", pSInfo.GetProcessorNumber());
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "------------------------------------------------------");
	m_dwWorkerThreadNumber = pSInfo.GetProcessorNumber() * S_HEURISTIC_NUM; // Processor * 2
		
	// Creaate worker threads based on the number of processors	
	DWORD dwThreadId;
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "%d Worker Thread", m_dwWorkerThreadNumber);

	for(DWORD dwCPU=0; dwCPU < m_dwWorkerThreadNumber; dwCPU++)  
    { 		
		HANDLE hThread;
		hThread = ::CreateThread(NULL, 
			0, 
			(LPTHREAD_START_ROUTINE)CServerWorkProc, 
			this, 
			0, 
			&dwThreadId);
		if (hThread == NULL)  
		{ 
			CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "%d Worker Thread Create Error code : %d", dwCPU+1, GetLastError());
			return NET_ERROR;
        } 
		else 
		{
			CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "%d st Worker Thread Create OK", dwCPU+1);
			
			// The system schedules threads on their preferred processors whenever possible.
			::SetThreadIdealProcessor(hThread, dwCPU % S_HEURISTIC_NUM);
        }		
		// store thread handle
		m_hWorkerThread[dwCPU] = hThread;
		::CloseHandle(hThread);
	} 
	return NET_OK;
}


// Start game update thread	
int	CServer::StartUpdateThread()
{
	DWORD	dwThreadId;
	
	m_hUpdateThread = ::CreateThread(NULL, 
								0, 
								(LPTHREAD_START_ROUTINE) CServerUpdateProc, 
								this, 
								0, 
								&dwThreadId);
	if (!m_hUpdateThread)
	{ 
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Server Update Thread Create Failed Error code : %d", GetLastError()); 
		m_bIsRunning = FALSE;
		return NET_ERROR;
	} 
	else 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Server Update Thread Create OK");
		return NET_OK;
	}
}

int	CServer::StartListenThread()
{
	int nRetCode;
	DWORD	dwThreadId;
	///////////////////////////////////////////////////////////////////////////
	// Prepare server socket
	m_sServer = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);	
	if(m_sServer == SOCKET_ERROR) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "CLoginServer::Start WSASocket failed. Error code : %d", WSAGetLastError());
		::WSACleanup();
		m_bIsRunning = FALSE;
		return NET_ERROR;
	} 
	else 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "WSASocket OK");
	}	
	
	//////////////////////////////////////////////////////////////////////////////
	// bind socket
	// 바인드될 로칼어드레스, 2개 이상의 랜카드일때..
	// 주의 : Ipv4 에 대응, ipv6 으로 변경시 코딩변경 필수~!
	struct sockaddr_in local;
		
	// The inet_addr function converts a string 
	// containing an (Ipv4) Internet Protocol dotted address 
	// into a proper address for the IN_ADDR structure.
	const char* a = CCfg::GetInstance()->GetServerIP();

	local.sin_addr.s_addr = ::inet_addr(CCfg::GetInstance()->GetServerIP());
	
	if (local.sin_addr.s_addr == INADDR_NONE)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "inet_addr error");
		local.sin_addr.s_addr = ::htonl(INADDR_ANY);
	}

	local.sin_family = AF_INET;
	local.sin_port = ::htons(m_nPort);

	nRetCode = ::bind(m_sServer, (struct sockaddr *)&local, sizeof(local));
	if (nRetCode == SOCKET_ERROR) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "bind Error. Error Code : %d", WSAGetLastError());
		::closesocket(m_sServer);
		::WSACleanup();
		m_bIsRunning = FALSE;
		return NET_ERROR;		
	} 
	else 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "bind OK");
	}

	//////////////////////////////////////////////////////////////////////////////
	// Server 의 ip Address 를 얻는다.
	char szHostName[128];	
	if (::gethostname(szHostName, 128) == 0 )
	{
		// Get host adresses
		struct hostent * pHost;
		int i; 
		pHost = ::gethostbyname(szHostName); 
		for (i=0; pHost!= NULL && pHost->h_addr_list[i]!= NULL; i++ )
 		{
			char str[20] = "";
 			int j; 
 			for (j=0; j<pHost->h_length; j++)
 			{
				char addr[10];
 				if (j>0)
					::strcat(str, "."); 
				::sprintf(addr, "%u", (unsigned int)((unsigned char*)pHost->h_addr_list[i])[j]); 				
				::strcat(str, addr);
 			}
 		}
	}
	CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE,"%s:%d",m_cAddress, m_nPort);	
	
	///////////////////////////////////////////////////////////////////////////	
	// Set the socket option

    // Disable send buffering on the socket.  Setting SO_SNDBUF 
    // to 0 causes winsock to stop bufferring sends and perform 
    // sends directly from our buffers, thereby reducing CPU usage. 
    int nZero = 0; 
	nRetCode = ::setsockopt(m_sServer, SOL_SOCKET, SO_SNDBUF, (char *)&nZero, sizeof(nZero)); 
    if (SOCKET_ERROR == nRetCode)  
    {         
		m_bIsRunning = FALSE;
        return NET_ERROR;
    }         

	LINGER      lingerStruct;     
	lingerStruct.l_onoff = 1; 
	lingerStruct.l_linger = 0;
	nRetCode = ::setsockopt(m_sServer, SOL_SOCKET, SO_LINGER, 
				(char *)&lingerStruct, sizeof(lingerStruct) ); 
	if (SOCKET_ERROR == nRetCode)  
	{         
		m_bIsRunning = FALSE;
		return NET_ERROR;
	} 

	// Prepare socket for listening
	nRetCode = ::listen(m_sServer, NET_CLIENT_LISTEN);
	if (nRetCode == SOCKET_ERROR) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "listen error. Error code : %d", WSAGetLastError());
		::closesocket(m_sServer);
		::WSACleanup();
		m_bIsRunning = FALSE;
		return NET_ERROR;
	}	
	
	//////////////////////////////////////////////////////////////////////////////
	// Create Listen (Accept) Thread
	m_hAcceptThread = ::CreateThread(NULL, 
								0, 
								(LPTHREAD_START_ROUTINE) CServerListenProc, 
								this, 
								0, &dwThreadId); 

	if (m_hAcceptThread == NULL)  
	{ 
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Server Accept Thread Create Failed Error code : %d", GetLastError()); 
		m_bIsRunning = FALSE;
		return NET_ERROR;
	} 
	else 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Server Accept Thread Create OK");	
		return NET_OK;
	}
}


int	CServer::StopListenThread()
{	
	// Close socket, will close Listen Thread	
	int nRetCode = ::closesocket(m_sServer);
	if (nRetCode == SOCKET_ERROR)
	{
		TRACE("StopListenThread closesocket ERROR \n");
		return NET_ERROR;
	}
	else
	{
		return NET_OK;
	}
}

// Delete iocp queue
int CServer::StopIOCPList()
{	
	int nCount;
	if (m_pIOCP)
	{
		nCount = m_pIOCP->GetUseCount();
		for (int i=0; i<nCount; i++) 
		{
			::PostQueuedCompletionStatus(m_hIOServer,
								0,
								m_dwCompKey, 
								NULL);
		}
		Sleep(1000);
	}
	return NET_OK;
}

// Stop Work Thread	
int CServer::StopWorkThread()
{
	for (DWORD dwNum=0; dwNum<m_dwWorkerThreadNumber; dwNum++)
	{
		::PostQueuedCompletionStatus(m_hIOServer, 
							0, 
							m_dwCompKey, 
							NULL);
	}
	// Wait until all worker thread exit...	
	::Sleep(1000);
	return NET_OK;
}

int	CServer::StopIOCP()
{	
	DWORD dwExitCode;
	if (m_hIOServer) 
	{
		dwExitCode = ::WaitForSingleObject(m_hIOServer, 2000);
		if (dwExitCode == WAIT_FAILED)
			::CloseHandle(m_hIOServer);
		m_hIOServer = NULL;
	}
	return NET_OK;
}