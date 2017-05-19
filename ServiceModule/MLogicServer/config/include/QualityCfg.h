#ifndef __QUALITY_CFG_H__
#define __QUALITY_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
装备品质配置
*/

#include <map>
#include "CfgCom.h"

class QualityCfg
{
public:
	QualityCfg();
	~QualityCfg();

private:
	std::map<BYTE/*品质id*/, BYTE/*最高强化等级*/> m_qualityCfgMap;

public:
	//载入品质配置
	BOOL Load(const char* filename);
	//获取最高强化等级
	int GetMaxStrengthenLevel(BYTE qualityId, BYTE& maxStrengthenLv);
};

#endif