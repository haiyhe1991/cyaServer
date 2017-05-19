#ifndef __ACHIEVEMENT_MAN_H__
#define __ACHIEVEMENT_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/**
* �ɾ͹���
*/

#include <set>
#include <map>
#include <vector>
#include "cyaTypes.h"
#include "cyaLocker.h"
#include "cyaCoreTimer.h"
#include "GameCfgFileMan.h"

class AchievementMan
{
	AchievementMan(const AchievementMan&);
	AchievementMan& operator = (const AchievementMan&);

#pragma pack(push, 1)
	struct SDBAchievementItem
	{
		UINT16 id;				//�ɾ�ID
		bool receive;			//�Ƿ���ȡ
	};
#pragma pack(pop)
	typedef std::map<UINT16, bool> achievement_map;
	typedef std::vector<SDBAchievementItem> achievement_array;

public:
	enum ComparisonType
	{
		EQUAL,						// ����
		EQUAL_OR_GREATER,			// ���ڻ��ߵ���
	};

	struct SAchievementRecord
	{
		UINT32 kill_common;			//ɱ����ͨ������
		UINT32 kill_cream;			//ɱ����Ӣ������
		UINT32 kill_boss;			//ɱ��boss����
		UINT32 pickup_box_sum;		//�򿪱�������
		UINT32 continuous;			//��������
		UINT32 speed_evaluation;	//�ٶ�����
		UINT32 hurt_evaluation;		//�˺�����
		UINT32 star_evaluation;		//��������
		UINT32 complatednum;		//��ɫͨ���ܴ���

		SAchievementRecord()
			: kill_common(0)
			, kill_cream(0)
			, kill_boss(0)
			, pickup_box_sum(0)
			, continuous(0)
			, speed_evaluation(0)
			, hurt_evaluation(0)
			, star_evaluation(0)
			, complatednum(0)
		{

		}
	};

public:
	AchievementMan();
	~AchievementMan();

	// ��ѯ�ɾ͵���
	int QueryAchievementPoint(UINT32 roleId, SQueryAchievementRes *pAchievementInfo);

	// ��ѯ�ɳ�֮·
	int QueryRoadToGrowth(UINT32 userId, UINT32 roleId, UINT16 start_pos, SQueryAchievementRes *pAchievementInfo);

	// ��ѯð������
	int QueryAdventureCourse(UINT32 userId, UINT32 roleId, UINT16 start_pos, SQueryAchievementRes *pAchievementInfo);

	// ��ѯ��������
	int QueryPerfectSkill(UINT32 userId, UINT32 roleId, UINT16 start_pos, SQueryAchievementRes *pAchievementInfo);

	// ��ȡ�ɾͽ���
	int ReceiveAchievementReward(UINT32 userId, UINT32 roleId, UINT32 achievement_id, SReceiveRewardRes *pAchievementReward, SReceiveRewardRefresh* pRefresh);

public:
	// ��������
	bool CreateCache(UINT32 roleId, int &error_code);

	// ������
	bool ClearCache(UINT32 roleId);

public:
	// ս��������(�����ڴ˺�����ѯ��ɫ�����Ϣ)
	int OnUpdateCombatPower(UINT32 roleId, UINT32 cp);

	// ��ȡ��ʯ(�����ڴ˺�����ѯ��ɫ�����Ϣ)
	int OnGotGem(UINT32 roleId, BYTE gem_level);

public:
	// ��ɫ����
	int OnRoleUpgrade(UINT32 userId, UINT32 roleId);

	// ��Ƕ��ʯ
	int OnInlaidGem(UINT32 userId, UINT32 roleId);

	// װ��λ����
	int OnRiseStar(UINT32 userId, UINT32 roleId);

	// ʰȡ����
	int OnPickupInstBoxDrop(UINT32 userId, UINT32 roleId);

	// ��ɱ����
	int OnKillMonsterRecord(UINT32 userId, UINT32 roleId, UINT16 monsterId);

	// ��ɸ���
	int OnFinishInst(UINT32 userId, UINT32 roleId, UINT32 continuous, bool speed_evaluation, bool hurt_evaluation, bool star_evaluation);

	// ��������
	int OnSkillUpgrade(UINT32 userId, UINT32 roleId);

public:
	// ��ȡ�ɾͼ�¼
	int GetAchievementRecord(UINT32 userId, UINT32 roleId, SAchievementRecord &record);

private:
	static void RefreshRoleTimer(void* param, TTimerID id);
	void OnRefreshRoleInfo();

public:
	// д��ɾͼ�¼�����ݿ�
	bool WriteAchievementRecordToDB(UINT32 roleId, achievement_map &record);

	// �����ݿ��ж�ȡ�ɾͼ�¼
	bool ReadAchievementRecordFromDB(UINT32 roleId, achievement_map &record);

	// ����ɾ�
	bool ActivateAchievement(UINT32 roleId, AchievementCfg::TargetType target_type, UINT32 point, ComparisonType comparison, achievement_map &achievement);

private:
	CXTLocker m_total_locker;
	CXTLocker m_record_locker;
	CXTLocker m_update_locker;

	std::set<UINT32> m_updateSet;
	std::map<UINT32, achievement_map> m_achievement_record;
	std::map<UINT32, SAchievementRecord> m_achievement_total;

	CyaCoreTimer m_refreshDBTimer;
};

void InitAchievementMan();
AchievementMan* GetAchievementMan();
void DestroyAchievementMan();


#endif