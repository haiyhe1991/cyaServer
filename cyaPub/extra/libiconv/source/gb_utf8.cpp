#include "gb_utf8.h"

GBUTF8Codec::GBUTF8Codec()
{

}

GBUTF8Codec::~GBUTF8Codec()
{

}

int GBUTF8Codec::GB2312ToUTF8(const char* pszGb2312, int gbLen, char* pszUtf8, int utf8BufLen)
{
	if (!pszGb2312 || gbLen <= 0 || !pszUtf8 || utf8BufLen <= 0)
		return 0;

	char** pIn = (char**)&pszGb2312;
	size_t inLen = gbLen;
	char** pOut = &pszUtf8;
	size_t outLen = utf8BufLen;

	iconv_t convfd = iconv_open("utf-8", "gb2312");
	if (!convfd)
		return 0;

	size_t n = iconv(convfd, pIn, (size_t*)&inLen, pOut, (size_t*)&outLen);
	iconv_close(convfd);
	if (n != 0)
		return 0;
	return (int)(utf8BufLen - outLen);
}

int GBUTF8Codec::UTF8ToGB2312(const char* pszUtf8, int utf8Len, char* pszGb2312, int gbBufLen)
{
	if (!pszUtf8 || utf8Len <= 0 || !pszGb2312 || gbBufLen <= 0)
		return 0;

	char** pIn = (char**)&pszUtf8;
	size_t inLen = utf8Len;
	char** pOut = &pszGb2312;
	size_t outLen = gbBufLen;

	iconv_t convfd = iconv_open("gb2312", "utf-8");
	if (!convfd)
		return 0;

	size_t n = iconv(convfd, pIn, (size_t*)&inLen, pOut, (size_t*)&outLen);
	iconv_close(convfd);
	if (n != 0)
		return 0;
	return (int)(gbBufLen - outLen);
}

int gb2312_utf8(const char* pszGb2312, int gbLen, char* pszUtf8, int utf8BufLen)
{
	GBUTF8Codec codec;
	return codec.GB2312ToUTF8(pszGb2312, gbLen, pszUtf8, utf8BufLen);
}

int utf8_gb2312(const char* pszUtf8, int utf8Len, char* pszGb2312, int gbBufLen)
{
	GBUTF8Codec codec;
	return codec.UTF8ToGB2312(pszUtf8, utf8Len, pszGb2312, gbBufLen);
}