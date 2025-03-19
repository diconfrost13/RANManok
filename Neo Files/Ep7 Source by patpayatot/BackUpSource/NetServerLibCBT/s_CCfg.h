///////////////////////////////////////////////////////////////////////////////
// s_CCfg.h
//
// class CCfg
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
// Config file load class head file
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_CCFG_H_
#define S_CCFG_H_

#include "s_NetGlobal.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCfg
{
protected:
	///////////////////////////////////////////////////////////////////////////
	// Server Setting Values
	int		m_nServerVersion;						// ���� ����, Ŭ���̾�Ʈ ������ ����
	int		m_nPatchVersion;						// 
	char	m_szServerName[DB_SVR_NAME_LENGTH];		// ������	
	char	m_szServerIP[IP_LENGTH];				// ���� IP, ���α׷����� �ڵ����� �����ɼ� �ִ�
	char	m_szAgentIP [IP_LENGTH];				// Agent ���� IP
	int		m_nPortService;							// ���񽺿� ��Ʈ
	int		m_nPortControl;							// ��Ʈ�ѿ� ��Ʈ
	int		m_nServerType;							// ���� ����
	int		m_nServerGroup;							// ���� �׷�
	int		m_nServerNumber;						// ���� ��ȣ
	int		m_nServerField;							// ���� �ʵ� ��ȣ
	int		m_nServerMaxClient;						// �ִ� ���� ���� Ŭ���̾�Ʈ ��

	///////////////////////////////////////////////////////////////////////////
	// database setting value 
	char	m_szDBServer[DB_SVR_NAME_LENGTH];		// database server name
	char	m_szDBUser[USR_ID_LENGTH];				// ���Ӱ����� ����� id
	char	m_szDBPass[USR_PASS_LENGTH];			// ��й�ȣ
	char	m_szDBDatabase[DB_NAME_LENGTH];			// database ��
	int		m_nDBPoolSize;							// ���ῡ ����� pool �� size
	int		m_nDBResponseTime;						// ���� ���ð� (sec)

	// database setting value 
	char	m_szUserDBServer[DB_SVR_NAME_LENGTH];	// database server name
	char	m_szUserDBUser[USR_ID_LENGTH];			// ���Ӱ����� ����� id
	char	m_szUserDBPass[USR_PASS_LENGTH];		// ��й�ȣ
	char	m_szUserDBDatabase[DB_NAME_LENGTH];		// database ��
	int		m_nUserDBPoolSize;						// ���ῡ ����� pool �� size
	int		m_nUserDBResponseTime;					// ���� ���ð� (sec)

	// database setting value 
	char	m_szGameDBServer[DB_SVR_NAME_LENGTH];	// database server name
	char	m_szGameDBUser[USR_ID_LENGTH];			// ���Ӱ����� ����� id
	char	m_szGameDBPass[USR_PASS_LENGTH];		// ��й�ȣ
	char	m_szGameDBDatabase[DB_NAME_LENGTH];		// database ��
	int		m_nGameDBPoolSize;						// ���ῡ ����� pool �� size
	int		m_nGameDBResponseTime;					// ���� ���ð� (sec)

	//
	int		m_nGAMEServerNumber;
	int		m_nLOGINServerNumber;
	int		m_nFTPServerNumber;
	int		m_nSESSIONServerNumber;

	G_SERVER_INFO m_sGAMEServer[100];
	G_SERVER_INFO m_sLOGINServer[20];
	G_SERVER_INFO m_sFTPServer[20];
	G_SERVER_INFO m_sSESSIONServer[20];	

	F_SERVER_INFO m_sFIELDServer[FIELDSERVER_MAX];	// Field Server Information

public:
	long ConvertStrToAddr ( const char* szAddr );

public:
	static CCfg* GetInstance();
	static void	ReleaseInstance();

	void				SetDefault(void);
	int					Process(char* sLine);
	int					Load(const char* filename);

	F_SERVER_INFO*		GetFieldServer(int nServerID);
	F_SERVER_INFO*		GetFieldServerArray()		{	return m_sFIELDServer; }

	G_SERVER_INFO*		GetFTPServer(void);
	
	char*				GetSessionServerIP(void);
	int					GetSessionServerPort(void);
	
	int					GetServerGroup(void);	
	int					GetServerNumber(void);
	int					GetServerField(void);
	int					GetServerType(void);
	int					GetServerMaxClient(void);
	int					GetServerVersion(void);
	int					GetPatchVersion(void);
	const char*			GetServerIP(void);
	int					GetServicePort();

	int					GetDBPoolSize(void);
	char*				GetDBServer(void);
	char*				GetDBUser(void);
	char*				GetDBPass(void);
	char*				GetDBDatabase(void);
	int					GetDBResponseTime(void);

	int					GetUserDBPoolSize(void);
	char*				GetUserDBServer(void);
	char*				GetUserDBUser(void);
	char*				GetUserDBPass(void);
	char*				GetUserDBDatabase(void);
	int					GetUserDBResponseTime(void);
	
	int					GetGameDBPoolSize(void);
	char*				GetGameDBServer(void);
	char*				GetGameDBUser(void);
	char*				GetGameDBPass(void);
	char*				GetGameDBDatabase(void);
	int					GetGameDBResponseTime(void);
	char*				GetAgentIP(void);
		
	int					GetControlPort(void);
	int					GetMaxClient(void);

private:
	CCfg();
	CCfg(const char* filename);
	virtual ~CCfg();

	static CCfg* SelfInstance;
};

#endif // S_CCFG_H_