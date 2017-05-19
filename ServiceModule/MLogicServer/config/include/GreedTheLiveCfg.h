#ifndef __GREED_THE_LIVE_CFG_H__
#define __GREED_THE_LIVE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include <vector>
#include "CfgCom.h"

class GreedTheLiveCfg
{
public:
	// 升级所需道具
	struct SUpCost
	{
		UINT32 id;							//物品ID
		BYTE type;							//物品类型
		BYTE num;							//物品数量
	};

	// 升级属性加成
	struct SAttributes
	{
		BYTE type;							//属性类型
		UINT32 attrVal;						//属性加成值
	};

	// 炼魂部件配置
	struct ChainSoulPosConfig
	{
		std::vector<SUpCost> cost;			//升级消耗
		std::vector<SAttributes> attr_plus;	//属性加成
		UINT16 next_id;						//下一级id
	};

	// 炼魂核心部件配置
	struct ChainSoulCoreConfig
	{
		BYTE condition_level;				//升级条件
		std::vector<SAttributes> attr_plus;	//属性加成
	};

	// 部件id->配置
	typedef std::map<UINT16, ChainSoulPosConfig> cspc_map;

public:
	GreedTheLiveCfg();
	~GreedTheLiveCfg();

public:
	//载入配置
	BOOL Load(const char* pszFile);

	//是否启用
	bool IsOpen() const;

	//获取部件配置
	bool GetChainSoulPosConfig(BYTE type, BYTE level, ChainSoulPosConfig *&out_config);

	//获取圣物核心配置
	bool GetChainSoulCoreConfig(BYTE level, ChainSoulCoreConfig *&out_config);

	//根据条件获取核心等级
	BYTE GetCoreLevelByConditionLevel(BYTE condition_level);

private:
	bool m_is_open;
	std::map<BYTE, cspc_map> m_chainSoulPosConfig;
	std::map<BYTE, ChainSoulCoreConfig> m_chainSoulCoreConfig;
	std::map<BYTE, std::vector<UINT16> > m_typeSet;
	std::map<BYTE, BYTE> m_conditionCorrespondsCore;
};

#endif