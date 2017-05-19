#ifndef __SIGN_IN_CFG_H__
#define __SIGN_IN_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
签到配置
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class SignInCfg
{
public:
	SignInCfg();
	~SignInCfg();

#pragma pack(push, 1)
	struct SSignInItem
	{
		std::vector<SDropPropCfgItem> rewardVec;
	};
#pragma pack(pop)

public:
	//载入配置文件
	BOOL Load(const char* filename);
	//获取签到数据
	int GetSignInItem(BYTE nTimes, SSignInItem*& pItem);

private:
	std::map<BYTE/*连续签到次数*/, SSignInItem> m_signInMap;
};

#endif