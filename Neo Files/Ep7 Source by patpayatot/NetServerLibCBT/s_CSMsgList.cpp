///////////////////////////////////////////////////////////////////////////////
// s_CSMsgList.cpp
//
// class CSMsgList
//
// * History
// 2002.05.30 jgkim Create
// 2003.02.04 jgkim ����ó�� �߰�
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note 
//
// �Ϲ� ����Ʈ�ʹ� �޸� ������ �Բ� �������� ����Ʈ�� �̸� �����Ѵ�.
// (new, delete �ð��� ���ҽ�Ű�� ���ؼ��̴�)
//
// ���Խÿ��� �����ִ� �� ��尡 ���ٸ� ���ο� ��带 �����ϰ�,
// ���� ������ ����ִ� ��尡 �ִٸ� �����Ѵ�.
//
// �����ÿ��� ��带 �������� �ʴ´�.
// ����� ������ ��ü�� �Ҹ��Ҷ��� �̷������.
//
// �޽����� head ���� ���� ����Ǹ�, ������ �޽����� ����Ǿ���� ��ġ�� m_pCurrent �̴�.
// �޽����� �������� head �������� m_pCurrent ���� ������ �ȴ�.
// �޽����� �ϳ��� ������ ���� m_pRead �� �����ϸ鼭 ���� �о���� �޽����� ����Ų��.
// m_pRead �� m_pCurrent �� �����ϸ� null �� ��ȯ�ϰ� ���̻� �о���� �޽����� ����.
//
// Reset() �� ȣ���ϸ� ����Ʈ�� �ʱ�ȭ�ǰ� �ٽ� �޽����� ���� �غ� �Ѵ�.
//
// ���������� Front �޽�������Ʈ�� Back �޽��� ����Ʈ�� �ΰ�
// ����� ������ Front �޽��� ����Ʈ�� �޽����� �����Ѵ�.
// ������ ��ü���� ������Ʈ ���ν���������
// Back �޽��� ����Ʈ���� �ϳ��� �޽����� ������ ó���ϰ�
// ��� ó���� ������ Flip �� ȣ���Ͽ� Front �� Back �޽�������Ʈ�� ��ȯ�Ѵ�.
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
	// �޸��Ҵ�
	pNewElement = (MSG_LIST*) HeapAlloc(GetProcessHeap(), 
										HEAP_ZERO_MEMORY, 
										sizeof(MSG_LIST));	
	// ����ִٸ�...
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
// ���� �޽��� ������
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

// Front Buffer �� Back Buffer �� Flip �Ѵ�
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

// Front Buffer �� �޽����� �����Ѵ�
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

// Back Buffer ���� �޽����� �����´�
MSG_LIST* CSMsgManager::GetMsg()
{
	MSG_LIST* pTemp = NULL;
	LockOn();
	pTemp = m_pMsgBack->GetMsg();
	LockOff();
	return pTemp;
}