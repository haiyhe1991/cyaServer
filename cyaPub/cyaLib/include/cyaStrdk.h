#ifndef __CYA_STRDK_H__
#define __CYA_STRDK_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaString.h"

BOOL IsLowerLetter(int c);
BOOL IsUpperLetter(int c);
BOOL IsLetter(int c);
BOOL IsNumLetter(int c);
BOOL IsHexLetter(int c);

// 判断num是否为一个正整数字符串
BOOL IsStrNum(const char* num, INT_PTR len = -1);
BOOL IsStrNum(const WCHAR* num, INT_PTR len = -1);
// 判断num是否为一个正整数字符串，并且位数不得超过digit
BOOL IsStrNum2(const char* num, INT_PTR digit);
BOOL IsStrNum2(const WCHAR* num, INT_PTR digit);

int ToUpperLetter(int c);
int ToLowerLetter(int c);
char*  ToUpperString(char* str, INT_PTR len = -1);
WCHAR* ToUpperString(WCHAR* str, INT_PTR len = -1);
char*  ToLowerString(char* str, INT_PTR len = -1);
WCHAR* ToLowerString(WCHAR* str, INT_PTR len = -1);

BOOL IsLetterNum(int c);
BOOL IsBlankLetter(int c);
BOOL IsTrimLetter(char c, const char* trims = STRDK_TRIM_CHARS_A);
BOOL IsTrimLetter(WCHAR c, const WCHAR* trims = STRDK_TRIM_CHARS_W);
BOOL IsInBoundLetter(int c, int chLower, int chUpper);

INT64 Atoi64_(const char* nptr);
INT64 Atoi64_(const WCHAR* nptr);

int HexLetterToNum(int hex_char);
UINT64 HexStrToUINT64(const char* hex_str);
UINT64 HexStrToUINT64(const WCHAR* hex_str);

// 内联增强版strchr
const char*  StrChr_(const char* str, int ch, INT_PTR strLen = -1, BOOL checkMultiChar = false);
const WCHAR* StrChr_(const WCHAR* str, int ch, INT_PTR strLen = -1, BOOL checkMultiChar = false);

const char*  StrRChrRs(const char* const base, const char* str, const char* chrs);
const WCHAR* StrRChrRs(const WCHAR* const base, const WCHAR* str, const WCHAR* chrs);

inline BOOL IsSlashChar(int c)
{
	return '\\' == c || '/' == c;
}

inline const char* StrChr_(const char* str, int ch, INT_PTR strLen /*= -1*/, BOOL checkMultiChar /*= false*/)
{
	if (checkMultiChar)
	{
		if (strLen < 0)
		{
			while (*str)
			{
				BOOL isWide = (IsMultiBytesCharCC(str) && str[1]);
				if (!isWide && *str == (char)ch)
					return str;

				if (isWide)
					str += 2;
				else
					str++;
			}
		}
		else
		{
			for (INT_PTR i = 0; i<strLen;)
			{
				BOOL isWide = (IsMultiBytesCharCC(str + i) && (i + 1<strLen));
				if (!isWide && str[i] == (char)ch)
					return str + i;

				if (isWide)
					i += 2;
				else
					i++;
			}
		}
	}
	else
	{
		if (strLen < 0)
		{
			while (*str && *str != (char)ch)
				str++;

			if (*str == (char)ch)
				return str;
		}
		else
		{
			for (INT_PTR i = 0; i<strLen; ++i)
			{
				if (str[i] == (char)ch)
					return str + i;
			}
		}
	}

	return NULL;
}

inline const WCHAR* StrChr_(const WCHAR* str, int ch, INT_PTR strLen /*= -1*/, BOOL /*checkMultiChar*/ /*= false*/)
{
	if (strLen < 0)
	{
		while (*str && *str != (WCHAR)ch)
			str++;

		if (*str == (WCHAR)ch)
			return str;
	}
	else
	{
		for (INT_PTR i = 0; i<strLen; ++i)
		{
			if (str[i] == (WCHAR)ch)
				return str + i;
		}
	}

	return NULL;
}

inline BOOL IsLowerLetter(int c)
{
	return 'a' <= c && c <= 'z';
}
inline BOOL IsUpperLetter(int c)
{
	return 'A' <= c && c <= 'Z';
}
inline BOOL IsLetter(int c)
{
	return IsLowerLetter(c) || IsUpperLetter(c);
}
inline BOOL IsNumLetter(int c)
{
	return ('0' <= c && c <= '9');
}
inline BOOL IsHexLetter(int c)
{
	return IsNumLetter(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
}
inline const char* GetLastStr(const char* s)
{
	return s + strlen(s) - 1;
}
inline const WCHAR* GetLastStr(const WCHAR* s)
{
	return s + rs_wcslen(s) - 1;
}
inline const char* GetLastStr(const char* s, INT_PTR len)
{
	return s + len - 1;
}
inline const WCHAR* GetLastStr(const WCHAR* s, INT_PTR len)
{
	return s + len - 1;
}
CHAR_TYPE_INLINE BOOL IsStrNum_Tmpl_(const CHAR_TYPE* num, INT_PTR len /*= -1*/)
{
	if (NULL == num || 0 == num[0] || 0 == len)
		return false;

	if (len < 0)
	{
		while (*num)
		{
			if (!IsNumLetter(*num))
				return false;
			num++;
		}
	}
	else
	{
		ASSERT(len > 0);
		for (INT_PTR i = 0; i<len; ++i)
		{
			if (!IsNumLetter(num[i]))
				return false;
		}
	}
	return true;
}
inline BOOL IsStrNum(const char* num, INT_PTR len /*= -1*/)
{
	return IsStrNum_Tmpl_(num, len);
}
inline BOOL IsStrNum(const WCHAR* num, INT_PTR len /*= -1*/)
{
	return IsStrNum_Tmpl_(num, len);
}

CHAR_TYPE_INLINE BOOL IsStrNum2_Tmpl_(const CHAR_TYPE* num, INT_PTR digit)
{
	ASSERT(digit > 0);
	if (NULL == num || 0 == num[0])
		return false;
	int d = 0;
	while (*num)
	{
		if (!IsNumLetter(*num))
			return false;
		num++;
		d++;
		if (d > digit)
			return false;
	}
	return true;
}
inline BOOL IsStrNum2(const char* num, INT_PTR digit)
{
	return IsStrNum2_Tmpl_(num, digit);
}
inline BOOL IsStrNum2(const WCHAR* num, INT_PTR digit)
{
	return IsStrNum2_Tmpl_(num, digit);
}

inline int ToUpperLetter(int c)
{
	return IsLowerLetter(c) ? (c - 'a' + 'A') : c;
}
inline int ToLowerLetter(int c)
{
	return IsUpperLetter(c) ? (c - 'A' + 'a') : c;
}

template<typename CHAR_TYPE, typename TRAITS>
static inline CHAR_TYPE* StringSwitch_Tmpl_(CHAR_TYPE* str, INT_PTR len /*= -1*/, TRAITS* /*dummy*/)
{
	ASSERT(str);
	if (len < 0)
	{
		for (CHAR_TYPE* p = str; *p; ++p)
			*p = (CHAR_TYPE)TRAITS::Switch(*p);
	}
	else
	{
		CHAR_TYPE* e = str + len;
		for (CHAR_TYPE* p = str; p != e; ++p)
			*p = (CHAR_TYPE)TRAITS::Switch(*p);
	}
	return str;
}

template<typename CHAR_TYPE>
struct ToUpper_Traits_ { static inline int Switch(int c) { return ToUpperLetter(c); } };
inline char* ToUpperString(char* str, INT_PTR len /*= -1*/)
{
	return StringSwitch_Tmpl_(str, len, (ToUpper_Traits_<char>*)0);
}
inline WCHAR* ToUpperString(WCHAR* str, INT_PTR len /*= -1*/)
{
	return StringSwitch_Tmpl_(str, len, (ToUpper_Traits_<WCHAR>*)0);
}

template<typename CHAR_TYPE>
struct ToLower_Traits_ { static inline int Switch(int c) { return ToLowerLetter(c); } };
inline char* ToLowerString(char* str, INT_PTR len /*= -1*/)
{
	return StringSwitch_Tmpl_(str, len, (ToLower_Traits_<char>*)0);
}
inline WCHAR* ToLowerString(WCHAR* str, INT_PTR len /*= -1*/)
{
	return StringSwitch_Tmpl_(str, len, (ToLower_Traits_<WCHAR>*)0);
}

inline BOOL IsLetterNum(int c)
{
	return IsLetter(c) || IsNumLetter(c);
}
inline BOOL IsBlankLetter(int c)
{
	return ' ' == c || '\t' == c;
}
inline BOOL IsTrimLetter(char c, const char* trims /*= STRDK_TRIM_CHARS_A*/)
{
	return NULL != StrChr_(trims, c);
}
inline BOOL IsTrimLetter(WCHAR c, const WCHAR* trims /*= STRDK_TRIM_CHARS_W*/)
{
	return NULL != StrChr_(trims, c);
}
inline BOOL IsInBoundLetter(int c, int chLower, int chUpper)
{
	ASSERT(chLower <= chUpper); return chLower <= c && c <= chUpper;
}

#if defined(_MSC_VER)
	#pragma warning(disable: 4702) // unreachable code
#endif

CHAR_TYPE_INLINE INT64 Atoi64_Tmpl_(const CHAR_TYPE* nptr)
{
	// skip whitespace
	while (IsBlankLetter(*nptr))
		++nptr;

	if (1 == sizeof(CHAR_TYPE))
	{
		int c = (BYTE)*nptr++;
		int sign = c;
		if (c == '-' || c == '+')
			c = (BYTE)*nptr++;

		INT64 total = 0;
		while (IsNumLetter(c))
		{
			total = 10 * total + (c - '0');
			c = (BYTE)*nptr++;
		}
		return (sign == '-' ? -total : total);
	}
	else if (2 == sizeof(CHAR_TYPE))
	{
		int c = (WORD)*nptr++;
		int sign = c;
		if (c == '-' || c == '+')
			c = (WORD)*nptr++;

		INT64 total = 0;
		while (IsNumLetter(c))
		{
			total = 10 * total + (c - '0');
			c = (WORD)*nptr++;
		}
		return (sign == '-' ? -total : total);
	}
	else if (4 == sizeof(CHAR_TYPE))
	{
		int c = (DWORD)*nptr++;
		int sign = c;
		if (c == '-' || c == '+')
			c = (DWORD)*nptr++;

		INT64 total = 0;
		while (IsNumLetter(c))
		{
			total = 10 * total + (c - '0');
			c = (DWORD)*nptr++;
		}
		return (sign == '-' ? -total : total);
	}
	else
	{
		//NPR_ASSERT_EX(false, _T("sizeof(CHAR_TYPE) only be 1, or 2, or 4"));
		return 0;
	}
}
#if defined(_MSC_VER)
#pragma warning(default: 4702) // unreachable code
#endif

inline INT64 Atoi64_(const char* nptr)
{
	return Atoi64_Tmpl_(nptr);
}
inline INT64 Atoi64_(const WCHAR* nptr)
{
	return Atoi64_Tmpl_(nptr);
}

inline int HexLetterToNum(int hex_char)
{
	ASSERT(IsHexLetter(hex_char));
	if ('a' <= hex_char && hex_char <= 'f')
		return hex_char - 'a' + 10;
	else if ('A' <= hex_char && hex_char <= 'F')
		return hex_char - 'A' + 10;
	else
		return hex_char - '0';
}

CHAR_TYPE_INLINE UINT64 HexStrToUINT64_Tmpl(const CHAR_TYPE* hex_str)
{
	ASSERT(hex_str);
	const CHAR_TYPE* p = hex_str;
	UINT64 i64 = 0;
	while (IsHexLetter(*p))
	{
		i64 = (i64 << 4) + (UINT64)HexLetterToNum(*p);
		++p;
	}
	return i64;
}
inline UINT64 HexStrToUINT64(const char* hex_str)
{
	return HexStrToUINT64_Tmpl(hex_str);
}
inline UINT64 HexStrToUINT64(const WCHAR* hex_str)
{
	return HexStrToUINT64_Tmpl(hex_str);
}
#endif