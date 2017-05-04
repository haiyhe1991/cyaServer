#ifndef __CYA_IP_CVT_H__
#define __CYA_IP_CVT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"

//�Ƿ�Ϸ��ַ���IP
BOOL IsValidStringIP(const char* lpszIP, DWORD* dwIp = NULL);

// DWORD ip ת��Ϊ �ַ���ip
// szBuf���봫�ݣ�����Ϊ16�ֽڳ�
const char* GetStringIP(char szBuf[16], DWORD dwIP);

// �ַ���ip ת��Ϊ DWORD ip��ʧ�ܷ���INADDR_NONE
DWORD GetDWordIP(const char* lpszIP);

#endif