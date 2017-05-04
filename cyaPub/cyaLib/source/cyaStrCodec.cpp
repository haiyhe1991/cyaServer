#include "cyaStrCodec.h"
#include "cyaString.h"

const char* wctoc(const WCHAR* src, char* dst, int dstBytes, int srcLen /*= -1*/, OUT int* outLen/* = NULL*/)
{
	if (src == NULL || dst == NULL || dstBytes <= 0)
	{
		return NULL;
	}
	if (outLen)
	{
		*outLen = 0;
	}
#if defined(WIN32)
	//���ص���д��dst���ֽ�����src>0��������������\0�����ֽڣ�-1�������
	int ret = WideCharToMultiByte(CP_ACP, 0, src, src <=0 ? -1 : srcLen, dst, dstBytes, NULL, NULL);
	if (ret <= 0)
		return NULL;
	if (srcLen > 0)
	{
		if (dstBytes <= ret)
			return NULL;
		dst[ret] = 0;
	}
	if (outLen)
		*outLen = srcLen > 0 ? ret : ret - 1;
	return dst;
#else
	#if defined(OS_IS_ANDROID)||defined(OS_IS_APPLE)
		return dst;
	#else
		int n = (int)wcstombs(dst, src, dstBytes);
		if (n <= 0)
			return NULL;
		if (dstBytes <= n)
			return NULL;
		dst[n] = 0;
		if (outLen)
			*outLen = n;
		return dst;
	#endif
#endif
}

const WCHAR* ctowc(const char* src, WCHAR* dst, int dstBytes, int srcLen /*= -1*/, OUT int* outLen /*= NULL*/)
{
	if (src == NULL || dst == NULL || dstBytes <= 0)
		return NULL;
	if (outLen)
		*outLen = 0;
#if defined(WIN32)
	//���ص���д��dst���ַ�����(src>0������������'\0', -1�����)
	int ret = MultiByteToWideChar(CP_ACP, 0, src, srcLen <= 0 ? -1 : srcLen, dst, dstBytes / sizeof(WCHAR));
	if (ret <= 0)
		return NULL;
	if (srcLen > 0)
	{
		if (dstBytes / sizeof(WCHAR) <= ret)
			return NULL;
		dst[ret] = 0;
	}
	if (outLen)
		*outLen = srcLen > 0 ? ret : ret - 1;
	return dst;
#else
	#if defined(OS_IS_ANDROID) || defined(OS_IS_APPLE)
		return dst;
	#else
		int n = (int)mbstowcs(dst, src, dstBytes / sizeof(WCHAR));
		if (n <= 0)
			return NULL;
		if (dstBytes <= n)
			return NULL;
		dst[n] = 0;
		if (outLen)
			*outLen = n;
		return dst;
	#endif
#endif
}

const WCHAR* tctowc(const TCHAR* src, WCHAR* dst, int dstBytes, int srcLen /*= -1*/, OUT int* outLen /*= NULL*/)
{
#if defined(UNICODE) || defined(_UNICODE)
	if (outLen)
		*outLen = srcLen > 0 ? srcLen : (int)wcslen(src);
	wcscpy(dst, src);
	return src;
#else
	return ctowc(src, dst, dstBytes, srcLen, outLen);
#endif
}

const TCHAR* wctotc(const WCHAR* src, TCHAR* dst, int dstBytes, int srcLen /*= -1*/, OUT int* outLen /*= NULL*/)
{
#if defined(UNICODE) || defined(_UNICODE)
	if (outLen)
		*outLen = srcLen > 0 ? srcLen : (int)wcslen(src);
	wcscpy(dst, src);
	return src;
#else
	return wctoc(src, dst, dstBytes, srcLen, outLen);
#endif
}

const char* tctoc(const TCHAR* src, char* dst, int dstBytes, int srcLen /*= -1*/, OUT int* outLen /*= NULL*/)
{
#if defined(UNICODE) || defined(_UNICODE)
	return wctoc(src, dst, dstBytes, srcLen, outLen);
#else
	dst; dstBytes; //��ֹ���뾯��
	if (outLen)
		*outLen = srcLen > 0 ? srcLen : (int)strlen(src);
	strcpy(dst, src);
	return src;
#endif
}

const TCHAR* ctotc(const char* src, TCHAR* dst, int dstBytes, int srcLen /*= -1*/, OUT int* outLen /*= NULL*/)
{
#if defined(UNICODE) || defined(_UNICODE)
	return ctowc(src, dst, dstBytes, srcLen, outLen);
#else
	dst; dstBytes;	//��ֹ���뾯��
	if (outLen)
		*outLen = srcLen > 0 ? srcLen : (int)strlen(src);
	strcpy(dst, src);
	return src;
#endif
}