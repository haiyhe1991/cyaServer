#ifndef __STR_SEP_H__
#define __STR_SEP_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaString.h"

template<typename Container, typename CHAR_TYPE>
inline void SplitString_Tmpl_(const XCHAR_TYPE* str, int sepChar, Container& ctr, INT_PTR str_len = -1, BOOL includeEmpyStr = false)
{
	if (str == NULL)
		return;

	str_len;
	const CHAR_TYPE* p1 = str;
	const CHAR_TYPE* pPrev = str;
	while (*p1)
	{
		if (*p1 == sepChar)
		{
			int count = p1 - pPrev;
			if (count > 0)
			{
				XTMPL_STD_STRING s;
				s.assign(pPrev, p1 - pPrev);
				ctr.push_back(s);
			}
			else if (includeEmpyStr)
			{
				XTMPL_STD_STRING s;
				ctr.push_back(s);
			}
			pPrev = ++p1;
		}
		else
			++p1;
	}
	int count = p1 - pPrev;
	if (count > 0)
	{
		XTMPL_STD_STRING s;
		s.assign(pPrev, p1 - pPrev);
		ctr.push_back(s);
	}
	else if (includeEmpyStr)
	{
		XTMPL_STD_STRING s;
		ctr.push_back(s);
	}
}

//Container可以是vector或list容器
template<typename Container>
inline void SplitString(const char* str, int sepChar, Container& ctr, INT_PTR str_len = -1, BOOL includeEmpyStr = false)
{
	SplitString_Tmpl_(str, sepChar, ctr, str_len, includeEmpyStr);
}

template<typename Container>
inline void SplitString(const WCHAR* str, int sepChar, Container& ctr, int str_len = -1, BOOL includeEmpyStr = false)
{
	SplitString_Tmpl_(str, sepChar, ctr, str_len, includeEmpyStr);
}

#endif