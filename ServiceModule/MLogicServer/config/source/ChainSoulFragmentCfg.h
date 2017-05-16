#ifndef __CHAIN_SOUL_FRAGMENT_CFG_H__
#define __CHAIN_SOUL_FRAGMENT_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
炼魂碎片配置
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class ChainSoulFragmentCfg
{
public:
	struct ChainSoulFragmentAttr
	{
		BYTE type;			//类型
		BYTE quality;		//品质
		UINT32 maxStack;	//最大数量
	};

public:
	ChainSoulFragmentCfg();
	~ChainSoulFragmentCfg();

public:
	//载入配置
	BOOL Load(const char* filename);
	//获取材料用户出售价格(单价)
	int GetFragmentUserSellPrice(UINT32 fragmentId, UINT64& price);
	//获取炼魂碎片属性
	bool GetChainSoulFragmentAttr(UINT32 fragmentId, ChainSoulFragmentAttr &out_attr);
	//通过类型获取炼魂碎片ID
	bool GetFragmentIdByType(BYTE type, UINT32 &out_id);

private:
	std::map<BYTE, UINT32> m_chainSoulFragmentType;
	std::map<UINT32, ChainSoulFragmentAttr> m_chainSoulFragment;
};

#endif