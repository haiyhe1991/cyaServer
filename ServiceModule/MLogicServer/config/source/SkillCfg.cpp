#include "SkillCfg.h"
#include "cyaFdk.h"
#include "moduleInfo.h"
#include "ServiceErrorCode.h"

SkillCfg::SkillCfg()
{

}

SkillCfg::~SkillCfg()
{

}

BOOL SkillCfg::Load(const char* pszSkillFile, const char* pszComboFile, const char* pszSpecialSkillFile)
{
	if (!LoadSkillCfg(pszSkillFile))
		return false;

	if (!LoadComboCfg(pszComboFile))
		return false;

	return LoadSpecialSkillCfg(pszSpecialSkillFile);
}

BOOL SkillCfg::LoadSkillCfg(const char* pszSkillFile)
{
	Json::Value rootValue;
	if (!OpenCfgFile(pszSkillFile, rootValue))
		return false;

	if (!rootValue.isMember("Skill") || !rootValue["Skill"].isArray())
		return false;

	int si = rootValue["Skill"].size();
	if (si <= 0)
		return false;

	for (int i = 0; i < si; ++i)
	{
		SSkillAttrCfg item;
		BYTE jobId = (BYTE)rootValue["Skill"][i]["BelongTo"].asUInt();
		UINT16 skillId = (UINT16)rootValue["Skill"][i]["ID"].asUInt();
		item.key = (BYTE)rootValue["Skill"][i]["Key"].asUInt();
		item.openLvLimt = (BYTE)rootValue["Skill"][i]["OpenLv"].asUInt();
		if (rootValue["Skill"][i].isMember("InitLv"))
			item.birthLv = (BYTE)rootValue["Skill"][i]["InitLv"].asUInt();
		item.GoldCost = rootValue["Skill"][i]["GoldCost"].asUInt();
		item.Damage = rootValue["Skill"][i]["Damage"].asUInt() / 100.0f;
		item.DamageUp = rootValue["Skill"][i]["DamageUp"].asUInt() / 100.0f;
		item.DamageCount = rootValue["Skill"][i]["DamageCount"].asUInt();
		item.DamageCountUp = rootValue["Skill"][i]["DamageCountUp"].asUInt();
		item.CDTime = rootValue["Skill"][i]["CD"].asUInt();
		item.SkillTime = rootValue["Skill"][i]["SkillTime"].asUInt() / 100.0f;
		item.GoldCostCoefficient = rootValue["Skill"][i]["GoldCoefficient"].asUInt() / 100.0f;
		InsertSkillItem(jobId, skillId, item);
	}

	return true;
}

BOOL SkillCfg::LoadComboCfg(const char* pszComboFile)
{
	Json::Value rootValue;
	if (!OpenCfgFile(pszComboFile, rootValue))
		return false;

	if (!rootValue.isMember("Skill") || !rootValue["Skill"].isArray())
		return false;

	int si = rootValue["Skill"].size();
	if (si <= 0)
		return false;

	for (int i = 0; i < si; ++i)
	{
		SSkillAttrCfg item;
		BYTE jobId = (BYTE)rootValue["Skill"][i]["BelongTo"].asUInt();
		UINT16 skillId = (UINT16)rootValue["Skill"][i]["ID"].asUInt();
		item.openLvLimt = (BYTE)rootValue["Skill"][i]["OpenLv"].asUInt();
		if (rootValue["Skill"][i].isMember("InitLv"))
			item.birthLv = (BYTE)rootValue["Skill"][i]["InitLv"].asUInt();
		item.GoldCost = rootValue["Skill"][i]["GoldCost"].asUInt();
		item.GoldCostCoefficient = rootValue["Skill"][i]["GoldCoefficient"].asUInt();
		InsertComboItem(jobId, skillId, item);
	}

	return true;
}

BOOL SkillCfg::LoadSpecialSkillCfg(const char* pszSpecialSkillFile)
{
	Json::Value rootValue;
	if (!OpenCfgFile(pszSpecialSkillFile, rootValue))
		return false;

	//闪避
	if (rootValue.isMember("Evade") && rootValue["Evade"].isArray())
	{
		int si = rootValue["Evade"].size();
		for (int i = 0; i < si; ++i)
		{
			SSkillAttrCfg item;
			BYTE jobId = (BYTE)rootValue["Evade"][i]["BelongTo"].asUInt();
			UINT16 skillId = (UINT16)rootValue["Evade"][i]["ID"].asUInt();
			item.openLvLimt = (BYTE)rootValue["Evade"][i]["OpenLv"].asUInt();
			if (rootValue["Evade"][i].isMember("InitLv"))
				item.birthLv = (BYTE)rootValue["Evade"][i]["InitLv"].asUInt();
			item.GoldCost = rootValue["Evade"][i]["GoldCost"].asUInt();
			item.GoldCostCoefficient = rootValue["Evade"][i]["GoldCoefficient"].asUInt();
			InsertSpecialSkillItem(jobId, skillId, item);
		}
	}

	//跳
	if (rootValue.isMember("Jump") && rootValue["Jump"].isArray())
	{
		int si = rootValue["Jump"].size();
		for (int i = 0; i < si; ++i)
		{
			SSkillAttrCfg item;
			BYTE jobId = (BYTE)rootValue["Jump"][i]["BelongTo"].asUInt();
			UINT16 skillId = (UINT16)rootValue["Jump"][i]["ID"].asUInt();
			item.openLvLimt = (BYTE)rootValue["Jump"][i]["OpenLv"].asUInt();
			if (rootValue["Jump"][i].isMember("InitLv"))
				item.birthLv = (BYTE)rootValue["Jump"][i]["InitLv"].asUInt();
			item.GoldCost = rootValue["Jump"][i]["GoldCost"].asUInt();
			item.GoldCostCoefficient = rootValue["Jump"][i]["GoldCoefficient"].asUInt();
			InsertSpecialSkillItem(jobId, skillId, item);
		}
	}

	//防御
	if (rootValue.isMember("Defense") && rootValue["Defense"].isArray())
	{
		int si = rootValue["Defense"].size();
		for (int i = 0; i < si; ++i)
		{
			SSkillAttrCfg item;
			BYTE jobId = (BYTE)rootValue["Defense"][i]["BelongTo"].asUInt();
			UINT16 skillId = (UINT16)rootValue["Defense"][i]["ID"].asUInt();
			item.openLvLimt = (BYTE)rootValue["Defense"][i]["OpenLv"].asUInt();
			if (rootValue["Defense"][i].isMember("InitLv"))
				item.birthLv = (BYTE)rootValue["Defense"][i]["InitLv"].asUInt();
			item.GoldCost = rootValue["Defense"][i]["GoldCost"].asUInt();
			item.GoldCostCoefficient = rootValue["Defense"][i]["GoldCoefficient"].asUInt();
			InsertSpecialSkillItem(jobId, skillId, item);
		}
	}

	//2段跳
	if (rootValue.isMember("DoubleJump") && rootValue["DoubleJump"].isArray())
	{
		int si = rootValue["DoubleJump"].size();
		for (int i = 0; i < si; ++i)
		{
			SSkillAttrCfg item;
			BYTE jobId = (BYTE)rootValue["DoubleJump"][i]["BelongTo"].asUInt();
			UINT16 skillId = (UINT16)rootValue["DoubleJump"][i]["ID"].asUInt();
			item.openLvLimt = (BYTE)rootValue["DoubleJump"][i]["OpenLv"].asUInt();
			if (rootValue["DoubleJump"][i].isMember("InitLv"))
				item.birthLv = (BYTE)rootValue["DoubleJump"][i]["InitLv"].asUInt();
			item.GoldCost = rootValue["DoubleJump"][i]["GoldCost"].asUInt();
			item.GoldCostCoefficient = rootValue["DoubleJump"][i]["GoldCoefficient"].asUInt();
			InsertSpecialSkillItem(jobId, skillId, item);
		}
	}

	//冲刺
	if (rootValue.isMember("Spurt") && rootValue["Spurt"].isArray())
	{
		int si = rootValue["Spurt"].size();
		for (int i = 0; i < si; ++i)
		{
			SSkillAttrCfg item;
			BYTE jobId = (BYTE)rootValue["Spurt"][i]["BelongTo"].asUInt();
			UINT16 skillId = (UINT16)rootValue["Spurt"][i]["ID"].asUInt();
			item.openLvLimt = (BYTE)rootValue["Spurt"][i]["OpenLv"].asUInt();
			if (rootValue["Spurt"][i].isMember("InitLv"))
				item.birthLv = (BYTE)rootValue["Spurt"][i]["InitLv"].asUInt();
			item.GoldCost = rootValue["Spurt"][i]["GoldCost"].asUInt();
			item.GoldCostCoefficient = rootValue["Spurt"][i]["GoldCoefficient"].asUInt();
			InsertSpecialSkillItem(jobId, skillId, item);
		}
	}

	return true;
}

void SkillCfg::InsertSkillItem(BYTE jobId, UINT16 skillId, const SSkillAttrCfg& item)
{
	std::map<BYTE/*职业id*/, SRoleSkillComboMap>::iterator it = m_jobSkillMap.find(jobId);
	if (it == m_jobSkillMap.end())
	{
		SRoleSkillComboMap skillMap;
		m_jobSkillMap.insert(std::make_pair(jobId, skillMap));
		SRoleSkillComboMap& skillMapRef = m_jobSkillMap[jobId];
		skillMapRef.skillMap.insert(std::make_pair(skillId, item));
	}
	else
	{
		std::map<UINT16 /*技能id*/, SSkillAttrCfg>& skillsRef = m_jobSkillMap[jobId].skillMap;
		std::map<UINT16 /*技能id*/, SSkillAttrCfg>::iterator it2 = skillsRef.find(skillId);
		if (it2 == skillsRef.end())
			skillsRef.insert(std::make_pair(skillId, item));
		else
			ASSERT(false);
	}
}

void SkillCfg::InsertComboItem(BYTE jobId, UINT16 skillId, const SSkillAttrCfg& item)
{
	std::map<BYTE/*职业id*/, SRoleSkillComboMap>::iterator it = m_jobSkillMap.find(jobId);
	if (it == m_jobSkillMap.end())
	{
		SRoleSkillComboMap skillMap;
		m_jobSkillMap.insert(std::make_pair(jobId, skillMap));
		SRoleSkillComboMap& skillMapRef = m_jobSkillMap[jobId];
		skillMapRef.skillMap.insert(std::make_pair(skillId, item));
	}
	else
	{
		std::map<UINT16 /*技能id*/, SSkillAttrCfg>& skillsRef = m_jobSkillMap[jobId].comboMap;
		std::map<UINT16 /*技能id*/, SSkillAttrCfg>::iterator it2 = skillsRef.find(skillId);
		if (it2 == skillsRef.end())
			skillsRef.insert(std::make_pair(skillId, item));
		else
			ASSERT(false);
	}
}

void SkillCfg::InsertSpecialSkillItem(BYTE jobId, UINT16 skillId, const SSkillAttrCfg& item)
{
	std::map<BYTE/*职业id*/, SRoleSkillComboMap>::iterator it = m_jobSkillMap.find(jobId);
	if (it == m_jobSkillMap.end())
	{
		SRoleSkillComboMap skillMap;
		m_jobSkillMap.insert(std::make_pair(jobId, skillMap));
		SRoleSkillComboMap& skillMapRef = m_jobSkillMap[jobId];
		skillMapRef.skillMap.insert(std::make_pair(skillId, item));
	}
	else
	{
		std::map<UINT16 /*技能id*/, SSkillAttrCfg>& skillsRef = m_jobSkillMap[jobId].specialSkillMap;
		std::map<UINT16 /*技能id*/, SSkillAttrCfg>::iterator it2 = skillsRef.find(skillId);
		if (it2 == skillsRef.end())
			skillsRef.insert(std::make_pair(skillId, item));
		else
			ASSERT(false);
	}
}

int SkillCfg::GetRoleInitSkills(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& skills)
{
	std::map<BYTE/*职业id*/, SRoleSkillComboMap>::iterator it = m_jobSkillMap.find(jobId);
	if (it == m_jobSkillMap.end())
		return MLS_NOT_EXIST_JOB;

	std::map<UINT16 /*技能id*/, SSkillAttrCfg>& skillMapRef = it->second.skillMap;
	std::map<UINT16 /*技能id*/, SSkillAttrCfg>::iterator it2 = skillMapRef.begin();
	for (; it2 != skillMapRef.end(); ++it2)
	{
		SSkillMoveInfo item;
		BYTE openLvLimt = it2->second.openLvLimt;
		if (openLvLimt > from && openLvLimt <= to)
		{
			item.id = it2->first;
			item.key = it2->second.key;
			item.level = it2->second.birthLv;
			skills.push_back(item);
		}
	}
	return MLS_OK;
}

int SkillCfg::GetRoleInitCombos(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& combos)
{
	std::map<BYTE/*职业id*/, SRoleSkillComboMap>::iterator it = m_jobSkillMap.find(jobId);
	if (it == m_jobSkillMap.end())
		return MLS_NOT_EXIST_JOB;

	std::map<UINT16 /*技能id*/, SSkillAttrCfg>& skillMapRef = it->second.comboMap;
	std::map<UINT16 /*技能id*/, SSkillAttrCfg>::iterator it2 = skillMapRef.begin();
	for (; it2 != skillMapRef.end(); ++it2)
	{
		SSkillMoveInfo item;
		BYTE openLvLimt = it2->second.openLvLimt;
		if (openLvLimt > from && openLvLimt <= to)
		{
			item.id = it2->first;
			item.key = it2->second.key;
			item.level = it2->second.birthLv;
			combos.push_back(item);
		}
	}
	return MLS_OK;
}

int SkillCfg::GetRoleInitSpecialSkills(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& specialSkills)
{
	std::map<BYTE/*职业id*/, SRoleSkillComboMap>::iterator it = m_jobSkillMap.find(jobId);
	if (it == m_jobSkillMap.end())
		return MLS_NOT_EXIST_JOB;

	std::map<UINT16 /*技能id*/, SSkillAttrCfg>& skillMapRef = it->second.specialSkillMap;
	std::map<UINT16 /*技能id*/, SSkillAttrCfg>::iterator it2 = skillMapRef.begin();
	for (; it2 != skillMapRef.end(); ++it2)
	{
		SSkillMoveInfo item;
		BYTE openLvLimt = it2->second.openLvLimt;
		if (openLvLimt > from && openLvLimt <= to)
		{
			item.id = it2->first;
			item.key = it2->second.key;
			item.level = it2->second.birthLv;
			specialSkills.push_back(item);
		}
	}
	return MLS_OK;
}

int SkillCfg::GetRoleAllInitSkills(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& allSkills)
{
	GetRoleInitSkills(jobId, from, to, allSkills);
	GetRoleInitCombos(jobId, from, to, allSkills);
	GetRoleInitSpecialSkills(jobId, from, to, allSkills);
	return MLS_OK;
}

int SkillCfg::GetUpgradeSkillAttr(BYTE jobId, UINT16 skillId, BYTE /*newSkillLevel*/, SSkillAttrCfg& skillAttr)
{
	std::map<BYTE/*职业id*/, SRoleSkillComboMap>::iterator it = m_jobSkillMap.find(jobId);
	if (it == m_jobSkillMap.end())
	{
		ASSERT(false);
		return MLS_NOT_EXIST_JOB;
	}

	std::map<UINT16 /*技能id*/, SSkillAttrCfg>& skillMapRef = it->second.skillMap;
	std::map<UINT16 /*技能id*/, SSkillAttrCfg>::iterator it2 = skillMapRef.find(skillId);
	if (it2 == skillMapRef.end())
		return MLS_SKILL_ID_NOT_EXIST;

	skillAttr = it2->second;
	return MLS_OK;
}

int SkillCfg::GetUpgradeComboAttr(BYTE jobId, UINT16 skillId, BYTE /*newSkillLevel*/, SSkillAttrCfg& skillAttr)
{
	std::map<BYTE/*职业id*/, SRoleSkillComboMap>::iterator it = m_jobSkillMap.find(jobId);
	if (it == m_jobSkillMap.end())
	{
		ASSERT(false);
		return MLS_NOT_EXIST_JOB;
	}

	std::map<UINT16 /*技能id*/, SSkillAttrCfg>& skillMapRef = it->second.comboMap;
	std::map<UINT16 /*技能id*/, SSkillAttrCfg>::iterator it2 = skillMapRef.find(skillId);
	if (it2 == skillMapRef.end())
		return MLS_SKILL_ID_NOT_EXIST;

	skillAttr = it2->second;
	return MLS_OK;
}

int SkillCfg::GetUpgradeSpecialSkillAttr(BYTE jobId, UINT16 skillId, BYTE /*newSkillLevel*/, SSkillAttrCfg& skillAttr)
{
	std::map<BYTE/*职业id*/, SRoleSkillComboMap>::iterator it = m_jobSkillMap.find(jobId);
	if (it == m_jobSkillMap.end())
	{
		ASSERT(false);
		return MLS_NOT_EXIST_JOB;
	}

	std::map<UINT16 /*技能id*/, SSkillAttrCfg>& skillMapRef = it->second.specialSkillMap;
	std::map<UINT16 /*技能id*/, SSkillAttrCfg>::iterator it2 = skillMapRef.find(skillId);
	if (it2 == skillMapRef.end())
		return MLS_SKILL_ID_NOT_EXIST;

	skillAttr = it2->second;
	return MLS_OK;
}

int SkillCfg::GetUpgradeAnySkillAttr(BYTE jobId, UINT16 skillId, BYTE newSkillLevel, SSkillAttrCfg& skillAttr)
{
	int retCode = GetUpgradeSkillAttr(jobId, skillId, newSkillLevel, skillAttr);
	if (retCode == MLS_OK)
		return retCode;

	retCode = GetUpgradeComboAttr(jobId, skillId, newSkillLevel, skillAttr);
	if (retCode == MLS_OK)
		return retCode;

	return GetUpgradeSpecialSkillAttr(jobId, skillId, newSkillLevel, skillAttr);
}

/* zpy 成就系统新增 */
bool SkillCfg::IsSkill(BYTE occuId, UINT16 skillID)
{
	std::map<BYTE/*职业id*/, SRoleSkillComboMap>::const_iterator itr = m_jobSkillMap.find(occuId);
	if (itr != m_jobSkillMap.end())
	{
		return itr->second.skillMap.find(skillID) != itr->second.skillMap.end();
	}
	return false;
}

/* zpy 新增 获取普通技能配置 */
bool SkillCfg::GetSkillAttr(BYTE jobId, UINT16 skillID, SSkillAttrCfg *&item)
{
	item = NULL;
	std::map<BYTE, SRoleSkillComboMap>::iterator itr = m_jobSkillMap.find(jobId);
	if (itr == m_jobSkillMap.end())
		return false;

	std::map<UINT16, SSkillAttrCfg> &skillMap = itr->second.skillMap;
	std::map<UINT16, SSkillAttrCfg>::iterator result = skillMap.find(skillID);
	if (result == skillMap.end())
	{
		return false;
	}
	item = &result->second;
	return true;
}
