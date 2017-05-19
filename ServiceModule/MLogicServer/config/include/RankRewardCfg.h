#ifndef __RANK_REWARD_CFG_H__
#define __RANK_REWARD_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

/*
等级排行奖励活动 add by hxw 20150929
*/

#include <vector>
#include "CfgCom.h"
#include "ServiceErrorCode.h"

//排行等级 类型
const int __Level_Rank_Type = 1;
//排行副本挑战次数类型
const int __InstanceNum_Rank_Type = 2;
//强者奖励类型
const int __Stronger_Reward_Type = 3;
//在线时间奖励
const int __Online_Reward_Type = 4;

const char C_REWARD_INFO[4][32] = { "冲级达人奖励", "刷图达人奖励", "强者奖励", "在线奖励" };

class CRankReward
{
public:
	struct SRkRewardRes
	{
		std::vector<SDropPropCfgItem> rewardvs;
		UINT32	  Value;

	};

public:
	CRankReward() :m_IsValidity(false), m_IsOpen(false){}
	virtual ~CRankReward(){}

	void SetFile(const char* fname){ m_fileName = fname; }

	virtual BOOL Load(const char* fname);

	INT GetRewardNum() const {
		return m_LvRewardsMap.size();
	}

	//获取等级排行奖励
	BOOL GetReward(const UINT16 rankId, const int iValue, CRankReward::SRkRewardRes*& items);
	//是否是活动时间，返回TRUE表示活动继续，FLASE表示无法活动
	BOOL bValiTime();
	//是否开启
	BOOL bOpen() const { return m_IsOpen; }
	//设置开启状态
	void SetOpen(const BOOL bOpen) { m_IsOpen = bOpen; }

	BOOL IsCanGet(const UINT16 id, const UINT32 value) const;

	int GetValue(UINT16 id);

private:
	std::string m_fileName;
	BOOL m_IsOpen;
	//是否有效时间内
	BOOL m_IsValidity;
	std::map<UINT32, SRkRewardRes> m_LvRewardsMap;
	//活动开始时间
	LTMSEL m_StartTime;
	//活动结束时间
	LTMSEL  m_EndTime;
};

class RankRewardCfg
{
public:
	RankRewardCfg(void);
	virtual ~RankRewardCfg(void);
	struct SLvRanking
	{
		BOOL IsOpen;
		LTMSEL StartTime;
		int  EndTime;
		std::vector<SDropPropCfgItem> propVec;
		SLvRanking()
		{
			IsOpen = false;
			EndTime = 0;
		}
	};

public:
	//载入配置文件
	BOOL Load(std::vector<std::string> vnames);
	//获取等级排行奖励
	/*
	type > 0
	rankid 排行ID
	*/
	//
	//获取排行奖励玩家数量
	INT GetRewardPlayNum(const BYTE type) const;

	BOOL GetRankReward(const BYTE type, const UINT16 rankid, CRankReward::SRkRewardRes*& items, const int iValue);
	//是否是活动时间，返回TRUE表示活动继续，FLASE表示无法活动
	BOOL IsValidityTime(const BYTE type);
	//是否开启
	BOOL IsOpen(const BYTE type) const;

	void SetOpen(const BYTE type, const BOOL bOpen);

	BOOL IsCanGet(const BYTE type, const UINT16 id, const UINT32 value) const;

	//获取日志信息
	int GetLoginfoName(std::string& logsz, BYTE type, UINT16 reward = 0);


private:
	/*1 LV排行   2 刷图    3 cp     4 time*/
	CRankReward m_rankReward[RANK_REWARD_TYPE_NUM];
};


#endif