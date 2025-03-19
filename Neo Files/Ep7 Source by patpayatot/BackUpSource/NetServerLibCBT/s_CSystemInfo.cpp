///////////////////////////////////////////////////////////////////////////////
// s_CSystemInfo.cpp
//
// *History
// 2002.05.30 jgkim Create
// 2003.01.18 jgkim Add memory function
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "s_CSystemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SERVER_UTIL
{
	CSystemInfo::CSystemInfo()
	{
	}
	CSystemInfo::~CSystemInfo()
	{
	}

	DWORD CSystemInfo::GetProcessorNumber(void)
	{
		SYSTEM_INFO SystemInfo;	
		GetSystemInfo(&SystemInfo);	
		return SystemInfo.dwNumberOfProcessors;
	}

	DWORD CSystemInfo::GetProcessorType(void)
	{
		SYSTEM_INFO SystemInfo;	
		GetSystemInfo(&SystemInfo);	
		return SystemInfo.dwProcessorType; 
	}

	WORD CSystemInfo::GetProcessorFamilyCode(void)
	{
		SYSTEM_INFO SystemInfo;	
		GetSystemInfo(&SystemInfo);	
		return SystemInfo.wProcessorLevel;
	}

	WORD CSystemInfo::GetProcessorModelNumber(void)
	{
		SYSTEM_INFO SystemInfo;	
		GetSystemInfo(&SystemInfo);	
		return HIBYTE(SystemInfo.wProcessorRevision);
	}

	WORD CSystemInfo::GetProcessorSteppingID(void)
	{
		SYSTEM_INFO SystemInfo;	
		GetSystemInfo(&SystemInfo);		
		return LOBYTE(SystemInfo.wProcessorRevision);
	}

	// Get CPU name, Support only INTEL CPU
	char* CSystemInfo::GetProcessorName(char *string)
	{	
		SYSTEM_INFO SystemInfo;	
		GetSystemInfo(&SystemInfo);

		switch(SystemInfo.wProcessorLevel)
		{
		case 6 : // Family code 0110 
			strcpy(string, "Pentium III processor");
			// Pentium Celeron or Pentium III
			switch(HIBYTE(SystemInfo.wProcessorRevision))
			{
			case 6 : // 0110
				// Intel Celeron processor, model 6
				strcpy(string, "Intel Celeron processor, model 6");
				break;
			case 7 : // 0111
				// Pentium III processor, model 7, and Pentium III Xeon processor, model 7
				strcpy(string, "Pentium III processor model 7");
				break;
			case 8 : // 1000
				// Pentium III processor, model 8, Pentium III Xeon processor, model 8, and intel Celeron processor, model 8
				strcpy(string, "Pentium III processor, model 8");
				break;
			case 10 : // 1010
				// Pentium III Xeon processor, model A
				strcpy(string, "Pentium III Xeon processor, model A");
				break;
			case 11 : // 1011
				// Pentium III processor, model B
				strcpy(string, "Pentium III processor, model B");
				break;
			case 3 : // 0011
				// Intel Pentium II OverDrive processor
				strcpy(string, "Intel Pentium II OverDrive processor");
				break;
			default :
				break;
			}
			break;
		case 15 : // Family code 1111
			// Pentium 4 processor, Mobile Intel Pentium 4 processor, 
			strcpy(string, "Pentium 4 processor");
			break;
		default :
			strcpy(string, "Unknown Processor");
			break;
		}	
		return string;
	}

	char* CSystemInfo::GetOSName(char *string)
	{
		OSVERSIONINFO OSInfo;
		memset(&OSInfo, 0, sizeof(OSInfo));

		// Set size
		OSInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		if(GetVersionEx((OSVERSIONINFO *) &OSInfo) == FALSE)
		return false;

		switch(OSInfo.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_NT:
			// Microsoft Windows NT
			if(OSInfo.dwMajorVersion == 4)
				strcpy(string, "Microsoft Windows NT");
			// Microsoft Windows 2000
			if((OSInfo.dwMajorVersion == 5) && (!OSInfo.dwMinorVersion))
				strcpy(string, "Microsoft Windows 2000");

			// Microsoft Windows XP
			if((OSInfo.dwMajorVersion == 5) && (OSInfo.dwMinorVersion == 1))
				strcpy(string, "Microsoft Windows XP");
			break;

		case VER_PLATFORM_WIN32_WINDOWS:
			if((OSInfo.dwMajorVersion == 4) && (!OSInfo.dwMinorVersion)) 
			{		
				// Microsoft Windows 95 OSR2
				if(OSInfo.szCSDVersion[1] == 'C') 
				{
					strcpy(string, "Microsoft Windows 95 OSR2");
				}
				// Microsoft Windows 95
				else
				{
					strcpy(string, "Microsoft Windows 95");
				}			
			}
			if((OSInfo.dwMajorVersion == 4) && (OSInfo.dwMinorVersion == 10))
			{
				// Microsoft Windows 98 SE
				if(OSInfo.szCSDVersion[1] == 'A')
				{
					strcpy(string, "Microsoft Windows 98 SE");
				}			
				// Microsoft Windows 98
				else
				{
					strcpy(string, "Microsoft Windows 98");
				}
			}
			// Microsoft Windows ME
			if((OSInfo.dwMajorVersion == 4) && (OSInfo.dwMinorVersion == 90))
				strcpy(string, "Microsoft Windows ME");
			break;
		case VER_PLATFORM_WIN32s:
			// Microsoft Win32s
			strcpy(string, "Microsoft Win32s");
			break;
		}
		return string;
	}

	#define UINT_MAX 0xffffffff

	DWORD CSystemInfo::GetCpuSpeed(void)
	{
		const int MAX_TRIES = 10;
		DWORD dwSpeed = 0;
		DWORD dwTestValue = UINT_MAX;
		int   nNumTimes = 0;

		DWORD dwStartingPriority = GetPriorityClass(GetCurrentProcess());
		SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

		//--------------------------------------------------------
		// Find the first two similarities up to ten times.
		// This should be a maximum of five seconds.
		// 
		while ((dwSpeed != dwTestValue) && (nNumTimes < MAX_TRIES))
		{
			dwTestValue = dwSpeed;
			dwSpeed = CalculateCpuSpeed();
			nNumTimes++;
		}	
		SetPriorityClass(GetCurrentProcess(), dwStartingPriority);
		return dwSpeed;
	}

	// Stolen from AMD
	DWORD CSystemInfo::DetermineTimeStamp(void)
	{
	DWORD dwTickVal;
	__asm
	{
		_emit 0Fh
		_emit 31h
		mov   dwTickVal, eax
	}
	return (dwTickVal);
	}

	DWORD CSystemInfo::CalculateCpuSpeed()
	{
	int   nTimeStart = 0;
	int   nTimeStop = 0;
	DWORD dwStartTicks = 0;
	DWORD dwEndTicks = 0;
	DWORD dwTotalTicks = 0;
	DWORD dwCpuSpeed = 0;

	nTimeStart = timeGetTime();
	for (;;)
	{
		nTimeStop = timeGetTime();
		if ((nTimeStop - nTimeStart) > 1)
		{
			dwStartTicks = DetermineTimeStamp();
			break;
		}
	}
	nTimeStart = nTimeStop;
	for (;;)
	{
		nTimeStop = timeGetTime();
		if ((nTimeStop - nTimeStart) > 500)    // one-half second
		{
			dwEndTicks = DetermineTimeStamp();
			break;
		}
	}
	dwTotalTicks = dwEndTicks - dwStartTicks;
	dwCpuSpeed = dwTotalTicks / 500000;
	return (dwCpuSpeed);
	}

	// Retrieves the current directory for the current process
	// DWORD nBufferLength : size of directory buffer
	// LPTSTR lpBuffer : directory buffer
	int CSystemInfo::GetCurrentDirectory(DWORD nBufferLength, LPTSTR lpBuffer)
	{
		if (::GetCurrentDirectory(nBufferLength, lpBuffer) == 0)
			return -1;

		return 0;
	}

	CString	CSystemInfo::GetAppPath ()
	{
		CString strFullPath;
		CString strCommandLine = GetCommandLine ();

		if ( !strCommandLine.IsEmpty() )
		{
			DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
			if ( dwFind != -1 )
			{
				strFullPath = strCommandLine.Left ( dwFind );
				
				if ( !strFullPath.IsEmpty() )
				if ( strFullPath.GetAt(0) == '"' )
					strFullPath = strFullPath.Right ( strFullPath.GetLength() - 1 );
			}
		}

		return strFullPath;
	}

	MEMORYSTATUSEX CSystemInfo::GetMemoryStatus()
	{
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof (statex);
		GlobalMemoryStatusEx (&statex);
		return statex;
	}

	DWORD CSystemInfo::GetMemoryUsagePercent()
	{
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof (statex);
		GlobalMemoryStatusEx (&statex);
		return statex.dwMemoryLoad;
	}

	DWORDLONG CSystemInfo::GetMemoryPhysicalTotal()
	{
		MEMORYSTATUSEX statex;	
		statex.dwLength = sizeof (statex);
		GlobalMemoryStatusEx (&statex);
		return statex.ullTotalPhys; 
	}

	DWORDLONG CSystemInfo::GetMemoryPhysicalAvail()
	{
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof (statex);
		GlobalMemoryStatusEx (&statex);
		return statex.ullAvailPhys; 
	}

	DWORDLONG CSystemInfo::GetMemoryPhysicalUse()
	{
		MEMORYSTATUSEX statex;	
		statex.dwLength = sizeof (statex);
		GlobalMemoryStatusEx (&statex);
		return (statex.ullTotalPhys - statex.ullAvailPhys);
	}

	DWORDLONG CSystemInfo::GetMemoryVirtualTotal()
	{
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof (statex);
		GlobalMemoryStatusEx (&statex);
		return statex.ullTotalVirtual;	
	}

	DWORDLONG CSystemInfo::GetMemoryVirtualAvail()
	{
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof (statex);
		GlobalMemoryStatusEx (&statex);
		return statex.ullAvailVirtual;
	}

	DWORDLONG CSystemInfo::GetMemoryVirtualUse()
	{
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof (statex);
		GlobalMemoryStatusEx (&statex);
		return (statex.ullTotalVirtual - statex.ullAvailVirtual);
	}
	
	BOOL CSystemInfo::SystemDown(int nMethod)
	{
		OSVERSIONINFO os;
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&os);
		if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{			
			if (os.dwMajorVersion == 4) // Microsoft Windows NT
			{
			}				
			if ((os.dwMajorVersion == 5) && (!os.dwMinorVersion)) // Microsoft Windows 2000
			{
			}				
			if ((os.dwMajorVersion == 5) && (os.dwMinorVersion == 1)) // Microsoft Windows XP
			{
			}				
		}
		else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) // 95, 98, ME
		{
		}
		else
		{
		}
		return ExitWindowsEx(nMethod ,EWX_FORCE);
	}

	BOOL CSystemInfo::SystemLogoff()
	{
		return SystemDown(EWX_LOGOFF);
	}

	BOOL CSystemInfo::SystemShutdown()
	{
		return SystemDown(EWX_SHUTDOWN);		
	}

	BOOL CSystemInfo::SystemReboot()
	{	
		return SystemDown(EWX_REBOOT);
	}

	BOOL CSystemInfo::SystemPoweroff()
	{
		return SystemDown(EWX_POWEROFF);
	}
};