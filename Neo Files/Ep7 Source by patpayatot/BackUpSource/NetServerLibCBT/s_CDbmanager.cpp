///////////////////////////////////////////////////////////////////////////////
// s_CDbmanager.cpp
//
// class CDbmanager
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
#include "s_CDbmanager.h"
#include "GLogicData.h"
#include "s_CCfg.h"
#include <strstream>
#include "GLCharAG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDbmanager* CDbmanager::SelfInstance = NULL;

///////////////////////////////////////////////////////////////////////////////
// Class CDbmanager
// This class for MS-SQL Server.
// This is rapper class of DB Library for C
// Present, this class only work for MS-SQL Server.
//
///////////////////////////////////////////////////////////////////////////////
CDbmanager::CDbmanager(void)
{
	m_nGameDBPoolSize	= 5;
	m_nGameDBTimeOut	= 10;
	m_nUserDBPoolSize	= 5;
	m_nUserDBTimeOut	= 10;

	m_pGameDB		= NULL;
	m_pUserDB		= NULL;
}

CDbmanager::~CDbmanager(void)
{
	SAFE_DELETE(m_pGameDB);
	SAFE_DELETE(m_pUserDB);
}

CDbmanager* CDbmanager::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CDbmanager();
	return SelfInstance;
}

void CDbmanager::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
void CDbmanager::SetGameDBPoolSize(int nSize)
{
	m_nGameDBPoolSize = nSize;
}

void CDbmanager::SetGameDBTimeOut(int nSize)
{
	m_nGameDBTimeOut = nSize;
}

void CDbmanager::SetGameServerName(const char* szName)
{
	StringCchCopyN(m_szGameServerName, DB_SVR_NAME_LENGTH, szName, DB_SVR_NAME_LENGTH);
}

void CDbmanager::SetGameUserID(const char* szUsrID)
{
	StringCchCopyN(m_szGameUsrID, USR_ID_LENGTH, szUsrID, USR_ID_LENGTH);
}

void CDbmanager::SetGameUserPass(const char* szUserPasswd)
{
	StringCchCopyN(m_szGameUsrPass, USR_PASS_LENGTH, szUserPasswd, USR_PASS_LENGTH);
}

void CDbmanager::SetGameDBName(const char* szDBName)
{
	StringCchCopyN(m_szGameDBName, DB_NAME_LENGTH, szDBName, DB_NAME_LENGTH);
}

///////////////////////////////////////////////////////////////////////////////
void CDbmanager::SetUserDBPoolSize(int nSize)
{
	m_nUserDBPoolSize = nSize;
}

void CDbmanager::SetUserDBTimeOut(int nSize)
{
	m_nUserDBTimeOut = nSize;
}

void CDbmanager::SetUserServerName(const char* szName)
{
	StringCchCopyN(m_szUserServerName, DB_SVR_NAME_LENGTH, szName, DB_SVR_NAME_LENGTH);
}

void CDbmanager::SetUserUserID(const char* szUsrID)
{
	StringCchCopyN(m_szUserUsrID, USR_ID_LENGTH, szUsrID, USR_ID_LENGTH);
}

void CDbmanager::SetUserUserPass(const char* szUserPasswd)
{
	StringCchCopyN(m_szUserUsrPass, USR_PASS_LENGTH, szUserPasswd, USR_PASS_LENGTH);
}

void CDbmanager::SetUserDBName(const char* szDBName)
{
	StringCchCopyN(m_szUserDBName, DB_NAME_LENGTH, szDBName, DB_NAME_LENGTH);
}

///////////////////////////////////////////////////////////////////////////////
void CDbmanager::SetGameDBServer(const char* szName,
						const char* szUsrID,
						const char* szUsrPass,
						const char* szDBName,
						int nPoolSize,
						int nTimeOut)
{
	SetGameServerName	(szName);
	SetGameUserID		(szUsrID);
	SetGameUserPass		(szUsrPass);
	SetGameDBName		(szDBName);
	SetGameDBPoolSize	(nPoolSize);
	SetGameDBTimeOut	(nTimeOut);
}

void CDbmanager::SetUserDBServer(const char* szName,
						const char* szUsrID,
						const char* szUsrPass,
						const char* szDBName,
						int nPoolSize,
						int nTimeOut)
{
	SetUserServerName	(szName);
	SetUserUserID		(szUsrID);
	SetUserUserPass		(szUsrPass);
	SetUserDBName		(szDBName);
	SetUserDBPoolSize	(nPoolSize);
	SetUserDBTimeOut	(nTimeOut);
}

int CDbmanager::OpenGameDB(char* szSvrName, 
					char* szUsrID, 
					char* szUsrPasswd,
					char* szDbName,
					int nPoolSize,
					int nTimeOut)
{
	SetGameDBServer(szSvrName, szUsrID, szUsrPasswd, szDbName, nPoolSize, nTimeOut);
	return OpenGameDB();
}

int CDbmanager::OpenUserDB(char* szSvrName, 
					char* szUsrID, 
					char* szUsrPasswd,
					char* szDbName,
					int nPoolSize,
					int nTimeOut)
{
	SetUserDBServer(szSvrName, szUsrID, szUsrPasswd, szDbName, nPoolSize, nTimeOut);
	return OpenUserDB();
}

int CDbmanager::OpenGameDB()
{
	int nRetCode = 0;

	SAFE_DELETE(m_pGameDB);
	
	m_pGameDB = new CDbSupervisor();
	nRetCode = m_pGameDB->Open(m_szGameServerName,
						m_szGameUsrID,
						m_szGameUsrPass,
						m_szGameDBName,
						m_nGameDBPoolSize,
						m_nGameDBTimeOut);

	return DB_OK;
}

int CDbmanager::OpenUserDB()
{
	int nRetCode = 0;

	SAFE_DELETE(m_pUserDB);

	m_pUserDB = new CDbSupervisor();
	
	if (m_pUserDB == NULL) return DB_ERROR;
		
	nRetCode = m_pUserDB->Open(m_szUserServerName,
						m_szUserUsrID,
						m_szUserUsrPass,
						m_szUserDBName,
						m_nUserDBPoolSize,
						m_nUserDBTimeOut);
	return DB_OK;
}

DB_LIST* CDbmanager::GetFreeConnectionGame(void)
{
	DB_LIST* temp = m_pGameDB->GetFreeConnection();
	
	assert(temp);
	if ( temp == NULL ) 
		return NULL;
	return temp;
}

DB_LIST* CDbmanager::GetFreeConnectionUser(void)
{
	DB_LIST* temp = m_pUserDB->GetFreeConnection();
	
	assert(temp);
	if ( temp == NULL ) 
		return NULL;
	return temp;
}

void CDbmanager::ReleaseConnectionGame(DB_LIST* pDB)
{
	m_pGameDB->ReleaseConnection(pDB);	
}

void CDbmanager::ReleaseConnectionUser(DB_LIST* pDB)
{
	m_pUserDB->ReleaseConnection(pDB);	
}



//
//                __                             ___            _aaaa
//               d8888aa,_                    a8888888a   __a88888888b
//              d8P   `Y88ba.                a8P'~~~~Y88a888P""~~~~Y88b
//             d8P      ~"Y88a____aaaaa_____a8P        888          Y88
//            d8P          ~Y88"8~~~~~~~88888P          88g          88
//           d8P                           88      ____ _88y__       88b
//           88                           a88    _a88~8888"8M88a_____888
//           88                           88P    88  a8"'     `888888888b_
//          a8P                           88     88 a88         88b     Y8,
//           8b                           88      8888P         388      88b
//          a88a                          Y8b       88L         8888.    88P
//         a8P                             Y8_     _888       _a8P 88   a88
//        _8P                               ~Y88a888~888g_   a888yg8'  a88'
//        88                                   ~~~~    ~""8888        a88P
//       d8'                                                Y8,      888L
//       8E                                                  88a___a8"888
//      d8P                                                   ~Y888"   88L
//      88                                                      ~~      88
//      88                                                              88
//      88                                                              88b
//  ____88a_.      a8a                                                __881
//88""P~888        888b                                 __          8888888888
//      888        888P                                d88b             88
//     _888ba       ~            aaaa.                 8888            d8P
// a888~"Y88                    888888                 "8P          8aa888_
//        Y8b                   Y888P"                                88""888a
//        _88g8                  ~~~                                 a88    ~~
//    __a8"888_                                                  a_ a88
//   88"'    "88g                                                 "888g_
//   ~         `88a_                                            _a88'"Y88gg,
//                "888aa_.                                   _a88"'      ~88
//                   ~~""8888aaa______                ____a888P'
//                           ~~""""""888888888888888888""~~~
//                                      ~~~~~~~~~~~~
// jgkim
