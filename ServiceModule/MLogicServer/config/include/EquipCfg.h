#ifndef __EQUIP_CFG_H__
#define __EQUIP_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
装备配置
*/

#include <map>
#include <set>
#include <vector>
#include "CfgCom.h"

class EquipCfg
{
public:
	EquipCfg();
	~EquipCfg();

#pragma pack(push, 1)
	//赠送物品信息
	// 	struct SGiveProps
	// 	{
	// 		UINT16 propId;
	// 		BYTE   propType;
	// 		BYTE   num;
	// 		SGiveProps()
	// 		{
	// 			propId = 0;
	// 			propType = 0;
	// 			num = 0;
	// 		}
	// 	};
	//赠送装备信息
	// 	struct SGiveWearEquip
	// 	{
	// 		UINT16 handId;		//护手
	// 		UINT16 shoesId;		//鞋子
	// 		UINT16 necklaceId;	//项链
	// 		UINT16 ringId;		//戒子
	// 		UINT16 weaponId;	//武器
	// 		UINT16 armorId;		//盔甲
	// 
	// 		SGiveWearEquip()
	// 		{
	// 			handId = 0;
	// 			shoesId = 0;
	// 			necklaceId = 0;
	// 			ringId = 0;
	// 			weaponId = 0;
	// 			armorId = 0;
	// 		}
	// 	};
	//赠送信息
	// 	struct SGiveInfo
	// 	{
	// 		UINT64 gold;							//赠送金币
	// 		UINT32 rpcoin;							//赠送代币
	// 		SGiveWearEquip giveWearEquip;			//赠送装备
	// 		std::vector<SGiveProps> giveProps;	//赠送物品
	// 		SGiveInfo()
	// 		{
	// 			gold = 0;
	// 			rpcoin = 0;
	// 			giveProps.clear();
	// 		}
	// 	};
	//装备加点
	struct SEquipAddPoint
	{
		BYTE   type;	//加点类型
		UINT32 val;		//加点值
		BYTE   master;	//主/副属性

		SEquipAddPoint()
		{
			type = 0;
			val = 0;
			master = 0;
		}
	};
	//装备属性
	struct SEquitAttr
	{
		std::string name;				//名称
		BYTE quality;		//品质
		BYTE pos;			//装备位置
		BYTE limitLevel;	//人物限制等级(暂时无效,不做判断)
		UINT16 suitId;		//套装id
		UINT64 sellPrice;	//售价(金币)
		std::vector<SEquipAddPoint> equipPointVec;	//装备属性加点
		SEquitAttr()
		{
			name = "";
			quality = 0;
			pos = 0;
			limitLevel = 0;
			suitId = 0;
			sellPrice = 0;
			equipPointVec.clear();
		}
	};
#pragma pack(pop)

public:
	//载入职业装备配置
	BOOL Load(const char* pszFile);
	//获取装备信息
	int GetEquipPropAttr(UINT16 equipId, SPropertyAttr& propAttr, BYTE& maxStack);
	//获取装备用户出售价格
	int GetEquipUserSellPrice(UINT16 equipId, UINT64& price);
	//获取装备最大堆叠数
	BYTE GetEquipMaxStack(UINT16 equipId);
	//获取装备名称
	void GetEquipName(UINT16 equipId, std::string& name);
	//获取装备属性点
	int GetEquipAttrPoint(UINT16 equipId, std::vector<SEquipAddPoint>*& addPoint);

private:
	std::map<UINT16/*装备id*/, SEquitAttr> m_equipMap;			//装备表
	std::map<UINT16/*套装id*/, std::set<UINT16>/*装备id*/ > m_suitEquipMap;	//套装
};
#endif