#ifndef __INIT_ROLE_GIVE_CFG_H__
#define __INIT_ROLE_GIVE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
角色初始赠送
*/

#include <vector>
#include "CfgCom.h"

class InitRoleGiveCfg
{
public:
	InitRoleGiveCfg();
	~InitRoleGiveCfg();

#pragma pack(push, 1)
	struct SMoneyItem
	{
		UINT32 gold;
		UINT32 rpcoin;
		SMoneyItem()
		{
			gold = 0;
			rpcoin = 0;
		}
	};
	struct SOtherItem
	{
		UINT32 exp;
		UINT32 vit;
		SOtherItem()
		{
			exp = 0;
			vit = 0;
		}
	};
	// 	struct SEquipItem
	// 	{
	// 		UINT16 handId;		//护手
	// 		UINT16 shoesId;		//鞋子
	// 		UINT16 necklaceId;	//项链
	// 		UINT16 ringId;		//戒子
	// 		UINT16 weaponId;	//武器
	// 		UINT16 armorId;		//盔甲
	// 		SEquipItem()
	// 		{
	// 			handId = 0;
	// 			shoesId = 0;
	// 			necklaceId = 0;
	// 			ringId = 0;
	// 			weaponId = 0;
	// 			armorId = 0;
	// 		}
	// 	};
	struct SInitRoleGiveItem
	{
		SMoneyItem money;
		SOtherItem other;
		//SEquipItem wearEquip;
		std::vector<UINT16> wearEquipVec;
		std::vector<SDropPropCfgItem> propVec;
		SInitRoleGiveItem()
		{
			wearEquipVec.clear();
			propVec.clear();
		}
	};
#pragma pack(pop)

public:
	//载入配置文件
	BOOL Load(const char* filename);
	//获取角色初始赠送
	BOOL GetRoleInitGiveItem(SInitRoleGiveItem& giveItem);

private:
	std::string m_filename;
};

#endif