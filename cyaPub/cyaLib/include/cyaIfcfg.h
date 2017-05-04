#ifndef __CYA_IFCFG_H__
#define __CYA_IFCFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"
#include <vector>

struct IP_MASK_GW
{
	union
	{
		DWORD ip;
		struct
		{
			BYTE bip0_, bip1_, bip2_, bip3_;
		};
	};

	union
	{
		DWORD mask;
		struct
		{
			BYTE bmk0_, bmk1_, bmk2_, bmk3_;
		};
	};

	union
	{
		DWORD gateway;
		struct
		{
			BYTE bgw0_, bgw1_, bgw2_, bgw3_;
		};
	};
};

/// �г�����������, ��linux�°��� lo
/// @param OUT ifs/��������������,
void IfCfgListInterface(std::vector<std::string>& ifs);

/// �������MAC��ַ
/// @param IN name ������
/// @param OUT macAddr MAC��ַ
/// @return false �Ҳ���ָ��������
BOOL IfCfgGetMacAddrByName(const char* name, BYTE macAddr[8]);

/// �������MAC��ַ
/// @param IN ipAddr IP��ַ,
/// @param OUT macAddr MAC��ַ
/// @return false �Ҳ���ָ��������
BOOL IfCfgGetMacAddrByIp(DWORD ipAddr, BYTE macAddr[8]);

//��ȡ��������IP��ַ
int IfcfgGetHostIps(DWORD ips[32]);

// ͨ����������õ�IP��ʧ�ܷ���INADDR_NONE
DWORD GetIPByName(const char* lpszName);

#if defined(OS_IS_APPLE)
	BOOL GetHostMacAddr(BYTE macAddr[8]);
#endif

//��ջ�����Ϣ 
void IfCfgReset();


#endif