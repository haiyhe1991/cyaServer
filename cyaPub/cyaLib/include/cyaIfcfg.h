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

/// 列出所有网卡名, 在linux下包括 lo
/// @param OUT ifs/所有网卡的名字,
void IfCfgListInterface(std::vector<std::string>& ifs);

/// 获得网卡MAC地址
/// @param IN name 网卡名
/// @param OUT macAddr MAC地址
/// @return false 找不到指定的网卡
BOOL IfCfgGetMacAddrByName(const char* name, BYTE macAddr[8]);

/// 获得网卡MAC地址
/// @param IN ipAddr IP地址,
/// @param OUT macAddr MAC地址
/// @return false 找不到指定的网卡
BOOL IfCfgGetMacAddrByIp(DWORD ipAddr, BYTE macAddr[8]);

//获取主机所有IP地址
int IfcfgGetHostIps(DWORD ips[32]);

// 通过计算机名得到IP，失败返回INADDR_NONE
DWORD GetIPByName(const char* lpszName);

#if defined(OS_IS_APPLE)
	BOOL GetHostMacAddr(BYTE macAddr[8]);
#endif

//清空缓存信息 
void IfCfgReset();


#endif