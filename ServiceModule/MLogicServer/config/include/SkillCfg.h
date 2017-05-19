#ifndef __SKILL_CFG_H__
#define __SKILL_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��������
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
	//����
	struct SSkillAttrCfg
	{
		BYTE key;				//Ĭ�ϼ�ֵ
		BYTE openLvLimt;		//���ŵȼ�����
		BYTE birthLv;			//���ܳ����ȼ�
		UINT64 GoldCost;		//�������
		float GoldCostCoefficient;	//�������ϵ��
		//UINT32 GoldCostCoefficient;	//�������ϵ��
		float Damage;
		float DamageUp;
		UINT16 DamageCount;
		UINT16 DamageCountUp;
		UINT16 CDTime;
		float SkillTime;

		SSkillAttrCfg()
		{
			key = 0;			//Ĭ�ϼ�ֵ
			openLvLimt = 0;		//����ȼ�����
			birthLv = 0;
			GoldCost = 0;		//�������
			GoldCostCoefficient = 0.0f;	//�������ϵ��
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
		std::map<UINT16 /*����id*/, SSkillAttrCfg> skillMap;		//��ͨ���ܱ�
		std::map<UINT16 /*����id*/, SSkillAttrCfg> comboMap;		//���б�
		std::map<UINT16 /*����id*/, SSkillAttrCfg> specialSkillMap;	//���⼼�ܱ�
		SRoleSkillComboMap()
		{
			skillMap.clear();
			comboMap.clear();
			specialSkillMap.clear();
		}
	};
#pragma pack(pop)

public:
	//���뼼������
	BOOL Load(const char* pszSkillFile, const char* pszComboFile, const char* pszSpecialSkillFile);
	//���ü���/���еĳ�ʼ�ȼ�
	void SetSkillComboInitLv(BYTE skillInitLv, BYTE comboInitLv);

	//��ȡĳ��ɫ��from����to�����ŵ���ͨ����(from, to]
	int GetRoleInitSkills(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& skills);
	//��ȡĳ��ɫ��from����to�����ŵ�����
	int GetRoleInitCombos(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& combos);
	//��ȡĳ��ɫ��from����to�����ŵ����⼼��
	int GetRoleInitSpecialSkills(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& specialSkills);
	//��ȡĳ��ɫ��from����to�����ŵ����м���
	int GetRoleAllInitSkills(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& allSkills);

	//��ȡ������������
	int GetUpgradeSkillAttr(BYTE jobId, UINT16 skillId, BYTE newSkillLevel, SSkillAttrCfg& skillAttr);
	//��ȡ������������
	int GetUpgradeComboAttr(BYTE jobId, UINT16 skillId, BYTE newSkillLevel, SSkillAttrCfg& skillAttr);
	//��ȡ���⼼����������
	int GetUpgradeSpecialSkillAttr(BYTE jobId, UINT16 skillId, BYTE newSkillLevel, SSkillAttrCfg& skillAttr);
	//��ȡ���⼼����������
	int GetUpgradeAnySkillAttr(BYTE jobId, UINT16 skillId, BYTE newSkillLevel, SSkillAttrCfg& skillAttr);

	/* zpy �ɾ�ϵͳ���� */
	bool IsSkill(BYTE occuId, UINT16 skillID);

	/* zpy ���� ��ȡ��ͨ�������� */
	bool GetSkillAttr(BYTE jobId, UINT16 skillID, SSkillAttrCfg *&item);

private:
	//���뼼������
	BOOL LoadSkillCfg(const char* pszSkillFile);
	//������������
	BOOL LoadComboCfg(const char* pszComboFile);
	//�������⼼������
	BOOL LoadSpecialSkillCfg(const char* pszSpecialSkillFile);
	//���뼼��
	void InsertSkillItem(BYTE jobId, UINT16 skillId, const SSkillAttrCfg& item);
	//��������
	void InsertComboItem(BYTE jobId, UINT16 skillId, const SSkillAttrCfg& item);
	//�������⼼��
	void InsertSpecialSkillItem(BYTE jobId, UINT16 skillId, const SSkillAttrCfg& item);

private:
	std::map<BYTE/*ְҵid*/, SRoleSkillComboMap> m_jobSkillMap;	//ְҵ�������ñ�
};

#endif