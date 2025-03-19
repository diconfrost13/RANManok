///////////////////////////////////////////////////////////////////////////////
// s_CDbmanager.h
//
// class CDbmanager
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CDBMANAGER_H_
#define _CDBMANAGER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define	DBNTWIN32		   // must identify operating system environment

#include "GLDBMan.h"

// DB lib
// This lib will can find "mssql/devtools/include" and "mssql/devtools/x86lib"
// odbccp32.lib 
#ifdef WIN32
	#pragma comment (lib,"ntwdblib.lib")
//	#pragma message ("Auto linking ntwdblib.lib library")
#endif

#include <windows.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <sqlfront.h> 
#include <sqldb.h> 
#include <time.h>
#include "s_NetGlobal.h"
#include "s_CLock.h"
#include "s_CConsoleMessage.h"
#include "GLCharData.h"
#include "GLContrlMsg.h"

// Seconds of database response time
#define DB_RESPONSE_TIME	30
#define DB_CONNECTION_COUNT 10

#define DB_ERROR			-1
#define DB_OK				0

#define DB_USE				1
#define DB_NOT_USE			0

#define DB_CHA_MAX			-2
#define DB_CHA_DUF			-3

#ifndef DB_PACKET_SIZE
#define DB_PACKET_SIZE		8196
#endif
#ifndef DB_IMAGE_BUF_SIZE
#define DB_IMAGE_BUF_SIZE	4096
#endif
#ifndef DB_IMAGE_MIN_SIZE
#define DB_IMAGE_MIN_SIZE   12
#endif
//	Global functions
int		DB_Initialize(void);
void	DB_Shutdown(void);

// Forward declarations of the error handler and message handler.
int		DB_err_handler(PDBPROCESS, int, int, int, char*, char*);
int		DB_msg_handler(PDBPROCESS, DBINT, int, int, char*);

///////////////////////////////////////////////////////////////////////////////
// struct DB_LIST
//
///////////////////////////////////////////////////////////////////////////////
struct DB_LIST
{
	PDBPROCESS dbproc;
	DB_LIST* prev;
	DB_LIST* next;
};
typedef  DB_LIST* LPDB_LIST;

struct GLCHARAG_DATA;

///////////////////////////////////////////////////////////////////////////////
// class CDbList
// 
///////////////////////////////////////////////////////////////////////////////
class CDbList : public CLock
{
public:	
	CDbList(PLOGINREC hDB, const char* szServerName, const char* szDBName, int nStartSize = 0);
	~CDbList();

protected:
	int			m_nSize;
	PLOGINREC	m_pHDB;
	char		m_szServerName[DB_SVR_NAME_LENGTH];
	char		m_szDBName[DB_NAME_LENGTH];
	DB_LIST*	m_pHead;
	DB_LIST*	m_pTail;

public:	
	DB_LIST* GetHead(void); // Returns the head element of the list (cannot be empty). 	
	DB_LIST* GetTail(void); // Returns the tail element of the list (cannot be empty). 	
	DB_LIST* GetNext(DB_LIST* pElement); // Gets the next element for iterating. 		
	DB_LIST* AddTail(); // Adds an element (or all the elements in another list) to the tail of the list (makes a new tail).  
	DB_LIST* AddTail(DB_LIST* pElement);
	DB_LIST* GetRef();
	
	void RemoveAt(DB_LIST* pElement); // Removes an element from this list, specified by position. 	
	void RemoveHead(void); // Removes the element from the head of the list. 	
	void RemoveTail(void); // Removes the element from the tail of the list.  
	void RemoveAll(); // Returns the number of elements in this list. 	
	int  GetCount(); // Tests for the empty list condition (no elements). 	
	bool IsEmpty();
};

///////////////////////////////////////////////////////////////////////////////
// class CDbSupervisor
//
///////////////////////////////////////////////////////////////////////////////
class CDbSupervisor : public CLock
{
public:
	CDbSupervisor();
	~CDbSupervisor();

protected:
	PLOGINREC	m_pLogin;   // allocate	a DB-LIB login structure
	RETCODE		m_nRetCode; // Return code
	
	CDbList*	m_pUse;
	CDbList*	m_pUnUse;

	// DB connection information
	char		m_szServerName[DB_SVR_NAME_LENGTH];
	char		m_szUsrID[USR_ID_LENGTH];
	char		m_szUsrPass[USR_PASS_LENGTH];
	char		m_szDBName[DB_NAME_LENGTH];
	int			m_nDBPoolSize;
	int			m_nDBTimeOut;

public:
	void		SetServerName	(const char* szName);
	void		SetUserID		(const char* szUsrID);
	void		SetUserPass		(const char* szUsrPasswd);
	void		SetDBName		(const char* szDBName);
	void		SetDBServer		(const char* szName,
								const char* szUsrID,
								const char* szUsrPass,
								const char* szDBName,
								int nPoolSize,
								int nTimeOut);
	void		SetDBTimeOut	(int nTimeSec);
	void		SetDBPoolSize	(int nSize);
	
	int			Open();
	int			Open(char* szSvrName, 
					char* szUsrID, 
					char* szUsrPasswd,
					char* szDbName,
					int nPoolSize,
					int nTimeOut);
	DB_LIST*	GetFreeConnection(void);
	void		ReleaseConnection(DB_LIST* pDB);
};

///////////////////////////////////////////////////////////////////////////////
// Class CDbmanager
// This class for MS-SQL Server.
// This is rapper class of DB Library for C
// Present, this class only work for MS-SQL Server.
///////////////////////////////////////////////////////////////////////////////
class CDbmanager : public CLock, public GLDBMan
{
public:
	CDbmanager();
	~CDbmanager();
	static CDbmanager* GetInstance();
	static void ReleaseInstance();
	static CDbmanager* SelfInstance;

protected:
	RETCODE		nRetCode; // Return code
	
	CDbSupervisor* m_pGameDB;
	CDbSupervisor* m_pUserDB;
	
	// DB connection information
	char		m_szGameServerName[DB_SVR_NAME_LENGTH];
	char		m_szUserServerName[DB_SVR_NAME_LENGTH];

	char		m_szGameUsrID[USR_ID_LENGTH];
	char		m_szUserUsrID[USR_ID_LENGTH];

	char		m_szGameUsrPass[USR_PASS_LENGTH];
	char		m_szUserUsrPass[USR_PASS_LENGTH];

	char		m_szGameDBName[DB_NAME_LENGTH];
	char		m_szUserDBName[DB_NAME_LENGTH];

	int			m_nGameDBPoolSize;
	int			m_nUserDBPoolSize;

	int			m_nGameDBTimeOut;
	int			m_nUserDBTimeOut;
	
public:
	void		SetGameServerName	(const char* szName);
	void		SetUserServerName	(const char* szName);

	void		SetGameUserID		(const char* szUsrID);
	void		SetUserUserID		(const char* szUsrID);

	void		SetGameUserPass		(const char* szUsrPasswd);
	void		SetUserUserPass		(const char* szUsrPasswd);

	void		SetGameDBName		(const char* szDBName);
	void		SetUserDBName		(const char* szDBName);

	void		SetGameDBServer		(const char* szName,
									const char* szUsrID,
									const char* szUsrPass,
									const char* szDBName,
									int nPoolSize,
									int nTimeOut);
	void		SetUserDBServer		(const char* szName,
									const char* szUsrID,
									const char* szUsrPass,
									const char* szDBName,
									int nPoolSize,
									int nTimeOut);
	
	void		SetGameDBTimeOut	(int nSize);
	void		SetUserDBTimeOut	(int nSize);

	void		SetGameDBPoolSize	(int nSize);
	void		SetUserDBPoolSize	(int nSize);
	
	int			OpenGameDB();
	int			OpenUserDB();

	int			OpenGameDB(char* szSvrName, 
					char* szUsrID, 
					char* szUsrPasswd,
					char* szDbName,
					int nPoolSize,
					int nTimeOut);
	int			OpenUserDB(char* szSvrName, 
					char* szUsrID, 
					char* szUsrPasswd,
					char* szDbName,
					int nPoolSize,
					int nTimeOut);

	DB_LIST*	GetFreeConnectionGame(void);
	DB_LIST*	GetFreeConnectionUser(void);

	void		ReleaseConnectionGame(DB_LIST* pDB);	
	void		ReleaseConnectionUser(DB_LIST* pDB);
			
			
	///////////////////////////////////////////////////////////////////////////
	// DB Function
	int			Command			(char* szCmdStr);
	
	int			WriteImage		(const char* objName, int nChaNum, char* pData, int nSize);
	int			ReadImage		(const char* objName, int nChaNum, CByteStream &ByteStream);	

	int			MakeUserInven	(int SGNum, DWORD dwUserNum);
	int			WriteUserInven  (int SGNum, LONGLONG llMoney /* LONGLONG __int64 */, DWORD dwUserNum, char* pData, int nSize); // 사용자 인벤토리에 데이타를 저장한다.

	virtual int	ReadUserInven	(SCHARDATA2* pChaData2);
	int	        ReadUserInven   (int SGNum, DWORD dwUserNum, CByteStream &ByteStream);	// 사용자 인벤토리에서 데이타를 읽어온다.	
	bool        CheckInven      (int SGNum, DWORD dwUserNum);							// 유저인벤토리가 있는지 체크한다.
	
	int			CreateNewCharacter ( SCHARDATA2* pCharData2 );	// 새로운 캐릭터를 생성한다.	
	virtual int	SaveCharacter ( LPVOID _pbuffer );				// 캐릭터를 저장한다

	int			DelCharacter	(int nUsrNum, int nChaNum, const char* szPass2); // 캐릭터 삭제
	
	int			UserCheck		(const char* szUsrID, const char* szPasswd, 
								const char* szUsrIP, int nSvrGrp, int nSvrNum); // 로그인하려는 사용자를 체크한다.	
	int			UserLogout		(const char* szUsrID, int nUsrNum, int nGameTime); // 해당사용자를 Logout 한다
	int			UserLogoutSimple(const char* szUsrID);
	int			UserLogoutSvr	(int nSvrGrp, int nSvrNum); // 해당 게임서버의 전체 유저를 로그아웃 시킨다.
	int			UserUpdateCha	(int nUsrNum, const char* szChaName);

	// 사용자 정보를 가져온다.	
	USER_INFO_BASIC GetUserInfo (const char* szUsrId, const char* szPasswd);
	int			GetUserNum      (const char* szUsrId); // 해당 id 사용자의 사용자 번호를 가져온다.
	
	SCHARDATA*	GetCharacter    (int nChaNum);
	int			GetCharacterInfo(int nChaNum, SCHARDATA2* pChaData2);
	int			GetCharacterInfo(int nChaNum, GLCHARAG_DATA* pChaData);
	
	int			GetChaAllInfo   (int nUsrNum); // 해당 사용자의 캐릭터 정보를 가져온다.	
	int 		GetChaBAInfo    (int nUsrNum, int nSvrGrp, NET_CHA_BBA_INFO* ncbi);
	int			GetChaBInfo     (int nChaNum, SCHARINFO_LOBBY* sci);

	int			AddBlockIPList	(char* szIP, char* szReason);	
};

#endif // _CDBMANAGER_H_

//       _==/          i     i          \==_
//     /XX/            |\___/|            \XX\
//   /XXXX\            |XXXXX|            /XXXX\
//  |XXXXXX\_         _XXXXXXX_         _/XXXXXX|
// XXXXXXXXXXXxxxxxxxXXXXXXXXXXXxxxxxxxXXXXXXXXXXX
//|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|
// XXXXXX/^^^^"\XXXXXXXXXXXXXXXXXXXXX/^^^^^\XXXXXX
//  |XXX|       \XXX/^^\XXXXX/^^\XXX/       |XXX|
//    \XX\       \X/    \XXX/    \X/       /XX/
//       "\       "      \X/      "      /"
//
// A U BATMAN ? 
// Help Me! Batman!
// jgkim