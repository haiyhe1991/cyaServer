#ifndef __CFG_COM_H__
#define __CFG_COM_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "json.h"
#include "cyaStrCodec.h"
#include "ServiceMLogic.h"

#define SGS_DISCOUNT_EPSINON  0.00001
#define IS_DISCOUNT_EQUAL_ZERO(x) (((x) >= - SGS_DISCOUNT_EPSINON) && ((x) <= SGS_DISCOUNT_EPSINON))

#define SGS_PROP_DROP_CHANCE		10000
#define MAX_CHANCE_NUM				100
#define MAX_EQUIP_COMPOSE_CHANCE	10000

#pragma pack(push, 1)
struct SDropPropCfgItem
{
	BYTE type;		//类型(ESGSPropType)
	UINT16 propId;	//物品id
	UINT32 num;		//物品数量
	SDropPropCfgItem()
	{
		type = 0;
		propId = 0;
		num = 0;
	}
};
#pragma pack(pop)

//add by hxw 20150929
#pragma pack(push, 1)
struct SCfgItems
{
	std::vector<SDropPropCfgItem> rewardVec;
};
#pragma pack(pop)
//end

BOOL OpenCfgFile(const char* filename, Json::Value& rootValue);

#endif