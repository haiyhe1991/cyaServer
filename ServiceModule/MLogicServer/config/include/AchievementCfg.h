#ifndef __ACHIEVEMENT_CFG_H__
#define __ACHIEVEMENT_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include <vector>
#include "CfgCom.h"

class AchievementCfg
{
public:
	// Ŀ������ö��
	enum TargetType
	{
		ZZCZ = 11,			// ��׳�ɳ�
		SQLZ = 12,			// ��ǿ��׳
		SSYY = 13,			// ����ҫ��
		SYDM = 14,			// ��ҫ��Ŀ
		XXZB = 15,			// ����װ��
		YQDQ = 21,			// һ�ﵱǧ
		SBKD = 22,			// �Ʋ��ɵ�
		YMWD = 23,			// �����޵�
		BZSC = 24,			// ��սɳ��
		DBQB = 25,			// �ᱦ���
		LJDR = 31,			// ��������
		JSXF = 32,			// �����ȷ�
		YYWZ = 33,			// ��������
		SXQZ = 34,			// ����ǿ��
		WDZZ = 35,			// �������
	};

	// �ɾ�����
	enum AchievementType
	{
		RoadToGrowth = 1,		//�ɳ�֮·
		AdventureCourse = 2,	//ð������
		PerfectSkill = 3,		//��������
	};

	// �ɾ�����
	struct SAchievementCondition
	{
		UINT32 condition;							//�������
		UINT32 premise_id;							//ǰ�óɾ�ID
	};

	// �ɾ͵��߽���
	struct SAchievementPropAward
	{
		UINT32 id;									//��ƷID
		BYTE type;									//��Ʒ����
		BYTE num;									//��Ʒ����
	};

	// �ɾͽ���
	struct SAchievementReward
	{
		UINT32 achievement_count;					//�ɾ͵㽱��
		std::vector<SAchievementPropAward> reward;	//���߽���
	};

	// �ɾ�����
	struct SAchievementMisc
	{
		BYTE target_type;							//Ŀ������
		SAchievementReward reward;					//�ɾͽ���
	};

public:
	AchievementCfg();
	~AchievementCfg();

public:
	//��������
	BOOL Load(const char* pszFile);

	//��ȡ�ɾ�Ŀ������
	bool GetAchievementTargetType(UINT32 id, TargetType &type) const;

	//��ȡ�ɾͽ���
	bool GetAchievementReward(UINT32 id, SAchievementReward &reward) const;

	//��ȡ�ɾ�����
	bool GetAchievementCondition(UINT32 id, SAchievementCondition &condition) const;

	//���ݳɾ����ͻ�ȡ�ɾ�
	bool GetAchievementFromType(AchievementType type, const std::vector<UINT32> *&out) const;

	//����Ŀ�����ͻ�ȡ�ɾ�
	bool GetAchievementFromTargetType(TargetType type, const std::vector<UINT32> *&out) const;

private:
	std::map<UINT32, std::vector<UINT32> > m_targetType;
	std::map<UINT32, SAchievementMisc> m_achievementMisc;
	std::map<UINT32, std::vector<UINT32> > m_achievementType;
	std::map<UINT32, SAchievementCondition> m_achievementCondition;
};

#endif