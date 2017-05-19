#ifndef __MALL_CFG_H__
#define __MALL_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
商城配置
*/

#include <map>
#include <set>
#include "CfgCom.h"

class MallCfg
{
public:
	MallCfg();
	~MallCfg();

#pragma pack(push, 1)
	struct SMallGoodsAttr
	{
		BYTE  type;				//商品类型
		UINT16 propId;			//物品id
		double discount;		//折扣
		UINT32 priceToken;		//售价(代币)
		LTMSEL sellDateLimit;	//售卖时间限制
		UINT16 useDays;			//使用天数
		UINT16 stackNum;		//单次购买数量
		BYTE   vipLvLimit;		//VIP等级限制(VIP等级>=vipLvLimit才能购买此物品)
		BYTE   discountVIPLvLimit;	//折扣VIP等级限制(VIP等级>=discountVIPLvLimit才能打discount折)

		SMallGoodsAttr()
		{
			type = 0;				//商品类型
			propId = 0;				//物品id
			discount = 0.0;			//折扣
			priceToken = 0;			//售价(代币)
			sellDateLimit = 0;		//售卖时间限制
			useDays = 0;			//使用天数
			stackNum = 0;			//单次购买最大数量
			vipLvLimit = 0;			//VIP等级限制
			discountVIPLvLimit = 0;	//折扣VIP等级限制
		}
	};
#pragma pack(pop)

public:
	//载入商城配置
	BOOL LoadMall(const char* filename);
	//获取商城物品属性
	int GetMallBuyGoodsAttr(UINT32 buyId, SMallGoodsAttr*& pGoodsAttr);
	int GetMallBuyGoodsAttrByPropId(UINT16 propId, SMallGoodsAttr *&pGoodsAttr);

private:
	std::map<UINT16, UINT32> m_goodsIndex;
	std::map<UINT32, SMallGoodsAttr> m_goodsMap;
};

#endif