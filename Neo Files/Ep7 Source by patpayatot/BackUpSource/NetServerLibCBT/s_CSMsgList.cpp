///////////////////////////////////////////////////////////////////////////////
// s_CSMsgList.cpp
//
// class CSMsgList
//
// * History
// 2002.05.30 jgkim Create
// 2003.02.04 jgkim 에러처리 추가
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note 
//
// 일반 리스트와는 달리 생성과 함께 일정량의 리스트를 미리 생성한다.
// (new, delete 시간을 감소시키기 위해서이다)
//
// 삽입시에는 남아있는 빈 노드가 없다면 새로운 노드를 생성하고,
// 기존 생성된 비어있는 노드가 있다면 재사용한다.
//
// 삭제시에는 노드를 삭제하지 않는다.
// 노드의 삭제는 객체가 소멸할때만 이루어진다.
//
// 메시지는 head 에서 부터 저장되며, 다음에 메시지가 저장되어야할 위치는 m_pCurrent 이다.
// 메시지를 읽을때는 head 에서부터 m_pCurrent 까지 읽으면 된다.
// 메시지를 하나씩 읽을때 마다 m_pRead 가 증가하면서 다음 읽어야할 메시지를 가르킨다.
// m_pRead 가 m_pCurrent 에 도달하면 null 을 반환하고 더이상 읽어야할 메시지가 없다.
//
// Reset() 을 호출하면 리스트가 초기화되고 다시 메시지를 받을 준비를 한다.
//
// 서버에서는 Front 메시지리스트와 Back 메시지 리스트를 두고
// 통신이 들어오면 Front 메시지 리스트에 메시지를 삽입한다.
// 서버의 전체게임 업데이트 프로시저에서는
// Back 메시지 리스트에서 하나씩 메시지를 꺼내서 처리하고
// 모든 처리가 끝나면 Flip 을 호출하여 Front 와 Back 메시지리스트를 전환한다.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "s_CSMsgList.h"
#include "s_CConsoleMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

CSMsgList::CSMsgList(int nStartSize)
{
	int i;
	m_pHead = NULL;
	m_pTail = NULL;
	m_pCurrent = NULL;
	m_pRead = NULL;
	m_nSize = 0;
	
	for (i=0; i<nStartSize; i++ )
	{
		AddTail();
	}
}

CSMsgList::~CSMsgList()
{	
	int i;
	int nSize = m_nSize;

	for (i=0; i<nSize; i++)
	{		
		RemoveHead();
	}
}
	
// Returns the head element of the list (cannot be empty). 
MSG_LIST* CSMsgList::GetHead()
{
	return m_pHead;
}
// Returns the tail element of the list (cannot be empty). 
MSG_LIST* CSMsgList::GetTail()
{
	return m_pTail;
}

MSG_LIST* CSMsgList::GetCurrent()
{
	return m_pCurrent;
}

MSG_LIST* CSMsgList::GetRead()
{
	return m_pRead;
}

// Gets the next element for iterating. 
MSG_LIST* CSMsgList::GetNext(MSG_LIST* pElement)
{
	return pElement->next;
}

// Adds an element (or all the elements in another list) to the tail of the list (makes a new tail).  
MSG_LIST* CSMsgList::AddTail()
{
	MSG_LIST* pNewElement = NULL;	
	// 메모리할당
	pNewElement = (MSG_LIST*) HeapAlloc(GetProcessHeap(), 
										HEAP_ZERO_MEMORY, 
										sizeof(MSG_LIST));	
	// 비어있다면...
	if (m_nSize == 0)
	{
		pNewElement->next = NULL;
		m_pHead = pNewElement;		
		m_pTail = pNewElement;
		m_pCurrent = m_pHead;
		m_pRead = m_pHead;
	}
	else
	{	
		pNewElement->next = NULL;
		m_pTail->next = pNewElement;
		m_pTail = pNewElement;		
	}
	m_nSize++;	
	return m_pTail;
}

// Removes the element from the head of the list. 
void CSMsgList::RemoveHead(void)
{
	MSG_LIST* pTemp = NULL;
	
	// Check empty list
	if ( m_pHead == NULL ) return;

	LockOn();	
	pTemp = m_pHead->next;
	
	// Todo : Check memory error...	
	if ( m_pCurrent == m_pHead )	m_pCurrent = pTemp;
	else							m_pCurrent = NULL;

	HeapFree(GetProcessHeap(), 0, m_pHead);

	if ( pTemp == NULL ) 
	{		
		m_pHead = NULL;
		m_pTail = NULL;		
	}
	else 
	{	
		m_pHead = pTemp;		
	}
	m_nSize--;
	LockOff();
}

void CSMsgList::AddMsg(DWORD dwClient, char* cBuffer)
{	
	if (m_pCurrent == NULL)
		m_pCurrent = AddTail();
	m_pCurrent->dwClient = dwClient;
	// ::memset(m_pCurrent->Buffer, 0,		NET_DATA_BUFSIZE);
	// ::memcpy(m_pCurrent->Buffer, cBuffer, NET_DATA_BUFSIZE);
	::ZeroMemory(m_pCurrent->Buffer, NET_DATA_BUFSIZE);
	::CopyMemory(m_pCurrent->Buffer, cBuffer, NET_DATA_BUFSIZE);
	m_pCurrent = m_pCurrent->next;
}

MSG_LIST* CSMsgList::GetMsg(void)
{	
	MSG_LIST* pTemp;
	if (m_pRead == m_pCurrent || m_pRead == NULL)
		return NULL;
	pTemp = m_pRead;
	m_pRead = m_pRead->next;
	return pTemp;
}

// Returns the number of elements in this list. 
INT CSMsgList::GetCount()
{
	return m_nSize;
}

// Tests for the empty list condition (no elements). 
BOOL CSMsgList::IsEmpty()
{
	if (m_nSize == 0)
		return TRUE;
	return FALSE;
}

// Clear all message buffer of the list
void CSMsgList::Reset()
{
	m_pCurrent	= m_pHead;
	m_pRead		= m_pHead;
}

///////////////////////////////////////////////////////////////////////////////
// class CSMsgManager
// 서버 메시지 관리기
///////////////////////////////////////////////////////////////////////////////
CSMsgManager::CSMsgManager(int nAmount)
{
	m_pMsgFront = NULL;
	m_pMsgBack  = NULL;

	m_pMsgFront = new CSMsgList(nAmount);
	m_pMsgBack  = new CSMsgList(nAmount);
}

CSMsgManager::~CSMsgManager()
{
	SAFE_DELETE(m_pMsgFront);
	SAFE_DELETE(m_pMsgBack);
}

// Front Buffer 와 Back Buffer 를 Flip 한다
void CSMsgManager::MsgQueueFlip()
{
	CSMsgList* pTemp;
	LockOn();
	pTemp		= m_pMsgFront;
	m_pMsgFront = m_pMsgBack;
	m_pMsgBack	= pTemp;
	m_pMsgFront->Reset();
	LockOff();
}

// Front Buffer 에 메시지를 삽입한다
void CSMsgManager::MsgQueueInsert(int nClient, char* strMsg)
{	
	if (strMsg == NULL) 
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "ERROR:MsgQueueInsert strMsg == NULL");		
		return;
	}
	else
	{
		LockOn();
		m_pMsgFront->AddMsg(nClient, strMsg);
		LockOff();
	}
}

// Back Buffer 에서 메시지를 가져온다
MSG_LIST* CSMsgManager::GetMsg()
{
	MSG_LIST* pTemp = NULL;
	LockOn();
	pTemp = m_pMsgBack->GetMsg();
	LockOff();
	return pTemp;
}