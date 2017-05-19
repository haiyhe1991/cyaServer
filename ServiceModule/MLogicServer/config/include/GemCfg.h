#ifndef __GEM_CFG_H__
#define __GEM_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
宝石配置
*/

#include <map>
#include "CfgCom.h"
#include "ServiceMLogic.h"

class GemCfg
{
public:
	GemCfg();
	~GemCfg();

#pragma pack(push, 1)
	struct SGemAttrCfg
	{
		BYTE type;	//宝石类型
		BYTE level;	//等级
		UINT64 playerSellGold;	//玩家出售金币价
		//UINT32 shopSellToken;	//商城出售代币
		//UINT64 playerBuyBackGold;	//玩家回购金币价
		UINT32 attrVal;				//宝石属性值
		BYTE   maxStack;			//最大堆叠数
		SGemAttrCfg()
		{
			type = 0;
			level = 0;
			playerSellGold = 0;
			//shopSellToken = 0;
			//playerBuyBackGold = 0;
			attrVal = 0;
			maxStack = 0;
		}
	};
	struct SEquipGemHoleLimitCfg
	{
		BYTE equipLv;	//装备强化等级
		BYTE gemHoles;	//宝石开孔号
		SEquipGemHoleLimitCfg()
		{
			equipLv = 0;
			gemHoles = 0;
		}
	};
	struct SGemConsumeCfg
	{
		UINT64 loadGold;	//镶嵌消耗
		UINT64 unloadGold;	//取出消耗
		UINT64 composeGold;	//合成消耗
		UINT64 decomposeGold;	//分解消耗
		SGemConsumeCfg()
		{
			loadGold = 0;
			unloadGold = 0;
			composeGold = 0;
			decomposeGold = 0;
		}
	};
	struct SEquipGemAttr
	{
		BYTE gemType;		//宝石类型
		BYTE equipLimitlv;	//装备强化等级限制
		SEquipGemAttr()
		{
			gemType = 0;
			equipLimitlv = 0;
		}
	};
#pragma pack(pop)

public:
	//载入宝石相关配置
	BOOL Load(const char* szGemFile, const char* szGemCostFile, const char* szGemHoleFile);
	//根据装备类型,宝石孔获取对应宝石类型,装备等级限制
	int GetEquipInlaidGemAttr(BYTE equipType, BYTE holeSeq, SEquipGemAttr& attr);
	//根据等级获取宝石消耗
	int GetGemSpendByLevel(BYTE gemLv, SGemConsumeCfg& spend);
	//获取宝石配置属性
	int GetGemCfgAttr(UINT16 gemPropId, SGemAttrCfg& gemAttr);
	//获取宝石配置属性
	int GetGemPropAttr(UINT16 gemPropId, SPropertyAttr& propAttr, BYTE& maxStack);
	//根据宝石类型,等级获取宝石id,最大堆叠数量
	int GetGemIdMaxStackByTypeLevel(BYTE gemType, BYTE gemLv, UINT16& gemId, BYTE& maxStack);
	//根据宝石id获取最大堆叠数
	BYTE GetGemMaxStack(UINT16 gemId);
	//获取宝石用户出售价格(单价)
	int GetGemUserSellPrice(UINT16 gemId, UINT64& price);
	//根据宝石id获取宝石消耗
	int GetGemSpendById(UINT16 gemId, SGemConsumeCfg& spend);
	//获取宝石合成消耗宝石数量
	UINT16 GetComposeGemConsumeGems() const;

private:
	//载入宝石配置
	BOOL LoadGemCfg(const char* filename);
	//载入装备宝石开孔限制配置
	BOOL LoadGemHoleLimitCfg(const char* filename);
	//载入宝石消耗配置
	BOOL LoadGemConsumeCfg(const char* filename);
	//载入装备宝石配置
	BOOL LoadEquipGemCfg(const char* filename);

private:
	std::map<UINT16/*宝石id*/, SGemAttrCfg> m_gemAttrMap;	//宝石配置
	std::map<BYTE/*装备强化等级*/, BYTE/*开孔数*/> m_equipLimitHolesMap;	//装备开孔限制
	std::map<BYTE/*宝石等级*/, SGemConsumeCfg> m_gemConsumeMap;	//宝石消耗
	std::map<BYTE/*装备类型*/, std::vector<SEquipGemAttr>/*孔镶嵌宝石类型*/ > m_equipGemHolesMap;
};


#endif