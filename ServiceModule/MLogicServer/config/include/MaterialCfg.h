#ifndef __MATERIAL_CFG_H__
#define __MATERIAL_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
材料配置
*/
#include <map>
#include "CfgCom.h"
#include "ServiceMLogic.h"

class MaterialCfg
{
public:
	MaterialCfg();
	~MaterialCfg();

#pragma pack(push, 1)
	struct SMaterialAttr
	{
		BYTE   level;			//材料等级
		BYTE   limitRoleLevel;	//限制角色等级(角色等级小于该值则不应该有该物品)
		UINT64 sellPrice;	//售价
		//UINT64 backPrice;	//回购价
		//UINT64 shopSellPrice;	//商店售价
		BYTE   bindMode;	//绑定模式
		BYTE   maxStackCount;	//最大堆叠数
		BYTE   quality;		//品质
		std::vector<UINT32> funcValVec;	//材料功能值
		SMaterialAttr()
		{
			level = 0;
			limitRoleLevel = 0;
			sellPrice = 0;
			//backPrice = 0;
			bindMode = 0;
			maxStackCount = 0;
			quality = 0;
			funcValVec.clear();
		}
	};
#pragma pack(pop)

public:
	//载入物品配置文件
	BOOL Load(const char* filename);
	//获取材料属性
	int GetMaterialPropAttr(UINT16 materiaId, SPropertyAttr& propAttr, BYTE& maxStackCount);
	//获取物品最大堆叠数量
	BYTE GetMaterialMaxStack(UINT16 materiaId);
	//获取材料用户出售价格(单价)
	int GetMaterialUserSellPrice(UINT16 materiaId, UINT64& price);
	//获取材料功能值
	int GetMaterialFuncValue(UINT16 materiaId, UINT32& funcVal);
	//获取材料功能值
	int GetMaterialFuncValueVec(UINT16 materiaId, std::vector<UINT32>& funcValVec);

private:
	std::map<UINT16/*物品id*/, SMaterialAttr> m_materialAttrCfg;	//物品配置表
};

#endif