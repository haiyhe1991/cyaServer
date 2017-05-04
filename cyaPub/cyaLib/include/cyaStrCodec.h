#ifndef __CYA_STR_CODEC_H__
#define __CYA_STR_CODEC_H__

#if defined(_MSC_VER)&&_MSC_VER>1000
	#pragma once
#endif

#include "cyaTypes.h"
/* char��WCHAR �� TCHAR�ַ��� ֮��Ļ���ת��
src : Ҫת����Դ�ַ���
dst : ת�����Ŀ��bufer
dstBytes : Ŀ��buf���ֽڳ���
srcLen : Ҫת�����ַ�����(linux�¸ò�����Ч)
outLen : ����������ַ����ȣ�������'\0'��
*/
const char* wctoc(const WCHAR* src, char* dst, int dstBytes, int srcLen = -1, OUT int* outLen = NULL);
const WCHAR* ctowc(const char* src, WCHAR* dst, int dstBytes, int srcLen = -1, OUT int* outLen = NULL);

//��unicode�º�ǰ����������һ��������ֱ�ӷ���src,��������
const WCHAR* tctowc(const TCHAR* src, WCHAR* dst, int dstBytes, int srcLen = -1, OUT int* outLen = NULL);
const TCHAR* wctotc(const WCHAR* src, TCHAR* dst, int dstBytes, int srcLen = -1, OUT int* outLen = NULL);
const char*  tctoc(const TCHAR* src, char* dst, int dstBytes, int srcLen = -1, OUT int* outLen = NULL);
const TCHAR* ctotc(const char* src, TCHAR* dst, int dstBytes, int srcLen = -1, OUT int* outLen = NULL);

#endif