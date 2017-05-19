#ifndef __GAME_TYPE_CFG_H__
#define __GAME_TYPE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include <vector>
#include "CfgCom.h"

class GameTypeCfg
{
public:
	struct SRewardInfo
	{
		BYTE type;
		UINT32 id;
		UINT32 num;
	};

	struct SRewardCondition
	{
		BYTE type;
		UINT32 num;
		std::vector<SRewardInfo> reward;
	};

	struct SEexpendInfo
	{
		std::vector<UINT32> eliminate;			// cd消耗
		std::vector<UINT32> refurbish;			// 刷新消耗
		std::vector<UINT32> buyPrice;			// 购买价格
	};

	typedef std::map<int/*玩法类型*/, std::vector<SRewardCondition> > condition_map;

public:
	GameTypeCfg();
	~GameTypeCfg();

public:
	//载入配置
	BOOL Load(const char* pszFile);

	//获取世界Boss血量
	UINT32 GetWorldBossBlood() const
	{
		return m_worldBossBlood;
	}

	//获取cd消耗
	UINT32 GetGameTypeEliminateExpend(BYTE type, UINT16 times);

	//获取刷新消耗
	UINT32 GetGameTypeRefurbishExpend(BYTE type, UINT16 times);

	//获取购买价格
	UINT32 GetGameTypeBuyPriceExpend(BYTE type, UINT16 times);

	//获取复活规则
	BYTE GetGameTypeReviveRule(BYTE type);

	//获取条件和奖励
	bool GetConditionAndReward(BYTE type, std::vector<SRewardCondition> *&ret);

	//获取天梯最大挑战次数
	UINT32 GetLadderMaxChallengeNum() const
	{
		return m_ladderMaxNum;
	}

	//获取天梯挑战CD
	UINT32 GetLadderChallengeCDTime() const
	{
		return m_ladderCD;
	}

	//获取1v1奖励比率
	BYTE GetOneOnOneRewardRatio(BYTE finCode) const;

	//解除奖励嵌套
	void UnlockNesting(const std::vector<SRewardInfo> &source, std::vector<SRewardInfo> *ret);

private:
	UINT32 m_worldBossBlood;				// 世界boss血量
	condition_map m_rewardCondition;		// 奖励达成条件
	std::map<BYTE, SEexpendInfo> m_expend;	// 玩法消耗
	std::map<BYTE, BYTE> m_reviveRule;		// 复活规则
	UINT32 m_ladderMaxNum;					// 天梯最大挑战次数
	UINT32 m_ladderCD;						// 天梯挑战CD
	std::vector<BYTE>	m_oneOnOne;			// 1v1奖励比率
};

#endif