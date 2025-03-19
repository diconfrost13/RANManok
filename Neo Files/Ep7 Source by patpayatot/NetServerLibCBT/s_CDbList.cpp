///////////////////////////////////////////////////////////////////////////////
// s_CDbmanager.cpp
//
// class CDbList
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

///////////////////////////////////////////////////////////////////////////////
// Class CDbList
// This class for MS-SQL Server.
// This is rapper class of DB Library for C
// Present, this class only work for MS-SQL Server.
//
///////////////////////////////////////////////////////////////////////////////
CDbList::CDbList(PLOGINREC hDB, const char* szServerName,  const char* szDBName, int nStartSize)
{
	m_pHead = NULL;
	m_pTail = NULL;
	m_nSize = 0;
	m_pHDB = hDB;
	int i;

	StringCchCopyN(m_szServerName, DB_SVR_NAME_LENGTH, szServerName, DB_SVR_NAME_LENGTH);
	StringCchCopyN(m_szDBName, DB_NAME_LENGTH, szDBName, DB_NAME_LENGTH);

	for (i=0; i < nStartSize; i++)
	{
		AddTail();
	}
}

CDbList::~CDbList()
{
	int i;
	for (i=0; i < m_nSize; i++)
	{		
		RemoveHead();
	}	
}
	
// Returns the head element of the list (cannot be empty). 
DB_LIST* CDbList::GetHead()
{
	return m_pHead;
}
// Returns the tail element of the list (cannot be empty). 
DB_LIST* CDbList::GetTail()
{
	return m_pTail;
}

// Gets the next element for iterating. 
DB_LIST* CDbList::GetNext(DB_LIST* pElement)
{
	return pElement->next;
}

DB_LIST* CDbList::AddTail(DB_LIST* pElement)
{
	// 비어있다면...	
	if (m_nSize == 0)
	{
		pElement->next = NULL;
		pElement->prev = NULL;
		m_pHead = pElement;	
		m_pTail = pElement;		
	}
	else
	{	
		pElement->next = NULL;
		pElement->prev = m_pTail;
		m_pTail->next = pElement;
		m_pTail = pElement;
	}
	m_nSize++;
	return pElement;
}

// Adds an element (or all the elements in another list) to the tail of the list (makes a new tail).  
DB_LIST* CDbList::AddTail()
{
	DB_LIST* pNewElement = NULL;	
	
	pNewElement = (DB_LIST*) HeapAlloc(GetProcessHeap(), 
										HEAP_ZERO_MEMORY, 
										sizeof(DB_LIST));
	
	pNewElement->dbproc = dbopen(m_pHDB, m_szServerName);

	if (pNewElement->dbproc)
	{
		CConsoleMessage::GetInstance()->Write("DB Packet Size %d", dbgetpacket(pNewElement->dbproc));
		DBSETLPACKET(pNewElement->dbproc, DB_PACKET_SIZE);
		CConsoleMessage::GetInstance()->Write("DB Packet Size Change To %d", dbgetpacket(pNewElement->dbproc));
		dbuse(pNewElement->dbproc, m_szDBName);
		CConsoleMessage::GetInstance()->Write("DB Pool open Success");
		return AddTail(pNewElement);
	}
	else // NULL
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "CRITICAL:DB Open Failed. Check DataBase server!");
		return NULL;
	}
}

DB_LIST* CDbList::GetRef()
{	
	DB_LIST* temp = NULL;
	if (m_nSize == 0) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "WARNING:DB Pool size small, next time increase size");		
		temp = AddTail();
	}
	else
	{
		temp = GetHead();
	}
	
	if (temp != NULL)
		RemoveAt(temp); // ??
	
	return temp;
}

// Removes an element from this list, specified by position. 
// 실제로 지우는것은 아니다.
// 실제로 dbproc 을 무효화 시키는 것은 dbclose 밖에 없다.
void CDbList::RemoveAt(DB_LIST* pElement)
{
	if (m_nSize == 0) return;

	assert(pElement);
	dbfreebuf(pElement->dbproc);
	dbcancel(pElement->dbproc);

	if (pElement == m_pHead)
	{
		m_pHead = pElement->next;
		// Check Head and Tail
		if (m_pHead == NULL)
			m_pTail = NULL;
		else
			pElement->next->prev = NULL;
	}
	else
	{
		pElement->prev->next = pElement->next;

		// Check tail
		if (pElement->next == NULL)
			m_pTail = pElement->prev;
		else
			pElement->next->prev = pElement->prev;
	}
	m_nSize--;	
	return;	
}

// Removes the element from the head of the list. 
void CDbList::RemoveHead(void)
{
	DB_LIST* pTemp = NULL;
	
	if (m_pHead == NULL) return;
	
	pTemp = m_pHead->next;
	// Todo : Check memory error...
	if (pTemp != NULL ) 
	{
		dbfreebuf(m_pHead->dbproc);
		dbcancel(m_pHead->dbproc);
		HeapFree(GetProcessHeap(), 0, m_pHead);
		m_pHead = pTemp;
		m_pHead->prev = NULL;
		m_nSize--;
	}
}

// Returns the number of elements in this list. 
int CDbList::GetCount()
{
	return m_nSize;
}

// Tests for the empty list condition (no elements). 
bool CDbList::IsEmpty()
{
	if (m_nSize==0)		return true;
	else				return false;
}