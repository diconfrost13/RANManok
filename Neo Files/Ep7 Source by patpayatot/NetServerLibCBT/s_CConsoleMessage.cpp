///////////////////////////////////////////////////////////////////////////////
// s_CConsoleMessage.cpp
//
// class CConsoleMessage
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
#include <stdio.h>
#include "s_CConsoleMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CConsoleMessage* CConsoleMessage::SelfInstance = NULL;

CConsoleMessage::CConsoleMessage(int nType)
{	
	m_pEditBox = NULL;	
	m_pEditBoxInfo = NULL;
	m_nDefaultType = nType;
}

CConsoleMessage::~CConsoleMessage()
{
}

CConsoleMessage* CConsoleMessage::GetInstance()
{
	if (SelfInstance == NULL)
		SelfInstance = new CConsoleMessage();
	return SelfInstance;
}

void CConsoleMessage::ReleaseInstance()
{
	if (SelfInstance != NULL)
	{
		delete SelfInstance;
		SelfInstance = NULL;
	}
}

void CConsoleMessage::SetControl(HWND hControl, HWND hEditBoxInfo)
{
	m_pEditBox = hControl;
	m_pEditBoxInfo = hEditBoxInfo;
	// only accept n k of text
	// n * 1024 = n k
	SendMessage(m_pEditBox,     EM_LIMITTEXT, (WPARAM) (64 * 1024), (LPARAM) 0);
	SendMessage(m_pEditBoxInfo, EM_LIMITTEXT, (WPARAM) (32 * 1024), (LPARAM) 0);	
}

HWND CConsoleMessage::GetControl(void)
{
	return m_pEditBox;
}

void CConsoleMessage::Write(const char* msg, ...)
{
	char sBuf[C_BUFFER_SIZE];	

	va_list ap;
	va_start(ap, msg);
	_vsnprintf ( sBuf, C_BUFFER_SIZE, msg, ap );
	va_end(ap);	

	Write(C_MSG_CONSOLE, C_MSG_NORMAL, sBuf);
}

void CConsoleMessage::Write(int nType, const char* msg, ...)
{
	char sbuf[C_BUFFER_SIZE];	

	va_list ap;
	va_start(ap, msg);
	_vsnprintf ( sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	

	Write(nType, C_MSG_NORMAL, sbuf);
}

void CConsoleMessage::WriteInfo(const char* msg, ...)
{
	char sbuf[C_BUFFER_SIZE];	

	va_list ap;
	va_start(ap, msg);
	_vsnprintf ( sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	

	Write(C_MSG_CONSOLE, C_MSG_INFO, sbuf);
}

void CConsoleMessage::WriteInfo(int nType, const char* msg, ...)
{
	char sbuf[C_BUFFER_SIZE];	

	va_list ap;
	va_start(ap, msg);
	_vsnprintf ( sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	

	Write(nType, C_MSG_INFO, sbuf);
}


void CConsoleMessage::Write(int nType, int nPosition, const char* msg, ...)
{		
	// Memory allocation for buffer
	char sbuf[C_BUFFER_SIZE];
	
	LockOn();
	// va_start            va_arg              va_end
    // va_list             va_dcl (UNIX only)
	va_list ap;
	va_start(ap, msg);
	_vsnprintf ( sbuf, C_BUFFER_SIZE, msg, ap);
	va_end(ap);	

	switch (nType)
	{
	case C_MSG_CONSOLE :	
		if (nPosition == C_MSG_NORMAL)
		{		
			UpdateEditControl(sbuf); // Write to Console
		}
		else if(nPosition == C_MSG_INFO)
		{
			UpdateEditControlInfo(sbuf);
		}
        else
        {
        }
		break;	
	case C_MSG_FILE_CONSOLE	:		
		if (nPosition == C_MSG_NORMAL)
		{		
			UpdateEditControl(sbuf); // Write to Console
		}
		else if(nPosition == C_MSG_INFO)
		{
			UpdateEditControlInfo(sbuf);
		}
        else
        {
        }
		break;
	case C_MSG_ALL :
		// todo : write to db		
		if (nPosition == C_MSG_NORMAL)
		{		
			UpdateEditControl(sbuf); // Write to Console
		}
		else if(nPosition == C_MSG_INFO)
		{
			UpdateEditControlInfo(sbuf);
		}
        else
        {
        }
		break;		
	default:
		break;
	}
	LockOff();
}

void CConsoleMessage::UpdateEditControl(char* sbuf)
{
	if ( !sbuf ) return;
	if ( !m_pEditBox ) return;

	DWORD dwRetCode;
	
	// Update Control and Scroll to end of the line	
	::strcat(sbuf, "\r\n");
	
	::SendMessageTimeout(m_pEditBox, EM_SETSEL,		(WPARAM) UINT_MAX-1,	(LPARAM) UINT_MAX,	SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBox, EM_SCROLL,		(WPARAM) SB_PAGEDOWN,	(LPARAM) 0,			SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBox, EM_SCROLLCARET,(WPARAM) 0,				(LPARAM) 0,			SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBox, EM_REPLACESEL,	(WPARAM) FALSE,			(LPARAM) sbuf,		SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBox, EM_LINESCROLL,	(WPARAM) 0,				(LPARAM) 1,			SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBox, EM_SETSEL,		(WPARAM) -1,			(LPARAM) UINT_MAX,	SMTO_BLOCK, 15, &dwRetCode);
}

void CConsoleMessage::UpdateEditControlInfo(char* sbuf)
{
	if ( !sbuf ) return;
	if ( !m_pEditBoxInfo ) return;

	DWORD dwRetCode;
	
	// Update Control and Scroll to end of the line	
	::strcat(sbuf, "\r\n");
	
	::SendMessageTimeout(m_pEditBoxInfo, EM_SETSEL,		(WPARAM) UINT_MAX-1,	(LPARAM) UINT_MAX,	SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBoxInfo, EM_SCROLL,		(WPARAM) SB_PAGEDOWN,	(LPARAM) 0,			SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBoxInfo, EM_SCROLLCARET,(WPARAM) 0,				(LPARAM) 0,			SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBoxInfo, EM_REPLACESEL,	(WPARAM) FALSE,			(LPARAM) sbuf,		SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBoxInfo, EM_LINESCROLL,	(WPARAM) 0,				(LPARAM) 1,			SMTO_BLOCK, 15, &dwRetCode);
	::SendMessageTimeout(m_pEditBoxInfo, EM_SETSEL,		(WPARAM) -1,			(LPARAM) UINT_MAX,	SMTO_BLOCK, 15, &dwRetCode);
}