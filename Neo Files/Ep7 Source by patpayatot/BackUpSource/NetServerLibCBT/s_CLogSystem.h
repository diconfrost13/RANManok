///////////////////////////////////////////////////////////////////////////////
// s_CLogSysem.h
//
// class CLogSystem
//
// * History
// 2002.05.30 jgkim Create
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note
//
///////////////////////////////////////////////////////////////////////////////

#ifndef S_LOG_SYSTEM_H_
#define S_LOG_SYSTEM_H_

#include <time.h>
#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef C_BUFFER_SIZE
#define C_BUFFER_SIZE 1024
#endif

namespace SERVER_UTIL
{
	const LOG_ARRAY_SIZE = 1;
	const LOG_INTERVAL = 600; // 60 sec * 10 = 10 min

	class CLogSystem 
	{
	public:
		CLogSystem();
		~CLogSystem();

	protected:
		time_t ltime;
		char log_tmpbuf[100];
		char log_filename[MAX_PATH];
		double dLogInterval;
		std::vector<std::string> m_vString;
		
	public:		
		void Write(char *szStr, ...);
		void StartLog(void);

	protected:
		void WriteToFile(void);		
		void WriteToFileFinal(void);
		char* GetTime(void);
	};
}

#endif // S_LOG_SYSTEM_H_