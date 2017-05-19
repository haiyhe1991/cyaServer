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
		std::vector<UINT32> eliminate;			// cd����
		std::vector<UINT32> refurbish;			// ˢ������
		std::vector<UINT32> buyPrice;			// ����۸�
	};

	typedef std::map<int/*�淨����*/, std::vector<SRewardCondition> > condition_map;

public:
	GameTypeCfg();
	~GameTypeCfg();

public:
	//��������
	BOOL Load(const char* pszFile);

	//��ȡ����BossѪ��
	UINT32 GetWorldBossBlood() const
	{
		return m_worldBossBlood;
	}

	//��ȡcd����
	UINT32 GetGameTypeEliminateExpend(BYTE type, UINT16 times);

	//��ȡˢ������
	UINT32 GetGameTypeRefurbishExpend(BYTE type, UINT16 times);

	//��ȡ����۸�
	UINT32 GetGameTypeBuyPriceExpend(BYTE type, UINT16 times);

	//��ȡ�������
	BYTE GetGameTypeReviveRule(BYTE type);

	//��ȡ�����ͽ���
	bool GetConditionAndReward(BYTE type, std::vector<SRewardCondition> *&ret);

	//��ȡ���������ս����
	UINT32 GetLadderMaxChallengeNum() const
	{
		return m_ladderMaxNum;
	}

	//��ȡ������սCD
	UINT32 GetLadderChallengeCDTime() const
	{
		return m_ladderCD;
	}

	//��ȡ1v1��������
	BYTE GetOneOnOneRewardRatio(BYTE finCode) const;

	//�������Ƕ��
	void UnlockNesting(const std::vector<SRewardInfo> &source, std::vector<SRewardInfo> *ret);

private:
	UINT32 m_worldBossBlood;				// ����bossѪ��
	condition_map m_rewardCondition;		// �����������
	std::map<BYTE, SEexpendInfo> m_expend;	// �淨����
	std::map<BYTE, BYTE> m_reviveRule;		// �������
	UINT32 m_ladderMaxNum;					// ���������ս����
	UINT32 m_ladderCD;						// ������սCD
	std::vector<BYTE>	m_oneOnOne;			// 1v1��������
};

#endif