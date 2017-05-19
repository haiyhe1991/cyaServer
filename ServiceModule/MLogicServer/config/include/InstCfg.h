#ifndef __INST_CFG_H__
#define __INST_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��������
*/

#include <set>
#include <map>
#include <vector>
#include "CfgCom.h"

class MonsterCfg;
class GameDropCfg;
class InstCfg
{
public:
	InstCfg(GameDropCfg* pDropCfg, MonsterCfg* pMonsterCfg);
	~InstCfg();

#pragma pack(push, 1)
	//ˢ������
	struct SBrushMonster
	{
		std::map<BYTE/*�ֵȼ�*/, UINT16/*����*/> lvMonsterMap;
		SBrushMonster()
		{
			lvMonsterMap.clear();
		}
	};

	//ս���������
	struct SBattleAreaDrop
	{
		UINT32 dropId;	//����id
		BYTE   dropNum;	//��������
		UINT32 gold;	//���
		BYTE   hasCream;	//�Ƿ��о�Ӣ��
		BYTE   hasBoss;		//�Ƿ���boss��
		BYTE   hasMonster;	//�Ƿ���С��
		std::map<UINT16/*��id*/, SBrushMonster>	brushMonster;	//��

		SBattleAreaDrop()
		{
			dropId = 0;
			dropNum = 0;
			gold = 0;
			hasBoss = 0;
			hasCream = 0;
			hasMonster = 0;
			brushMonster.clear();
		}
	};
	//����
	struct SStorageAreaBox
	{
		UINT32 dropId;	//����id
		BYTE   dropNum;	//��������
		UINT32 gold;	//���

		SStorageAreaBox()
		{
			dropId = 0;
			dropNum = 0;
			gold = 0;
		}
	};
	//������Ʒ
	struct SBrokenPropDrop
	{
		UINT32 dropId;	//����id
		BYTE   dropNum;	//��������
		UINT32 gold;	//���

		SBrokenPropDrop()
		{
			dropId = 0;
			dropNum = 0;
			gold = 0;
		}
	};

	//����
	struct SInstAttrCfg
	{
		UINT32 spendVit;		//��������
		BYTE   limitRoleLevel;	//���ƽ�ɫ�ȼ�(��ɫ�ȼ�С�ڸ�ֵ���ܽ���ø���)
		UINT16 reviveId;		//����id
		UINT16 frontInstId;		//ǰ�ùؿ�id
		UINT16 chapterId;		//�����½�
		UINT16 enterNum;		//�������
		BYTE   type;			//��������
		UINT32 sweepExp;		//ɨ������
		float degree;			//�����Ѷ�ϵ��
		std::string name;		//��������
		SBrokenPropDrop brokenDrops;	//���������
		std::map<BYTE/*ս������id*/, SBattleAreaDrop> battleArea;	//ս���������
		std::map<BYTE/*����id*/, SStorageAreaBox> boxDrops;			//����

		SInstAttrCfg()
		{
			spendVit = 0;
			limitRoleLevel = 0;
			degree = 0.0f;
			frontInstId = 0;
			chapterId = 0;
			type = 0;
			sweepExp = 0;
			reviveId = 0;
			battleArea.clear();
			boxDrops.clear();
		}
	};
	//�������䱦��
	struct SInstDropBox
	{
		std::map<BYTE/*����id*/, std::vector<SDropPropCfgItem> > boxMap;
		SInstDropBox()
		{
			boxMap.clear();
		}
	};
	//��������
	struct SInstBrushMonster
	{
		std::map<UINT16/*��id*/, SBrushMonster> monsterMap;
		SInstBrushMonster()
		{
			monsterMap.clear();
		}
	};
	//����������Ʒ
	struct SInstDropProp
	{
		std::vector<SDropPropCfgItem> props;
		SInstDropProp()
		{
			props.clear();
		}
	};
#pragma pack(pop)

public:
	//���븱������
	BOOL LoadInst(const char* filename);
	//�����������
	BOOL LoadRandInst(const char* filename);
	//��ȡ������Ҫ���ĵ�������
	int GetInstSpendVit(UINT16 instId, UINT32& spendVit);
	//��ȡ�����ȼ�����
	int GetInstLvLimit(UINT16 instId, BYTE& lvLimit);
	//��ȡ��������
	int GetInstConfig(UINT16 instId, SInstAttrCfg *&config);
	//��ȡ����������Ϣ
	int GetInstBasicInfo(UINT16 instId, BYTE&instType, BYTE& lvLimit, UINT32& spendVit);
	//�����������id
	UINT16 GenerateRandInstId();
	//��ȡ��������
	int GetInstName(UINT16 instId, std::string& instName);
	//���ɱ������
	int GenerateDropBox(UINT16 instId, SInstDropBox& dropBox);
	//������Ʒ����
	int GenerateAreaMonsterDrops(UINT16 instId, BYTE areaId, SInstDropProp& dropProps, BYTE& monsterDrops, BYTE& creamDrops, BYTE& bossDrops);
	//��ȡս�������
	int GetBattleAreaMonster(UINT16 instId, BYTE areaId, SInstBrushMonster& brushMonster);
	//�������������
	int GenerateBrokenDrops(UINT16 instId, SInstDropProp& brokenDrops);
	//���ò���ֵ
	void SetParamValue(UINT32 goldBranchThrehold, BYTE goldBranchMaxPercent, BYTE dropLv1ToBossPercent, BYTE dropLv1ToCreamPercent);
	//��ȡ����ɨ������
	UINT64 GetInstSweepExp(UINT16 instId);
	//����ɨ������
	int GenerateInstSweepDrops(UINT16 instId, SInstDropProp& drops);
	//��ȡ���������½�
	int GetInstOwnerChapter(UINT16 instId, UINT16& chapterId);
	//��ȡ��������
	BYTE GetInstType(UINT16 instId);
	//��ȡ�������
	UINT16 GetInstEnterNum(UINT16 instId);
	//ͨ�����ͻ�ȡ����id
	bool GetInstByInstType(BYTE type, std::set<UINT16> *&ret);
	/* zpy 2015.12.22���� չ�������� */
	void SpreadDropGroups(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem> *ret);

private:
	void GoldBranch(UINT32 gold, std::vector<SDropPropCfgItem>& vec);

private:
	UINT32 m_goldBranchThrehold;	//��ҷֶѷ�ֵ
	BYTE   m_goldBranchMaxPercent;	//��ҷֶ����ٷֱ�
	BYTE m_dropLv1ToBossPercent;	//��ͨ��Ʒ�ֵ�boss�ٷֱ�
	BYTE m_dropLv1ToCreamPercent;	//��ͨ��Ʒ�ֵ���Ӣ�ٷֱ�

	MonsterCfg* m_pMonsterCfg;		//������
	GameDropCfg* m_gameDropCfg;		//��������
	std::map<UINT16/*�ؿ�id*/, SInstAttrCfg> m_instCfg;		//�ؿ����ñ�
	std::map<UINT16/*�ؿ�id*/, SInstAttrCfg> m_randInstCfg;	//����ؿ����ñ�
	std::map<BYTE, std::set<UINT16> > m_instByType;
};

class ExConfig
{
public:
	ExConfig(int id, const char* info = NULL)
	{
		printf("instId=%d,info=%s is error!", id, info);
		eid = id;
	}
	~ExConfig(){}

	int eid;
};


#endif