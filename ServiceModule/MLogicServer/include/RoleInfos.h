#ifndef __ROLE_INFOS_H__
#define __ROLE_INFOS_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�����ɫ��Ϣ���ݽṹ
*/
#include <map>
#include <vector>
#include "ServiceMLogic.h"
#include "EquipCfg.h"
#include "InstCfg.h"
#include "StrengthenCfg.h"

#define USE_MEM_POOL 1

#if USE_MEM_POOL
	#include "ssmempool.h"
#endif

#define GENERIC_SQL_BUF_LEN		1024
#define MAX_SQL_BUF_LEN			64*1024
#define MAX_PUT_ITEM_RET_NUM	255	//0xFF

#pragma pack(push, 1)

//ʱʰȡ��������֪ͨ
struct SPickupInstDropNotify
{
	SGoldUpdateNotify* pGold;
	STokenUpdateNotify* pToken;
	SVitUpdateNotify* pVit;
	SBackpackUpdateNotify* pBackpack;
	SFragmentUpdateNotify* pFragment;
	SChainSoulFragmentNotify* pChainSoulFragment;
};

//��ȡ�������������
struct SReceiveRewardRefresh
{
	BOOL gold;
	BOOL rpcoin;
	BOOL fragment;
	BOOL prop;
	BOOL chainSoulFragment;
	BOOL exp;
	BOOL vit;
	BOOL dress;
	BOOL magics;
	BOOL upgrade;
};


struct SOnlineInfo
{
	UINT32 id;	 //����ID	
	UINT32 itmes;//������ʱ��
	LTMSEL lasttime;//���һ�ν�����Ϸʱ��

	SOnlineInfo()
	{
		id = 0;
		itmes = 0;
		lasttime = 0;
	}
	void OnClear()
	{
		id = 0;
		itmes = 0;
		lasttime = 0;

	}
};



//��ɫ��������
struct SRoleInfos
{
	UINT32 roleId;			//��ɫid
	char nick[33];			//�ǳ�[33];
	UINT16 titleId;			//�ƺ�id;
	BYTE level;				//��ɫ�ȼ�
	SBigNumber gold;		//��Ϸ��
	SBigNumber exp;			//����
	UINT32	cash;			//�ֽ�
	BYTE	occuId;			//ְҵid
	BYTE	sex;			//�Ա�
	UINT32	cashcount;		//��ֵ�ܶ�
	UINT32	hp;				//����
	UINT32	mp;				//ħ��
	UINT32	attack;			//����
	UINT32	def;			//����
	UINT32	vit;			//����
	UINT16	ap;				//ʣ�����Ե���
	BYTE    packSize;		//������С
	BYTE    wareSize;		//�ֿ��С
	UINT32  guildId;		//��������id
	UINT32	cp;				//ս��;
	SGSPosition	pos;		//����λ��, ��ʱ����
	BYTE   newPlayerState;	//��������״̬ δ��� �����
	UINT32 vist;			//���ʴ���
	BYTE   factionId;		//��Ӫid
	UINT16 dressId;			//ʱװid
	char   lastEnterTime[SGS_DATE_TIME_LEN];	//���һ�ν�����Ϸʱ��
	LTMSEL enterInstTTS;	//���븱��ʱ��

	BYTE   isOnline;	//�Ƿ�����
	BYTE   isEnterGame;	//�Ƿ�������Ϸ
	UINT32 uid;			//װ��Ψһid
	UINT32 onlineRecoveryVit;	//���߻ָ���������
	BYTE   reviveTimes;			//�������
	UINT16 exchangedTimes;		//��ǰ����Ѷһ�����
	UINT32 recommendFriendTimes;	//�Ƽ����Ѵ���
	UINT32 instDropGold;		//����������
	UINT32 instDropToken;		//�����������

	UINT16 vitExchangedTimes;		//��ǰ�����һ�����

	UINT16 ladder_win_in_row;			//������ʤ����
	UINT16 ladder_refresh_count;		//����ˢ�´���
	UINT16 ladder_challenge_count;		//������ս����
	UINT16 ladder_eliminate_count;		//��������CD����
	LTMSEL ladder_last_finish_time;		//�ϴ��������ʱ��
	UINT32 ladder_matching_rank[LADDER_MATCHTING_MAX_NUM];	//����ƥ������

	std::vector<SPropertyAttr>	packs;			//��������
	std::vector<SPropertyAttr>  wearEquipment;	//�Ѵ���װ��
	std::vector<SSkillMoveInfo> studySkills;	//�Ѿ�ѧ��������
	std::vector<SAcceptTaskInfo> acceptTasks;	//�ѽ�������
	std::vector<SBufferInfo> buffers;			//Buff����
	std::vector<UINT16> dress;					//ӵ��ʱװ
	std::vector<SEquipFragmentAttr> fragments;	//װ����Ƭ
	std::vector<UINT16> helps;					//��������
	std::vector<UINT16> magics;					//ħ����
	std::vector<SEquipPosAttr> equipPos;		//װ��λ����
	std::vector<LTMSEL> signInVec;				//ǩ��ʱ��
	std::vector<SChainSoulFragmentAttr> chainSoulFragments;	//������Ƭ
	std::vector<SChainSoulPosAttr> chainSoulPos;//���겿������
	std::set<UINT32> robotSet;					//�����˼���					

	//add by hxw 20151020 	
	SOnlineInfo onlineVec;						//���߽�����ȡ����
	std::vector<UINT16> onlines;				//���߽�����ȡ����
	std::vector<UINT16> strongerVec;			//ǿ�߽�����ȡ״��
	//end

	InstCfg::SInstDropBox dropBox;		//�������䱦��
	InstCfg::SInstDropProp brokenDrops;		//�������������
	InstCfg::SInstDropProp dropProps;	//��������ֵ�����Ʒ
	InstCfg::SInstBrushMonster brushMonster;	//��������ˢ�ֱ�

	SRoleInfos()
	{
		roleId = 0;			 //��ɫid
		titleId = 0;		 //ְҵ�ƺŵȼ�;
		level = 0;			 //��ɫ�ȼ�
		gold.hi = 0;
		gold.lo = 0;
		exp.hi = 0;
		exp.lo = 0;
		cash = 0;			//�ֽ�
		occuId = 0;			//��ɫ����
		sex = 0;
		cashcount = 0;		//��ֵ�ܶ�
		hp = 0;				//����
		mp = 0;				//ħ��
		attack = 0;			//����
		def = 0;			//����
		vit = 0;			//����
		ap = 0;				//ʣ�����Ե���
		packSize = 0;		//������С
		wareSize = 0;		//�ֿ��С
		guildId = 0;		//��������id
		cp = 0;				//ս����;
		newPlayerState = 0;	//��������״̬ δ��� �����
		vist = 0;			//���ʴ���
		isOnline = 0;		//�Ƿ�����
		factionId = 0;		//��Ӫid
		dressId = 0;		//ʱװid
		enterInstTTS = 0;	//���븱��ʱ��

		isEnterGame = 0;	//�Ƿ�������Ϸ
		uid = 1;			//װ��Ψһid
		onlineRecoveryVit = 0;
		reviveTimes = 0;	//�������
		exchangedTimes = 0;	//��Ҷһ�����
		recommendFriendTimes = 0;	//�Ƽ����Ѵ���
		instDropGold = 0;		//����������
		instDropToken = 0;		//�����������

		memset(nick, 0, sizeof(nick));
		memset(lastEnterTime, 0, sizeof(lastEnterTime));
		memset(&pos, 0, sizeof(SGSPosition));

		vitExchangedTimes = 0;		//�����һ�����

		ladder_win_in_row = 0;			//������ʤ����
		ladder_refresh_count = 0;		//����ˢ�����ݴ���	
		ladder_challenge_count = 0;		//������ս����
		ladder_eliminate_count = 0;		//��������CD����
		ladder_last_finish_time = 0;	//�ϴ��������ʱ��
		memset(ladder_matching_rank, 0, sizeof(ladder_matching_rank));

		for (BYTE pos = ESGS_EQUIP_WEAPON; pos <= ESGS_EQUIP_NECKLACE; ++pos)
		{
			SEquipPosAttr item;
			memset(&item, 0, sizeof(SEquipPosAttr));
			item.pos = pos;
			equipPos.push_back(item);
		}

		for (BYTE idx = 0; idx < MAX_CHAIN_SOUL_HOLE_NUM; ++idx)
		{
			SChainSoulPosAttr attr;
			attr.pos = idx + 1;
			attr.level = 0;
			chainSoulPos.push_back(attr);
		}

		packs.clear();
		wearEquipment.clear();
		studySkills.clear();
		acceptTasks.clear();
		buffers.clear();
		dress.clear();
		fragments.clear();
		helps.clear();
		chainSoulFragments.clear();

		dropBox.boxMap.clear();
		dropProps.props.clear();
		brushMonster.monsterMap.clear();
		brokenDrops.props.clear();

		memset(&onlineVec, 0, sizeof(SOnlineInfo));
		onlines.clear();
		strongerVec.clear();
	}

	// zpy 2015.9.29ע�� �������캯���͸�ֵ��������غ���ʵ�����沢δʵ����ȫ��������֪����
	//SRoleInfos(const SRoleInfos& role)
	//{
	//	roleId = role.roleId;			//��ɫid
	//	titleId = role.titleId;			//ְҵ�ƺŵȼ�;
	//	level = role.level;				//��ɫ�ȼ�
	//	gold.hi = role.gold.hi;
	//	gold.lo = role.gold.lo;
	//	exp.hi = role.exp.hi;
	//	exp.lo = role.exp.lo;
	//	cash = role.cash;			//�ֽ�
	//	occuId = role.occuId;			//��ɫ����
	//	sex = role.sex;
	//	cashcount = role.cashcount;		//��ֵ�ܶ�
	//	hp = role.hp;				//����
	//	mp = role.mp;				//ħ��
	//	attack = role.attack;			//����
	//	def = role.def;			//����
	//	vit = role.vit;			//����
	//	ap = role.ap;				//ʣ�����Ե���
	//	packSize = role.packSize;		//������С
	//	wareSize = role.wareSize;		//�ֿ��С
	//	guildId = role.guildId;		//��������id
	//	cp = role.cp;				//ս����;
	//	newPlayerState = role.newPlayerState;	//��������״̬ δ��� �����
	//	vist = role.vist;			//���ʴ���
	//	isOnline = role.isOnline;			//�Ƿ�����
	//	factionId = role.factionId;	//��Ӫid
	//	dressId = role.dressId;		//ʱװid
	//	reviveTimes = role.reviveTimes;			//�������
	//	exchangedTimes = role.exchangedTimes;
	//	recommendFriendTimes = role.recommendFriendTimes;	//�Ƽ����Ѵ���
	//	instDropGold = role.instDropGold;		//����������
	//	instDropToken = role.instDropToken;		//�����������
	//	enterInstTTS = role.enterInstTTS;	//���븱��ʱ��
	//	strcpy(nick, role.nick);
	//	strcpy(lastEnterTime, role.lastEnterTime);
	//	memcpy(&pos, &role.pos, sizeof(SGSPosition));

	//	vitExchangedTimes = role.vitExchangedTimes;			//�����һ�����

	//	packs.assign(role.packs.begin(), role.packs.end());
	//	wearEquipment.assign(role.wearEquipment.begin(), role.wearEquipment.end());
	//	studySkills.assign(role.studySkills.begin(), role.studySkills.end());
	//	acceptTasks.assign(role.acceptTasks.begin(), role.acceptTasks.end());
	//	buffers.assign(role.buffers.begin(), role.buffers.end());
	//	dress.assign(role.dress.begin(), role.dress.end());
	//	fragments.assign(role.fragments.begin(), role.fragments.end());
	//	chainSoulFragments.assign(role.chainSoulFragments.begin(), role.chainSoulFragments.end());
	//}
	//SRoleInfos& operator = (const SRoleInfos& role)
	//{
	//	roleId = role.roleId;			//��ɫid
	//	titleId = role.titleId;			//�ƺ�id;
	//	level = role.level;				//��ɫ�ȼ�
	//	gold.hi = role.gold.hi;
	//	gold.lo = role.gold.lo;
	//	exp.hi = role.exp.hi;
	//	exp.lo = role.exp.lo;
	//	cash = role.cash;			//�ֽ�
	//	occuId = role.occuId;			//��ɫ����
	//	sex = role.sex;
	//	cashcount = role.cashcount;		//��ֵ�ܶ�
	//	hp = role.hp;				//����
	//	mp = role.mp;				//ħ��
	//	attack = role.attack;	//����
	//	def = role.def;			//����
	//	vit = role.vit;			//����
	//	ap = role.ap;				//ʣ�����Ե���
	//	packSize = role.packSize;		//������С
	//	wareSize = role.wareSize;		//�ֿ��С
	//	guildId = role.guildId;		//��������id
	//	cp = role.cp;				//ս����;
	//	newPlayerState = role.newPlayerState;	//��������״̬ δ��� �����
	//	vist = role.vist;			//���ʴ���
	//	isOnline = role.isOnline;			//�Ƿ�����
	//	factionId = role.factionId;	//��Ӫid
	//	dressId = role.dressId;		//ʱװid
	//	reviveTimes = role.reviveTimes;			//�������
	//	exchangedTimes = role.exchangedTimes;
	//	recommendFriendTimes = role.recommendFriendTimes;	//�Ƽ����Ѵ���
	//	enterInstTTS = role.enterInstTTS;	//���븱��ʱ��
	//	instDropGold = role.instDropGold;		//����������
	//	instDropToken = role.instDropToken;		//�����������
	//	strcpy(nick, role.nick);
	//	strcpy(lastEnterTime, role.lastEnterTime);
	//	memcpy(&pos, &role.pos, sizeof(SGSPosition));

	//	vitExchangedTimes = role.vitExchangedTimes;			//�����һ�����

	//	packs.assign(role.packs.begin(), role.packs.end());
	//	wearEquipment.assign(role.wearEquipment.begin(), role.wearEquipment.end());
	//	studySkills.assign(role.studySkills.begin(), role.studySkills.end());
	//	acceptTasks.assign(role.acceptTasks.begin(), role.acceptTasks.end());
	//	buffers.assign(role.buffers.begin(), role.buffers.end());
	//	dress.assign(role.dress.begin(), role.dress.end());
	//	fragments.assign(role.fragments.begin(), role.fragments.end());
	//	chainSoulFragments.assign(role.chainSoulFragments.begin(), role.chainSoulFragments.end());
	//	return *this;
	//}

	//�Զ��ָ�����
	void AutoRecoveryVit(UINT32 maxAutoVit, LTMSEL nowMsel);
	//��ʼ������
	void InitBackpack(const BYTE* pData, int nDataLen);
	//��ʼ��buffer
	void InitBuffer(const BYTE* pData, int nDataLen);
	//��ʼ���ѽ�������
	void InitAcceptedTasks(const BYTE* pData, int nDataLen, LTMSEL nowMsel);
	//��ʼ������
	void InitSkills(const BYTE* pData, int nDataLen);
	//��ʼ���Ѵ���װ��
	void InitWearEquipment(const BYTE* pData, int nDataLen);
	//��ʼ����ӵ��ʱװ
	void InitHasDress(const BYTE* pData, int nDataLen);
	//��ʼ����ӵ��װ����Ƭ
	void InitHasEquipFragment(const BYTE* pData, int nDataLen);
	//��ʼ��װ��λ
	void InitEquipPos(const BYTE* pData, int nDataLen);

	//��������
	int SkillUpgrade(UINT16 skillId, SSkillUpgradeRes* pUpgradeRes);
	//��ȡ����
	int GetStudySkills(SSkillMoveInfo* pSkills, BYTE maxNum);
	//�����ɫս��
	UINT32 CalcRoleCombatPower();
	//��ȡ��ɫ�������Ե�
	void GetRoleBasicAttrPoint(SRoleAttrPoint& attrPoint);
	//��ȡ��ɫ�Ѵ���װ�����Ե�
	void GetRoleWearEquipAttrPoint(SRoleAttrPoint& attrPoint);
	//��ȡ��ɫ�Ѵ����ƺ����Ե�
	void GetRoleWearTitleAttrPoint(SRoleAttrPoint& attrPoint);
	//��ȡ��ɫ����ʱװ���Ե�
	void GetRoleWearFashionAttrPoint(SRoleAttrPoint& attrPoint);
	/* zpy ��ȡ�������Ե� */
	void GetRoleChainSoulAttrPoint(SRoleAttrPoint& attrPoint);
	//��������ս��
	UINT32 ConvertAttrToPower(const SRoleAttrPoint& attrPoint);

	//��ȡ������Ʒ�����±�
	int GetBackpackPropIdx(UINT32 id);
	//��ȡ�Ѵ���װ�������±�
	int GetWearEquipIdx(UINT32 id);

	//�����Ƿ����ĳ��Ʒ
	BOOL IsExistProperty(UINT16 propId, UINT16 leastNum);
	//������ӵ����Ʒ����(>=leastNum����leastNum���򷵻�ʵ������)
	UINT16 NumberOfProperty(UINT16 propId);
	UINT16 NumberOfProperty(UINT16 propId, UINT16 leastNum);
	//����Ψһid��ȡ������Ʒ��Ϣ
	int GetBackpackPropAttrById(UINT32 id, SPropertyAttr& propAttr);
	//���뵥����Ʒ���뱳��(prop.num=1, maxStack-��������Ʒ���ѵ���)
	int PutBackpackSingleNumProperty(SPropertyAttr& prop, BYTE maxStack);
	//��ȡ������������Ʒ��Ϣ
	int GetBackpackProps(SPropertyAttr* pProps, BYTE maxNum, BYTE from = 0, BYTE* totals = NULL);
	//����������Ʒ(<=num��ȫ������)
	int DropBackpackProp(UINT32 id, BYTE num);
	//������������(<=num��ȫ������)
	int DropBackpackPropEx(UINT16 propId, UINT16 num);
	//������������(<=num��ȫ������)
	int DropBackpackPropEx(UINT16 propId, UINT16 num, SConsumeProperty* pConsumeItem, BYTE maxItemNum, BYTE& retNum);
	int DropBackpackPropEx(UINT16 propId, UINT16 num, SPropertyAttr* pBackpackMod, BYTE maxModNum, BYTE& retNum);
	//�ܷ������Ʒ
	BOOL CanPutIntoBackpack(UINT16 propId, UINT16 num, BYTE maxStack);
	//������Ʒ(�ܷ���)
	int PutBackpackProperty(BYTE propType, UINT16 propId, UINT16 num, BYTE maxStack, SConsumeProperty* pPutItem, BYTE maxItemNum, BYTE& retNum);
	//������Ʒ(�ܷ���)
	int PutBackpackProperty(BYTE propType, UINT16 propId, UINT16 num, BYTE maxStack, SPropertyAttr* pBackpackMod, BYTE maxModNum, BYTE& retNum);
	//�Ƿ�ӵ��ħ����
	BOOL IsHasMagic(UINT16 magicId);
	//��ȡ��Ƭ����
	UINT16 GetFragmentNum(UINT16 fragId);
	//�Ƿ�ӵ��ʱװ
	BOOL IsHasDress(UINT16 dressId);
	//�۳���Ƭ
	void DropFragment(UINT16 fragId, UINT16 num);
	//�۳�ħ����
	void DropMagic(UINT16 magicId);
	//�۳�ʱװ
	void DropDress(UINT16 dressId);

	//������Ʒ
	int SellProperty(UINT32 id, BYTE num, SPropertyAttr& propAttr, SSellPropertyRes* pSellRes);
	//�ع���Ʒ
	int BuyBackProperty(SPropertyAttr& buyProp, BYTE num, SPropBuyBackRes* pBuyBackRes);

	//��ʼ���½�ɫ����
	void InitNewRoleGiveItems(UINT32 userid = 0);
	//��ʼ�½�ɫ�ʼ�
	void InitNewRoleEmail(UINT32 userId, UINT32 roleId);

	//��ȡ�����Ѵ���װ����Ϣ
	int GetWearEquipmentProps(SPropertyAttr* pProps, BYTE maxNum, BYTE from = 0, BYTE* totals = NULL);
	//��ȡ�Ѵ���װ��
	int GetWearEquipmentProp(UINT32 id, SPropertyAttr& propAttr);

	//װ���ϳ�
	int EquipmentCompose(UINT16 composeId, SEquipComposeRes* pComposeRes);
	//����װ��
	int LoadEquipment(UINT32 id, BYTE pos);
	//ж��װ��
	int UnLoadEquipment(UINT32 id);
	//zpy �滻װ��
	int ReplaceEquipment(UINT32 id, const SPropertyAttr& propAttr);

	//װ��λ��Ƕ��ʯ
	int EquipPosInlaidGem(BYTE pos, UINT32 gemId, BYTE holeSeq);
	//װ��λ�Ƴ���ʯ
	int EquipPosRemoveGem(BYTE pos, BYTE holeSeq, SEquipPosRemoveGemRes* pRemoveRes);
	//��ʯ�ϳ�
	int GemCompose(BYTE num, const SConsumeProperty* pMaterial, SGemComposeRes* pComposeRes);

	//��������
	int AcceptTask(UINT16 taskId);
	//��������
	int DropTask(UINT16 taskId);
	//�ж��������
	int JudgeTaskFinish(UINT16 taskId, SFinishTaskFailNotify* pFinTaskNotify);
	//��ȡ�ճ�����
	int GetDailyTasks(UINT32 roleId, SGetDailyTaskRes* pDailyTasks, BYTE maxNum);
	//�ѽ����������Ƿ��Ѵ���
	BOOL IsExistAcceptedTasks(UINT16 taskId);
	//��ȡ������
	int ReceiveTaskReward(UINT32 userId, UINT32 roleId, UINT16 taskId, SReceiveTaskRewardRes* pTaskRewardRes);
	//��������Ŀ��
	int UpdateTaskTarget(UINT16 taskId, const UINT32* pTarget, int n = MAX_TASK_TARGET_NUM);

	//��ȡ�ʼ�����
	int ReceiveEmailAttachments(BYTE emailType, const SPropertyAttr* pAttachmentsProp, BYTE attachmentsNum, SReceiveEmailAttachmentsRes* pEmailAttachmentsRes, BOOL& isUpgrade);
	//�̳ǹ�����Ʒ
	int MallBuyGoods(UINT32 buyId, UINT16 num, SMallBuyGoodsRes* pBuyGoodsRes, UINT32& spendToken);
	//����װ����Ƭ
	UINT16 AddEquipFragment(UINT16 fragmentId, UINT16 num);

	//����
	int RoleRevive(UINT16 reviveId, SReviveRoleRes* pReviveRes);
	//��Ҷһ�
	int GoldExchange(SGoldExchangeRes* pExchangeRes);

	//��ȡVIP����
	int ReceiveVIPReward(BYTE vipLv, SReceiveVIPRewardRes* pVIPRewardRes, std::vector<SPropertyAttr>& emailAttachmentsVec, SReceiveRewardRefresh* pRefresh);

	/* zpy �ɾ�ϵͳ���� */
	int ReceiveAchievementReward(UINT32 achievement_id, SReceiveRewardRes *pAchievementReward, std::vector<SPropertyAttr>& emailAttachmentsVec, SReceiveRewardRefresh *pRefresh);

	/* zpy ��ʱ����� */
	int ReceiveLimitedTimeActivityReward(const std::vector<SPropertyItem> &rewardVec, SFinishInstRes* pFinishInstRes, std::vector<SPropertyAttr> emailAttachmentsVec, SReceiveRewardRefresh *pRefresh);

	//װ��λǿ��
	int EquipPosStrengthen(BYTE pos, SEquipPosStrengthenRes* pStrengthenRes);
	//װ��λ����
	int EquipPosRiseStar(BYTE pos, const SRiseStarAttachParam& param, SEquipPosRiseStarRes* pRiseStarRes);

	//�������
	void ProduceGold(UINT64 goldNum, const char* pszAction, BOOL recordLog = true);
	//���Ľ��
	void ConsumeGold(UINT64 goldNum, const char* pszAction, BOOL recordLog = true);

	//��������
	void ProduceToken(UINT32 tokenNum, const char* pszAction, BOOL recordLog = true);
	//���Ĵ���
	void ConsumeToken(UINT32 tokenNum, const char* pszAction, BOOL recordLog = true);

	/* zpy �����һ� */
	int VitExchange(SVitExchangeRes* pExchangeRes);

	/* zpy ��ȡÿ��ǩ������ */
	int ReceiveDailySignReward(BYTE days, bool continuous, SRoleDailySignRes *pRoleSign, std::vector<SPropertyAttr>& emailAttachmentsVec, SReceiveRewardRefresh *pRefresh);

	// zpy ��ȡ������Ƭ����
	UINT16 GetChainSoulFragmentNum(UINT16 fragId);
	// zpy �۳�������Ƭ
	void DropChainSoulFragment(UINT16 fragId, UINT16 num);
	// zpy ����������Ƭ
	UINT16 AddChainSoulFragment(UINT16 fragmentId, UINT16 num);
	// zpy ��ʼ����ӵ��������Ƭ
	void InitHasChainSoulFragment(const BYTE* pData, int nDataLen);
	// zpy ��ʼ�����겿��
	void InitChainSoulPos(const BYTE* pData, int nDataLen);

	// zpy �������Ѵ���װ��װ�����Ƿ����ĳ��Ʒ
	BOOL IsExistInbackpackAdnWearEquipment(UINT16 propId, UINT16 leastNum);

	// zpy ��ʼ��������Ϣ
	void InitLadderInfo(UINT16 win_in_row, UINT16 refresh_count, UINT16 eliminate_count, UINT16 challenge_count, LTMSEL last_finish_time, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM]);

	// zpy ���ӽ�ɫ����
	SBigNumber AddRoleExp(UINT64 add_exp);
	SBigNumber AddRoleExp(SBigNumber add_exp);

	//add by hxw 20151015 Rank����ͨ����ȡ
	int ReceiveRankReward(BYTE type, UINT16 rewardid, SGetRankRewardRes *pRoleSign, std::vector<SPropertyAttr>& emailAttachmentsVec, SReceiveRewardRefresh *pRefresh);
	//add 20151022
	BOOL IsGetStrongerReward(UINT16 rewardid);
	BOOL IsGetOnlineReward(UINT16 rewardid);
	int GetOnlineInfo(SOnlineInfo* info, std::vector<UINT16>& vonline);
	int GetStrongerInfo(std::vector<UINT16>& infov);
	int InputStrongerReward(UINT16 rewardid);
	int InputOnlineReward(UINT16 rewardid);
	int SetOnlineInfo(UINT32 id, UINT32 time, LTMSEL lasttime = 0);
	//end 20151015


};

inline void CalcEquipAttachAttrPoint(BYTE type, UINT32 val, SRoleAttrPoint& equipAttr)
{
	switch (type)
	{
	case ESGS_EQUIP_ATTACH_HP:			//hp
		equipAttr.u32HP += val;
		break;
	case ESGS_EQUIP_ATTACH_MP:			//mp
		equipAttr.u32MP += val;
		break;
	case ESGS_EQUIP_ATTACH_ATTACK:		//����
		equipAttr.attack += val;
		break;
	case ESGS_EQUIP_ATTACH_DEF:			//����
		equipAttr.def += val;
		break;
	case ESGS_EQUIP_ATTACH_CRIT:		//����
		equipAttr.crit += val;
		break;
	case ESGS_EQUIP_ATTACH_TENACITY:	//����
		equipAttr.tenacity += val;
		break;
	default:
		break;
	}
}

inline void CalcEquipGemAttrPoint(BYTE type, UINT32 val, SRoleAttrPoint& equipAttr)
{
	switch (type)
	{
	case ESGS_GEM_HP:			//hp
		equipAttr.u32HP += val;
		break;
	case ESGS_GEM_MP:			//mp
		equipAttr.u32MP += val;
		break;
	case ESGS_GEM_ATTACK:		//����
		equipAttr.attack += val;
		break;
	case ESGS_GEM_DEF:			//����
		equipAttr.def += val;
		break;
	case ESGS_GEM_CRIT:			//����
		equipAttr.crit += val;
		break;
	case ESGS_GEM_TENACITY:		//����
		equipAttr.tenacity += val;
		break;
	default:
		break;
	}
}

//����ռ������
enum EInstOccupyType
{
	EINST_OCCUPY_BY_NEW_ROLE = 1,		//���µ����ռ��
	EINST_OCCUPY_REFRESH_BY_SELF = 2,	//���Լ�ˢ��
	EINST_OUT_RANK = 3,					//����������
	EINST_FIRST_OCCUPY = 4				//�״�ռ��
};

//����ռ�췢���ʼ�
struct SInstOccupyEmail
{
	EInstOccupyType type;
	UINT16 instId;
	UINT32 occupierRoleId;		//ռ���߽�ɫid
	UINT32 occupierUserId;		//ռ�����˺�id
	std::string occupierNick;	//ռ���߽�ɫ�ǳ�
	UINT32 occupiedRoleId;		//��ռ���߽�ɫid
	UINT32 occupiedUserId;		//��ռ�����˺�id
	std::string occupiedNick;	//��ռ���߽�ɫ�ǳ�
};

#pragma pack(pop)

class RoleInfosObjAllocator
{
	RoleInfosObjAllocator(const RoleInfosObjAllocator&);
	RoleInfosObjAllocator& operator = (const RoleInfosObjAllocator&);

public:
	RoleInfosObjAllocator();
	~RoleInfosObjAllocator();

	SRoleInfos* NewRoleInfosObj();
	void DeleteRoleInfosObj(SRoleInfos* pInfo);

private:
#if USE_MEM_POOL
	CMemPoolObj<SRoleInfos> m_roleAttrPool;
#endif
};

void InitRoleInfosObjAllocator();
RoleInfosObjAllocator* GetRoleInfosObjAllocator();
void DestroyRoleInfosObjAllocator();

class AutoRoleInfosObj
{
public:
	AutoRoleInfosObj(SRoleInfos* pObj)
		: m_roleObj(pObj)
	{

	}

	AutoRoleInfosObj()
		: m_roleObj(NULL)
	{
		m_roleObj = GetRoleInfosObjAllocator()->NewRoleInfosObj();
	}

	~AutoRoleInfosObj()
	{
		if (m_roleObj != NULL)
		{
			GetRoleInfosObjAllocator()->DeleteRoleInfosObj(m_roleObj);
			m_roleObj = NULL;
		}
	}

	BOOL IsValid()
	{
		return m_roleObj != NULL ? true : false;
	}

	operator SRoleInfos*()
	{
		return m_roleObj;
	}

	SRoleInfos* operator -> ()
	{
		return m_roleObj;
	}

	SRoleInfos* Get()
	{
		return m_roleObj;
	}

	SRoleInfos* Drop()
	{
		SRoleInfos* pObj = m_roleObj;
		m_roleObj = NULL;
		return pObj;
	}

private:
	SRoleInfos* m_roleObj;
};

#endif