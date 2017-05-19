#ifndef __GIFT_BAG_H__
#define __GIFT_BAG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
礼包配置
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class GiftBagCfg
{
public:
	GiftBagCfg();
	~GiftBagCfg();

#pragma pack(push, 1)
	struct SGiftBagItem
	{
		UINT32 gold;
		UINT32 rpcoin;
		UINT32 exp;
		UINT32 vit;
		char   name[32]; //add by hxw 20151222
		char   desc[128];//add by hxw 20151222
		std::vector<SDropPropCfgItem> propVec;
		SGiftBagItem()
		{
			memset(name, 0, sizeof(name));
			memset(desc, 0, sizeof(desc));
		}
	};
#pragma pack(pop)

public:
	/*载入配置*/
	BOOL Load(const char* filename, BOOL loadInMem = false);
	//获取礼包物品
	int GetGiftBagItem(UINT16 id, SGiftBagItem& item);

private:
	BOOL m_loadInMem;
	std::string m_filename;
	std::map<UINT16, SGiftBagItem> m_giftBagMap;
};

#endif