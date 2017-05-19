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
��������
*/

class LadderRankMan
{
	struct SLadderItem
	{
		UINT32 userId;				//�˺�id
		UINT32 roleId;				//��ɫid
		UINT32 rank;				//����
		UINT16 win_in_row;			//��ʤ����
	};

	struct SRankRoleInfo
	{
		UINT32 roleId;				//��ɫId
		LTMSEL challenge_time;		//��սʱ��
		SRankRoleInfo(UINT32 rid, LTMSEL time)
			: roleId(rid), challenge_time(time) {}
	};

public:
	LadderRankMan();
	~LadderRankMan();

	// �����Ϣ
	int GetChallengeInfo(UINT32 userId, UINT32 roleId, SLadderRoleInfoRes *pLadderInfo);

	// ƥ�����
	int MatchingPlayer(UINT32 userId, UINT32 roleId, SMatchingLadderRes *pMatchingLadder);

	// ˢ���������з�Χ
	int RefreshLadderRankScope(UINT32 userId, UINT32 roleId, SRefreshLadderRankRes *pRefreshRank);

	// ��ս���
	int ChallengePlayer(UINT32 userId, UINT32 roleId, INT32 target_rank, SChallengeLadderRes *pChallengeLadder);

	// ������ȴʱ��
	int EliminateCoolingTime(UINT32 userId, UINT32 roleId, SEliminateLadderCDTimeRes *pEliminate);

	// ��ս���
	int FinishChallenge(UINT32 userId, UINT32 roleId, BYTE finishCode/*, SFinishLadderChallengeRes *pFinishLadder*/);

public:
	// ÿ�ս���
	void DailySettlement();

	// ɾ����ɫ
	int DelRole(UINT32 roleId);

	// ��ѯ���ݳƺ�
	int QueryLadderRankTitle(UINT32 userId, UINT32 roleId);

	// ��ȡ���ݽ���
	void ReceiveLadderReward(UINT32 userId, UINT32 roleId, UINT32 rank, UINT16 win_in_row);

	// ƥ�����
	int MatchingPlayerFromDB(UINT32 userId, UINT32 roleId, std::vector<SMatchingLadderItem> &retVec);

	// ���ͽ����ʼ�
	void SendRewardEmail(UINT32 userId, UINT32 roleId, const std::vector<GameTypeCfg::SRewardInfo> &rewardVec, const char *body);

	// ��ѯ��ɫ������Ϣ
	int QueryRoleLadderInfoFromDB(UINT32 roleId, UINT16 *win_in_row, UINT16 *refresh_count, UINT16 *eliminate_count, UINT16 *challenge_count, LTMSEL *last_finish_time, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM]);

private:
	CXTLocker m_locker;
	std::map<UINT32/*��ս����*/, SRankRoleInfo> m_rankOnRoleId;
	std::map<UINT32/*��ɫID*/, UINT32/*��ս����*/> m_roleIdOnRank;
};

void InitLadderRankMan();
LadderRankMan* GetLadderRankMan();
void DestroyLadderRankMan();

#endif