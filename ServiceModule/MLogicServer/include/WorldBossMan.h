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
����Boss
*/

class WorldBossMan
{
	// ԭ�Ӳ���
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

	// �˺���Ϣ
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

	// ������Ϣ
	struct SRoleAction
	{
		UINT32 hurt;
		LTMSEL death_time;				// ��ɫ����ʱ��
		UINT32 consume_count;			// ���Ѹ������

		SRoleAction() : hurt(0), death_time(0), consume_count(0) {}
	};

	// ��������
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

	// ��������Boss����
	int EnterWorldBossInst(UINT32 userId, UINT32 roleId);

	// ��ѯ����Boss��Ϣ
	int QueryWorldBossInfo(UINT32 userId, UINT32 roleId, SQueryWorldBossInfoRes *pQueryWorldBossInfo);

	// ��������Boos
	int AttackWorldBoss(UINT32 userId, UINT32 roleId, UINT32 hurt, SAttackWorldBossRes *pAttackWorldBoss);

	// ������Bossɱ��
	int WasKilledByWorldBoss(UINT32 userId, UINT32 roleId);

	// ��ɫ����
	int ResurrectionInWorldBoss(UINT32 userId, UINT32 roleId, SResurrectionInWorldBossRes *pResurrectionInWorldBos);

	// ��ѯ���а�
	int QueryWorldBossHurtRank(UINT32 userId, UINT32 roleId, SQueryWorldBossRankRes *pWorldBossRank);

private:
	static void RefreshWorldBossTimer(void* param, TTimerID id);
	void OnRefreshWorldBoss();

	// ��ȡ����Boss����
	void ReceiveWorldBossReward(UINT32 userId, UINT32 roleId, UINT32 hurtValue, UINT32 rank, bool kill);

	// ���ͽ����ʼ�
	void SendRewardEmail(UINT32 userId, UINT32 roleId, const std::vector<GameTypeCfg::SRewardInfo> &rewardVec, const char *body);

private:
	AtomUINT								m_isOpen;				// �Ƿ��
	AtomUINT								m_isEnd;				// �Ƿ����
	AtomUINT								m_blood;				// boss Ѫ��
	AtomUINT								m_killRoleId;			// ��ɱ��id
	CXTLocker								m_locker;
	RankVector								m_rankInfos;			// ���а���Ϣ
	std::map<SAccountInfo, SRoleAction>		m_roleInfos;			// ��������Ϣ
	CyaCoreTimer							m_refreshTimer;
	UINT16									m_lastSendNoTiceTime;	// �ϴη��͹����ʱ��
	UINT16									m_lastActivityTime;		// �ϴλʱ��
	std::string								m_killNickName;			// ��ɱ���ǳ�
};

void InitWorldBossMan();
WorldBossMan* GetWorldBossMan();
void DestroyWorldBossMan();

#endif