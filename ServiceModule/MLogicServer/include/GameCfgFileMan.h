#ifndef __GAME_CFG_FILE_MAN_H__
#define __GAME_CFG_FILE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��Ϸ�����ļ�����
*/

#include "RoleBasicCfg.h"
#include "SkillCfg.h"
#include "TaskCfg.h"
#include "MaterialCfg.h"
#include "InstCfg.h"
#include "EquipCfg.h"
#include "RiseStarCfg.h"
#include "QualityCfg.h"
#include "GemCfg.h"
#include "FragmentCfg.h"
#include "MallCfg.h"
#include "RechargeCfg.h"
#include "TitleCfg.h"
#include "EmailCfg.h"
#include "RandInstSysCfg.h"
#include "FashionCfg.h"
#include "SuitCfg.h"
#include "ReviveCfg.h"
#include "GoldExchangeCfg.h"
#include "VIPCfg.h"
#include "MonsterCfg.h"
#include "StrengthenCfg.h"
#include "GameDropCfg.h"
#include "EquipComposeCfg.h"
#include "ChapterCfg.h"
#include "SignInCfg.h"
#include "ActivityCfg.h"
#include "NoticeCfg.h"
#include "UnlockCfg.h"
#include "GiftBagCfg.h"
#include "InitRoleGiveCfg.h"
#include "RouletteCfg.h"
#include "AchievementCfg.h"
#include "VitExchangeCfg.h"
#include "DailySignCfg.h"
#include "ChainSoulFragmentCfg.h"
#include "GreedTheLiveCfg.h"
#include "MonsterAttributeCfg.h"
#include "MonsterTypeConfig.h"
#include "StrengthenAttriCfg.h"
#include "PlayerCfg.h"
#include "ActivityStageCfg.h"

//add by hxw 20151013 �������н�������
#include "RankRewardCfg.h"
//end

#include "LimitedTimeActivityCfg.h"
#include "GameTypeCfg.h"

class GameCfgFileMan
{
	GameCfgFileMan(const GameCfgFileMan&);
	GameCfgFileMan& operator = (const GameCfgFileMan&);

public:
	GameCfgFileMan();
	~GameCfgFileMan();

#pragma pack(push, 1)
	//ս��ƽ��ֵ
	struct SCPBalanceAttr
	{
		float HPBalance;
		float MPBalance;
		float ATKBalance;
		float DEFBalance;
		float CTBalance;
		float SGBalance;
		float SKBalance;
		float CritHurt;
		float CriticalRating;
	};
	//ǿ������
	struct SEquipStrengthenSpend
	{
		UINT64 gold;		//���
		UINT16 stones;		//ǿ��ʯ��������
		SEquipStrengthenSpend()
		{
			gold = 0;
			stones = 0;
		}
	};
#pragma pack(pop)

	//������Ϸ�����ļ�
	BOOL LoadGameCfgFile();

	//��ȡ��󴴽���ɫ��
	BYTE GetMaxCreateRoles() const
	{
		return m_maxCreateRoles;
	}

	//��ȡ��ɵ����������������
	BYTE GetMaxInstRankNum() const
	{
		return m_maxInstRankNum;
	}

	//�õ���������Ƕ��ʯ����
	BYTE GetMaxInlaidStones(UINT16 /*propId*/, BYTE /*propGrade*/)
	{
		return 4;
	}

	//��ȡ�����ָ�ʱ����
	UINT16 GetVitRecoveryInterval() const
	{
		return m_vitRecoveryInterval;
	}

	//��ȡ��λʱ���ڻָ�����ֵ
	UINT16 GetVitRecoveryVal() const
	{
		return m_vitRecoveryVal;
	}

	//�Զ��ָ��������ֵ
	UINT32 GetMaxAutoRecoveryVit() const
	{
		return m_maxAutoRecoveryVit;
	}

	//��ȡ��������������
	BYTE GetMaxAcceptTasks() const
	{
		return m_maxAcceptTasks;
	}

	//��ȡװ������Ǽ�
	BYTE GetEquipMaxStarLevel() const
	{
		return m_equipMaxStarLv;
	}

	//��ȡװ�����ǿ���ȼ�
	BYTE GetEquipMaxStrengthenLevel() const
	{
		return m_equipMaxStrengthenLv;
	}

	//��ȡ��󱣴���Ʒ���ۼ�¼
	BYTE GetMaxSaveSellProps() const
	{
		return m_maxSaveSellProps;
	}

	//��ȡ�ʼ���󸽼�����
	BYTE GetMaxEmailAttachments() const
	{
		return m_maxEmailAttachments;
	}

	//��ȡ����������
	BYTE GetMaxFriendNum() const
	{
		return m_maxFriendNum;
	}

	//��ȡ��ɫ���ȼ�
	BYTE GetMaxRoleLevel() const
	{
		return m_maxRoleLevel;
	}

	//��ȡ�Ƽ���������
	BYTE GetRecommendFriendNum() const
	{
		return m_recommendFriendNum;
	}

	//��ȡ�Ƽ����ѵȼ�
	BYTE GetRecommendFriendLv() const
	{
		return m_recommendFriendLv;
	}

	//��ȡ��ʼ������С
	BYTE GetInitBackpackSize() const
	{
		return m_initBackpackSize;
	}

	//��ȡ��ʼ�ֿ��С
	BYTE GetInitWarehouseSize() const
	{
		return m_initWarehouseSize;
	}

	//��ȡ��󱳰���С
	BYTE GetMaxBackpackSize() const
	{
		return m_maxBackpackSize;
	}

	//��ȡ����ϵ��
	BYTE GetPackParam() const
	{
		return m_packParam;
	}

	//��ȡ���ֿ��С
	BYTE GetMaxWarehouseSize() const
	{
		return m_maxWarehouseSize;
	}

	//��ȡս��ƽ��ֵ
	void GetCPBalanceAttr(SCPBalanceAttr& cpBalanceAttr)
	{
		cpBalanceAttr = m_CPBanlance;
	}

	//��ȡ�ع��۸���
	float GetBuybackPriceTimes() const
	{
		return m_buybackPriceTimes;
	}

	//��ȡ��ҵ����ֵ
	UINT32 GetDropGoldBasicVal() const
	{
		return m_dropGoldBasicVal;
	}

	//��ȡ��ҵ����ֵ�����ٷֱ�
	BYTE GetDropGoldFloat() const
	{
		return m_dropGoldFloat;
	}

	//�������ɹ���
	BYTE GetRiseStarMaxSuccessChance() const
	{
		return m_riseStarMaxSuccessChance;
	}

	//������С������
	BYTE GetRiseStarMinDamageChance() const
	{
		return m_riseStarMinDamageChance;
	}

	//��ȡ��������ֵ
	int GetExpCorrectVal() const
	{
		return m_expCorrectVal;
	}

	//��ȡս��������Χֵ
	UINT32 GetPowerCorrectVal() const
	{
		return m_powerCorrectVal;
	}

	//��ȡɾ����ɫʱ������
	UINT32 GetDelRoleTimeLimit() const
	{
		return m_delRoleTimeLimit;
	}

	//��ȡ���������
	UINT32 GetActivityInstUpperLimit() const
	{
		return m_activityInstUpperLimit;
	}

	//��ȡǿ������
	int GetEquipStrengthenSpend(BYTE pos, BYTE lv, SEquipStrengthenSpend& spend);

	////���������� add by hxw 20151212
	////ֻ�Բ����ã�������������
	//int TestTask()
	//{
	//	return m_taskCfg.TestTask();
	//}
	////end

	private:
		BYTE m_maxCreateRoles;	//��ഴ���Ľ�ɫ����
		BYTE m_maxInstRankNum;	//����������(ǰ����)

		UINT16 m_vitRecoveryInterval;	//�����ָ�ʱ����(��)
		UINT16 m_vitRecoveryVal;		//�����ָ�ʱ�����ڻָ�ֵ
		UINT32 m_maxAutoRecoveryVit;	//�Զ��ָ��������ֵ(������ֵ�򲻴���,��ͨ�������������ʽ������Ӧ�ۼ�,���ܸ�ֵ��Ӱ��)

		BYTE m_initBackpackSize;	//��ʼ������С
		BYTE m_initWarehouseSize;	//��ʼ�ֿ��С
		BYTE m_maxBackpackSize;		//��󱳰���С
		BYTE m_maxWarehouseSize;	//���ֿ��С
		BYTE m_packParam;			//����ϵ��

		BYTE  m_maxAcceptTasks;			//��������������
		BYTE  m_equipMaxStarLv;			//װ������Ǽ�
		BYTE  m_equipMaxStrengthenLv;	//װ�����ǿ���ȼ�
		BYTE  m_maxSaveSellProps;		//��󱣴���Ʒ���ۼ�¼
		BYTE  m_maxEmailAttachments;	//�ʼ���󸽼�����
		BYTE  m_maxRoleLevel;			//��ɫ���ȼ�

		BYTE  m_maxFriendNum;			//����������
		BYTE  m_recommendFriendNum;		//�Ƽ���������
		BYTE  m_recommendFriendLv;		//�Ƽ����ѵȼ�

		SCPBalanceAttr m_CPBanlance;	//ս��ƽ��

		float m_strengthenStoneModulus;	//ǿ��ʯϵ��
		float m_strengthenGoldModulus;	//ǿ�����ϵ��
		int m_stoneConsumeCorrectVal;	//ʯͷ��������ֵ
		int m_goldConsumeCorrectVal;	//�����������ֵ
		std::vector<UINT32> m_strengthenBasicConsumeStoneVec;	//ǿ����������ʯͷ
		std::vector<UINT32> m_strengthenBasicConsumeGoldVec;	//ǿ���������Ľ��

		float m_buybackPriceTimes;		//�ع��۸���
		UINT32 m_dropGoldBasicVal;		//��ҵ����ֵ
		BYTE   m_dropGoldFloat;			//��ҵ����ֵ�����ٷֱ�

		UINT32 m_goldBranchThrehold;	//��ҷֶѷ�ֵ
		BYTE   m_goldBranchMaxPercent;	//��ҷֶ����ٷֱ�
		BYTE   m_dropLv1ToBossPercent;	//��ͨ��Ʒ�ֵ�boss�ٷֱ�
		BYTE   m_dropLv1ToCreamPercent;	//��ͨ��Ʒ�ֵ���Ӣ�ٷֱ�
		int    m_expCorrectVal;			//��������ֵ

		BYTE m_riseStarMaxSuccessChance;	//�������ɹ���
		BYTE m_riseStarMinDamageChance;		//������С������
		UINT32 m_powerCorrectVal;			//ս��������Χֵ
		UINT32 m_activityInstUpperLimit;	//���������
		UINT32 m_delRoleTimeLimit;			//ɾ����ɫʱ������

		std::map<UINT32, UINT32> m_ladderMatching;	//����ƥ������

		RoleBasicCfg m_roleCfg;		//��ɫ����
		InstCfg m_instCfg;			//��������
		SkillCfg m_skillCfg;		//��������
		MaterialCfg m_materialCfg;	//��������
		TaskCfg m_taskCfg;			//��������
		EquipCfg m_equipCfg;		//װ������
		RiseStarCfg m_riseStarCfg;	//װ����������
		QualityCfg m_qualityCfg;	//װ��Ʒ������
		GemCfg m_gemCfg;			//��ʯ����
		FragmentCfg m_fragmentCfg;	//��Ƭ����
		MallCfg m_mallCfg;			//�̳�����
		FashionCfg m_fashionCfg;	//ʱװ����
		RechargeCfg m_rechargeCfg;	//��ֵ����
		TitleCfg m_titleCfg;		//�ƺ�����
		EmailCfg m_emailCfg;		//�ʼ���������
		RandInstSysCfg m_randInstSysCfg;	//���������������
		SuitCfg m_suitCfg;			//��װ����
		ReviveCfg m_reviveCfg;		//��������
		GoldExchangeCfg m_goldExchangeCfg;	//��Ҷһ�����
		VIPCfg m_vipCfg;			//VIP����
		MonsterCfg m_monsterCfg;	//������
		StrengthenCfg m_strengthenCfg;	//ǿ������
		GameDropCfg m_gameDropCfg;	//��Ϸ��������
		EquipComposeCfg m_equipComposeCfg;	//��Ƭ�ϳ�����
		ChapterCfg m_chapterCfg;	//�½�����
		SignInCfg m_signInCfg;		//ǩ������
		ActivityCfg m_activityCfg;	//�����
		NoticeCfg m_noticeCfg;		//��������
		UnlockCfg m_unlockCfg;		//��������
		GiftBagCfg m_giftBagCfg;	//�������
		InitRoleGiveCfg m_newRoleGiveCfg;	//�½�ɫ����
		RouletteCfg m_rouletteCfg;	//��������
		LimitedTimeActivityCfg m_limitedTimeActivity;	//��ʱ�����
		GameTypeCfg m_gameTypeCfg;						//�ؿ��淨����
		MonsterAttributeCfg m_monsterAttributeCfg;		//������������
		MonsterTypeConfig m_monsterTypeCfg;				//������������
		StrengthenAttriCfg m_strengthenAttriCfg;		//װ��λ��������
		PlayerCfg m_playerCfg;							//��ɫ����
		ActivityStageCfg m_activityStageCfg;			//���������

		/* zpy �ɾ�ϵͳ���� */
		AchievementCfg m_achievementCfg;

		/* zpy �����һ� */
		VitExchangeCfg m_vitExchangeCfg;

		/* zpy ÿ��ǩ�� */
		DailySignCfg m_dailySignCfg;

		/* zpy ������Ƭ */
		ChainSoulFragmentCfg m_chainSoulFragmentCfg;
		/* zpy ����ϵͳ���� */
		GreedTheLiveCfg m_greedTheLiveCfg;

		//add by hxw 20151013 ���н�������
		RankRewardCfg m_rankRewardCfg;
		//end

		private:
			BOOL LoadGameGlobalCfg(const char* filename);
			BOOL OpenCfgFile(const char* filename, Json::Value& rootValue);

			public:
				//��ȡ��ɫ��������
				int GetRoleCfgBasicAttr(BYTE jobId, BYTE roleLevel, SRoleAttrPoint& attr);
				//�жϽ�ɫ����
				int JudgeRoleUpgrade(UINT16 jobId, BYTE curLevel, UINT64 curExp, BYTE& newLevel);
				//���ݾ���������ȼ�
				BYTE GetRoleLevelByExp(BYTE jobId, UINT64 exp);

				public:
					//��ȡĳ��ɫ��from����to��ӵ�����м���
					int GetRoleAllInitSkills(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& allSkills);
					//��������(skillId->��ǰҪ�����ļ���id)
					int GetUpgradeAnySkillAttr(BYTE jobId, UINT16 skillId, BYTE newSkillLevel, SkillCfg::SSkillAttrCfg& skillAttr);
					//�Ƿ�����ͨ����
					bool IsSkill(BYTE occuId, UINT16 skillID);
					/* zpy ���� ��ȡ��ͨ�������� */
					bool GetSkillAttr(BYTE jobId, UINT16 skillID, SkillCfg::SSkillAttrCfg *&item);

					public:
						//��ȡ������Ҫ���ĵ�������
						int GetInstSpendVit(UINT16 instId, UINT32& spendVit);
						//��ȡ��������
						int GetInstName(UINT16 instId, std::string& instName);
						//��ȡ�����ȼ�����
						int GetInstLvLimit(UINT16 instId, BYTE& lvLimit);
						//��ȡ��������
						int GetInstConfig(UINT16 instId, InstCfg::SInstAttrCfg *&config);
						//��ȡ����������Ϣ
						int GetInstBasicInfo(UINT16 instId, BYTE&instType, BYTE& lvLimit, UINT32& spendVit);
						//�����������id
						UINT16 GenerateRandInstId();
						//���ɱ������
						int GenerateDropBox(UINT16 instId, InstCfg::SInstDropBox& dropBox);
						//��������ֵ���
						int GenerateAreaMonsterDrops(UINT16 instId, BYTE areaId, InstCfg::SInstDropProp& dropProps, BYTE& monsterDrops, BYTE& creamDrops, BYTE& bossDrops);
						/* zpy 2015.12.22���� չ�������� */
						void SpreadDropGroups(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem> *ret);
						//��ȡս�������
						int GetBattleAreaMonster(UINT16 instId, BYTE areaId, InstCfg::SInstBrushMonster& brushMonster);
						//�������������
						int GenerateBrokenDrops(UINT16 instId, InstCfg::SInstDropProp& brokenDrops);
						//��ȡ����ɨ������
						UINT64 GetInstSweepExp(UINT16 instId);
						//����ɨ������
						int GenerateInstSweepDrops(UINT16 instId, InstCfg::SInstDropProp& drops);
						//��ȡ���������½�
						int GetInstOwnerChapter(UINT16 instId, UINT16& chapterId);
						//��ȡ��������
						BYTE GetInstType(UINT16 instId);
						//��ȡ�������
						UINT16 GetInstEnterNum(UINT16 instId);
						//ͨ�����ͻ�ȡ����id
						bool GetInstByInstType(BYTE type, std::set<UINT16> *&ret);

						public:
							//��ȡ��Ʒ����
							int GetPropertyAttr(BYTE propType, UINT16 propId, SPropertyAttr& propAttr, BYTE& maxStackCount);
							//��ȡ��Ʒ���ѵ���
							BYTE GetPropertyMaxStack(BYTE propType, UINT16 propId);
							//��ȡ��Ʒ�û����ۼ۸�
							int GetPropertyUserSellPrice(BYTE propType, UINT16 propId, UINT64& price);
							//��ȡ��Ʒ�û��ع��۸�
							int GetPropertyUserBuyBackPrice(BYTE propType, UINT16 propId, UINT64& price);
							//��ȡ��Ʒ����
							void GetPropertyName(BYTE propType, UINT16 propId, std::string& name);

							public:
								//��ȡ��ɫ�ճ�����
								int GetDailyTasks(BYTE roleLv, TaskCfg::SDailyTasks& dailyTasks);
								//��ȡ������Ϣ
								int GetTaskInfo(UINT16 taskId, TaskCfg::STaskAttrCfg*& pTaskInfo);
								//��ȡ������
								int GetTaskReard(UINT16 taskId, BYTE roleLv, TaskCfg::STaskReward& taskReward);

								public:
									//��ȡװ����Ϣ
									int GetEquipPropAttr(UINT16 equipId, SPropertyAttr& propAttr, BYTE& maxStack);
									//��ȡװ�����Ե�
									int GetEquipAttrPoint(UINT16 equipId, std::vector<EquipCfg::SEquipAddPoint>*& addPoint);

									public:
										//��ȡ��ʯ����
										int GetGemSpendByLevel(BYTE gemlv, GemCfg::SGemConsumeCfg& spend);
										//���ݱ�ʯid��ȡ��ʯ����
										int GetGemSpendById(UINT16 gemId, GemCfg::SGemConsumeCfg& spend);
										//����װ������,��ʯ�׻�ȡ��Ӧ��ʯ����,װ���ȼ�����
										int GetEquipInlaidGemAttr(BYTE equipType, BYTE holeSeq, GemCfg::SEquipGemAttr& attr);
										//��ȡ��ʯ��������
										int GetGemCfgAttr(UINT16 gemPropId, GemCfg::SGemAttrCfg& gemAttr);
										//���ݱ�ʯ����,�ȼ���ȡ��ʯid,���ѵ�����
										int GetGemIdMaxStackByTypeLevel(BYTE gemType, BYTE gemLv, UINT16& gemId, BYTE& maxStack);
										//��ȡ��ʯ��������
										int GetGemPropAttr(UINT16 gemPropId, SPropertyAttr& propAttr, BYTE& maxStack);
										//��ȡ��ʯ�ϳ����ı�ʯ����
										UINT16 GetComposeGemConsumeGems() const;

										public:
											//��ȡ���Ϲ���ֵ
											int GetMaterialFuncValue(UINT16 materiaId, UINT32& funcVal);
											//��ȡ���Ϲ���ֵ
											int GetMaterialFuncValueVec(UINT16 materiaId, std::vector<UINT32>& funcValVec);

											public:
												//��ȡ�̳���Ʒ����
												int GetMallBuyGoodsAttr(UINT32 buyId, MallCfg::SMallGoodsAttr*& pGoodsAttr);
												int GetMallBuyGoodsAttrByPropId(UINT16 propId, MallCfg::SMallGoodsAttr *&pGoodsAttr);

public:
	//ʱװ��ְҵ�Ƿ�ƥ��
	int IsJobFashionMatch(BYTE jobId, UINT16 fashionId);
	//��ȡʱװ����
	int GetFashionAttr(UINT16 fashionId, std::vector<FashionCfg::SFashionAttr>*& pAttrVec);

	public:
		//��ȡ��ֵ����
		int GetRechargeAttr(UINT16 cashId, RechargeCfg::SRechargeCfgAttr& cashAttr);

		//��ȡ��ֵ����
		const std::map<UINT16, RechargeCfg::SRechargeCfgAttr>& GetRechargeConfig() const;

public:
	//��ȡ�ƺ�����
	int GetRoleTitleAttr(BYTE jobId, UINT16 titleId, std::vector<TitleCfg::STitleAttr>& attrVec);

	public:
		//�ʼ���������
		int GetNewPlayerEmailGiveProps(std::vector<EmailCfg::SNewPlayerEmailItem>& giveVec);

public:
	//�����������
	BOOL GetRandInstSysParam(RandInstSysCfg::SRandInstSysParam& param);

	public:
		//��ȡ��Ƭ����
		int GetFragmentAttr(UINT16 fragmentId, FragmentCfg::SFragmentCfgAttr& fragmentAttr);
		//��ȡ������Ƭ
		int GetGiveFragment(std::vector<FragmentCfg::SGiveFragmentInfo>& giveVec);

public:
	//װ���ϳ�
	int GetEquipComposeItem(UINT16 composeId, EquipComposeCfg::SEquipComposeItem*& pComposeItem);
	//��ȡ���ǻ���
	int GetRiseStarSpend(BYTE starLv, RiseStarCfg::SRiseStarSpend& spend);
	//��ȡ��������
	int GetRiseStarCfg(BYTE starLv, RiseStarCfg::SRiseStarCfg& cfg);
	//��ȡװ����������
	int GetEquipAttrIncreament(BYTE pos, BYTE quality, StrengthenCfg::SEquipAttrInc& attrInc);


	public:
		//��ȡ����CD
		int GetReviveCDTimes(UINT16 reviveId);

		//��ȡ��������
		int GetReviveConsume(UINT16 reviveId, UINT32 times, UINT16 vipAddTimes, UINT32& reviveCoin);

public:
	//��ȡ��Ҷһ�
	int GetGoldExchange(UINT16 exchangeTimes, UINT16 vipAddTimes, UINT64& getGold, UINT32& spendToken);

	/* zpy �һ����� */
	int GetVitExchange(UINT16 exchangeTimes, UINT16 vipAddTimes, UINT64& getVit, UINT32& spendToken);

	public:
		//����VIP�ȼ�
		BYTE CalcVIPLevel(UINT32 cashcount);
		//��ȡVIP������Ʒ
		void GetVIPGiveProps(BYTE vipLv, std::vector<VIPCfg::SVIPGiveProp>*& pGivePropVec);
		//��ȡVIP�������
		void GetVIPMaxVit(BYTE vipLv, UINT32& maxVit);
		//��ȡVIP������������
		void GetVIPBuyVitTimes(BYTE vipLv, UINT16& buyVitTimes);
		//��ȡVIP��Ҷһ�����
		void GetVIPGoldExchangeTimes(BYTE vipLv, UINT16& exchangeTimes);
		//��ȡVIP�����������
		void GetVIPDeadReviveTimes(BYTE vipLv, BYTE& reviveTimes);
		//��ȡVIP����ɨ������
		void GetVIPInstSweepTimes(BYTE vipLv, UINT16& sweepTimes);
		//��ȡVIP��Դ������ս����
		void GetVIPResourceInstChalNum(BYTE vipLv, UINT16& chalTimes);
		//��ȡVIP�̳ǹ�������ۿ�
		void GetVIPMallBuyDiscount(BYTE vipLv, BYTE& discount);
		//��ȡVIPǩ����������
		void GetVIPSignReward(BYTE vipLv, BYTE& signReward);
		/* zpy �����һ� */
		void GetVIPVitExchangeTimes(BYTE vipLv, UINT16& exchangeTimes);
		//��ȡVIP���򾺼�ģʽ����
		void GetVIPBuyKOFTimes(BYTE vipLv, UINT16& buyKOFTimes);
		//��ȡ1v1��������
		void GetOneVsOneEnterTimes(BYTE vipLv, UINT16& enterTimes);

		public:
			//��ȡ�־���
			UINT32 GetMonsterExp(UINT16 momsterId);
			/* zpy �ɾ�ϵͳ���� */
			BYTE GetMonsterType(UINT16 momsterId);
			//��ȡ���Ѷ�ϵ��
			UINT32 GetMonsterDifficulty(UINT16 monsterId);

public:
	//��ȡǩ����Ϣ
	int GetSignInItem(BYTE nTimes, SignInCfg::SSignInItem*& pItem);

public:
	//���Ϣ
	int GetActivityItem(UINT32 activityId, ActivityCfg::SActivityItem& activityItem);

public:
	//����
	int GetNoticeItems(std::vector<NoticeCfg::SNoticeItem>& noticeVec, UINT32& timeInterval);
	//��ȡʱ����
	UINT32 GetNoticeInterval();

public:
	//��ȡ��������
	int GetUnlockItem(BYTE type, UINT16 unlockId, UnlockCfg::SUnlockItem*& pUnlockItem);
	//��ȡδ����id
	int GetUnlockedItems(BYTE type, std::vector<UINT16>& itemsVec);

public:
	//��ȡ�����Ʒ
	int GetGiftBagItem(UINT16 id, GiftBagCfg::SGiftBagItem& item);

public:
	//��ȡ�½�����
	int GetChapterItem(UINT16 chapterId, ChapterCfg::SChapterItem*& pChapterItem);

public:
	//��ȡ��ɫ��ʼ����
	BOOL GetRoleInitGiveItem(InitRoleGiveCfg::SInitRoleGiveItem& giveItem);

public:
	//�������̽�Ʒ
	BOOL GenerateRouletteReward(SGetRoulettePropRes* pRoulettePropRes, std::vector<RouletteCfg::SRoulettePropCfgItem>& saveDBItemVec);
	BOOL GenerateRouletteReward(std::vector<RouletteCfg::SRoulettePropCfgItem>& itemVec);
	//�������̴�
	BOOL GenerateRouletteBigReward(RouletteCfg::SRoulettePropCfgItem& bigReward, UINT32& spendToken);
	//��ȡÿ�����ͳ齱����
	BYTE GetEverydayGiveTimes() const;

public:
	//��ȡ�ʼ���������
	const char* GetEmailTypeDesc(BYTE type);

public:
	//��ȡ�ɾ�Ŀ������
	bool GetAchievementTargetType(UINT32 id, AchievementCfg::TargetType &type) const;
	//��ȡ�ɾͽ���
	bool GetAchievementReward(UINT32 id, AchievementCfg::SAchievementReward &reward) const;
	//��ȡ�ɾ�����
	bool GetAchievementCondition(UINT32 id, AchievementCfg::SAchievementCondition &condition);
	//���ݳɾ����ͻ�ȡ�ɾ�
	bool GetAchievementFromType(AchievementCfg::AchievementType type, const std::vector<UINT32> *&out) const;
	//����Ŀ�����ͻ�ȡ�ɾ�
	bool GetAchievementFromTargetType(AchievementCfg::TargetType type, const std::vector<UINT32> *&out) const;

public:
	//�Ƿ���ÿ��ǩ��
	bool IsDailySignValid() const;

	//��ȡÿ��ǩ������
	bool GetDailySignConfig(BYTE days, const DailySignCfg::SDailySignItem *&config);

public:
	//��ȡ������Ƭ���ۼ۸�(����)
	int GetFragmentUserSellPrice(UINT32 fragmentId, UINT64& price);
	//��ȡ������Ƭ����
	bool GetChainSoulFragmentAttr(UINT32 fragmentId, ChainSoulFragmentCfg::ChainSoulFragmentAttr &out_attr);
	//ͨ�����ͻ�ȡ������ƬID
	bool GetFragmentIdByType(BYTE type, UINT32 &out_id);

public:
	//�Ƿ�������ϵͳ
	bool IsOpenChainSoulSystem() const;
	//��ȡ��������
	bool GetChainSoulPosConfig(BYTE type, BYTE level, GreedTheLiveCfg::ChainSoulPosConfig *&out_config);
	//��ȡʥ���������
	bool GetChainSoulCoreConfig(BYTE level, GreedTheLiveCfg::ChainSoulCoreConfig *&out_config);
	//���������ȼ���ȡ���ĵȼ�
	BYTE GetCoreLevelByConditionLevel(BYTE condition_level);


	//add by hxw 20151014 ���н���������Ϣ
public:
	//��ȡ���н����������
	INT GetRewardPlayNum(const BYTE type) const;
	BOOL GetRankReward(const BYTE type, const UINT16 rankid, CRankReward::SRkRewardRes*& items, const int iValue = 0);
	//�Ƿ��ǻʱ�䣬����TRUE��ʾ�������FLASE��ʾ�޷��
	BOOL IsValidityTime(const BYTE type);
	//�Ƿ���
	BOOL IsOpen(const BYTE type) const;
	void SetOpen(const BYTE type, const BOOL bOpen);
	BOOL IsCanGet(const BYTE type, const UINT16 id, const UINT32 value) const;
	int GetRankLogs(std::string& str, const BYTE type, const UINT16 rewardid = 0);


	//end


public:
	//��ȡ��ʱ�����
	bool GetLimitedTimeActivityCfg(LimitedTimeActivityCfg::ActivityType type, LimitedTimeActivityCfg::SActivityConfig &out);

	//���ɻ����
	bool GenerateActivityInst(int level, std::set<UINT16> passInsts, SActivityInstItem &item);

public:
	//��ȡ����BossѪ��
	UINT32 GetWorldBossBlood() const;

	//��ȡcd����
	UINT32 GetGameTypeEliminateExpend(BYTE type, BYTE times);

	//��ȡˢ������
	UINT32 GetGameTypeRefurbishExpend(BYTE type, BYTE times);

	//��ȡ����۸�
	UINT32 GetGameTypeBuyPriceExpend(BYTE type, UINT16 times);

	//��ȡ�������
	BYTE GetGameTypeReviveRule(BYTE type);

	//��ȡ�����ͽ���
	bool GetConditionAndReward(BYTE type, std::vector<GameTypeCfg::SRewardCondition> *&out);

	//��ȡ���������ս����
	UINT32 GetLadderMaxChallengeNum() const;

	//��ȡ������սCD
	UINT32 GetLadderChallengeCDTime() const;

	//��ȡ1v1��������
	BYTE GetOneOnOneRewardRatio(BYTE finCode) const;

	//�������Ƕ��
	void UnlockRewardNesting(const std::vector<GameTypeCfg::SRewardInfo> &source, std::vector<GameTypeCfg::SRewardInfo> *ret);

public:
	//��ȡ����Ѫ��
	UINT32 GetMonsterBlood(UINT16 level);

	//��ȡ����Ѫ����
	UINT32 GetMonsterBloodProportion(UINT16 id);

public:
	//��ȡװ��λǿ������
	BOOL GetStrengthenAttri(BYTE level, StrengthenAttriCfg::SStrengthenAttr &ret);

public:
	//��ȡ����ƥ�䷶Χ
	UINT32 GetLadderMatchingScope(UINT32 rank);

public:
	// ���������ɫ
	BYTE GeneraWeekFreeActorType();

public:
	//��ȡ�������������
	bool GetActivityStageOpenCondition(UINT16 instId, ActivityStageCfg::Condition &ret);
};

BOOL InitGameCfgFileMan();
GameCfgFileMan* GetGameCfgFileMan();
void DestroyGameCfgFileMan();

#endif