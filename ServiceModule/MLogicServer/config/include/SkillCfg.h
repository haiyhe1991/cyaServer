#ifndef __SKILL_CFG_H__
#define __SKILL_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
技能配置
*/

#include <map>
#include <vector>
#include "ServiceMLogic.h"
#include "CfgCom.h"

class SkillCfg
{
public:
	SkillCfg();
	~SkillCfg();

#pragma pack(push, 1)
	//技能
	struct SSkillAttrCfg
	{
		BYTE key;				//默认键值
		BYTE openLvLimt;		//开放等级限制
		BYTE birthLv;			//技能出生等级
		UINT64 GoldCost;		//金币消耗
		float GoldCostCoefficient;	//金币消耗系数
		//UINT32 GoldCostCoefficient;	//金币消耗系数
		float Damage;
		float DamageUp;
		UINT16 DamageCount;
		UINT16 DamageCountUp;
		UINT16 CDTime;
		float SkillTime;

		SSkillAttrCfg()
		{
			key = 0;			//默认键值
			openLvLimt = 0;		//人物等级限制
			birthLv = 0;
			GoldCost = 0;		//金币消耗
			GoldCostCoefficient = 0.0f;	//金币消耗系数
			Damage = 0.0f;
			DamageUp = 0.0f;
			DamageCount = 0;
			DamageCountUp = 0;
			CDTime = 0;
			SkillTime = 0.0f;
		}
	};

	struct SRoleSkillComboMap
	{
		std::map<UINT16 /*技能id*/, SSkillAttrCfg> skillMap;		//普通技能表
		std::map<UINT16 /*技能id*/, SSkillAttrCfg> comboMap;		//连招表
		std::map<UINT16 /*技能id*/, SSkillAttrCfg> specialSkillMap;	//特殊技能表
		SRoleSkillComboMap()
		{
			skillMap.clear();
			comboMap.clear();
			specialSkillMap.clear();
		}
	};
#pragma pack(pop)

public:
	//载入技能配置
	BOOL Load(const char* pszSkillFile, const char* pszComboFile, const char* pszSpecialSkillFile);
	//设置技能/连招的初始等级
	void SetSkillComboInitLv(BYTE skillInitLv, BYTE comboInitLv);

	//获取某角色从from级到to级开放的普通技能(from, to]
	int GetRoleInitSkills(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& skills);
	//获取某角色从from级到to级开放的连招
	int GetRoleInitCombos(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& combos);
	//获取某角色从from级到to级开放的特殊技能
	int GetRoleInitSpecialSkills(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& specialSkills);
	//获取某角色从from级到to级开放的所有技能
	int GetRoleAllInitSkills(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& allSkills);

	//获取技能升级属性
	int GetUpgradeSkillAttr(BYTE jobId, UINT16 skillId, BYTE newSkillLevel, SSkillAttrCfg& skillAttr);
	//获取连招升级属性
	int GetUpgradeComboAttr(BYTE jobId, UINT16 skillId, BYTE newSkillLevel, SSkillAttrCfg& skillAttr);
	//获取特殊技能升级属性
	int GetUpgradeSpecialSkillAttr(BYTE jobId, UINT16 skillId, BYTE newSkillLevel, SSkillAttrCfg& skillAttr);
	//获取任意技能升级属性
	int GetUpgradeAnySkillAttr(BYTE jobId, UINT16 skillId, BYTE newSkillLevel, SSkillAttrCfg& skillAttr);

	/* zpy 成就系统新增 */
	bool IsSkill(BYTE occuId, UINT16 skillID);

	/* zpy 新增 获取普通技能配置 */
	bool GetSkillAttr(BYTE jobId, UINT16 skillID, SSkillAttrCfg *&item);

private:
	//载入技能配置
	BOOL LoadSkillCfg(const char* pszSkillFile);
	//载入连招配置
	BOOL LoadComboCfg(const char* pszComboFile);
	//载入特殊技能配置
	BOOL LoadSpecialSkillCfg(const char* pszSpecialSkillFile);
	//插入技能
	void InsertSkillItem(BYTE jobId, UINT16 skillId, const SSkillAttrCfg& item);
	//插入连招
	void InsertComboItem(BYTE jobId, UINT16 skillId, const SSkillAttrCfg& item);
	//插入特殊技能
	void InsertSpecialSkillItem(BYTE jobId, UINT16 skillId, const SSkillAttrCfg& item);

private:
	std::map<BYTE/*职业id*/, SRoleSkillComboMap> m_jobSkillMap;	//职业技能配置表
};

#endif