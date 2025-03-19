#ifndef S_CRANDOM_NUMBER_H
#define S_CRANDOM_NUMBER_H 

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace SERVER_UTIL
{
	class CRandomNumber
	{
	public:
		CRandomNumber(void);
		virtual ~CRandomNumber(void);
		bool GenerateRandomString(CString& str, int Length, int Small, int Capital, int Numbers);
	};
}

#endif // S_CRANDOM_NUMBER_H