///////////////////////////////////////////////////////////////////////////////
// s_CCfg.cpp
// 
// class CCfg
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note 
// Config file load class
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include <time.h>
#include "s_CCfg.h"
#include "s_CSystemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCfg* CCfg::SelfInstance = NULL;

CCfg::CCfg()
{
	SetDefault();
}

CCfg::CCfg(const char* filename)
{
	SetDefault();
	Load(filename);
}

CCfg* CCfg::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CCfg();

	return SelfInstance;
}

void CCfg::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

int CCfg::GetMaxClient()
{
	return m_nServerMaxClient;
}

void CCfg::SetDefault(void)
{
	///////////////////////////////////////////////////////////////////////////
	// Server Setting Values
	m_nServerVersion		= 0;	// 서버 버전, 클라이언트 버전과 동일
	m_nPatchVersion			= 0;	// 패치프로그램 버전
	m_nPortService			= 5001;	// 서비스용 포트
	m_nPortControl			= 6001;	// 컨트롤용 포트
	m_nServerType			= 0;	// 서버 유형
	m_nServerGroup			= 0;	// 서버 그룹
	m_nServerNumber			= 0;	// 서버 번호
	m_nServerField			= 0;	// 서버 필드 번호.
	m_nServerMaxClient		= 1000;	// 최대 접속 가능 클라이언트 수

	m_nGAMEServerNumber		= 0;
	m_nLOGINServerNumber	= 0;
	m_nFTPServerNumber		= 0;
	m_nSESSIONServerNumber	= 0;

	///////////////////////////////////////////////////////////////////////////
	// database setting value 
	m_nDBPoolSize		= 10; // 연결에 사용할 pool 의 size
	m_nDBResponseTime = 30; // 쿼리 대기시간 (sec)
}

CCfg::~CCfg()
{
}

int CCfg::Load(const char* filename)
{
	// Data type problem
	// program directory + cfg + filename	
	SERVER_UTIL::CSystemInfo SysInfo;
	CString strAppPath;
	char szAppPath[MAX_PATH];

	strAppPath = SysInfo.GetAppPath ();
	strcpy ( szAppPath, strAppPath.GetBuffer () );
	strcat(szAppPath, "\\cfg\\");
	strcat(szAppPath, filename);

	FILE *oFile;
	char line[300];

	// Open for read 
	if ((oFile = ::fopen(szAppPath, "r" )) == NULL)
	{
		return -1;
	}	

	// Read a line and process
	while (::fgets(line, 200, oFile))
	{
		Process(line);
	}
	
	// Close file
	if (::fclose( oFile ))
	{
		return -1;
	}	
	return 0;
}
int CCfg::Process(char* sLine)
{
	// 토큰으로 분리할 분리자...
	// space, comma, tab, new line
	char seps[]   = " ,\t\n";
	char *token;

	// Establish string and get the first token
	token = ::strtok(sLine, seps);
	while (token != NULL)
	{	
		// 주석일때는 무시...
		if (::strcmp(token, "//") == 0) 
		{
			return 0;
		} 
		//////////////////////////////////////////////////////////////////////
		// server setting value
		else if (::strcmp(token, "server_version") == 0) 
		{
			// 서버 버전, 클라이언트 버전과 동일
			token = ::strtok(NULL, seps);
			if (token)
				m_nServerVersion = atoi(token);
			else
				return 0;
		}
		else if (::strcmp(token, "patch_version") == 0)
		{
			// 패치프로그램 버전
			token = ::strtok(NULL, seps);
			if (token)
				m_nPatchVersion = atoi(token);
			else
				return 0;
		}
		else if (::strcmp(token, "server_name") == 0) 
		{
			// 서버명
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szServerName, token);
			else
				return 0;
		}
		else if (::strcmp(token, "server_max_client") == 0) 
		{
			// 최대 접속 가능 클라이언트 수
			token = ::strtok(NULL, seps );
			if (token)
				m_nServerMaxClient = atoi(token);
			else
				return 0;
		}
		else if (::strcmp(token, "server_ip") == 0) 
		{
			// 서버 IP, 프로그램에서 자동으로 결정될수 있다
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szServerIP, token);
			else
				return 0;
		}
		else if (::strcmp(token, "server_service_port") == 0) 
		{
			// 서비스용 포트
			token = ::strtok(NULL, seps );
			if (token)
				m_nPortService = atoi(token);
			else
				return 0;
		}
		else if (::strcmp(token, "server_control_port") == 0) 
		{
			// 컨트롤용 포트
			token = ::strtok(NULL, seps);
			if (token)
				m_nPortControl = atoi(token);
			else
				return 0;
		}
		else if (::strcmp(token, "server_type") == 0)
		{
			// server type [type]
			// [type]
			// 1 : login server
			// 2 : session server
			// 3 : game server
			token = ::strtok(NULL, seps);
			if (token)
				m_nServerType = atoi(token);
			else
				return 0;
		}
		else if (::strcmp(token, "server_group") == 0)
		{
			// server group
			token = ::strtok(NULL, seps);
			if (token)
				m_nServerGroup = atoi(token);
			else
				return 0;
		}
		else if (::strcmp(token, "server_number") == 0)
		{
			// Server number
			token = ::strtok(NULL, seps);
			if (token)
				m_nServerNumber = atoi(token);
			else
				return 0;
		}
		else if (::strcmp(token, "server_field") == 0 )
		{
			// Server field
			token = ::strtok(NULL, seps);
			if (token)
				m_nServerField = atoi(token);
			else
				return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// database setting value 
		else if (::strcmp(token, "db_server") == 0) 
		{
			// database server name
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szDBServer, token);
			else
				return 0;
		}
		else if (::strcmp(token, "db_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szDBUser, token);
			else
				return 0;
		}
		else if (::strcmp(token, "db_pass") == 0) 
		{
			// 비밀번호
			token = ::strtok(NULL, seps);
			if (token)
				::strcpy(m_szDBPass, token);
			else
				return 0;
		}
		else if (::strcmp(token, "db_database") == 0) 
		{
			// database 명
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szDBDatabase, token);
			else
				return 0;
		}
		else if (::strcmp(token, "db_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::strtok(NULL, seps );
			if (token)
				m_nDBPoolSize = atoi(token);
			else
				return 0;
		}
		else if (::strcmp(token, "db_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::strtok(NULL, seps );
			if (token)
				m_nDBResponseTime = atoi(token);
			else
				return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// User database setting value 
		else if (::strcmp(token, "user_db_server") == 0) 
		{
			// database server name
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szUserDBServer, token);
			else
				return 0;
		}
		else if (::strcmp(token, "user_db_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szUserDBUser, token);
			else
				return 0;
		}
		else if (::strcmp(token, "user_db_pass") == 0) 
		{
			// 비밀번호
			token = ::strtok(NULL, seps);
			if (token)
				::strcpy(m_szUserDBPass, token);
			else
				return 0;
		}
		else if (::strcmp(token, "user_db_database") == 0) 
		{
			// database 명
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szUserDBDatabase, token);
			else
				return 0;
		}
		else if (::strcmp(token, "user_db_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::strtok(NULL, seps );
			if (token)
				m_nUserDBPoolSize = atoi(token);
			else
				return 0;
		}
		else if (::strcmp(token, "user_db_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::strtok(NULL, seps );
			if (token)
				m_nUserDBResponseTime = atoi(token);
			else
				return 0;
		}

		//////////////////////////////////////////////////////////////////////
		// Game database setting value 
		else if (::strcmp(token, "game_db_server") == 0) 
		{
			// database server name
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szGameDBServer, token);
			else
				return 0;
		}
		else if (::strcmp(token, "game_db_user") == 0) 
		{
			// 접속가능한 사용자 id
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szGameDBUser, token);
			else
				return 0;
		}
		else if (::strcmp(token, "game_db_pass") == 0) 
		{
			// 비밀번호
			token = ::strtok(NULL, seps);
			if (token)
				::strcpy(m_szGameDBPass, token);
			else
				return 0;
		}
		else if (::strcmp(token, "game_db_database") == 0) 
		{
			// database 명
			token = ::strtok(NULL, seps );
			if (token)
				::strcpy(m_szGameDBDatabase, token);
			else
				return 0;
		}
		else if (::strcmp(token, "game_db_pool_size") == 0) 
		{
			// 연결에 사용할 pool 의 size
			token = ::strtok(NULL, seps );
			if (token)
				m_nGameDBPoolSize = atoi(token);
			else
				return 0;
		}
		else if (::strcmp(token, "game_db_response_time") == 0) 
		{
			// 쿼리 대기시간 (sec)
			token = ::strtok(NULL, seps );
			if (token)
				m_nGameDBResponseTime = atoi(token);
			else
				return 0;
		}

		/////////////////////////////////////////////////////////////////////////////
		// Other server information
		else if (::strcmp(token, "login_server") == 0) 
		{
			// login_server login1.ran-online.co.kr 211.203.233.100 5001 6001 ran ran
			// login_server [server_name] [ip] [service port] [control port] [userid] [userpass]
			// [server_name]
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sLOGINServer[m_nLOGINServerNumber].szServerName, token);
			else return 0;
			// [ip]
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sLOGINServer[m_nLOGINServerNumber].szServerIP, token);
			else return 0;
			// [service port]
			token = ::strtok(NULL, seps);
			if (token) m_sLOGINServer[m_nLOGINServerNumber].nServicePort = atoi(token);
			else return 0;
			// [control port]
			token = ::strtok(NULL, seps);
			if (token) m_sLOGINServer[m_nLOGINServerNumber].nControlPort = atoi(token);
			else return 0;
			// [userid]
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sLOGINServer[m_nLOGINServerNumber].szUserID, token);
			else return 0;
			// [userpass]
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sLOGINServer[m_nLOGINServerNumber].szUserPass, token);
			else return 0;
			m_nLOGINServerNumber++;
			return 0;
		}
		else if (::strcmp(token, "game_server") == 0) 
		{			
			// game_server [server_name] [ip] [service port] [control port] [userid] [userpass]
			// game_server [servername] [port]			
			token = ::strtok(NULL, seps );
			if (token) ::strcpy(m_sGAMEServer[m_nGAMEServerNumber].szServerName, token);
			else return 0;
			// ip
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sGAMEServer[m_nGAMEServerNumber].szServerIP, token);
			else return 0;
			// service port
			token = ::strtok(NULL, seps);
			if (token) m_sGAMEServer[m_nGAMEServerNumber].nServicePort = atoi(token);
			else return 0;
			// control port
			token = ::strtok(NULL, seps);
			if (token) m_sGAMEServer[m_nGAMEServerNumber].nControlPort = atoi(token);
			else return 0;
			// userid
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sGAMEServer[m_nGAMEServerNumber].szUserID, token);
			else return 0;
			// userpass
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sGAMEServer[m_nGAMEServerNumber].szUserPass, token);
			else return 0;
			m_nGAMEServerNumber++;
			return 0;
		} 		
		else if (::strcmp(token, "ftp_server") == 0) 
		{
			// ftp_server [server_name] [ip] [service port] [control port] [userid] [userpass]		
			// ftp_server [server_name] [ip] [port] [userid] [password]
			// server_name			 
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sFTPServer[m_nFTPServerNumber].szServerName, token);			
			else return 0;
			// ip
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sFTPServer[m_nFTPServerNumber].szServerIP, token);
			else return 0;
			// port
			token = ::strtok(NULL, seps);
			if (token) m_sFTPServer[m_nFTPServerNumber].nServicePort = atoi(token);
			else return 0;
			// userid
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sFTPServer[m_nFTPServerNumber].szUserID, token);
			else return 0;
			// password
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sFTPServer[m_nFTPServerNumber].szUserPass, token);
			else return 0;
			m_nFTPServerNumber++;
			return 0;
		}
		else if (::strcmp(token, "session_server") == 0) 
		{			
			// session_server [server_name] [ip] [port] [userid] [password]
			// server_name			 
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sSESSIONServer[m_nSESSIONServerNumber].szServerName, token);
			else return 0;
			// ip
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sSESSIONServer[m_nSESSIONServerNumber].szServerIP, token);
			else return 0;
			// port
			token = ::strtok(NULL, seps);
			if (token) m_sSESSIONServer[m_nSESSIONServerNumber].nServicePort = atoi(token);
			else return 0;
			// userid
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sSESSIONServer[m_nSESSIONServerNumber].szUserID, token);
			else return 0;
			// password
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_sSESSIONServer[m_nSESSIONServerNumber].szUserPass, token);
			else return 0;
			m_nSESSIONServerNumber++;
			return 0;
		}		
		else if (::strcmp(token, "agent_server") == 0) 
		{			
			// agent_server [server_name] [ip]
			// server_name			 
			token = ::strtok(NULL, seps);
			if (token)
			{
			}
			else return 0;
			// ip
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(m_szAgentIP, token);
			else return 0;
		}
		else if (::strcmp(token, "field_server") == 0) 
		{				
			// field_server [serverID] [server_name] [ip] [port]
			
			int nServerID = 0;

			// [Server id]
			token = ::strtok(NULL, seps);
			if (token)	nServerID = atoi(token);
			else 		return 0;

			if ( nServerID >= FIELDSERVER_MAX )		return 0;

			F_SERVER_INFO sFieldSVR;

			// [server name]
			token = ::strtok(NULL, seps);
			if (token) 	::strcpy(sFieldSVR.szServerName, token);
			else 		return 0;

			// [ip]
			token = ::strtok(NULL, seps);
			if (token) ::strcpy(sFieldSVR.szServerIP, token);
			else return 0;

			// [string ip -> address]
			long nAddr = ConvertStrToAddr ( sFieldSVR.szServerIP );
			if ( nAddr==0 )		return 0;
			sFieldSVR.nServerIP = nAddr;

			// [port]
			token = ::strtok(NULL, seps);
			if (token) sFieldSVR.nServicePort = atoi(token);
			else return 0;
			

			for ( int i=0; i<FIELDSERVER_MAX; i++ )
			{
				if ( m_sFIELDServer[i].nServerIP==sFieldSVR.nServerIP &&
					m_sFIELDServer[i].nServicePort==sFieldSVR.nServicePort )
				{
					MessageBox ( NULL, "field_server [serverID] [server_name] [ip] [port]\r\n"
						"등록중 이미 등록된 ip,port가 발견되었습니다.", "error", MB_OK );
					return 0;
				}
			}

			m_sFIELDServer[nServerID] = sFieldSVR;

			return 0;
		}

		// Get next token
		token = ::strtok( NULL, seps );
	}
	return 0;
}

long CCfg::ConvertStrToAddr ( const char* szAddr )
{
	hostent* hst;	
	char strRet[20];
	unsigned long ulIP;		
	struct in_addr inetAddr;

	hst = ::gethostbyname(szAddr);		
	if ( hst == NULL )	return 0;

	ulIP = *(DWORD*)(*hst->h_addr_list);
	inetAddr.s_addr = ulIP;
	::StringCchCopyN(strRet, 20, inet_ntoa(inetAddr), 20);

	return ::inet_addr(strRet);
}

char* CCfg::GetSessionServerIP(void)
{
	return &m_sSESSIONServer[0].szServerIP[0] ;
}

int	CCfg::GetSessionServerPort(void)
{
	return m_sSESSIONServer[0].nServicePort;
}

F_SERVER_INFO* CCfg::GetFieldServer(int nServerID)
{
	if ( FIELDSERVER_MAX <= nServerID )		return NULL;

	return &m_sFIELDServer[nServerID];
}

G_SERVER_INFO* CCfg::GetFTPServer(void)
{
	int nServer=0;
	srand((unsigned)time(NULL));
	nServer = rand() % m_nFTPServerNumber;
	return &m_sFTPServer[nServer];
}

int CCfg::GetServerVersion()
{
	return m_nServerVersion;
}

int CCfg::GetPatchVersion()
{
	return m_nPatchVersion;
}

int CCfg::GetServicePort()
{
	return m_nPortService;
}

int CCfg::GetControlPort()
{
	return m_nPortControl;
}

int CCfg::GetServerGroup()
{
	return m_nServerGroup;
}

int CCfg::GetServerNumber()
{
	return m_nServerNumber;
}

int CCfg::GetServerField(void)
{
	return m_nServerField;
}

int CCfg::GetServerType()
{
	return m_nServerType;
}

int CCfg::GetServerMaxClient()
{
	return m_nServerMaxClient;
}

const char* CCfg::GetServerIP()
{
	return m_szServerIP;
}

//
//
int CCfg::GetDBPoolSize()
{
	return m_nDBPoolSize;
}

char* CCfg::GetDBServer(void)
{
	return m_szDBServer;
}

char* CCfg::GetDBUser(void)
{
	return m_szDBUser;
}

char* CCfg::GetDBPass(void)
{
	return m_szDBPass;
}

char* CCfg::GetDBDatabase(void)
{
	return m_szDBDatabase;
}

int CCfg::GetDBResponseTime(void)
{
	return m_nDBResponseTime;
}

//
//
int CCfg::GetUserDBPoolSize()
{
	return m_nUserDBPoolSize;
}

char* CCfg::GetUserDBServer(void)
{
	return m_szUserDBServer;
}

char* CCfg::GetUserDBUser(void)
{
	return m_szUserDBUser;
}

char* CCfg::GetUserDBPass(void)
{
	return m_szUserDBPass;
}

char* CCfg::GetUserDBDatabase(void)
{
	return m_szUserDBDatabase;
}

int CCfg::GetUserDBResponseTime(void)
{
	return m_nUserDBResponseTime;
}

//
//
int CCfg::GetGameDBPoolSize()
{
	return m_nGameDBPoolSize;
}

char* CCfg::GetGameDBServer(void)
{
	return m_szGameDBServer;
}

char* CCfg::GetGameDBUser(void)
{
	return m_szGameDBUser;
}

char* CCfg::GetGameDBPass(void)
{
	return m_szGameDBPass;
}

char* CCfg::GetGameDBDatabase(void)
{
	return m_szGameDBDatabase;
}

int CCfg::GetGameDBResponseTime(void)
{
	return m_nGameDBResponseTime;
}

char* CCfg::GetAgentIP(void)
{
	return m_szAgentIP;
}