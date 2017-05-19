#ifndef __FRAGMENT_CFG_H__
#define __FRAGMENT_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
碎片配置
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class FragmentCfg
{
public:
	FragmentCfg();
	~FragmentCfg();

#pragma pack(push, 1)
	struct SFragmentCfgAttr
	{
		BYTE quality;	//品质
		UINT16 maxStack;	//数量
		SFragmentCfgAttr()
		{
			quality = 0;
			maxStack = 0;
		}
	};

	struct SGiveFragmentInfo
	{
		UINT16 id;
		UINT16 num;

		SGiveFragmentInfo()
		{
			id = 0;
			num = 0;
		}
	};

#pragma pack(pop)

public:
	//载入配置
	BOOL Load(const char* filename);
	//载入赠送碎片
	BOOL LoadGiveFragment(const char* filename);
	//获取材料用户出售价格(单价)
	int GetFragmentUserSellPrice(UINT16 fragmentId, UINT64& price);
	//获取碎片属性
	int GetFragmentAttr(UINT16 fragmentId, SFragmentCfgAttr& fragmentAttr);
	//获取赠送碎片
	int GetGiveFragment(std::vector<SGiveFragmentInfo>& giveVec);

private:
	std::map<UINT16/*碎片id*/, SFragmentCfgAttr> m_fragmentMap;
	std::vector<SGiveFragmentInfo> m_giveFragmentVec;
};


#endif