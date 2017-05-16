#include "AchievementCfg.h"


AchievementCfg::AchievementCfg()
{

}

AchievementCfg::~AchievementCfg()
{

}

BOOL AchievementCfg::Load(const char* pszFile)
{
	Json::Value rootValue;
	if (!OpenCfgFile(pszFile, rootValue))
		return false;

	if (!rootValue["Achievement"].isArray())
		return false;

	const int sisze = rootValue["Achievement"].size();
	if (sisze <= 0)
		return false;

	SAchievementMisc achievement_misc;
	SAchievementCondition achievement_details;
	for (int i = 0; i < sisze; ++i)
	{
		UINT32 id = rootValue["Achievement"][i]["ID"].asUInt();

		// ȡ�ɾ�����
		achievement_details.condition = rootValue["Achievement"][i]["ConditionValue"].asUInt();
		achievement_details.premise_id = rootValue["Achievement"][i]["PremiseID"].asUInt();

		// ȡ�ɾ���Ϣ
		achievement_misc.target_type = rootValue["Achievement"][i]["Target"].asUInt();
		achievement_misc.reward.achievement_count = rootValue["Achievement"][i]["AchievementCount"].asUInt();
		for (int j = 0; j < rootValue["Achievement"][i]["Award"].size(); ++j)
		{
			SAchievementPropAward award;
			award.id = rootValue["Achievement"][i]["Award"][j]["ID"].asInt();
			award.type = rootValue["Achievement"][i]["Award"][j]["Type"].asInt();
			award.num = rootValue["Achievement"][i]["Award"][j]["Num"].asInt();
			achievement_misc.reward.reward.push_back(award);
		}

		// �ɾ�����
		m_achievementType[rootValue["Achievement"][i]["Type"].asUInt()].push_back(id);

		// Ŀ������
		m_targetType[achievement_misc.target_type].push_back(id);

		// ��������
		m_achievementMisc.insert(std::make_pair(id, achievement_misc));
		m_achievementCondition.insert(std::make_pair(id, achievement_details));
		achievement_misc.reward.reward.resize(0);
	}

	return true;
}

//��ȡ�ɾ�Ŀ������
bool AchievementCfg::GetAchievementTargetType(UINT32 id, TargetType &type) const
{
	std::map<UINT32, SAchievementMisc>::const_iterator itr = m_achievementMisc.find(id);
	if (itr != m_achievementMisc.end())
	{
		type = (TargetType)itr->second.target_type;
		return true;
	}
	return false;
}

//��ȡ�ɾͽ���
bool AchievementCfg::GetAchievementReward(UINT32 id, SAchievementReward &reward) const
{
	std::map<UINT32, SAchievementMisc>::const_iterator itr = m_achievementMisc.find(id);
	if (itr != m_achievementMisc.end())
	{
		reward = itr->second.reward;
		return true;
	}
	return false;
}

//��ȡ�ɾ�����
bool AchievementCfg::GetAchievementCondition(UINT32 id, SAchievementCondition &condition) const
{
	std::map<UINT32, SAchievementCondition>::const_iterator itr = m_achievementCondition.find(id);
	if (itr != m_achievementCondition.end())
	{
		condition = itr->second;
		return true;
	}
	return false;
}

//���ݳɾ����ͻ�ȡ�ɾ�
bool AchievementCfg::GetAchievementFromType(AchievementType type, const std::vector<UINT32> *&out) const
{
	std::map<UINT32, std::vector<UINT32> >::const_iterator itr = m_achievementType.find(type);
	if (itr != m_achievementType.end())
	{
		out = &itr->second;
		return true;
	}
	return false;
}

//����Ŀ�����ͻ�ȡ�ɾ�
bool AchievementCfg::GetAchievementFromTargetType(TargetType type, const std::vector<UINT32> *&out) const
{
	std::map<UINT32, std::vector<UINT32> >::const_iterator itr = m_targetType.find(type);
	if (itr != m_targetType.end())
	{
		out = &itr->second;
		return true;
	}
	return false;
}