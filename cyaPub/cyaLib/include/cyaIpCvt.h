#ifndef __CYA_IP_CVT_H__
#define __CYA_IP_CVT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"

//是否合法字符串IP
BOOL IsValidStringIP(const char* lpszIP, DWORD* dwIp = NULL);

// DWORD ip 转化为 字符串ip
// szBuf必须传递，至少为16字节长
const char* GetStringIP(char szBuf[16], DWORD dwIP);

// 字符串ip 转化为 DWORD ip，失败返回INADDR_NONE
DWORD GetDWordIP(const char* lpszIP);

#endif