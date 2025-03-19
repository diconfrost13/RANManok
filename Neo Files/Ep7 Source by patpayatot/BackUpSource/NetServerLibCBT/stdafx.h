// stdafx.h : 잘 변경되지 않고 자주 사용하는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once

//#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.
//
//#ifndef VC_EXTRALEAN
//#define VC_EXTRALEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
//#endif
//
//#include <afx.h>
//#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

//	Note : 기본 해더 파일.
//
// #include <windows.h>
// #include <windowsx.h>
#include <basetsd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tchar.h>

#include <malloc.h> // _alloca
#include <mmsystem.h>
#include <objbase.h>
#include <assert.h>

#pragma warning (disable:4996)
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
using namespace std;

#ifndef STRICT
	#define STRICT
#endif //STRICT


//	Note : DX 관련 해더 파일.
//
#include <ddraw.h>

#ifndef DIRECTINPUT_VERSION
	#define DIRECTINPUT_VERSION 0x0800
#endif //DIRECTINPUT_VERSION
#include <dinput.h>

#include <d3d8.h>
#include <D3d8types.h>
#include <D3DX8.h>
#include <D3dx8math.h>
#include <DXErr8.h>
#include <rmxfguid.h>

//	Note : DX 유틸 해더 파일.
//
#include "dxutil.h"
#include "d3dutil.h"
#include "d3dres.h"
#include "d3dfile.h"
#include "d3dfont.h"
#include "d3dapp.h"

//	Note : engine lib 해더 파일.
//
#include "DebugSet.h"
#include "profile.h"

#ifdef _DEBUG
	void* __cdecl operator new(size_t nSize, LPCSTR lpszFileName, int nLine);
	#define DEBUG_NEW new(THIS_FILE, __LINE__)
	void __cdecl operator delete(void* p, LPCSTR lpszFileName, int nLine);
#else
	#define DEBUG_NEW new
#endif

void* __cdecl operator new[](size_t nSize, LPCSTR lpszFileName, int nLine);

void __cdecl operator delete[](void* p, LPCSTR lpszFileName, int nLine);
