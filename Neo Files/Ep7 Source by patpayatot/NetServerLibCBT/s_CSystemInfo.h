///////////////////////////////////////////////////////////////////////////////
// s_CSystemInfo.h
//
// * History
// 2002.05.30 jgkim First Coding
// 2003.01.18 jgkim Add memory function
//
// Copyright 2002-2003 (c) Mincoms. All rights reserved.                 
// 
// * Note 
// Some of class member functions do not support Windows 95/98/Me.
// Use Windows 2000 or Higher OS
//
// This class need winmm lib, must link with Winmm.lib
//
// #pragma comment (lib,"Winmm.lib")
// #pragma message ("Auto linking Windows mm library")
//
///////////////////////////////////////////////////////////////////////////////
#ifndef S_CSYSTEM_INFO_H_
#define S_CSYSTEM_INFO_H_

#include <windows.h>
#include <Mmsystem.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace SERVER_UTIL
{
	class CSystemInfo
	{
	public:
		CSystemInfo();
		~CSystemInfo();
	public:
		DWORD	GetProcessorNumber(void);
		DWORD	GetProcessorType(void);
		WORD	GetProcessorFamilyCode(void);
		WORD	GetProcessorModelNumber(void);
		WORD	GetProcessorSteppingID(void);
		char*	GetProcessorName(char *string);

		DWORD	CalculateCpuSpeed();
		
		DWORD	GetCpuSpeed(void);
		char*	GetOSName(char *string);
		int		GetCurrentDirectory(DWORD nBufferLength, LPTSTR lpBuffer);
		CString	GetAppPath ();

		DWORD	DetermineTimeStamp(void);

		MEMORYSTATUSEX	GetMemoryStatus();
		DWORD			GetMemoryUsagePercent(); // Percentage of memory usage
		DWORDLONG		GetMemoryPhysicalTotal(); // Total bytes of physical memory
		DWORDLONG		GetMemoryPhysicalAvail(); // Total bytes of available physical memory
		DWORDLONG		GetMemoryPhysicalUse();  // Total bytes of useage physical memory
		DWORDLONG		GetMemoryVirtualTotal(); // Total bytes of virtual memory
		DWORDLONG		GetMemoryVirtualAvail(); // Total bytes of available physical memory
		DWORDLONG		GetMemoryVirtualUse(); // Total bytes of useage physical memory


		BOOL	SystemLogoff();
		BOOL	SystemShutdown();
		BOOL	SystemReboot();
		BOOL	SystemPoweroff();

	protected:
		BOOL	SystemDown(int nMethod);
	};
}
#endif // S_CSYSTEM_INFO_H_