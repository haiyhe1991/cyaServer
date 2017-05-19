#ifndef __WORLD_BOSS_MAN_H__
#define __WORLD_BOSS_MAN_H__

#include <map>
#include <functional>
#include "cyaTypes.h"
#include "cyaLocker.h"
#include "cyaCoreTimer.h"
#include "ServiceMLogic.h"
#include "GameCfgFileMan.h"

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
世界Boss
*/

class WorldBossMan
{
	// 原子操作
	class AtomUINT
	{
	public:
		AtomUINT() : m_value(0)
		{
		}

		AtomUINT(UINT32 value)
			: m_value(value)
		{
		}

		~AtomUINT()
		{
		}

		AtomUINT& operator= (UINT32 value)
		{
			CXTAutoLock lock(m_locker);
			m_value = value;
			return *this;
		}

		AtomUINT& operator+=(UINT32 value)
		{
			CXTAutoLock lock(m_locker);
			m_value += value;
			return *this;
		}

		AtomUINT& operator-=(UINT32 value)
		{
			CXTAutoLock lock(m_locker);
			m_value = m_value > value ? m_value - value : 0;
			return *this;
		}

		AtomUINT& operator*=(UINT32 value)
		{
			CXTAutoLock lock(m_locker);
			m_value *= value;
			return *this;
		}

		AtomUINT& operator/=(UINT32 value)
		{
			CXTAutoLock lock(m_locker);
			m_value /= value;
			return *this;
		}

		operator UINT32() const
		{
			CXTAutoLock lock(m_locker);
			return m_value;
		}

	private:
		UINT32 m_value;
		mutable CXTLocker m_locker;
	};

	// 账号信息
	struct SAccountInfo
	{
		UINT32 userId;
		UINT32 roleId;

		SAccountInfo()
			: userId(0), roleId(0) {}

		SAccountInfo(UINT32 uid, UINT32 rid)
			: userId(uid), roleId(rid) {}

		bool operator< (const SAccountInfo &that) const
		{
			return userId < that.userId ? true : userId == that.userId ? roleId < that.roleId : false;
		}

		bool operator== (const SAccountInfo &that) const
		{
			return userId == that.userId && roleId == that.roleId;
		}
	};

	// 动作信息
	struct SRoleAction
	{
		UINT32 hurt;
		LTMSEL death_time;				// 角色死亡时间
		UINT32 consume_count;			// 消费复活次数

		SRoleAction() : hurt(0), death_time(0), consume_count(0) {}
	};

	// 排名子项
	struct SRankItem
	{
		std::string nick;
		UINT32 hurt_sum;
		SAccountInfo user;
	};

	struct SFindRankPred
	{
		SFindRankPred(const SAccountInfo &user)
		: m_user(user) {}

		bool operator() (const SRankItem &that) const
		{
			return m_user == that.user;
		}

	private:
		SAccountInfo m_user;
	};

	struct SMinRankPred
	{
		bool operator() (const SRankItem &a, const SRankItem &b) const
		{
			return a.hurt_sum < b.hurt_sum;
		}
	};

	struct SRankSortPred
	{
		bool operator() (const SRankItem &a, const SRankItem &b) const
		{
			return a.hurt_sum > b.hurt_sum;
		}
	};

	typedef std::vector<SRankItem> RankVector;

public:
	WorldBossMan();
	~WorldBossMan();

	// 进入世界Boss副本
	int EnterWorldBossInst(UINT32 userId, UINT32 roleId);

	// 查询世界Boss信息
	int QueryWorldBossInfo(UINT32 userId, UINT32 roleId, SQueryWorldBossInfoRes *pQueryWorldBossInfo);

	// 攻击世界Boos
	int AttackWorldBoss(UINT32 userId, UINT32 roleId, UINT32 hurt, SAttackWorldBossRes *pAttackWorldBoss);

	// 被世界Boss杀死
	int WasKilledByWorldBoss(UINT32 userId, UINT32 roleId);

	// 角色复活
	int ResurrectionInWorldBoss(UINT32 userId, UINT32 roleId, SResurrectionInWorldBossRes *pResurrectionInWorldBos);

	// 查询排行榜
	int QueryWorldBossHurtRank(UINT32 userId, UINT32 roleId, SQueryWorldBossRankRes *pWorldBossRank);

private:
	static void RefreshWorldBossTimer(void* param, TTimerID id);
	void OnRefreshWorldBoss();

	// 领取世界Boss奖励
	void ReceiveWorldBossReward(UINT32 userId, UINT32 roleId, UINT32 hurtValue, UINT32 rank, bool kill);

	// 发送奖励邮件
	void SendRewardEmail(UINT32 userId, UINT32 roleId, const std::vector<GameTypeCfg::SRewardInfo> &rewardVec, const char *body);

private:
	AtomUINT								m_isOpen;				// 是否打开
	AtomUINT								m_isEnd;				// 是否结束
	AtomUINT								m_blood;				// boss 血量
	AtomUINT								m_killRoleId;			// 击杀者id
	CXTLocker								m_locker;
	RankVector								m_rankInfos;			// 排行榜信息
	std::map<SAccountInfo, SRoleAction>		m_roleInfos;			// 参与者信息
	CyaCoreTimer							m_refreshTimer;
	UINT16									m_lastSendNoTiceTime;	// 上次发送公告的时间
	UINT16									m_lastActivityTime;		// 上次活动时间
	std::string								m_killNickName;			// 击杀者昵称
};

void InitWorldBossMan();
WorldBossMan* GetWorldBossMan();
void DestroyWorldBossMan();

#endif