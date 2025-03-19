///////////////////////////////////////////////////////////////////////////////
// s_COverlapped.cpp
//
// class COverlapped
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note 
//
// Overlapped I/O 를 위한 리스트이다.
//
// 리스트는 전송대기 / 전송요청을 함께 처리한다.
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "s_COverlapped.h"
#include "s_CConsoleMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// class COverlapped
//
COverlapped::COverlapped(int nStartSize)
{
	int i;	
	for (i=0; i<nStartSize; i++)
	{
		m_UnUse.AddTail();
	}
}

COverlapped::~COverlapped()
{	
	
}

LPPER_IO_OPERATION_DATA COverlapped::GetFreeOverIO(int nType)
{	
	LPPER_IO_OPERATION_DATA pTemp;
	int nSize = 0;
	
	LockOn();
	
	if (m_UnUse.GetSize() < 10)
	{
		CConsoleMessage::GetInstance()->Write(C_MSG_FILE_CONSOLE, "Warning! Overlapped queue size small !");
		for (int i=0; i<50; i++)
		{
			m_UnUse.AddHead();		
		}
	}
	pTemp = (LPPER_IO_OPERATION_DATA) m_UnUse.Get();	
	// pTemp->Reset();
	pTemp->OperationType = nType;
	pTemp->dwRcvBytes = 0;
	pTemp->dwSndBytes = 0;
	pTemp->dwTotalBytes = 0;	
	m_Use.AddTail((MEM_POOLER::NODE<PER_IO_OPERATION_DATA>*) pTemp);		
	LockOff();

	return pTemp;
}

void COverlapped::Release(LPPER_IO_OPERATION_DATA pElement)
{
	LockOn();
	m_Use.Release  ((MEM_POOLER::NODE<PER_IO_OPERATION_DATA>*) pElement);
	m_UnUse.AddTail((MEM_POOLER::NODE<PER_IO_OPERATION_DATA>*) pElement);	
	LockOff();
}

int COverlapped::GetUseCount(void)
{
	return m_Use.GetSize();
}

int COverlapped::GetUnUseCount(void)
{
	return m_UnUse.GetSize();
}