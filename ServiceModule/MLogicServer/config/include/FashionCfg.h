#ifndef __FASHION_CFG_H__
#define __FASHION_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
时装配置
*/

#include <map>
#include <set>
#include <vector>
#include "CfgCom.h"

class FashionCfg
{
public:
	FashionCfg();
	~FashionCfg();

#pragma pack(push, 1)
	struct SFashionAttr
	{
		BYTE type;
		UINT32 val;
		SFashionAttr()
		{
			type = 0;
			val = 0;
		}
	};
#pragma pack(pop)

public:
	//载入时装配置
	BOOL Load(const char* filename);
	//获取时装属性
	int GetFashionAttr(UINT16 fashionId, std::vector<SFashionAttr>*& pAttrVec);
	//时装和职业是否匹配
	int IsJobFashionMatch(BYTE jobId, UINT16 fashionId);

private:
	std::map<BYTE/*职业*/, std::set<UINT16>/*时装*/ > m_jobFashionMap;
	std::map<UINT16/*时装id*/, std::vector<SFashionAttr> > m_fashionAttrMap;
};

#endif