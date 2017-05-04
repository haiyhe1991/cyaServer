#ifndef __GB_UTF8_H__
#define __GB_UTF8_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
×Ö·û×ªÂë
*/

#include "iconv.h"

class GBUTF8Codec
{
public:
	GBUTF8Codec();
	~GBUTF8Codec();

	int GB2312ToUTF8(const char* pszGb2312, int gbLen, char* pszUtf8, int utf8BufLen);
	int UTF8ToGB2312(const char* pszUtf8, int utf8Len, char* pszGb2312, int gbBufLen);
};

int gb2312_utf8(const char* pszGb2312, int gbLen, char* pszUtf8, int utf8BufLen);
int utf8_gb2312(const char* pszUtf8, int utf8Len, char* pszGb2312, int gbBufLen);

#endif	//_gb_utf8_h__