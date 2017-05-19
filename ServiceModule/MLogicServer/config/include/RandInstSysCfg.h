#ifndef __RAND_INST_SYS_CFG_H__
#define __RAND_INST_SYS_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

/*
随机副本系统配置
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class RandInstSysCfg
{
public:
	RandInstSysCfg();
	~RandInstSysCfg();

#pragma pack(push, 1)
	struct SRandInstSysParam
	{
		BYTE isOpen;					//是否开启
		BYTE chance;					//概率
		UINT32 timerInterVal;			//刷新时间间隔
		UINT16 maxNumPerTransfer;		//每个传送阵人数限制
		UINT16 maxOpenTransferNum;		//最大传送阵开启数量
		UINT16 minOpenTransferNum;		//最小传送阵开启数量
		UINT32 openDurationTime;		//开启持续时间

		BYTE startHour;					//每日开启时间(小时)
		BYTE startMin;					//每日开启时间(分钟)
		BYTE endHour;					//每日结束时间(小时)
		BYTE endMin;					//每日结束时间(分钟)

		std::vector<UINT16> transferVec;	//传送阵数量
	};
#pragma pack(pop)

public:
	//载入配置
	BOOL Load(const char* filename);
	//获取随机副本系统参数
	BOOL GetRandInstSysParam(SRandInstSysParam& param);

private:
	std::string m_filename;
};

#endif