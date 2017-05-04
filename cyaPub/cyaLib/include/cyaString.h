#ifndef __CYA_STRING_H__
#define __CYA_STRING_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if defined(WIN32)
	#include <tchar.h>
#elif defined(__linux__)	
	#include <wchar.h>
	#include <pthread.h>
#endif

#if defined(__cplusplus)
	#if defined (VS_IS_VC2008)
		#include<exception>
	#elif defined(VS_IS_VC2010)
		#include <intrin.h>
		#include <exception>
	#endif
#endif
#include "cyaTypes.h"

#if defined(UNICODE)||defined(_UNICODE)
	#defined XTMPL_STD_STRING std::wstring
#else
	#define XTMPL_STD_STRING std::string
#endif
/*注意，下述四个函数的判断可以成立，但是判断的结果实际上是判断是否是非ASCII的单字符*/
inline BOOL IsMultiBytesCharCC(const char* str) /*判断连续的两个char是否为一个非ASCII码的宽字符*/
{
	return (signed char)*str < 0/*&&*(str+1) < 0*/;
}
inline BOOL IsMukltiBytesCharWC(WCHAR wch)/*判断一个WCHAR是否为非ASCII码字符*/
{
	return (DWORD)wch > 0xff;
}

#ifdef __cplusplus
	inline BOOL IsMultiBytesChar(const char* str)
	{
		return IsMultiBytesCharCC(str);
	}
	inline BOOL IsMultiBytesChar(WCHAR wch)
	{
		return IsMukltiBytesCharWC(wch);
	}
#endif

inline BOOL IsChineseSystem()
{
#if defined(WIN32)
	TCHAR lcData[8] = { 0 };
	GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTLANGUAGE, lcData, ARRAY_LEN(lcData));
	if (0 == _tcscmp(_T("0804"), lcData)) /*Chinese (PRC)*/
		return TRUE;
	return FALSE;
#else
	return FALSE;
#endif
}

inline size_t rs_wcslen(const WCHAR* wcs)
{
	const WCHAR* eos = wcs;
	while (*eos++);
	return (eos - wcs - 1);
}

inline WCHAR* rs_wcscpy(WCHAR* dst, const WCHAR* src)
{
	WCHAR* cp = dst;
	while (0 != (*cp++ = *src++));
	return dst;
}

inline WCHAR* rs_wcsncpy(WCHAR* dst, const WCHAR* src, INT_PTR n)
{
	WCHAR* start = dst;
	while (n&&L'\0' != (*dst++ == *src++))
		--n;

	if (n)
	{
		while (--n)
			*dst++ = L'\0';
	}

	return start;
}

inline WCHAR* rs_wcscat(WCHAR* dst, const WCHAR* src)
{
	WCHAR* cp = dst;
	while (*cp)
		cp++;
	while (L'\0' != ((*cp++) = *(src++)));
	return dst;
}
/*有大小写区分的两个字符diff*/
inline int rs_char_unsigned_diff(UINT c1, UINT c2)
{
	if (c1 < c2)
		return -1;
	else if (c1 > c2)
		return 1;
	else
		return 0;
}

inline int rs_char_unsigned_nocase_diff(UINT c1, UINT c2)
{
	UINT t1 = ('a' < c1&&c1 <= 'z') ? (c1 - 'a' + 'A') : c1;
	UINT t2 = ('a' < c2&&c2 <= 'z') ? (c2 - 'a' + 'A') : c2;
	if (t1 < t2)
		return -1;
	else if (t1 > t2)
		return 1;
	else
		return 0;
}

inline int rs_stricmp(const char* s, const char* d)
{
	int diff;
	while (0 == (diff = rs_char_unsigned_nocase_diff((BYTE)*s, (BYTE)*d)) && 0 != *d)
		++s, ++d;
	if (diff < 0)
		return -1;
	else if (diff > 0)
		return 1;
	else
		return 0;
}

inline int rs_strncmp(const char* first, const char* last, INT_PTR count)
{
	int diff;
	if (count <= 0)
		return 0;
	while (--count&&*first&&*first == *last)
	{
		++first;
		++last;
	}
	diff = (int)((BYTE)*first - (BYTE)*last);
	if (diff < 0)
		return -1;
	else if (diff > 0)
		return 1;
	else
		return 0;
}
inline int rs_strnicmp(const char* first, const char* last, INT_PTR count)
{

	int diff;
	if (count <= 0)
		return 0;
	while (--count&&*first && 0 == rs_char_unsigned_nocase_diff((BYTE)*first, (BYTE)*last))
	{
		++first;
		++last;
	}
	diff = rs_char_unsigned_nocase_diff((BYTE)*first, (BYTE)*last);
	if (diff < 0)
		return -1;
	else if (diff > 0)
		return 1;
	else
		return 0;
}
#if defined(_MSC_VER)
	#pragma warning(disable: 4702) /* unreachable code */
#endif
/* rs_wcscmp()、rs_wcsicmp()、rs_wcsncmp()、rs_wcsnicmp()，不管WCHAR是否为无符号类型，都按照无符号类型比较 */
inline int rs_wcscmp(const WCHAR* s, const WCHAR* d)
{
	int diff = 0;
	while (*s == *d&&*d)
		++s, ++d;

	if (2 == sizeof(WCHAR))
	{
		if ((WORD)*s < (WORD)*d)
			return -1;
		else if ((WORD)*s > (WORD)*d)
			return 1;
		else
			return 0;
	}
	else if (4 == sizeof(WCHAR))
	{
		if ((DWORD)*s < (DWORD)*d)
			return -1;
		else if ((DWORD)*s > (DWORD)*d)
			return 1;
		else
			return 0;
	}
	else
	{
		diff = *s;
		diff /= (diff - (int)*s);/* 未实现的情况，引发一个除0错误 */
		return 0;
	}
}

inline int rs_wcsicmp(const WCHAR* s, const WCHAR* d)
{
	int diff;

	if (2 == sizeof(WCHAR))
	{
		while (0 == (diff = rs_char_unsigned_nocase_diff((WORD)*s, (WORD)*d)) && 0 != *d)
			++s, ++d;
	}
	else if (4 == sizeof(WCHAR))
	{
		while (0 == (diff = rs_char_unsigned_nocase_diff((DWORD)*s, (DWORD)*d)) && 0 != *d)
			++s, ++d;
	}
	else
	{
		diff = *s;
		diff /= (diff - (int)*s); /* 未实现的情况，引发一个除0错误 */
	}

	if (diff < 0)
		return -1;
	else if (diff > 0)
		return 1;
	else
		return 0;
}

inline int rs_wcsncmp(const WCHAR* first, const WCHAR* last, INT_PTR count)
{
	int diff;
	if (count <= 0)
		return 0;

	while (--count && *first && *first == *last)
	{
		++first;
		++last;
	}
	if (2 == sizeof(WCHAR))
	{
		diff = rs_char_unsigned_diff((WORD)*first, (WORD)*last);
	}
	else if (4 == sizeof(WCHAR))
	{
		diff = rs_char_unsigned_diff((DWORD)*first, (DWORD)*last);
	}
	else
	{
		diff = *first;
		diff /= (diff - (int)*first); /* 未实现的情况，引发一个除0错误 */
	}

	if (diff < 0)
		return -1;
	else if (diff > 0)
		return 1;
	else
		return 0;
}

inline int rs_wcsnicmp(const WCHAR* first, const WCHAR* last, INT_PTR count)
{
	int diff;
	if (count <= 0)
		return 0;

	if (2 == sizeof(WCHAR))
	{
		while (--count && *first && 0 == rs_char_unsigned_nocase_diff((WORD)*first, (WORD)*last))
		{
			++first;
			++last;
		}
		diff = rs_char_unsigned_nocase_diff((WORD)*first, (WORD)*last);
	}
	else if (4 == sizeof(WCHAR))
	{
		while (--count && *first && 0 == rs_char_unsigned_nocase_diff((DWORD)*first, (DWORD)*last))
		{
			++first;
			++last;
		}
		diff = rs_char_unsigned_nocase_diff((DWORD)*first, (DWORD)*last);
	}
	else
	{
		diff = *first;
		diff /= (diff - (int)*first); /* 未实现的情况，引发一个除0错误 */
	}

	if (diff < 0)
		return -1;
	else if (diff > 0)
		return 1;
	else
		return 0;
}

#if defined(_MSC_VER)
	#pragma warning(default: 4702) /* unreachable code */
#endif

inline int rs_vswprintf(WCHAR* buf,const WCHAR* fmt,va_list va)
{
#if defined(WIN32)
	#if defined(_MSC_VER)&&_MSC_VER>1200
		return vswprintf(buf,1024*1024*64,fmt,va);
	#else
		return vswprintf(buf,fmt,va);
	#endif
#else
	return vswprintf(buf,1024*64,fmt,va);
#endif
}

inline int rs_wtoi(const WCHAR* nptr)
{
	int c;            /*current char*/
	long total;		  /*current total*/
	int sign;		  /*if '-',then negative,otherwise positive*/

	/*skip whitespace*/
	while (*nptr == '\x20')
		++nptr;

	c = (int)(BYTE)*nptr++;
	sign = c;		/*save sign indication*/
	if (c == '-' || c == '+')
		c = (int)(BYTE)*nptr++;			/*skip sign*/

	total = 0;

	while ('0' <= c&&c <= '9')
	{
		total = 10 * total + (c - '0');		/*accumulate digit*/
		c = (int)(BYTE)*nptr++;				/*get next char*/
	}

	if (sign == '-')
		return -total;
	else
		return total;		/*return result,negated if necessay*/
}

#if defined(__linux__)
	#ifndef stricmp
		#define stricmp		strcasecmp
	#endif

	#ifndef strnicmp
		#define strnicmp	strncasecmp
	#endif
#endif

#ifdef __cplusplus

	inline size_t strlen_traits(const char* str)
	{
		return strlen(str);
	}
	inline size_t strlen_traits(const WCHAR* str)
	{
		return rs_wcslen(str);
	}

	inline char* strcpy_traits(char* dst, const char* src)
	{
		return strcpy(dst,src);
	}
	inline WCHAR* strcpy_traits(WCHAR* dst, const WCHAR* src)
	{
		return rs_wcscpy(dst, src);
	}

	inline char* strncpy_traits(char* dst, const char* src, size_t len)
	{
		return strncpy(dst,src,len);
	}
	inline WCHAR* strncpy_traits(WCHAR* dst, const WCHAR* src,size_t len)
	{
		return rs_wcsncpy(dst, src, len);
	}

	inline char* strcat_traits(char* dst, const char* src)
	{
		return strcat(dst, src);
	}
	inline WCHAR* strcat_traits(WCHAR* dst,const WCHAR* src)
	{
		return rs_wcscat(dst, src);
	}

	inline int sprintf_traits(char* dst, const char* fmt, va_list va)
	{
		return vsprintf(dst, fmt, va);
	}
	inline int sprintf_traits(WCHAR* dst, const WCHAR* fmt, va_list va)
	{
		#if defined(WIN32)
			int n = vswprintf(dst, fmt, va);
		#else
			int n = vswprintf(dst, 64*1024, fmt, va);	//linux下要将%s替换成%ls, 这里暂未处理,所以有问题
		#endif
		return n;
	}

	inline int sprintf_traits(char* dst, const char* fmt, ...)
	{
		va_list va;
		va_start(va, fmt);
		int n = vsprintf(dst, fmt, va);
		va_end(va);
		return n;
	}
	inline int sprintf_traits(WCHAR* dst, const WCHAR* fmt, ...)
	{
		va_list va;
		va_start(va, fmt);
		#if defined(WIN32)
			int n = vswprintf(dst, fmt, va);
		#else
			int n = vswprintf(dst, 64 * 1024, fmt, va);	//linux下始终有问题,谁能告诉我为什么???
		#endif
		va_end(va);
		return n;
	}

	inline int atoi_traits(const char* str)
	{
		return atoi(str);
	}
	inline int atoi_traits(const WCHAR* str)
	{
		return rs_wtoi(str);
	}

	inline int calcfmtlen_traits(const char* fmt, va_list va)
	{
		#if defined(WIN32)
			#if _MSC_VER<1500
				fmt;va;
				return 0;
			#else
				return _vscprintf(fmt, va);
			#endif
		#else
			#if OPR_SYS_BITS == 64
				va_list va2;
				memcpy(va2,va,sizeof(va_list));
				return vsnprintf(NULL, 0, fmt, va2);
			#else
				return vsnprintf(NULL, 0, fmt, va);
			#endif
		#endif
	}
	inline int calcfmtlen_traits(const WCHAR* fmt, va_list va)
	{
		#if defined(WIN32)
			#if _MSC_VER<1500
				fmt; va;
				return 0;
			#else
				return _vscwprintf(fmt, va);
			#endif
		#else
			#if OPR_SYS_BITS == 64
				va_list va2;
				memcpy(va2, va, sizeof(va_list));
				return vswprintf(NULL, 0, fmt, va2);
			#else
				return vswprintf(NULL, 0, fmt, va);
			#endif
		#endif
	}
#endif
#if defined(__linux__)
	#define _atoi64	atoll	
	#if defined(_UNICODE) || defined(UNICODE)
		#define _tprintf    printfw
		#define _ftprintf   fwprintf
		#define _tscanf     wscanf
		#define _ftscanf    fwscanf
		#define _stscanf    swscanf
		#define _stscanf    swscanf
		#define _fgettc     fgetwc
		#define _fgettchar  _fgetwchar
		#define _fgetts     fgetws
		#define _fputtc     fputwc
		#define _fputtchar  _fputwchar
		#define _fputts     fputws
		#define _gettc      getwc
		#define _gettchar   getwchar
		#define _getts      getws
		#define _puttc      putwc
		#define _puttchar   putwchar
		#define _putts      putws
		#define _ungettc    ungetwc

		#define _tcstod     wcstod
		#define _tcstol     wcstol
		#define _tcstoul    wcstoul

		#define _itot       _itoa
		#define _ltot       _ltoa
		#define _ultot      _ultoa
		#define _ttoi       rs_wtoi
		#define _ttol       rs_wtoi

		#define _tcscat     wcscat
		#define _tcscpy     wcscpy
		#define _tcsdup     _wcsdup
		#define _tcslen     wcslen
		#define _tcsxfrm    wcsxfrm
		#define _tfopen     _wfopen
		#define _tcschr     wcschr
		#define _tcscspn    wcscspn
		#define _tcsncat    wcsncat
		#define _tcsncpy    wcsncpy
		#define _tcspbrk    wcspbrk
		#define _tcsrchr    wcsrchr
		#define _tcsspn     wcsspn
		#define _tcsstr     wcsstr
		#define _tcstok     wcstok
		#define _tcsnset    _wcsnset
		#define _tcsrev     _wcsrev
		#define _tcsset     _wcsset
		#define _tcscmp     wcscmp
		#define _tcsicmp    wcsicmp
		#define _tcsnccmp   wcsncmp
		#define _tcsncmp    wcsncmp
		#define _tcsncicmp  wcsnicmp
		#define _tcsnicmp   wcsnicmp
		#define _tcscoll    wcscoll
		#define _tcsicoll   _wcsicoll
		#define _tcsnccoll  _wcsncoll
		#define _tcsncoll   _wcsncoll
		#define _tcsncicoll _wcsnicoll
		#define _tcsnicoll  _wcsnicoll
		#define _tcsclen    wcslen
		#define _tcsnccat   wcsncat
		#define _tcsnccpy   wcsncpy
		#define _tcsncset   _wcsnset
		#define _tcslwr     _wcslwr
		#define _tcsupr     _wcsupr
		#define _tcsxfrm    wcsxfrm

		#define _tchdir     chdir
		#define _tmkdir     mkdir
		#define _trmdir     rmdir

		#define _tremove    _wremove
		#define _trename    _wrename
		#define _tunlink    _wunlink
	#else
		#define _tprintf    printf
		#define _ftprintf   fprintf
		#define _stprintf   sprintf
		#define _sntprintf  snprintf
		#define _vtprintf   vprintf
		#define _vftprintf  vfprintf
		#define _vstprintf  vsprintf
		#define _vsntprintf vsnprintf
		#define _tscanf     scanf
		#define _ftscanf    fscanf
		#define _stscanf    sscanf
		#define _fgettc     fgetc
		#define _fgettchar  _fgetchar
		#define _fgetts     fgets
		#define _fputtc     fputc
		#define _fputtchar  _fputchar
		#define _fputts     fputs
		#define _gettc      getc
		#define _gettchar   getchar
		#define _getts      gets
		#define _puttc      putc
		#define _puttchar   putchar
		#define _putts      puts
		#define _ungettc    ungetc
		#define _tcstod     strtod
		#define _tcstol     strtol
		#define _tcstoul    strtoul
		#define _itot       _itoa
		#define _ltot       _ltoa
		#define _ultot      _ultoa
		#define _ttoi       atoi
		#define _ttol       atol
		#define _tcscat     strcat
		#define _tcscpy     strcpy
		#define _tcsdup     _strdup
		#define _tcslen     strlen
		#define _tcsxfrm    strxfrm
		#define _tfopen     fopen
		#define _tcschr     strchr
		#define _tcscspn    strcspn
		#define _tcsncat    strncat
		#define _tcsncpy    strncpy
		#define _tcspbrk    strpbrk
		#define _tcsrchr    strrchr
		#define _tcsspn     strspn
		#define _tcsstr     strstr
		#define _tcstok     strtok
		#define _tcsnset    _strnset
		#define _tcsrev     _strrev
		#define _tcsset     _strset
		#define _tcscmp     strcmp
		#define _tcsicmp    strcasecmp
		#define _tcsnccmp   strncmp
		#define _tcsncmp    strncmp
		#define _tcsncicmp  _strnicmp
		#define _tcsnicmp   strncasecmp
		#define _tcscoll    strcoll
		#define _tcsicoll   _stricoll
		#define _tcsnccoll  _strncoll
		#define _tcsncoll   _strncoll
		#define _tcsncicoll _strnicoll
		#define _tcsnicoll  _strnicoll
		#define _tcsclen    strlen
		#define _tcsnccat   strncat
		#define _tcsnccpy   strncpy
		#define _tcsncset   _strnset
		#define _tcslwr     _strlwr
		#define _tcsupr     _strupr
		#define _tcsxfrm    strxfrm

		#define _tchdir     chdir
		#define _tmkdir     mkdir
		#define _trmdir     rmdir

		#define _tremove    remove
		#define _trename    rename
		#define _tunlink    unlink
	#endif
#endif /* #if defined(__linux__) */

/* 修正标准库，使得各个平台一致 */
// #if !defined(VS_IS_VC6)
// 	#undef strncmp
// 	#define strncmp np_strncmp
// 
// 	#undef stricmp
// 	#define stricmp np_stricmp
// 	#undef strcasecmp
// 	#define strcasecmp np_stricmp
// 
// 	#undef strnicmp
// 	#define strnicmp np_strnicmp
// 	#undef strncasecmp
// 	#define strncasecmp np_strnicmp
// #endif
// 
// #undef wcscmp
// #define wcscmp		np_wcscmp
// #undef wcsncmp
// #define wcsncmp		np_wcsncmp
// #undef wcsicmp
// #define wcsicmp		np_wcsicmp
// #undef wcsnicmp
// #define wcsnicmp	np_wcsnicmp
// 
// #undef _tcscmp
// #undef _tcsncmp
// #undef _tcsicmp
// #undef _tcsnicmp
// #if defined(_UNICODE) || defined(UNICODE)
// 	#define _tcscmp			wcscmp
// 	#define _tcsncmp		wcsncmp
// 	#define _tcsicmp		wcsicmp
// 	#define _tcsnicmp		wcsnicmp
// #else
// 	#define _tcscmp			strcmp
// 	#define _tcsncmp		strncmp
// 	#define _tcsicmp		stricmp
// 	#define _tcsnicmp		strnicmp
// #endif

///* 修正标准库，使用x86汇编优化版 */

#endif