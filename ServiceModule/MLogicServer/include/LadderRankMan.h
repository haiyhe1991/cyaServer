#ifndef __LADDER_RANK_MAN_H__
#define __LADDER_RANK_MAN_H__

#include <map>
#include <vector>
#include "cyaTime.h"
#include "cyaTypes.h"
#include "cyaLocker.h"
#include "ServiceMLogic.h"
#include "GameCfgFileMan.h"

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
天梯排名
*/

class LadderRankMan
{
	struct SLadderItem
	{
		UINT32 userId;				//账号id
		UINT32 roleId;				//角色id
		UINT32 rank;				//排名
		UINT16 win_in_row;			//连胜次数
	};

	struct SRankRoleInfo
	{
		UINT32 roleId;				//角色Id
		LTMSEL challenge_time;		//挑战时间
		SRankRoleInfo(UINT32 rid, LTMSEL time)
			: roleId(rid), challenge_time(time) {}
	};

public:
	LadderRankMan();
	~LadderRankMan();

	// 玩家信息
	int GetChallengeInfo(UINT32 userId, UINT32 roleId, SLadderRoleInfoRes *pLadderInfo);

	// 匹配玩家
	int MatchingPlayer(UINT32 userId, UINT32 roleId, SMatchingLadderRes *pMatchingLadder);

	// 刷新天梯排行范围
	int RefreshLadderRankScope(UINT32 userId, UINT32 roleId, SRefreshLadderRankRes *pRefreshRank);

	// 挑战玩家
	int ChallengePlayer(UINT32 userId, UINT32 roleId, INT32 target_rank, SChallengeLadderRes *pChallengeLadder);

	// 消除冷却时间
	int EliminateCoolingTime(UINT32 userId, UINT32 roleId, SEliminateLadderCDTimeRes *pEliminate);

	// 挑战完成
	int FinishChallenge(UINT32 userId, UINT32 roleId, BYTE finishCode/*, SFinishLadderChallengeRes *pFinishLadder*/);

public:
	// 每日结算
	void DailySettlement();

	// 删除角色
	int DelRole(UINT32 roleId);

	// 查询天梯称号
	int QueryLadderRankTitle(UINT32 userId, UINT32 roleId);

	// 领取天梯奖励
	void ReceiveLadderReward(UINT32 userId, UINT32 roleId, UINT32 rank, UINT16 win_in_row);

	// 匹配玩家
	int MatchingPlayerFromDB(UINT32 userId, UINT32 roleId, std::vector<SMatchingLadderItem> &retVec);

	// 发送奖励邮件
	void SendRewardEmail(UINT32 userId, UINT32 roleId, const std::vector<GameTypeCfg::SRewardInfo> &rewardVec, const char *body);

	// 查询角色天梯信息
	int QueryRoleLadderInfoFromDB(UINT32 roleId, UINT16 *win_in_row, UINT16 *refresh_count, UINT16 *eliminate_count, UINT16 *challenge_count, LTMSEL *last_finish_time, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM]);

private:
	CXTLocker m_locker;
	std::map<UINT32/*挑战名次*/, SRankRoleInfo> m_rankOnRoleId;
	std::map<UINT32/*角色ID*/, UINT32/*挑战名次*/> m_roleIdOnRank;
};

void InitLadderRankMan();
LadderRankMan* GetLadderRankMan();
void DestroyLadderRankMan();

#endif