//-----------------------------------------------------------------------------
// File: Pch.h
//
// Desc: Header file to precompile
//
// Copyright (C) 1998-2000 Microsoft Corporation. All Rights Reserved.
//
//	Note : Max 4.x ������ ��� �ִ� �÷����� SDK�� ���丮�� �����Ǿ� �־���Ѵ�.
//			���� �ɸ��� ��Ʈ����� SDK�� ���丮 ���� �����Ǿ�� �Ѵ�.
//
//-----------------------------------------------------------------------------
#ifndef __PCH__H
#define __PCH__H

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include <assert.h>
#include <io.h>
#include <fcntl.h>
#include <direct.h>
#include <commdlg.h>
#include "phyexp.h"
#include "d3dx9.h"
#include "stdmat.h"


inline BOOL _streql ( char *str1, char *str2, int nComp )
{
	while ( (*str1==*str2) && (*str1) && (nComp>=0) )
	{
		++str1;
		++str2;
		--nComp;
	}

	return (nComp==0);
}

#endif