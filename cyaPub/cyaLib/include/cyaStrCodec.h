#ifndef __CYA_STR_CODEC_H__
#define __CYA_STR_CODEC_H__

#if defined(_MSC_VER)&&_MSC_VER>1000
	#pragma once
#endif

#include "cyaTypes.h"
/* char、WCHAR 和 TCHAR字符串 之间的互相转换
src : 要转换的源字符串
dst : 转换后的目标bufer
dstBytes : 目标buf的字节长度
srcLen : 要转换的字符个数(linux下该参数无效)
outLen : 返回输出的字符长度（不包括'\0'）
*/
const char* wctoc(const WCHAR* src, char* dst, int dstBytes, int srcLen = -1, OUT int* outLen = NULL);
const WCHAR* ctowc(const char* src, WCHAR* dst, int dstBytes, int srcLen = -1, OUT int* outLen = NULL);

//在unicode下和前面两个函数一样，否则直接返回src,不做拷贝
const WCHAR* tctowc(const TCHAR* src, WCHAR* dst, int dstBytes, int srcLen = -1, OUT int* outLen = NULL);
const TCHAR* wctotc(const WCHAR* src, TCHAR* dst, int dstBytes, int srcLen = -1, OUT int* outLen = NULL);
const char*  tctoc(const TCHAR* src, char* dst, int dstBytes, int srcLen = -1, OUT int* outLen = NULL);
const TCHAR* ctotc(const char* src, TCHAR* dst, int dstBytes, int srcLen = -1, OUT int* outLen = NULL);

#endif