#ifndef __EQUIP_COMPOSE_CFG_H__
#define __EQUIP_COMPOSE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include <vector>
#include "CfgCom.h"

class EquipComposeCfg
{
public:
	EquipComposeCfg();
	~EquipComposeCfg();

#pragma pack(push, 1)
	union UComposeCondition
	{
		struct
		{
			BYTE minLv;
			BYTE maxLv;
		} lv;

		struct
		{
			LTMSEL start;
			LTMSEL end;
		} tm;

		struct
		{
			UINT16 magicId;
		} magic;

		struct
		{
			BYTE num;
			SDropPropCfgItem* pHasProp;
		} prop;
	};
	struct SComposeConditionItem
	{
		BYTE type;
		UComposeCondition item;
	};
	struct SEquipComposeItem
	{
		std::vector<SDropPropCfgItem> compsoePropVec;			//合成得到物品
		std::vector<SDropPropCfgItem> basicConsumePropVec;		//基本消耗物品
		std::vector<SDropPropCfgItem> additionalConsumePropVec;	//额外消耗物品
		std::vector<SComposeConditionItem> conditionVec;		//条件
		BYTE chance;		//合成概率
		UINT32 consumeGold;	//消耗金币
	};
#pragma pack(pop)

public:
	//载入配置
	BOOL Load(const char* pszFile);
	//获取装备合成属性
	int GetEquipComposeItem(UINT16 composeId, SEquipComposeItem*& pComposeItem);

private:
	std::map<UINT16, SEquipComposeItem> m_equipComposeMap;
};

#endif