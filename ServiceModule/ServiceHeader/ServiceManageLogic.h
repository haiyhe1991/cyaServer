#ifndef __SERVICE_MANAGE_LOGIC_H__
#define __SERVICE_MANAGE_LOGIC_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "ServiceCommon.h"

/***********************************************************************************
							���߼�������������ݽṹ
***********************************************************************************/
#define MAX_UTF8_NICK_LEN			128		//�ǳ���󳤶�(utf8)
#define MAX_UTF8_EMAIL_CAPTION_LEN	256		//�ʼ�����(utf8)
#define MAX_UTF8_EMAIL_BODY_LEN		1024	//�ʼ�����(utf8)
#define MAX_HAS_DRESS_NUM			1000	//���ӵ��ʱװ����

#pragma pack(push, 1)

//��������
enum ESGSChestConditionType
{
	ESGS_CONDITION_CHEST_SENIOR = 1,		//�߼�����
	ESGS_CONDITION_CHEST_LOWER = 2,			//�ͼ�����
};

//������������
enum ESGSRewardConditionType
{
	ESGS_CONDITION_SURPLUS_TIME = 1,		//ʣ��ʱ��
	ESGS_CONDITION_RESIST_NUMBER,			//�ֵ�����
	ESGS_CONDITION_KILL_NUMBER,				//��ɱ����
	ESGS_CONDITION_HURT_EVERY,				//ÿ����˺�
	ESGS_CONDITION_RANKING,					//����
	ESGS_CONDITION_CHEST,					//����
	ESGS_CONDITION_LEVEL_GAP,				//�ȼ���
	ESGS_CONDITION_CP_GAP,					//ս����
	ESGS_CONDITION_LAST_STRAW,				//���һ��
	ESGS_CONDITION_HURT_EXTREMELY,			//�˺���ֱ�
	ESGS_CONDITION_HIGH_LADDER_RANK,		//��������
	ESGS_CONDITION_HIGH_LADDER_CONTINUOUS_VICTORY_COUNT,	//������ʤ����
	ESGS_CONDITION_MAN_MACHINE_TO_WAR,		//�˻���ս
	ESGS_CONDITION_ONE_ON_ONE,				//1v1
	ESGS_CONDITION_COMPETITIVE_RANKING,		//�������������ڵ��ڣ�
};

//��Ʒ����
enum ESGSPropType
{
	ESGS_PROP_EQUIP = 1,		//װ��
	ESGS_PROP_GEM = 2,			//��ʯ
	ESGS_PROP_MATERIAL = 3,		//����
	ESGS_PROP_FRAGMENT = 4,		//��Ƭ
	ESGS_PROP_DRESS = 5,		//ʱװ
	ESGS_PROP_GOLD = 6,			//��Ϸ��
	ESGS_PROP_TOKEN = 7,		//����
	ESGS_PROP_VIT = 8,			//����
	ESGS_PROP_EXP = 9,			//����
	ESGS_PROP_DROP_ID = 10,		//����ID
	ESGS_PROP_MAGIC = 11,		//ħ����
	ESGS_PROP_CHAIN_SOUL_FRAGMENT = 12, //������Ƭ
};

//װ������
enum ESGSEquipType
{  
	ESGS_EQUIP_WEAPON = 1,			// ���� 
	ESGS_EQUIP_ARMOR = 2,			// ����
	ESGS_EQUIP_HAND	= 3,			// ����
	ESGS_EQUIP_SHOES = 4,			// Ь�� 
	ESGS_EQUIP_RING	= 5,			// ��ָ 
	ESGS_EQUIP_NECKLACE	= 6			// ����
};

//װ��������������
enum ESGSEquipAttachType
{
	ESGS_EQUIP_ATTACH_HP = 1,			//hp
	ESGS_EQUIP_ATTACH_MP = 2,			//mp
	ESGS_EQUIP_ATTACH_ATTACK = 3,		//����
	ESGS_EQUIP_ATTACH_DEF = 4,			//����
	ESGS_EQUIP_ATTACH_CRIT = 5,			//����
	ESGS_EQUIP_ATTACH_TENACITY = 6,		//����
};

//װ��Ʒ������
enum ESGSEquipQualityType
{
	ESGS_EQUIP_QUALITY_WHITE = 1,	//��
	ESGS_EQUIP_QUALITY_BLUE = 2,	//��
	ESGS_EQUIP_QUALITY_PRUPLE = 3,	//��
	ESGS_EQUIP_QUALITY_ORANGE = 4,	//��
	ESGS_EQUIP_QUALITY_GOLD = 5,	//��
};

//��������
enum ESGSMaterialType
{
	ESGS_MATERIAL_STRENGTHEN_STONE = 31002,		//ǿ��ʯ(ǿ��)
	ESGS_MATERIAL_VEINS_STAR_STONE = 31001,		//����ʯ(����)
	ESGS_MATERIAL_BRILLIANT_STONE = 31102,		//�Ի�ʯ(���Ǹ���,��߳ɹ���)
	ESGS_MATERIAL_SHINE_STONE = 31101,			//��âʯ(���Ǹ���,������)
	ESGS_MATERIAL_REVIVE_COIN = 31801,			//�����
	ESGS_MATERIAL_VIT_POSION_SMALL = 31802,		//����ҩˮ(С)
	ESGS_MATERIAL_VIT_POSION_MID = 31803,		//����ҩˮ(��)
	ESGS_MATERIAL_VIT_POSION_BIG = 31804,		//����ҩˮ(��)
	ESGS_MATERIAL_SWEEP_SCROLL = 31805,			//ɨ������
	ESGS_MATERIAL_BACKPACK_EXTEND = 31806,		//������չ
	ESGS_MATERIAL_LUCKY_COIN = 31807,			//���˱�
};
//�Ƿ�����ҩˮ
#define IS_VIT_POSION(id)	(id == ESGS_MATERIAL_VIT_POSION_BIG || id == ESGS_MATERIAL_VIT_POSION_MID || id == ESGS_MATERIAL_VIT_POSION_SMALL)

//����Ŀ������
enum ESGSTaskTargetType
{
	ESGS_TASK_TARGET_PASS_STAGE = 1,			//ͨ��
	ESGS_TASK_TARGET_KILL_MONSTER = 2,			//ɱ��
	ESGS_TASK_TARGET_DESTROY_SCENE = 3,			//�ƻ�������
	ESGS_TASK_TARGET_FETCH_PROP = 4,			//��ȡ����
	ESGS_TASK_TARGET_LIMIT_TIME = 5,			//��ʱͨ��
	ESGS_TASK_TARGET_GOT_AWAY = 6,				//����
	ESGS_TASK_TARGET_ESCORT = 7,				//����
	ESGS_TASK_TARGET_FIND_NPC = 8,				//�ҵ�NPC
	ESGS_TASK_TARGET_PASS_NO_HURT = 9,			//����ͨ��
	ESGS_TASK_TARGET_PASS_NO_BLOOD = 10,		//��ʹ��Ѫƿͨ��
	ESGS_TASK_TARGET_PASS_NO_DIE = 11,			//����ͨ��
	ESGS_TASK_TARGET_BLOCK_TIMES = 12,			//�񵲶��ٴ�
	ESGS_TASK_TARGET_COMBO_NUM_REACH = 13,		//�ﵽ�㹻���������
	ESGS_TASK_TARGET_KEEP_BLOOD = 14,			//Ѫ�������ڶ�������/��ͨ�أ�25%/50%/75%)
	ESGS_TASK_TARGET_CASH = 15,					//�һ�
	ESGS_TASK_TARGET_COMPOSE = 16,				//�ϳ�
	ESGS_TASK_TARGET_STRENGTHEN = 17,			//ǿ��
	ESGS_TASK_TARGET_RISE_STAR = 18,			//����
	ESGS_TASK_TARGET_ROLE_LV_REACH	= 19,		//��ɫ�ȼ��ﵽ
	ESGS_TASK_TARGET_KILL_MONSTER_LIMIT_TIME = 20,	//��ʱɱ��
	ESGS_TASK_TARGET_FETCH_PROP_LIMIT_TIME = 21,	//��ʱ��ȡ���ߣ�������
	ESGS_TASK_TARGET_DESTROY_SCENE_LIMIT_TIME = 22,	//��ʱ�ƻ������������
	ESGS_TASK_TARGET_NO_USE_SPECIAL_SKILL = 23,		//��ʹ�����⼼��ͨ��
	ESGS_TASK_TARGET_KILL_ALL_MONSTER = 24,			//ɱ�����й�
	ESGS_TASK_TARGET_DESTROY_ALL_SCENE = 25,		//�ƻ����г�����
	ESGS_TASK_TARGET_PASS_STAGE_NUM = 26,			//ͨ��������ͨ����n��
	ESGS_TASK_TARGET_PASS_RAND_STAGE_NUM = 27,		//ͨ�������ͨ����n��
	ESGS_TASK_TARGET_LOGIN_RECEIVE_VIT = 28,		//��½��ȡ����
	ESGS_TASK_TARGET_OPEN_CHEST_BOX_NUM = 29,		//�ֶ���������
};

//ϵͳ��������
enum ESGSSystaskType
{
	ESGS_SYS_MAIN_TASK = 1,		//����
	ESGS_SYS_BRANCH_TASK = 2,	//֧��
	ESGS_SYS_DAILY_TASK = 3,	//�ճ�
	ESGS_SYS_REWARD_TASK = 4,	//�ͽ�
	ESGS_SYS_RAND_TASK = 5,		//���
};

//��������
// enum ESGSRewardType
// {
// 	ESGS_REWARD_GOLD = 0,		//��Ϸ��
// 	ESGS_REWARD_TOKEN = 1,		//����
// 	ESGS_REWARD_VIT = 2,		//����
// 	ESGS_REWARD_EXP = 3,		//����
// 	ESGS_REWARD_EQUIP = 4,		//װ��
// 	ESGS_REWARD_GEM = 5,		//��ʯ
// 	ESGS_REWARD_MATERIAL = 6,	//����
// 	ESGS_REWARD_FRAGMENT = 7,	//��Ƭ
// };

//��ʯ����
enum ESGSGemType
{
	ESGS_GEM_HP = 1,			//HP��ʯ
	ESGS_GEM_MP = 2,			//MP��ʯ
	ESGS_GEM_ATTACK = 3,		//������ʯ
	ESGS_GEM_DEF = 4,			//������ʯ
	ESGS_GEM_CRIT = 5,			//������ʯ
	ESGS_GEM_TENACITY = 6,		//���Ա�ʯ
};

//��ʯ�ױ��
enum ESGSGemHoleCode
{
	ESGS_GEM_HOLE_0 = 0,	//1�ſ�
	ESGS_GEM_HOLE_1 = 1,	//2�ſ�
	ESGS_GEM_HOLE_2 = 2,	//3�ſ�
	ESGS_GEM_HOLE_3 = 3,	//4�ſ�
};

#define IsValidGemHole(hole) ((hole) >= ESGS_GEM_HOLE_0 && (hole) <= ESGS_GEM_HOLE_3)

//װ����������
enum ESGSEquipAttrType
{
	ESGS_INLAID_GEM = 1,	//��Ƕ��ʯ
};

//��Ϸ����
enum ESGSGameMode
{
	ESGS_GAME_CHALLENGE = 1,	//��ս
	ESGS_GAME_EXPLORE = 2,		//̽��
	ESGS_GAME_SPEED = 3,		//����
};

//��������
enum ESGSInstMode
{
	ESGS_INST_MAIN = 1,			//����
	ESGS_INST_BRANCH = 2,		//֧��
	ESGS_INST_FIND_CHEST = 3,	//�ұ��䣨���
	ESGS_INST_GUARD_ICON = 4,	//����ʥ�񣨻��
	ESGS_INST_WORLD_BOSS = 5,	//����Boss
	ESGS_INST_MAN_MACHINE_TO_WAR = 6,	//�˻���ս
	ESGS_INST_ONE_VS_ONE = 7,			//1v1
	ESGS_INST_HIGH_LADDER = 8,			//����
	ESGS_INST_TEAM = 9,					//��Ӹ���
	ESGS_INST_TRAINING = 10,			//��ϰģʽ
	ESGS_INST_ACTIVITY_INST = 11,		//����������
	ESGS_INST_COMPETITIVEMODE = 12,		//����ģʽ
};


//ϵͳ��������
enum ESGSNoticeType
{
	ESGS_NOTICE_PERSONAL_RAND_INST_OPEN = 1,	//���������������
	ESGS_NOTICE_PERSONAL_RAND_INST_CLOSE = 2,	//������������ر�
	ESGS_NOTICE_FRAGMENT_COMPOSE = 3,			//��Ƭ�ϳ�
	ESGS_NOTICE_SYSTEM = 4,						//ϵͳ����
	ESGS_NOTICE_WORLDBOSS_END = 5,				//����Boss����
};

//������
enum ESGSMonsterType
{
	ESGS_MONSTER_COMMON = 1,		//��ͨ
	ESGS_MONSTER_CREAM = 2,			//��Ӣ
	ESGS_MONSTER_SMALL_BOSS = 3,	//Сboss
	ESGS_MONSTER_BIG_BOSS = 4,		//��boss
	ESGS_MONSTER_RAGE = 5,			//��
	ESGS_MONSTER_MEAT = 6,			//���
	ESGS_MONSTER_WORLDBOSS = 10,	//����Boss
};

//�ʼ�����
enum ESGSEmailType
{
	ESGS_EMAIL_SYSTEM = 0,			//ϵͳ�ʼ�
	ESGS_EMAIL_INST_DROP = 1,		//��������
	ESGS_EMAIL_TASK_REWARD = 2,		//������
	ESGS_EMAIL_FIRST_RECHARGE = 3,	//�׳�����
	ESGS_EMAIL_VIP_REWARD = 4,		//VIP����
	ESGS_EMAIL_SIGN_IN_REWARD = 5,	//ǩ������
	ESGS_EMAIL_ACTIVITY_REWARD = 6,	//�����
	ESGS_EMAIL_GIFTBAG_REWARD = 7,	//�������
	ESGS_EMAIL_NEW_ACTOR_REWARD = 8,	//�½�ɫ����
	ESGS_EMAIL_EXTRACT_REWARD = 9,	//�齱
	ESGS_EMAIL_RECHARGE_RETURN = 10,	//��ֵ����
	ESGS_EMAIL_WORLD_BOSS_REWARD = 11,	//����Boss����
	ESGS_EMAIL_ACHIEVEMENT_REWARD = 12,	//�ɾͽ���
	ESGS_EMAIL_LIMITEDTIME_ACTIVITY_REWARD = 13,	//��ʱ�����
	ESGS_EMAIL_LADDER_REWARD = 14,	//���ݽ���
	ESGS_EMAIL_COMPETITIVE_REWARD = 15,	//����ģʽ����
};

//װ���ϳ�������������
enum ESGSEquipComposeConditionType
{
	ESGS_EQUIP_COMPOSE_ROLE_LEVEL = 1,		//��ɫ�ȼ�
	ESGS_EQUIP_COMPOSE_DATE_TIME = 2,		//ʱ��
	ESGS_EQUIP_COMPOSE_MAGIC = 3,			//ħ���� 
	ESGS_EQUIP_COMPOSE_PROP = 4,			//��Ʒ
};

//��������
enum ESGSUnlockType
{
	ESGS_UNLOCK_ROLE_JOB = 1,	//��ɫְҵ
	ESGS_UNLOCK_STAGE = 2,		//�ؿ�
	ESGS_UNLOCK_SKILL = 3,		//����
	ESGS_UNLOCK_LEVEL = 4,		//�ȼ�
};

//��ֵ״̬
enum ESGSRechargeStatus
{
	ESGS_WAIT_CONFIRM = 0,	//�ȴ�ȷ��
	ESGS_SYS_CONFIRMED = 1,	//ϵͳȷ��
	ESGS_MANUAL_CONFIRMED = 2,	//�ֶ�ȷ��
};

//�������Ե�
struct SRoleAttrPoint
{
	UINT32 u32HP;
	UINT32 u32MP;
	UINT32 crit;			//����
	UINT32 tenacity;		//����
	UINT32 attack;			//����
	UINT32 def;				//����

	SRoleAttrPoint()
	{
		u32HP = 0;
		u32MP = 0;
		crit = 0;			//����
		tenacity = 0;		//����
		attack = 0;	//Ԫ�ع���
		def = 0;	//Ԫ�ؿ���
	}
};

//����/������������
enum ESGSConsumeProduceType
{
	ESGS_GOLD = 0,
	ESGS_TOKEN = 1
};

struct SGSPosition	//λ��
{
	UINT16 owncopy;		//���ڸ���;
	BYTE   monsterArea;	//ˢ�������
	BYTE   reserve[13];	//����
	void hton()
		{	owncopy = htons(owncopy);	}
	void ntoh()
		{	owncopy = ntohs(owncopy);	}
};

struct SBasicAttr	//��Ʒ��������
{
	UINT16 valType;	//����ֵ����;
	UINT32 val;		//����ֵ;
	BYTE   type;	//��������;
	BYTE   source;	//������Դ;
	void hton()
	{	
		val = htonl(val);
		valType = htons(valType);
	}

	void ntoh()
	{	
		val = ntohl(val);
		valType = ntohs(valType);
	}
};

//װ��λ����
#define MAX_GEM_HOLE_NUM 4
struct SEquipPosAttr
{
	BYTE pos;		//װ��λ
	BYTE lv;		//ǿ���ȼ�
	BYTE star;		//�Ǽ�
	UINT16 gem[MAX_GEM_HOLE_NUM];	//��Ƕ��ʯid

	void hton()
	{
		for(int i = 0; i < MAX_GEM_HOLE_NUM; ++i)
			gem[i] = htons(gem[i]);
	}
	
	void ntoh()
	{
		for(int i = 0; i < MAX_GEM_HOLE_NUM; ++i)
			gem[i] = ntohs(gem[i]);
	}
};

struct SMonsterDropPropItem	//������Ʒ��Ϣ
{
	BYTE id;		//���
	BYTE type;		//����(ESGSPropType)
	UINT16 propId;	//��Ʒid
	UINT32 num;		//��Ʒ����
	
	void hton()
	{
		propId = htons(propId);
		num = htonl(num);
	}
	void ntoh()
	{
		propId = ntohs(propId);
		num = ntohl(num);
	}
};

struct SBoxDropPropItem
{
	BYTE id;		//��Ʒ���
	BYTE type;		//����(ESGSPropType)
	UINT16 propId;	//��Ʒid
	UINT32 num;		//��Ʒ����
	void hton()
	{
		propId = htons(propId);
		num = htonl(num);
	}
	void ntoh()
	{
		propId = ntohs(propId);
		num = ntohl(num);
	}
};

struct SPropertyItem
{
	UINT16 propId;	//��ƷID;
	BYTE   type;	//��Ʒ���� ESGSPropType
	UINT32 num;		//��Ʒ����

	void hton()
	{
		num = htonl(num);
		propId = htons(propId);
	}
	void ntoh()
	{
		num = ntohl(num);
		propId = ntohs(propId);
	}
};

//#define MAX_PROP_ATTR_NUM			10	//�������Ը���
struct SPropertyAttr		//-��������
{
	UINT32 id;		//ΨһID;
	UINT16 propId;	//��ƷID;
	BYTE   type;	//��Ʒ���� ESGSPropType
	BYTE   suitId;  //��װid;(װ�����е�����)
	BYTE   level;	//��Ʒ�ȼ�;(��ʯ,װ�����е�����)
	BYTE   grade;	//��ƷƷ��(��type==ESGS_PROP_GEM,���ʾ��ʯ����)
	BYTE   star;	//��Ʒ�Ǽ�;װ�����е�����
	BYTE   num;		//��Ʒ����;
	BYTE   pos;		//��Ʒλ��(װ�����е�����, >0��ʾװ��λ��, ESGSEquipType)
	//SBasicAttr basicAttr[MAX_PROP_ATTR_NUM]; //����,��������[10];
	void hton()
	{	
		id = htonl(id);
		propId = htons(propId);
// 		for(int i = 0; i < MAX_PROP_ATTR_NUM; ++i)
// 			basicAttr[i].hton();
	}

	void ntoh()
	{	
		id = ntohl(id);
		propId = ntohs(propId);
// 		for(int i = 0; i < MAX_PROP_ATTR_NUM; ++i)
// 			basicAttr[i].ntoh();
	}
};

//װ����Ƭ
struct SEquipFragmentAttr
{
	UINT16 fragmentId;	//��Ƭid
	UINT16 num;			//����
	void hton()
	{
		fragmentId = htons(fragmentId);
		num = htons(num);
	}
	void ntoh()
	{
		fragmentId = ntohs(fragmentId);
		num = ntohs(num);
	}
};

//������Ƭ
struct SChainSoulFragmentAttr
{
	UINT32 fragmentId;	//��Ƭid
	UINT32 num;			//����
	void hton()
	{
		fragmentId = htons(fragmentId);
		num = htons(num);
	}
	void ntoh()
	{
		fragmentId = ntohs(fragmentId);
		num = ntohs(num);
	}
};

//������Ʒ
struct SConsumeProperty
{
	UINT32 id;	//Ψһid
	BYTE   num;	//����
	void hton()
		{	id = htonl(id);		}
	void ntoh()
		{	id = ntohl(id);		}
};

// ��ѯ������ɫ��Ϣ, MLS_QUERY_ROLES
struct SQueryRolesReq	//����
{
	UINT32 userId;	//�˻�ID
	void hton()
		{	userId = htonl(userId);	}

	void ntoh()
		{	userId = ntohl(userId);	}
};

struct SBufferInfo
{
	UINT16 id;
	UINT32 timeout;
	UINT32 beat;

	void hton()
	{
		id = htonl(id);
		timeout = htonl(timeout);
		beat = htonl(beat);
	}
	void ntoh()
	{
		id = ntohl(id);
		timeout = ntohl(timeout);
		beat = ntohl(beat);
	}
};

struct SQueryRoleAttr	//��ѯ��ɫ����
{
	UINT32 id;			//��ɫID;
	BYTE occuId;		//ְҵid;
	BYTE sex;			//�Ա�
	UINT16 dress;		//��ɫʱװ;
	UINT16 mainWeapon;	//��ɫ��������;
	UINT16 subWeapon;	//��ɫ��������;
	char   nick[33];	//�ǳ�;
	BYTE   level;		//�ȼ�;
	SBigNumber gold;	//��Ϸ��;
	UINT32	rpcoin;		//����;
	UINT32  cashs;		//��ֵ���
	SBigNumber exp;		//����;
	UINT32	cashcount;	//��ֵ�ܽ��
	UINT16	owncopy;	//���ڸ���;
	char lastEnterTime[SGS_DATE_TIME_LEN];	//���һ�ν�����Ϸʱ��(1970-01-01 00:00:00)

	void hton()
	{
		id = htonl(id);
		dress = htons(dress);
		mainWeapon = htons(mainWeapon);
		subWeapon = htons(subWeapon);
		rpcoin = htonl(rpcoin);
		cashs = htonl(cashs);
		cashcount = htonl(cashcount);
		owncopy = htons(owncopy);
	}

	void ntoh()
	{
		id = ntohl(id);
		dress = ntohs(dress);
		mainWeapon = ntohs(mainWeapon);
		subWeapon = ntohs(subWeapon);
		rpcoin = ntohl(rpcoin);
		cashs = ntohl(cashs);
		cashcount = ntohl(cashcount);
		owncopy = ntohs(owncopy);
	}
};

struct SQueryRolesRes	//��Ӧ
{
	BYTE rolesCount;	//��ɫ����
	SQueryRoleAttr roles[1]; //��ɫ��Ϣ;
};

//�����ɫ, MLS_ENTER_ROLE
struct SEnterRoleReq	//user->linker
{
	UINT32 roleId;	//��ɫid
	void hton()
		{	roleId = htonl(roleId);	}
	void ntoh()
		{	roleId = ntohl(roleId);	}
};

struct SSkillMoveInfo	//����/������Ϣ
{
	UINT16 id;		//����/���б��
	BYTE   key;		//�ͻ��˰�key(0-δ��,1-�Ѱ�)
	BYTE   level;	//���ܵȼ�

	void hton()
		{	id = htons(id);		}

	void ntoh()
		{	id = ntohs(id);		}
};

struct SEnterRoleRes
{
	UINT32 userId;	//�˻�ID;
	UINT32 roleId;	//��ɫID;
	char   nick[33];//�ǳ�[33];
	UINT16 titleId;	//�ƺ�id;
	BYTE   roleLevel;	//��ɫ�ȼ�
	SBigNumber gold;// ��Ϸ��
	SBigNumber exp;		//����
	UINT32 cash;		//�ֽ�
	BYTE occuId;	//ְҵid
	BYTE sex;		//�Ա�
	UINT32 cashcount;	//��ֵ�ܶ�
	UINT32 guildId;		//����ID
	UINT32 hp;			//����
	UINT32 mp;			//ħ��
	UINT32 vit;		//����
	UINT16 ap;			//ʣ�����Ե���(���ݿ�����int)
	UINT32 attack;       //����
	UINT32 def;          //����
	BYTE packSize;		//������С
	BYTE wareSize;		//�ֿ��С
	BYTE unReadEmails;	//δ���ʼ�����;
	UINT32 bt;			//��ǰս����;
	SGSPosition pos;	//����λ��
	BYTE factionId;	//��Ӫid
	BYTE skillNum;	//����/���и���
	SSkillMoveInfo data[1];	//����
	void hton()
	{	
		userId = htonl(userId);
		roleId = htonl(roleId);
		titleId = htons(titleId);
		cash = htonl(cash);
		cashcount = htonl(cashcount);
		guildId = htonl(guildId);
		hp = htonl(hp);
		mp = htonl(mp);
		vit = htonl(vit);
		ap = htons(ap);
		attack = htonl(attack);
		def = htonl(def);
		bt = htonl(bt);
		//unReadEmails = htons(unReadEmails);
		pos.hton();
	}
	void ntoh()
	{	
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		titleId = ntohs(titleId);
		cash = ntohl(cash);
		cashcount = ntohl(cashcount);
		guildId = ntohl(guildId);
		hp = ntohl(hp);
		mp = ntohl(mp);
		vit = ntohl(vit);
		ap = ntohs(ap);
		attack = ntohl(attack);
		def = ntohl(def);
		bt = ntohl(bt);
		pos.ntoh();
	}
};

//��ѯ����, MLS_QUERY_BACKPACK
struct SQueryBackpackReq	//--�����
{
	BYTE from;	//��ʼλ��;
	BYTE num;	//����;
};

struct SQueryBackpackRes	//--���ذ�
{
	BYTE total;		//��������������;
	BYTE curNum;	//��ǰ�Ż�����;
	SPropertyAttr props[1];	////array<����>     ���صĵ�����Ϣ;
};

//��ѯ��ɫ������Ϣ	MLS_QUERY_ROLE_APPEARANCE
struct SQueryRoleAppearanceReq
{
	UINT32 roleId;	//��ɫId
	void hton()
		{	roleId = htonl(roleId);	}

	void ntoh()
		{	roleId = ntohl(roleId);	}
};

struct SQueryRoleAppearanceRes
{
	UINT32 roleId;		//��ɫId
	char   nick[33];	//�ǳ�[33];
	BYTE   level;		//��ɫ�ȼ�
	UINT16 dressId;		//ʱװid
	UINT16 titleId;		//�����ƺ�id
	BYTE   num;			//�Ѿ�װ��װ������
	SPropertyAttr props[1];	//�Ѿ�װ��װ��;
	void hton()
	{
		roleId = htonl(roleId);
		dressId = htons(dressId);
		titleId = htons(titleId);
	}

	void ntoh()
	{
		roleId = ntohl(roleId);
		dressId = ntohs(dressId);
		titleId = ntohs(titleId);
	}
};

//��ѯ��ɫ��ϸ��Ϣ	MLS_QUERY_ROLE_EXPLICIT
struct SQueryRoleExplicitReq
{
	UINT32 roleId;	//��ɫId
	void hton()
		{	roleId = htonl(roleId);	}

	void ntoh()
		{	roleId = ntohl(roleId);	}
};

struct SQueryRoleExplicitRes
{
	UINT32 roleId;			//��ɫId
	char   roleNick[33];	//��ɫ�ǳ�[33];
	BYTE   roleLevel;		//��ɫ�ȼ�
	UINT16 titleId;		//�ƺ�id;
	SBigNumber gold;	//��Ϸ��
	UINT32 guildId;		//����ID;
	char   guildNick[33];	//��������[33];
	SBigNumber exp;		//����
	UINT32 cash;		//�ֽ�
	BYTE occuId;	//ְҵid
	BYTE sex;		//�Ա�
	UINT32 cashcount;	//��ֵ�ܶ�
	UINT32 hp;			//����
	UINT32 mp;			//ħ��
	UINT32 vit;		//����
	UINT16 ap;			//ʣ�����Ե���(���ݿ�����int)
	UINT32 attack;       //����
	UINT32 def;          //����
	BYTE packSize;		//������С
	BYTE wareSize;		//�ֿ��С
	BYTE unReadEmails;	//δ���ʼ�����;
	SGSPosition pos;	//����λ��
	UINT32 cp;			//��ǰս����;
	BYTE   num;			//��������
	SSkillMoveInfo skills[1];	//��������;
	void hton()
	{
		roleId = htonl(roleId);
		titleId = htons(titleId);
		cash = htonl(cash);
		cashcount = htonl(cashcount);
		guildId = htonl(guildId);
		hp = htonl(hp);
		mp = htonl(mp);
		vit = htonl(vit);
		ap = htons(ap);
		attack = htonl(attack);
		def = htonl(def);
		cp = htonl(cp);
		pos.hton();
	}
	void ntoh()
	{	
		roleId = ntohl(roleId);
		titleId = ntohs(titleId);
		cash = ntohl(cash);
		cashcount = ntohl(cashcount);
		guildId = ntohl(guildId);
		hp = ntohl(hp);
		mp = ntohl(mp);
		vit = ntohl(vit);
		ap = ntohs(ap);
		attack = ntohl(attack);
		def = ntohl(def);
		cp = ntohl(cp);
		pos.ntoh();
	}
};

//��ѯ�ɾ�, MLS_QUERY_ACHIEVEMENT
struct SQueryAchievementReq	//����
{
	BYTE type;								//1�ɳ�֮·��2ð�����̣�3��������ID
	UINT16 start_pos;						//��ʼ��ѯλ��

	void hton()
	{
		start_pos = htons(start_pos);
	}

	void ntoh()
	{
		start_pos = ntohs(start_pos);
	}
};

struct SAchievementInfo		//�ɾ���Ϣ
{
	UINT32 id;								//�ɾ�ID
	UINT32 point;							//��ɵ���
	BYTE status;							//�ɾ�״̬��1��ʾ���ڽ��У�2��ʾ�Ѵ�ɣ�3��ʾ����ȡ

	void hton()
	{
		id = htonl(id);
		point = htonl(point);
	}

	void ntoh()
	{
		id = ntohl(id);
		point = ntohl(point);
	}
};

struct SQueryAchievementRes		//����
{
	bool done;								//�Ƿ��������������
	UINT32 point;							//�ɾ͵���
	UINT16 num;								//�ɾ�����
	SAchievementInfo data[1];				//�ɾ���Ϣ

	void hton()
	{
		for(UINT16 i = 0; i < num; ++i)
			data[i].hton();
		num = htons(num);
		point = htonl(point);
	}

	void ntoh()
	{
		num = ntohs(num);
		point = ntohl(point);
		for(UINT16 i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//������ɫ, MLS_CREATE_ROLE
struct SCreateRoleReq	//����
{
	BYTE occuId;		//ְҵid;
	BYTE sex;			//�Ա�
	char nick[33];	//�����ǳ�[33];
};

struct SCreateRoleRes	//�ظ�
{
	UINT32 id;			//��ɫID;
	BYTE occuId;		//ְҵid;
	BYTE sex;			//�Ա�
	UINT16 dress;		//��ɫʱװ;
	UINT16 mainWeapon;		//��ɫ��������;
	UINT16 subWeapon;		//��ɫ��������;
	char   nick[33];	//�ǳ�[33];
	BYTE   level;		//�ȼ�;
	SBigNumber gold;	//��Ϸ��;
	UINT32	rpcoin;		//����;
	SBigNumber exp;		//����;
	UINT16 owncopy;		//���ڸ���;

	void hton()
	{	
		id = htonl(id);
		dress = htons(dress);
		mainWeapon = htons(mainWeapon);
		subWeapon = htons(subWeapon);
		rpcoin = htonl(rpcoin);
		owncopy = htons(owncopy);
	}

	void ntoh()
	{	
		id = ntohl(id);
		dress = ntohs(dress);
		mainWeapon = ntohs(mainWeapon);
		subWeapon = ntohs(subWeapon);
		rpcoin = ntohl(rpcoin);
		owncopy = ntohs(owncopy);
	}
};

//��ѯ����, MLS_QUERY_FRIEND
struct SQueryFriendsReq
{
	BYTE from;	//��ʼλ��;
	BYTE num;//����;
}; 

struct SFriendInfo	//������Ϣ
{
	UINT32 userId;		//�˻�ID;
	UINT32 roleId;		//��ɫID;
	BYTE   occuId;		//ְҵid;
	BYTE   sex;			//�Ա�
	char   roleNick[33];//��ɫ�ǳ�[33];
	BYTE   roleLevel;	//��ɫ�ȼ�;
	UINT32 guildId;		//���ڹ���ID;
	char   guildName[33];	//���ڹ�������[33];
	UINT32 cp;				//ս��
	BYTE   online;		//�Ƿ�����;
	void hton()
	{	
		userId = htonl(userId);
		roleId = htonl(roleId);
		guildId = htonl(guildId);
		cp = htonl(cp);
	}

	void ntoh()
	{	
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		guildId = ntohl(guildId);
		cp = ntohl(cp);
	}
};

struct SQueryFriendsRes
{
	BYTE total;				//��������;
	BYTE num;				//���غ�����Ϣ��;
	SFriendInfo friends[1];	//�����б�;
};

//��ѯ��������, MLS_QUERY_FRIEND_APPLY
struct SQueryFriendApplyReq
{
	BYTE from;	//��ʼλ��;
	BYTE num;	//����;
};

struct SQueryFriendApplyRes
{
	BYTE total;				//��������;
	BYTE num;				//���غ�����Ϣ��;
	SFriendInfo data[1];	//���������б�;
};

//��Ӻ���, MLS_ADD_FRIEND
struct SAddFriendReq
{
	char nick[33];	//Ŀ������ǳ�[33];
};

//��������/ͬ��/�ܾ�֪ͨ, MLS_FRIEND_APPLY_NOTIFY/MLS_AGREE_FRIEND_APPLY_NOTIFY/MLS_REFUSE_FRIEND_APPLY_NOTIFY(����������)
struct SFriendApplyNotify
{
	SFriendInfo friendInfo;
	void hton()
		{	friendInfo.hton();	}

	void ntoh()
		{	friendInfo.ntoh();	}
};

//��������ȷ��, MLS_FRIEND_APPLY_CONFIRM
struct SFriendApplyConfirmReq
{
	UINT32 friendUserId;	//�����˺�id
	UINT32 friendRoleId;	//���ѽ�ɫid
	BYTE   status;			//0-�ܾ�, 1-ͬ��
	void hton()
	{	
		friendUserId = htonl(friendUserId);
		friendRoleId = htonl(friendRoleId);
	}

	void ntoh()
	{	
		friendUserId = ntohl(friendUserId);
		friendRoleId = ntohl(friendRoleId);
	}
};

//��ȡ�Ƽ������б�, MLS_GET_RECOMMEND_FRIEND_LIST
struct SGetRecommendFriendsRes
{
	BYTE num;				//���غ�����Ϣ��;
	SFriendInfo data[1];	//���������б�;
};

//�������, MLS_SEARCH_PLAYER
struct SPlayerSearchReq
{
	char playerNick[33];
};

struct SPlayerSearchRes
{
	SFriendInfo player;
	void hton()
		{	player.hton();	}
	void ntoh()
		{	player.ntoh();	}
};

//ɾ������, MLS_DEL_FRIEND
struct SDelFriendReq	//����
{
	char nick[33];	//���ѽ�ɫ�ǳ�[33];
};

//����������֪ͨ, MLS_FRIEND_ONLINE_NOTIFY/MLS_FRIEND_OFFLINE_NOTIFY(����������)
struct SFriendOnOffLineNotify
{
	UINT32 friendRoleId;
	void hton()
		{	friendRoleId = htonl(friendRoleId);		}
	void ntoh()
		{	friendRoleId = ntohl(friendRoleId);		}
};

//����ɾ��֪ͨ, MLS_FRIEND_DEL_NOTIFY(����������)
struct SFriendDelNotify
{
	UINT32 friendRoleId;
	void hton()
		{	friendRoleId = htonl(friendRoleId);		}
	void ntoh()
		{	friendRoleId = ntohl(friendRoleId);		}
};

//��ѯ�ѽ�������, MLS_QUERY_ACCEPTED_TASK
#define MAX_TASK_TARGET_NUM	5
struct SAcceptTaskInfo
{
	UINT16 taskId;		//����id
	BYTE   isFinished;	//�����Ƿ����(1-���, 0-δ���)
	UINT32 target[MAX_TASK_TARGET_NUM];

	void hton()
	{
		taskId = htons(taskId);
		for(BYTE i = 0; i < MAX_TASK_TARGET_NUM; ++i)
			target[i] = htonl(target[i]);
	}

	void ntoh()
	{	
		taskId = ntohs(taskId);
		for(BYTE i = 0; i < MAX_TASK_TARGET_NUM; ++i)
			target[i] = ntohl(target[i]);
	}
};

struct SQueryAcceptTasksRes
{
	BYTE num;					//��������;
	SAcceptTaskInfo tasks[1];	//����
};

//�����ѽ�����Ŀ��, MLS_UPDATE_TASK_TARGET(�ò����޷���)
struct SUpdateTaskTargetReq
{
	UINT16 taskId;		//����id
	UINT32 target[MAX_TASK_TARGET_NUM];

	void hton()
	{
		taskId = htons(taskId);
		for(BYTE i = 0; i < MAX_TASK_TARGET_NUM; ++i)
			target[i] = htonl(target[i]);
	}
	void ntoh()
	{
		taskId = ntohs(taskId);
		for(BYTE i = 0; i < MAX_TASK_TARGET_NUM; ++i)
			target[i] = ntohl(target[i]);
	}
};

//��������, MLS_ACCEPT_TASK
struct SAcceptTaskReq
{
	UINT16 taskId;	//����id
	void hton()
	{	taskId = htons(taskId);	}

	void ntoh()
	{	taskId = ntohs(taskId);	}
};

struct SAcceptTaskRes
{
	UINT16 taskId;	//����id
	void hton()
		{	taskId = htons(taskId);	}

	void ntoh()
		{	taskId = ntohs(taskId);	}
};

//�������, MLS_COMMIT_TASK_FINISH(�ôβ����޷���)
struct SFinishTaskReq
{
	UINT16 taskId;	//����id
	void hton()
		{	taskId = htons(taskId);		}

	void ntoh()
		{	taskId = ntohs(taskId);		}
};

//�������ʧ��֪ͨ, MLS_RECEIVE_FIN_TASK_FAIL_NOTIFY
struct SFinishTaskFailNotify
{
	UINT16 taskId;		//����id;
	UINT32 target[MAX_TASK_TARGET_NUM];	//����Ŀ�굱ǰ״̬
	void hton()
	{
		taskId = htons(taskId);
		for(BYTE i = 0; i < MAX_TASK_TARGET_NUM; ++i)
			target[i] = htonl(target[i]);
	}

	void ntoh()
	{
		taskId = ntohs(taskId);
		for(BYTE i = 0; i < MAX_TASK_TARGET_NUM; ++i)
			target[i] = ntohl(target[i]);
	}
};

//��ȡ������, MLS_RECEIVE_TASK_REWARD
struct SReceiveTaskRewardReq
{
	UINT16 taskId;		//����id;
	void hton()
		{	taskId = htons(taskId);		}

	void ntoh()
		{	taskId = ntohs(taskId);		}
};

struct SReceiveTaskRewardRes
{
	UINT16 taskId;		//����id;
	SBigNumber exp;		//��ӵ���ܵľ���(��ǰӵ��+��������)
	SBigNumber gold; 	//��ӵ���ܵ���Ϸ��(��ǰӵ��+��������)
	UINT32 rpcoin;     	//��ӵ���ܵĴ���(��ǰӵ��+��������)
	BYTE isUpgrade;		//��ɫ�Ƿ�����(0-��, 1-��)
	BYTE modifyNum;	   	//�������µ�������; 
	SPropertyAttr modifyProps[1];	//array<����>  �����޸���Ϣ;
	void hton()
	{	
		taskId = htons(taskId);
		rpcoin = htonl(rpcoin);	
	}

	void ntoh()
	{	
		taskId = ntohs(taskId);
		rpcoin = ntohl(rpcoin);
	}
};

//��������, MLS_DROP_TASK
struct SDropTaskReq
{
	UINT16 taskId;	//����id
	void hton()
		{	taskId = htons(taskId);	}

	void ntoh()
		{	taskId = ntohs(taskId);	}
};

struct SDropTaskRes
{
	UINT16 taskId;	//����id
	void hton()
		{	taskId = htons(taskId);	}

	void ntoh()
		{	taskId = ntohs(taskId);	}
};

//��ѯ�����������ʷ��¼, MLS_QUERY_FIN_TASK_HISTORY
struct SQueryFinishedTaskReq
{
	UINT16 from;	//�Ӷ��ٿ�ʼ
	UINT16 num;		//�������

	void hton()
	{
		from = htons(from);
		num = htons(num);
	}
	void ntoh()
	{
		from = ntohs(from);
		num = ntohs(num);
	}
};

struct SQueryFinishedTaskRes
{
	UINT16 num;
	UINT16 finTaskIds[1];

	void hton()
		{	num = htons(num);	}

	void ntoh()
		{	num = ntohs(num);	}
};

//��ȡ�ճ�����
struct SGetDailyTaskRes
{
	BYTE num;
	UINT16 dailyTasks[1];
};

//���ĳ��������, MLS_FINISH_ROLE_HELP
struct SFinishHelpReq
{
	UINT16 helpId;	//����id

	void hton()
		{	helpId = htons(helpId);		}

	void ntoh()
		{	helpId = ntohs(helpId);		}
};

//��ѯ���������������Ϣ, MLS_QUERY_FINISHED_HELP
struct SQueryFinishedHelpRes
{
	UINT16 num;			//����id����;
	UINT16 data[1];	//��������ID;

	void hton()
	{
		for(UINT16 i = 0; i < num; ++i)
			data[i] = htons(data[i]);

		num = htons(num);
	}
	void ntoh()
	{
		num = ntohs(num);
		for(UINT16 i = 0; i < num; ++i)
			data[i] = ntohs(data[i]);
	}
};

//��ѯ����ɸ�����Ϣ, MLS_QUERY_FINISHED_INST
struct SQueryFinishedInstReq
{
	UINT16 from;	//��ʼλ��;
	BYTE   num; 	//��ѯ����;
	void hton()
		{	from = htons(from);	}

	void ntoh()
		{	from = ntohs(from);	}
};

struct SFinishedInstInfo//��ɸ�����ʷ��Ϣ
{
	UINT32	id;			//��¼ID;
	UINT16  instId;		//����ID;
	BYTE    star;		//��������;
	UINT16  complatedNum;		//������ɴ���
	void hton()
	{	
		id = htonl(id);
		instId = htons(instId);
		complatedNum = htons(complatedNum);
	}

	void ntoh()
	{	
		id = ntohl(id);
		instId = ntohs(instId);
		complatedNum = ntohs(complatedNum);
	}
};

struct SQueryFinishedInstRes
{
	UINT16 total;		//����;
	BYTE   curNum; 		//��ǰ����;
	SFinishedInstInfo instInfos[1];	//array<��ɸ�����Ϣ>   ������Ϣ;
	void hton()
		{	total = htons(total);	}

	void ntoh()
		{	total = ntohs(total);	}
};

//�������Ե�, MLS_SAVE_ROLE_ATTR
struct SSaveRoleAttrValReq
{
	UINT16 str;				//����
	UINT16 dex;				//����
	UINT16 intelligence;	//�ǻ�
	UINT16 sp;				//����
	void hton()
	{	
		str = htons(str);
		dex = htons(dex);
		intelligence = htons(intelligence);
		sp = htons(sp);
	}

	void ntoh()
	{	
		str = ntohs(str);
		dex = ntohs(dex);
		intelligence = ntohs(intelligence);
		sp = ntohs(sp);
	}
};

struct SSaveRoleAttrValRes
{
	UINT32 hp;		//����
	UINT32 mp;			//ħ��
	void hton()
	{	
		hp = htonl(hp);
		mp = htonl(mp);
	}

	void ntoh()
	{	
		hp = ntohl(hp);
		mp = ntohl(mp);	
	}
};

//������Ʒ, MLS_SELL_PROPERTY
struct SSellPropertyReq
{
	UINT32 id;	//��ƷΨһID;
	BYTE   num;	//��Ʒ����;
	void hton()
		{	id = htonl(id);	}

	void ntoh()
		{	id = ntohl(id);	}
};

struct SSellPropertyRes
{
	UINT32 sellId;		//���ۼ�¼id
	SBigNumber gold;	//��ǰ�������;
	char szSellDate[SGS_DATE_TIME_LEN];	//����ʱ��
	BYTE num;		//�������ӱ仯����
	SPropertyAttr data[1];	//�������ӱ仯����
	void hton()
		{	sellId = htonl(sellId);		}

	void ntoh()
		{	sellId = ntohl(sellId);		}
};

//�һ�����, MLS_CASH_PROPERTY
struct SCashPropertyReq
{
	UINT16 targetId;	//�һ�Ŀ��ID;
	void hton()
		{	targetId = htons(targetId);	}
	void ntoh()
		{	targetId = ntohs(targetId);	}
};

struct SCashPropertyRes
{
	UINT16 targetId;	//�һ�Ŀ��ID;
	BYTE   packNum;		//����������;
	SPropertyAttr packs[1];	//array<����> �������µĵ�����Ϣ;

	void hton()
		{	targetId = htons(targetId);	}
	void ntoh()
		{	targetId = ntohs(targetId);	}
};

//��������, MLS_DROP_PROPERTY
struct SDropPropertyReq
{
	BYTE from;     //0-����, 1-�ֿ�
	UINT32 id;     //����ΨһID;
	BYTE num; 	   //������������;
	void hton()
		{	id = htonl(id);	}

	void ntoh()
		{	id = ntohl(id);	}
};

//���򱳰���ֿ�洢�ռ�, MLS_BUY_STORAGE_SPACE
struct SBuyStorageSpaceReq
{
	BYTE buyType;   //0.����  1.�ֿ�
	BYTE num;		//���������		
};

struct SBuyStorageSpaceRes
{
	BYTE buyType;   //0.����  1.�ֿ�
	UINT32 spend;	// ���������ʽ�(���һ���Ϸ��);
	BYTE newNum;	// ����󱳰���ֿ�����;
	void hton()
		{	spend = htonl(spend);	}

	void ntoh()
		{	spend = ntohl(spend);	}
};

//���븱��, MLS_ENTER_INST
struct SEnterInstReq
{
	UINT16 instId;	// ����ID;
	void hton()
		{	instId = htons(instId);	}

	void ntoh()
		{	instId = ntohs(instId);	}
};

struct SEnterInstRes
{
	UINT32 restVit;		//ʣ������
	BYTE   boxNum;		//��������
	BYTE   brokenNum;	//�������������
	char   data[1];		//��������, (����ID(BYTE) + ������Ʒ����(BYTE) + n * sizeof(SBoxDropPropItem)) * boxNum  + brokenNum * sizeof(SMonsterDropPropItem)
	void hton()
		{	restVit = htonl(restVit);	}

	void ntoh()
		{	restVit = ntohl(restVit);	}
};

//��ȡ�����������, MLS_FETCH_INST_AREA_DROPS
struct SFetchInstAreaDropsReq
{
	UINT16 instId;	//����ID;
	BYTE   areaId;	//����id
	void hton()
		{	instId = htons(instId);	}

	void ntoh()
		{	instId = ntohs(instId);	}
};

struct SFetchInstAreaDropsRes
{
	BYTE   monsterDrops;	//С�ֵ�������
	BYTE   creamDrops;		//��Ӣ�ֵ�������
	BYTE   bossDrops;		//boss�ֵ�������
	SMonsterDropPropItem data[1];	//��������(monsterDrops + creamDrops + bossDrops��SMonsterDropPropItem)
};

//ʰȡ�����ֵ���, MLS_PICKUP_INST_MONSTER_DROP
//��MLS_PICKUP_INST_MONSTER_DROP�����뷵��, ���ܻᴥ����ɫ���ݸ���֪ͨ
struct SPickupInstMonsterDropReq
{
	UINT16 instId;	//����id
	BYTE   areaId;	//����id
	BYTE   dropId;	//������(SMonsterDropPropItem:id)
	void hton()
		{	instId = htons(instId);		}
	void ntoh()
		{	instId = ntohs(instId);		}
};

//ʰȡ��������, MLS_PICKUP_INST_BOX
//��MLS_PICKUP_INST_BOX�����뷵��, ���ܻᴥ����ɫ���ݸ���֪ͨ
struct SPickupInstBoxReq
{
	UINT16 instId;	//����id
	BYTE   boxId;	//����id
	BYTE   id;		//��������Ʒ���
	void hton()
		{	instId = htons(instId);		}
	void ntoh()
		{	instId = ntohs(instId);		}
};

//ʰȡ���������, MLS_PICKUP_BROKEN_DROP
//��MLS_PICKUP_BROKEN_DROP�����뷵��, ���ܻᴥ����ɫ���ݸ���֪ͨ
struct SPickupBrokenDropReq
{
	UINT16 instId;	//����id
	BYTE   dropId;	//������(SMonsterDropPropItem:id)
	void hton()
		{	instId = htons(instId);		}
	void ntoh()
		{	instId = ntohs(instId);		}
};

//ɱ�ֵþ���, MLS_KILL_MONSTER_EXP
struct SKillMonsterExpReq
{
	UINT16 instId;		//����id
	BYTE   areaId;		//����id
	UINT16 monsterId;	//��id
	BYTE   monsterLv;	//�ֵȼ�

	void hton()
	{
		instId = htons(instId);
		monsterId = htons(monsterId);
	}
	void ntoh()
	{
		instId = ntohs(instId);
		monsterId = ntohs(monsterId);
	}
};

//��ɸ���, MLS_FINISH_INST
struct SChestItem
{
	BYTE type;			//��������
	UINT16 num;			//��������

	void hton()
	{
		num = htons(num);
	}
	void ntoh()
	{
		num = ntohs(num);
	}
};

struct SFinishInstReq
{
	UINT16 instId;	//����ID
	UINT32 score;	//����
	BYTE   star;	//����
	UINT16 finCode;	//�����(0-�ɹ���1-ƽ�֣�����-ʧ��)

	/* zpy �ɾ�ϵͳ���� */
	UINT32 continuous;		//��������
	bool speed_evaluation;	//�ٶ�����
	bool hurt_evaluation;	//�˺�����

	/* zpy ��ʱ����� */
	UINT32 surplus_time;	//ʣ��ʱ��
	UINT32 resist_number;	//�ֵ�����
	UINT32 kill_number;		//��ɱ��������
	INT32 level_gap;		//�ȼ���
	INT32 exp_gap;			//�����
	INT32 cp_gap;			//ս����
	UINT32 hurt_sum;		//�˺�����
	BYTE chest_num;			//��������
	SChestItem data[1];		//������Ϣ

	void hton()
	{	
		instId = htons(instId);
		score = htonl(score);
		finCode = htons(finCode);

		/* zpy �ɾ�ϵͳ���� */
		continuous = htonl(continuous);

		/* zpy ��ʱ����� */
		surplus_time = htonl(surplus_time);
		resist_number = htonl(resist_number);
		kill_number = htonl(kill_number);
		level_gap = htonl(level_gap);
		exp_gap = htonl(exp_gap);
		cp_gap = htonl(cp_gap);
		hurt_sum = htonl(hurt_sum);
		for(BYTE i = 0; i < chest_num; ++i)
			data[i].hton();
	}

	void ntoh()
	{
		instId = ntohs(instId);
		score = ntohl(score);
		finCode = ntohs(finCode);

		/* zpy �ɾ�ϵͳ���� */
		continuous = ntohl(continuous);

		/* zpy ��ʱ����� */
		surplus_time = ntohl(surplus_time);
		resist_number = ntohl(resist_number);
		kill_number = ntohl(kill_number);
		level_gap = ntohl(level_gap);
		exp_gap = ntohl(exp_gap);
		cp_gap = ntohl(cp_gap);
		hurt_sum = ntohl(hurt_sum);
		for(BYTE i = 0; i < chest_num; ++i)
			data[i].ntoh();
	}
};

struct SFinishInstRes
{
	UINT32 rpcoin;			//����
	UINT32 vit;				//����
	SBigNumber exp;			//����
	SBigNumber gold;		//���
	BYTE   num;				//����������
	SPropertyAttr data[1];	//������������

	void hton()
	{
		rpcoin = htonl(rpcoin);
		vit = htonl(vit);
		for(BYTE i = 0; i < num; ++i)
			data[i].hton();
	}

	void ntoh()
	{
		rpcoin = ntohl(rpcoin);
		vit = ntohl(vit);
		for(BYTE i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//����ɨ��, MLS_INST_SWEEP
struct SInstSweepReq
{
	UINT16 instId;	//����id
	void hton()
		{	instId = htons(instId);	}
	void ntoh()
		{	instId = ntohs(instId);	}
};

struct SInstSweepRes
{
	UINT32 remainVit;		//ʣ��������;
	SBigNumber exp;			//��ӵ���ܵľ���(��ǰӵ��+��������)
	SBigNumber gold; 		//��ӵ�е���Ϸ��;
	UINT32 rpcoin;        	//��ӵ�еĴ���;
	BYTE num;	   			//�������ӱ仯����; 
	SPropertyAttr data[1];	//�������ӱ仯;
	void hton()
	{	
		rpcoin = htonl(rpcoin);
		remainVit = htonl(remainVit);
		for(BYTE i = 0; i < num; ++i)
			data[i].hton();
	}
	void ntoh()
	{	
		rpcoin = ntohl(rpcoin);
		remainVit = ntohl(remainVit);
		for(BYTE i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//��ѯ�ʼ��б�, MLS_QUERY_EMAILS_LIST
struct SQueryEmailListReq
{
	UINT16 from;	//��ʼλ��;
	BYTE num;	//����;
	void hton()
		{	from = htons(from);	}
	void ntoh()
		{	from = ntohs(from);	}
};

struct SEmailListGenInfo
{
	UINT32 emailId;			//�ʼ�id
	BYTE   type;			//�ʼ�����
	BYTE   isRead;			//�Ƿ��ȡ(0-δ��ȡ, 1-�Ѷ�ȡ)
	BYTE   isGetAttachment;	//�Ƿ���ȡ����(0-δ��ȡ, 1-����ȡ)
	char   caption[33];		//�ʼ�����[33];
	char   senderNick[33];	//������
	char   sendTime[20];	//����ʱ��
	void hton()
		{	emailId = htonl(emailId);	}
	void ntoh()
		{	emailId = ntohl(emailId);	}
};

struct SQueryEmailListRes
{
	//UINT16 total;		//�ʼ�����;
	BYTE retNum;	//��ǰ��������;
	SEmailListGenInfo emails[1];	//array<�ʼ���Ϣ>       �����ʼ�����;
};

//��ѯ�ʼ�����, MLS_QUERY_EMAIL_CONTENT
struct SQueryEmailContentReq	//����
{
	UINT32 emailId; //�ʼ�ID
	void hton()
		{	emailId = htonl(emailId);	}
	void ntoh()
		{	emailId = ntohl(emailId);	}
};

struct SEmailPropGenInfo
{
	BYTE propType;	//��Ʒ����
	UINT16 propId;	//��Ʒid
	BYTE   num;		//����
	void hton()
		{	propId = htons(propId);		}
	void ntoh()
		{	propId = ntohs(propId);	}
};

struct SQueryEmailContentRes	//���ݷ���
{
	UINT32 emailId;		//�ʼ�ID;
	SBigNumber gold;	//���
	UINT32 rpcoin;		//����
	SBigNumber exp;		//����
	UINT32 vit;			//����
	BYTE   propNum;	//��Ʒ����
	UINT16 textLen;	//�����ֽ�����
	char   data[1];	//propNum��SEmailPropGenInfo + textLen���ֽ�����
	void hton()
	{
		emailId = htonl(emailId);
		rpcoin = htonl(rpcoin);
		vit = htonl(vit);
		textLen = htons(textLen);
	}
	void ntoh()
	{
		emailId = ntohl(emailId);
		rpcoin = ntohl(rpcoin);
		vit = ntohl(vit);
		textLen = ntohs(textLen);
	}
};

//��ȡ�ʼ�����, MLS_RECEIVE_EMAIL_ATTACHMENT
struct SReceiveEmailAttachmentsReq
{
	UINT32 emaiId;
	void hton()
		{	emaiId = htonl(emaiId);	}

	void ntoh()
		{	emaiId = ntohl(emaiId);	}
};

struct SReceiveEmailAttachmentsRes
{
	UINT32 emailId;		//�ʼ�id;
	SBigNumber gold;	//��ɫӵ�е���Ϸ��;
	UINT32 rpcoin;		//��ɫӵ�еĴ���;
	SBigNumber exp;		//����
	UINT32 vit;			//����
	BYTE propsNum;		//���ʼ��õ��ĵ�������;
	SPropertyAttr props[1];	//array<����>   �����޸ĵĵ�����Ϣ;
	void hton()
	{	
		emailId = htonl(emailId);
		rpcoin = htonl(rpcoin);
		vit = htonl(vit);
	}

	void ntoh()
	{	
		emailId = ntohl(emailId);
		rpcoin = ntohl(rpcoin);
		vit = ntohl(vit);
	}
};

//ɾ���ʼ�, MLS_DEL_EMAIL
struct SDelEmailReq
{
	BYTE num;	//ɾ���ʼ�����
	UINT32 emailIds[1];	//ɾ���ʼ�id
	void hton()
	{
		for(BYTE i = 0; i < num; ++i)
			emailIds[i] = htonl(emailIds[i]);
	}
	void ntoh()
	{
		for(BYTE i = 0; i < num; ++i)
			emailIds[i] = ntohl(emailIds[i]);
	}
};

//װ��װ��, MLS_LOAD_EQUIPMENT
struct SLoadEquipmentReq
{
	UINT32 id;	//����ΨһID;
	BYTE   pos;	//λ��;          //װ��Ŀ��λ��
	void hton()
		{	id = htonl(id);	}

	void ntoh()
		{	id = ntohl(id);	}
};

struct SEquipmentInfo
{
	UINT32  id;		//����ΨһID;
	BYTE	pos;	//���߸��ĺ�λ��;
	void hton()
		{	id = htonl(id);	}

	void ntoh()
		{	id = ntohl(id);	}
};

//ж��װ��, MLS_UNLOAD_EQUIPMENT
struct SUnLoadEquipmentReq
{
	UINT32 id;	//����ΨһID;
	void hton()
		{	id = htonl(id);	}

	void ntoh()
		{	id = ntohl(id);	}
};

//ʹ�õ���, MLS_USE_PROPERTY
struct SUsePropertyReq
{
	UINT32 id;	//����ΨһID;
	void hton()
		{	id = htonl(id);	}

	void ntoh()
		{	id = ntohl(id);	}
};

struct SPropertyDeduct
{
	BYTE num;
	BYTE type;
	UINT32 uid;

	void hton()
	{
		uid = htonl(uid);
	}

	void ntoh()
	{
		uid = ntohl(uid);
	}
};

struct SUsePropertyRes
{
	SBigNumber gold;						//��Ϸ��
	SBigNumber exp;							//����
	UINT32 vit;								//����
	UINT32 rpcoin;							//����
	BYTE deduct_num;						//�۳�������Ʒ����
	SPropertyDeduct data[1];				//�۳�������Ʒ

	void hton()
	{
		for(UINT16 i = 0; i < deduct_num; ++i)
			data[i].hton();
		rpcoin = htonl(rpcoin);
		vit = htonl(vit);
	}

	void ntoh()
	{
		for(UINT16 i = 0; i < deduct_num; ++i)
			data[i].ntoh();
		rpcoin = ntohl(rpcoin);
		vit = ntohl(vit);
	}
};

//��ȡ�ɾͽ���, MLS_RECEIVE_REWARD
struct SReceiveRewardReq
{
	UINT32 achievement_id;					//�ɾ�ID

	void hton()
	{
		achievement_id = htonl(achievement_id);
	}

	void ntoh()
	{
		achievement_id = ntohl(achievement_id);
	}
};

struct SReceiveRewardRes
{
	SBigNumber gold;						//��Ϸ��
	SBigNumber exp;							//����
	UINT32 vit;								//����
	UINT32 rpcoin;							//����
	UINT32 point;							//�ɾ͵�
	BYTE num;								//�������ӱ仯����
	SPropertyAttr data[1];					//�������ӱ仯����

	void hton()
	{
		for(UINT16 i = 0; i < num; ++i)
			data[i].hton();
		rpcoin = htonl(rpcoin);
		point = htonl(point);
		vit = htonl(vit);
	}

	void ntoh()
	{
		for(UINT16 i = 0; i < num; ++i)
			data[i].ntoh();
		rpcoin = ntohl(rpcoin);
		point = ntohl(point);
		vit = ntohl(vit);
	}
};

//ɾ����ɫ, MLS_DEL_ROLE
struct SDelRoleReq
{
	UINT32 roleId;
	void hton()
		{	roleId = htonl(roleId);	}

	void ntoh()
		{	roleId = ntohl(roleId);	}
};

struct SDelRoleRes
{
	bool success;
	UINT32 roleId;
	UINT16 remainder_day;

	void hton()
	{
		roleId = htonl(roleId);
		remainder_day = htons(remainder_day);
	}

	void ntoh()
	{
		roleId = ntohl(roleId);
		remainder_day = ntohs(remainder_day);
	}
};

//��ѯ��������, MLS_QUERY_GUILD_NAME
struct SQueryGuildNameReq
{
	UINT32 guildId;		//����ID
	void hton()
		{	guildId = htonl(guildId);	}

	void ntoh()
		{	guildId = ntohl(guildId);	}
};

struct SQueryGuildNameRes
{
	UINT32 guildId;			//����ID
	char   guildName[33];	//��������[33];
	void hton()
		{	guildId = htonl(guildId);	}

	void ntoh()
		{	guildId = ntohl(guildId);	}
};

//��ѯ�������������Ϣ, MLS_QUERY_INST_RANK
struct SQueryInstRankReq
{
	UINT16 instId;	//����id
	void hton()
		{	instId = htons(instId);	}
	void ntoh()
		{	instId = ntohs(instId);	}
};

struct SInstRankInfo
{
	UINT32 roleId;		//��ɫid
	char   roleNick[33];//��ɫ�ǳ�
	UINT32 score;	//����
	char   tts[20];		//���ʱ��1970-01-01 00:00:00

	void hton()
	{	
		roleId = htonl(roleId);
		score = htonl(score);
	}
	void ntoh()
	{
		roleId = ntohl(roleId);
		score = ntohl(score);
	}
};

struct SQueryInstRankRes
{
	UINT16 instId;	//����id
	BYTE   num;		//��������
	SInstRankInfo ranks[1];	//��������(�ɸߵ���)

	void hton()
		{	instId = htons(instId);	}
	void ntoh()
		{	instId = ntohs(instId);	}
};

//�û��ύ��ʱ����, MLS_USER_COMMIT_TIMER_TASK
struct SPersonalTimerTaskReq
{
	UINT16 taskId;	//��ʱ������
	void hton()
		{	taskId = htons(taskId);	}
	void ntoh()
		{	taskId = ntohs(taskId);	}
};

struct SPersonalTimerTaskRes
{
	UINT16 taskId;	//��ʱ������
	UINT32 newId;	//�µĶ�ʱ����id
	void hton()
	{	
		taskId = htons(taskId);
		newId = htonl(newId);
	}
	void ntoh()
	{	
		taskId = ntohs(taskId);
		newId = ntohl(newId);
	}
};

//��ѯ��ɫ����, MLS_QUERY_ROLE_SKILLS
struct SQuerySkillsRes
{
	BYTE skillNum;	//��������
	SSkillMoveInfo skills[1];	//��������
};

//��������, MLS_SKILL_UPGRADE
struct SSkillUpgradeReq
{
	UINT16 skillId;	//Ҫ�����ļ���id
	BYTE   step;	//��������
	void hton()
		{	skillId = htons(skillId);	}
	void ntoh()
		{	skillId = ntohs(skillId);	}
};

struct SSkillUpgradeRes
{
	SSkillMoveInfo newSkill;	//������������
	SBigNumber     gold;		//��ǰ�������
	void hton()
		{	newSkill.hton();	}
	void ntoh()
		{	newSkill.ntoh();	}
};

//�ı���Ӫ, MLS_CHANGE_FACTION
struct SChangeFactionReq
{
	BYTE factionId;	//��Ӫid
};

//װ�ؼ���, MLS_LOAD_SKILL
struct SLoadSkillReq
{
	UINT16 skillId;	//����id
	BYTE   key;		//��Ӧ�ͻ���key
	void hton()
		{	skillId = htons(skillId);	}
	void ntoh()
		{	skillId = ntohs(skillId);	}
};

//ȡ������, MLS_CANCEL_SKILL
struct SCancelSkillReq
{
	UINT16 skillId;	//����id
	void hton()
		{	skillId = htons(skillId);	}
	void ntoh()
		{	skillId = ntohs(skillId);	}
};


//װ��λ�����ǲ���
struct SRiseStarAttachParam		//���Ǹ��Ӳ���
{
	BYTE   decDamagedChance;	//�Ƿ񽵵�������(0-��,1-��)
	BYTE   addSuccedChance;		//�Ƿ����ӳɹ���(0-��,1-��)
};

//װ���ϳ�, MLS_EQUIPMENT_COMPOSE
struct SEquipComposeReq 
{
	UINT16 composeId;
	void hton()
		{	composeId = htons(composeId);	}

	void ntoh()
		{	composeId = ntohs(composeId);	}
};

struct SEquipComposeRes
{
	SBigNumber gold;		//��ǰ�����
	bool replace;			//�Ƿ��滻װ��λ��װ��
	SPropertyAttr equip;	//���滻��װ����Ϣ
	BYTE num;				//�������Ӹ�������
	SPropertyAttr data[1];	//�������Ӹ�������

	void hton()
	{
		equip.hton();
		for(BYTE i = 0; i < num; ++i)
			data[i].hton();
	}
	void ntoh()
	{
		equip.ntoh();
		for(BYTE i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//װ��λǿ��,MLS_EQUIP_POS_STRENGTHEN
struct SEquipPosStrengthenReq
{
	BYTE pos;	//װ��λ
};

struct SEquipPosStrengthenRes
{
	BYTE newStrengthenLv;		//�µ�ǿ���ȼ�
	SBigNumber gold;			//ʣ����
	BYTE num;					//���ı���ʯͷ��������
	SConsumeProperty consumeStones[1];	//���ı���ʯͷ����

	void hton()
	{
		for(BYTE i = 0; i < num; ++i)
			consumeStones[i].hton();
	}
	void ntoh()
	{
		for(BYTE i = 0; i < num; ++i)
			consumeStones[i].ntoh();
	}
};

//װ��λ����, MLS_EQUIP_POS_RISE_STAR
struct SEquipPosRiseStarReq
{
	BYTE pos;	//װ��λ
	SRiseStarAttachParam param;	//���Ǹ��Ӳ���
};

struct SEquipPosRiseStarRes
{
	BYTE newStarLv;		//�µ��Ǽ�
	SBigNumber gold;	//ʣ����
	BYTE num;		//���ı���ʯͷ��������
	SConsumeProperty consumeStones[1];	//���ı���ʯͷ����

	void hton()
	{
		for(BYTE i = 0; i < num; ++i)
			consumeStones[i].hton();
	}
	void ntoh()
	{
		for(BYTE i = 0; i < num; ++i)
			consumeStones[i].ntoh();
	}
};

//װ��λ��Ƕ��ʯ, MLS_EQUIP_POS_INLAID_GEM
struct SEquipPosInlaidGemReq
{
	BYTE   pos;	//װ��λ;
	UINT32 gemId;		//��ʯΨһID;
	BYTE   holeSeq;		//��ʯ�׺�(0-3)
	void hton()
		{	gemId = htonl(gemId);	}

	void ntoh()
		{	gemId = ntohl(gemId);	}
};

//װ��λȡ����ʯ, MLS_EQUIP_POS_REMOVE_GEM
struct SEquipPosRemoveGemReq
{
	BYTE   pos;	//װ��λ;
	BYTE   holeSeq;	//ȡ��Ŀ��װ����ʯ�׺�;  //0-3;
};

struct SEquipPosRemoveGemRes
{
	SPropertyAttr gemPropAttr;	//��ʯ���뱳����������

	void hton()
		{	gemPropAttr.hton();		}
	void ntoh()
		{	gemPropAttr.ntoh();		}
};

//��ȡװ��λ����
struct SGetEquipPosAttrRes
{
	BYTE num;		//����
	SEquipPosAttr data[1];	//��������

	void hton()
	{
		for(BYTE i = 0; i < num; ++i)
			data[i].hton();
	}

	void ntoh()
	{
		for(BYTE i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//��ʯ�ϳ�, MLS_GEM_COMPOSE
struct SGemComposeReq
{
	BYTE num;	//��������
	SConsumeProperty cosumeProp[1];
	void hton()
	{
		for(BYTE i = 0; i < num; ++i)
			cosumeProp[i].hton();
	}
	void ntoh()
	{
		for(BYTE i = 0; i < num; ++i)
			cosumeProp[i].ntoh();
	}
};

struct SGemComposeRes
{
	SPropertyAttr composeGemAttr;	//�ºϳɱ�ʯ����
	void hton()
		{	composeGemAttr.hton();	}
	void ntoh()
		{	composeGemAttr.ntoh();	}
};

//����ռ������֪ͨ, MLS_INST_OCCUPY_PUSH_NOTIFY
struct SInstOccupyPushNotify
{
	UINT16 instId;		//����id
	UINT32 roleId;		//��ɫid
	char   roleNick[33];//��ɫ�ǳ�
	UINT32 score;		//����
	char   tts[20];		//���ʱ��1970-01-01 00:00:00

	void hton()
	{
		instId = htons(instId);
		roleId = htonl(roleId);
		score = ntohl(score);
	}
	void ntoh()
	{
		instId = ntohs(instId);
		roleId = ntohl(roleId);
		score = ntohl(score);
	}
};

//��Ʒ�ع�, MLS_PROP_BUY_BACK
struct SPropBuyBackReq
{
	UINT32 sellId;	//����id
	BYTE   num;		//�ع�����
	void hton()
		{	sellId = htonl(sellId);		}
	void ntoh()
		{	sellId = ntohl(sellId);		}
};

struct SPropBuyBackRes
{
	SBigNumber gold;	//��ǰ�������;
	BYTE propNum;		//�����ı�����
	SPropertyAttr props[1];	//�����ı���Ʒ
};

//��ѯ��ɫ������Ʒ��¼, MLS_QUERY_ROLE_SELL_PROP_RCD
struct SQueryRoleSellPropRcdReq
{
	UINT32 from;	//�ӵڼ�����ʼ
	BYTE   num;		//�������
	void hton()
		{	from = htonl(from);		}

	void ntoh()
		{	from = ntohl(from);		}
};

struct SPropSellInfo
{
	SPropertyAttr prop;
	char sellDate[SGS_DATE_TIME_LEN];	//����ʱ��

	void hton()
		{	prop.hton();	}

	void ntoh()
		{	prop.ntoh();	}
};

struct SQueryRoleSellPropRcdRes
{
	UINT32 total;	//����
	BYTE   num;		//��������
	SPropSellInfo sells[1];	//��������

	void hton()
		{	total = htonl(total);	}

	void ntoh()
		{	total = ntohl(total);	}
};

//��������֪ͨ, MLS_VIT_RECOVERY_NOTIFY
struct SVitUpdateNotify
{
	UINT32 vit;		//��ǰ����
	void hton()
		{	vit = htonl(vit);	}
	void ntoh()
		{	vit = ntohl(vit);	}
};

//��ֵ, MLS_DELTA_TOKEN
struct SRechargeReq
{
	UINT16 cashId;	//��ֵid
	void hton()
		{	cashId = htons(cashId);		}
	void ntoh()
		{	cashId = ntohs(cashId);		}
};

struct SRechargeRes
{
	UINT32 cashcount;	//��ֵ�ܶ�(rmb)
	UINT32 rpcoin;		//��ǰ��������
	void hton()
	{
		cashcount = htonl(cashcount);
		rpcoin = htonl(rpcoin);
	}
	void ntoh()
	{
		cashcount = ntohl(cashcount);
		rpcoin = ntohl(rpcoin);
	}
};

//��ȡ��ɫռ�츱��, MLS_GET_ROLE_OCCUPY_INST
struct SGetRoleOccupyInstRes
{
	UINT16 num;	//ռ������
	UINT16 occupyInstIds[1];	//ռ�츱��id
	void hton()
	{
		for(int i = 0; i < num; ++i)
			occupyInstIds[i] = htons(occupyInstIds[i]);
		num = htons(num);
	}
	void ntoh()
	{
		num = ntohs(num);
		for(int i = 0; i < num; ++i)
			occupyInstIds[i] = ntohs(occupyInstIds[i]);
	}
};

//���ý�ɫְҵ�ƺ�id, MLS_SET_ROLE_TITLE_ID
struct SSetRoleTitleReq
{
	UINT16 titleId;	//�ƺ�id
	void hton()
		{	titleId = htons(titleId);	}
	void ntoh()
		{	titleId = ntohs(titleId);	}
};

//�̳ǹ�����Ʒ, MLS_MALL_BUY_GOODS
struct SMallBuyGoodsReq
{
	UINT32 buyId;	//����id
	UINT16 num;		//�������
	void hton()
	{
		buyId = htonl(buyId);
		num = htons(num);
	}
	void ntoh()
	{
		buyId = ntohl(buyId);
		num = ntohs(num);
	}
};

struct SMallBuyGoodsRes
{
	UINT32 rpcoin;		//��ǰʣ���������
	BYTE backpackNum;	//�������Ӹ�������
	SPropertyAttr props[1];	//�������Ӹ�������
	void hton()
		{	rpcoin = htonl(rpcoin);		}
	void ntoh()
		{	rpcoin = ntohl(rpcoin);		}
};

//��ѯ��ɫʱװ, MLS_QUERY_ROLE_DRESS
struct SQueryRoleDressRes
{
	UINT16 num;			//����
	UINT16 dressIds[1];	//ʱװid
	void hton()
	{
		for(UINT16 i = 0; i < num; ++i)
			dressIds[i] = htons(dressIds[i]);
		num = htons(num);
	}
	void ntoh()
	{
		num = ntohs(num);
		for(UINT16 i = 0; i < num; ++i)
			dressIds[i] = ntohs(dressIds[i]);
	}
};

//����ʱװ, MLS_WEAR_DRESS
struct SWearDressReq
{
	UINT16 dressId;		//ʱװid
	void hton()
		{	dressId = htons(dressId);	}
	void ntoh()
		{	dressId = ntohs(dressId);	}
};

//��Ҹ���֪ͨ, MLS_GOLD_UPDATE_NOTIFY
struct SGoldUpdateNotify
{
	SBigNumber gold;	//��ǰ���
};

//���Ҹ���֪ͨ, MLS_TOKEN_UPDATE_NOTIFY
struct STokenUpdateNotify
{
	UINT32 rpcoin;	//��ǰ����
	void hton()
		{	rpcoin = htonl(rpcoin);		}
	void ntoh()
		{	rpcoin = ntohl(rpcoin);		}
};

//�������֪ͨ, MLS_ROLE_EXP_UPDATE_NOTIFY
struct SExpUpdateNotify
{
	SBigNumber exp;	//��ǰ����
};

//��������֪ͨ, MLS_ROLE_BACKPACK_UPDATE_NOTIFY
struct SBackpackUpdateNotify
{
	BYTE propNum;
	SPropertyAttr props[1];
};

//��Ƭ����֪ͨ, MLS_FRAGMENT_UPDATE_NOTIFY
struct SFragmentUpdateNotify
{
	SEquipFragmentAttr fragment;
	void hton()
		{	fragment.hton();	}

	void ntoh()
		{	fragment.ntoh();	}
};

//������Ƭ����֪ͨ, MLS_CHAIN_SOUL_FRAGMENT_UPDATE_NOTIFY
struct SChainSoulFragmentNotify
{
	SChainSoulFragmentAttr fragment;
	void hton()
	{	fragment.hton();	}

	void ntoh()
	{	fragment.ntoh();	}
};

//��ɫ����֪ͨ, MLS_ROLE_UPGRADE_NOTIFY
struct SRoleUpgradeNotify
{
	BYTE roleLevel;		//��ɫ�ȼ�
	SBigNumber gold;	//��Ϸ��
	SBigNumber exp;		//����
	UINT32 hp;			//����
	UINT32 mp;			//ħ��
	UINT32 vit;			//����
	UINT16 ap;			//ʣ�����Ե���
	UINT32 attack;      //����
	UINT32 def;         //����
	UINT32 cp;			//��ǰս����;
	SGSPosition pos;	//����λ��
	BYTE skillNum;		//����/���и���
	SSkillMoveInfo data[1];	//����
	
	void hton()
	{
		hp = htonl(hp);
		mp = htonl(mp);
		vit = htonl(vit);
		attack = htonl(attack);
		def = htonl(def);
		cp = htonl(cp);
		ap = htons(ap);
		pos.hton();
	}
	void ntoh()
	{
		hp = ntohl(hp);
		mp = ntohl(mp);
		vit = ntohl(vit);
		attack = ntohl(attack);
		def = ntohl(def);
		cp = ntohl(cp);
		ap = ntohs(ap);
		pos.ntoh();
	}
};


//��ȡ����������, MLS_FETCH_ROBOT

#define MATCH_ROBOT_BY_CP 1
#define MATCH_ROBOT_BY_LEVEL 2

struct SFetchRobotReq
{
	BYTE rule;			//ƥ�����
	UINT32 targetId;	//Ŀ���ɫid

	void hton()
	{
		targetId = htonl(targetId);
	}
	void ntoh()
	{
		targetId = ntohl(targetId);
	}
};

struct SFetchRobotRes
{
	UINT32 robotId;			//������id
	char   robotNick[33];	//�������ǳ�
	UINT16 titleId;			//��ǰװ�سƺ�id;
	BYTE   level;			//��ɫ�ȼ�
	BYTE   occuId;			//ְҵid
	BYTE   sex;				//�Ա�
	UINT32 cp;				//ս����;
	BYTE   factionId;		//��Ӫid
	UINT16 dressId;			//ʱװid
	UINT32 guildId;			//��������id
	BYTE   vipLevel;		//VIP�ȼ�
	BYTE   core_level;		//�������Ĳ����ȼ�
	BYTE   wearEquipNum;	//����װ������
	BYTE   skillNum;		//ӵ����װ�ؼ�������
	BYTE   equipPosNum;		//װ��λ����
	BYTE   chainSoulPosNum;	//���겿������
	char   data[1];			//wearEquipNum��SPropertyAttr + skillNum��SSkillMoveInfo + equipPosNum��SEquipPosAttr + chainSoulPosNum��SChainSoulPosAttr

	void hton()
	{
		robotId = htonl(robotId);
		cp = htonl(cp);
		guildId = htonl(guildId);
		dressId = htons(dressId);
		titleId = htons(titleId);
	}

	void ntoh()
	{
		robotId = ntohl(robotId);
		cp = ntohl(cp);
		guildId = ntohl(guildId);
		dressId = ntohs(dressId);
		titleId = ntohs(titleId);
	}
};

//��ȡ������������������������, MLS_GET_PERSONAL_TRANSFER_ENTER
struct SGetPersonalTransferEnterReq
{
	UINT16 transferId;	//������id

	void hton()
		{	transferId = htons(transferId);		}
	void ntoh()
		{	transferId = ntohs(transferId);		}
};

struct SGetPersonalTransferEnterRes
{
	UINT16 enterNum;	//��������
	UINT32 leftSec;		//ʣ��ʱ��(��)
	void hton()
	{	
		enterNum = htons(enterNum);
		leftSec = htonl(leftSec);
	}
	void ntoh()
	{
		enterNum = ntohs(enterNum);
		leftSec = ntohl(leftSec);
	}
};

//��ѯװ����Ƭ, MLS_QUERY_EQUIP_FRAGMENT
struct SQueryEquipFragmentRes
{
	BYTE num;
	SEquipFragmentAttr data[1];
};

//��ɫ����, MLS_ROLE_REVIVE
struct SReviveRoleReq
{
	UINT16 reviveId;	//����id
	void hton()
		{	reviveId = htons(reviveId);		}
	void ntoh()
		{	reviveId = ntohs(reviveId);		}
};

struct SReviveRoleRes
{
	UINT32 token;	//��ǰ����
	BYTE   num;		//����Ҹ��¸�������
	SConsumeProperty data[1];	//���ĸ���Ҹ�������
	void hton()
		{	token = htonl(token);		}
	void ntoh()
		{	token = ntohl(token);		}
};

//��ȡ�Ѵ򿪵����������, MLS_GET_OPEN_PERSONAL_RAND_INST
struct SPersonalRandInstInfo
{
	UINT16 transferId;	//��������
	UINT16 instId;		//����id
	void hton()
	{
		transferId = htons(transferId);
		instId = htons(instId);
	}

	void ntoh()
	{
		transferId = ntohs(transferId);
		instId = ntohs(instId);
	}
};

struct SGetOpenPersonalRandInstRes
{
	BYTE num;
	SPersonalRandInstInfo data[1];
};

//ϵͳ����, MLS_SYS_NOTICE_NOTIFY
struct SSysNoticeNotify
{
	UINT16 type;	//��������(ESGSNoticeType)
	UINT16 len;		//data�ֽ�����
	char data[1];	//����

	void hton()
	{
		type = htons(type);
		len = htons(len);
	}
	void ntoh()
	{
		type = ntohs(type);
		len = ntohs(len);
	}
};

//��ѯ��Ҷһ�����, MLS_QUERY_GOLD_EXCHANGE
struct SQueryGoldExchangeRes
{
	UINT16 exchangedTimes;	//��ǰ�Ѷһ�����
	void hton()
		{	exchangedTimes = htons(exchangedTimes);		}
	void ntoh()
		{	exchangedTimes = ntohs(exchangedTimes);		}
};

//��Ҷһ�, MLS_GOLD_EXCHANGE
struct SGoldExchangeRes
{
	UINT32 rpcoin;		//��ǰ����
	SBigNumber gold;	//��ǰ���
	UINT16 exchangedTimes;	//�����Ѷһ�����
	void hton()
	{
		rpcoin = htonl(rpcoin);
		exchangedTimes = htons(exchangedTimes);
	}
	void ntoh()
	{
		rpcoin = ntohl(rpcoin);
		exchangedTimes = ntohs(exchangedTimes);
	}
};

//��ѯδ��ȡ������VIP�ȼ�, MLS_NOT_RECEIVE_VIP_REWARD
struct SQueryNotReceiveVIPRewardRes
{
	BYTE num;
	BYTE vipLv[1];
};

//��ȡVIP����, MLS_RECEIVE_VIP_REWARD
struct SReceiveVIPRewardReq
{
	BYTE vipLv;		//vip�ȼ�
};

struct SReceiveVIPRewardRes
{
	UINT32 rpcoin;		//��ǰ����
	SBigNumber gold;	//��ǰ���
	BYTE num;			//�������ӱ仯����
	SPropertyAttr data[1];	//�������ӱ仯����

	void hton()
		{	rpcoin = htonl(rpcoin);		}

	void ntoh()
		{	rpcoin = ntohl(rpcoin);		}
};

//��ѯ�ѳ�ֵ�����׳����͵ĳ�ֵid, MLS_QUERY_FIRST_CASH_GIVE
struct SQueryRechargeFirstGiveRes
{
	BYTE num;			//����
	UINT16 data[1];		//��ֵid

	void hton()
	{
		for(BYTE i = 0; i < num; ++i)
			data[i] = htons(data[i]);
	}

	void ntoh()
	{
		for(BYTE i = 0; i < num; ++i)
			data[i] = ntohs(data[i]);
	}
};

//��ѯ��ɫӵ��ħ����, MLS_QUERY_MAGICS
struct SQueryRoleMagicsRes
{
	BYTE num;			//����
	UINT16 data[1];		//ħ����id

	void hton()
	{
		for(BYTE i = 0; i < num; ++i)
			data[i] = htons(data[i]);
	}
	void ntoh()
	{
		for(BYTE i = 0; i < num; ++i)
			data[i] = ntohs(data[i]);
	}
};

//���ħ����, MLC_GAIN_MAGIC
struct SGainMagicReq
{
	UINT16 instId;	//����id
	UINT16 magicId;	//ħ����id

	void hton()
	{
		instId = htons(instId);
		magicId = htons(magicId);
	}
	void ntoh()
	{
		instId = ntohs(instId);
		magicId = ntohs(magicId);
	}
};

//���ħ����֪ͨ, MLC_GAIN_MAGIC_NOTIFY
struct SGainMagicNotify
{
	UINT16 magicId;	//ħ����id
	void hton()
		{	magicId = htons(magicId);	}
	void ntoh()
		{	magicId = ntohs(magicId);	}
};

//��ѯ��ǰǩ��, MLS_QUERY_ONLINE_SIGN_IN
struct SQueryOnlineSignInRes
{
	BYTE continueTimes;	//��������
	BYTE sameDayIsSign;	//�����Ƿ�ǩ��(1-ǩ��, 0-δǩ��)
};

//��ѯ�Ѿ���ȡ��б�
struct SQueryReceivedActivityRes
{
	UINT16 num;			//�����
	UINT32 data[1];		//�id�б�

	void hton()
	{
		for(UINT16 i = 0; i < num; ++i)
			data[i] = htonl(data[i]);
		num = htons(num);
	}
	void ntoh()
	{
		num = ntohs(num);
		for(UINT16 i = 0; i < num; ++i)
			data[i] = ntohl(data[i]);
	}
};

//��ȡ�����, MLS_RECEIVED_ACTIVITY
struct SReceiveActivityReq
{
	UINT32 activityId;	//�id

	void hton()
		{	activityId = htonl(activityId);		}
	void ntoh()
		{	activityId = ntohl(activityId);		}
};

//��ѯ��ǰ�����½�, MLS_QUERY_CUR_OPEN_CHAPTER
struct SQueryCurOpenChapterRes
{
	UINT16 chapterId;
	void hton()
		{	chapterId = htons(chapterId);	}
	void ntoh()
		{	chapterId = ntohs(chapterId);	}
};

//���������, MLS_RAND_INST_OPEN_NOTIFY
struct SRandInstItem
{
	UINT16 transferId;	//��ǰ��������
	UINT16 instId;		//����id
	void hton()
	{
		transferId = htons(transferId);
		instId = htons(instId);
	}

	void ntoh()
	{
		transferId = ntohs(transferId);
		instId = ntohs(instId);
	}
};

struct SRandInstOpenNotify
{
	BYTE num;
	SRandInstItem data[1];

	void hton()
	{
		for(BYTE i = 0; i < num; ++i)
			data[i].hton();
	}
	void ntoh()
	{
		for(BYTE i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//��������ر�, MLS_RAND_INST_CLOSE_NOTIFY
struct SRandInstCloseNotify
{
	SRandInstItem inst;

	void hton()
		{	inst.hton();	}
	void ntoh()
		{	inst.ntoh();	}
};

//��ȡ�һ��뽱��, MLS_RECEIVE_CASH_CODE_REWARD
struct SReceiveCashCodeRewardReq
{
	char cashCode[33];	//�һ���
};

//��ѯ��������, MLS_QUERY_UNLOCK_CONTENT
struct SQueryUnlockContentReq
{
	BYTE type;		//��������(ESGSUnlockType)
};

struct SQueryUnlockContentRes
{
	UINT32 userId;	//�˺�id
	UINT32 roleId;	//��ɫid
	UINT16 num;		//��������
	UINT16 data[1];	//��������

	void hton()
	{
		for(UINT16 i = 0; i < num; ++i)
			data[i]= htons(data[i]);
		num = htons(num);
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		num = ntohs(num);
		for(UINT16 i = 0; i < num; ++i)
			data[i]= ntohs(data[i]);
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//���ݽ���, MLS_CONTENT_UNLOCK
struct SContentUnlockReq
{
	BYTE type;			//��������(ESGSUnlockType)
	UINT16 unlockId;	//����id

	void hton()
		{	unlockId = htons(unlockId);		}

	void ntoh()
		{	unlockId = ntohs(unlockId);		}
};

//��ȡ���̽�Ʒ, MLS_GET_ROULETTE_REMAIN_REWARD
struct SRoulettePropItem
{
	BYTE id;		//���̸���id
	BYTE type;		//����(ESGSPropType)
	UINT16 propId;	//��Ʒid
	UINT32 num;		//��Ʒ����

	void hton()
	{
		propId = htons(propId);
		num = htonl(num);
	}
	void ntoh()
	{
		propId = ntohs(propId);
		num = ntohl(num);
	}
};

struct SGetRoulettePropRes
{
	BYTE num;
	UINT16 getNum;	//�Ѿ���ȡ����
	SRoulettePropItem data[1];

	void hton()
	{
		for(BYTE i = 0; i < num; ++i)
			data[i].hton();
		getNum = htons(getNum);
	}

	void ntoh()
	{
		for(BYTE i = 0; i < num; ++i)
			data[i].ntoh();
		getNum = ntohs(getNum);
	}
};

//��ȡ���̽���, MLS_EXTRACT_ROULETTE_REWARD
struct SExtractRouletteRewardRes
{
	BYTE id;		//���̸���id
	UINT16 getNum;	//�Ѿ���ȡ����
	UINT32 rpcoin;	//����
	UINT32 vit;		//����
	SBigNumber exp;	//����
	SBigNumber gold;	//���
	BYTE   num;		//����������
	SPropertyAttr data[1];	//������������

	void hton()
	{
		getNum = htons(getNum);
		rpcoin = htonl(rpcoin);
		vit = htonl(vit);

		for(BYTE i = 0; i < num; ++i)
			data[i].hton();
	}

	void ntoh()
	{
		getNum = ntohs(getNum);
		rpcoin = ntohl(rpcoin);
		vit = ntohl(vit);

		for(BYTE i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//��ȡ��ֵ������MLS_FETCH_RECHARGE_ORDER
struct SFetchRechargeOrderReq
{
	UINT16 rechargeId;	//��ֵid
	UINT32 rmb;			//�����
	void hton()
	{
		rechargeId = htons(rechargeId);
		rmb = htonl(rmb);
	}
	void ntoh()
	{
		rechargeId = ntohs(rechargeId);
		rmb = ntohl(rmb);
	}
};

struct SFetchRechargeOrderRes
{
	char   orderNO[65]; //������
};

//��ȡ��ֵ���ش���, MLS_GET_RECHARGE_TOKEN
struct SGetRechargeTokenReq
{
	char   orderNO[65]; //������
};

struct SGetRechargeTokenRes
{
	UINT32 rpcoin;

	void hton()
		{	rpcoin = htonl(rpcoin);		}
	void ntoh()
		{	rpcoin = ntohl(rpcoin);		}
};

//ˢ�����̴�, MLS_UPDATE_ROULETTE_BIG_REWARD
struct SUpdateRouletteBigRewardRes
{
	UINT32 rpcoin;	//����
	SRoulettePropItem bigReward;
	void hton()
	{
		bigReward.hton();
		rpcoin = htonl(rpcoin);
	}
	void ntoh()
	{
		bigReward.ntoh();
		rpcoin = ntohl(rpcoin);
	}
};

//��ȡ���̴�, MLS_FETCH_ROULETTE_BIG_REWARD
struct SFetchRouletteBigRewardRes
{
	UINT32 rpcoin;	//����
	UINT32 vit;		//����
	SBigNumber exp;	//����
	SBigNumber gold;	//���
	BYTE   num;		//����������
	SPropertyAttr data[1];	//������������

	void hton()
	{
		rpcoin = htonl(rpcoin);
		vit = htonl(vit);
		for(BYTE i = 0; i < num; ++i)
			data[i].hton();
	}

	void ntoh()
	{
		rpcoin = ntohl(rpcoin);
		vit = ntohl(vit);
		for(BYTE i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//��ȡ�����������ս����, MLS_GET_ACTIVE_INST_CHALLENGE_TIMES
struct SActiveInstItem
{
	UINT16 instId;
	BYTE   times;

	void hton()
		{	instId = htons(instId);		}
	void ntoh()
		{	instId = ntohs(instId);		}
};

struct SGetActiveInstChallengeTimesRes
{
	UINT16 num;
	SActiveInstItem data[1];
	void hton()
	{
		for(UINT16 i = 0; i < num; ++i)
			data[i].hton();
		num = htons(num);
	}
	void ntoh()
	{
		num = ntohs(num);
		for(UINT16 i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//��ѯ�����һ�����, MLS_QUERY_VIT_EXCHANGE
struct SQueryVitExchangeRes
{
	UINT16 exchangedTimes;	//��ǰ�Ѷһ�����
	void hton()
	{	exchangedTimes = htons(exchangedTimes);		}
	void ntoh()
	{	exchangedTimes = ntohs(exchangedTimes);		}
};

//�����һ�, MLS_VIT_EXCHANGE
struct SVitExchangeRes
{
	UINT32 rpcoin;			//��ǰ����
	UINT32 vit;				//��ǰ����
	UINT16 exchangedTimes;	//�����Ѷһ�����
	void hton()
	{
		vit = htonl(vit);
		rpcoin = htonl(rpcoin);
		exchangedTimes = htons(exchangedTimes);
	}
	void ntoh()
	{
		vit = ntohl(vit);
		rpcoin = htonl(rpcoin);
		exchangedTimes = ntohs(exchangedTimes);
	}
};

//��ѯ��ɫÿ��ǩ��, MLS_QUERY_ROLE_DAILY_SIGN
struct SQueryRoleDailySignRes
{
	BYTE month;             //�·�
	bool now_signed;		//�����Ƿ�ǩ��
	BYTE num;				//ǩ������
	BYTE data[1];			//ǩ������
};

//��ɫÿ��ǩ��, MLS_ROLE_DAILY_SIGN
struct SRoleDailySignRes
{
	UINT32 rpcoin;			//����
	UINT32 vit;				//����
	SBigNumber exp;			//����
	SBigNumber gold;		//���
	BYTE num;				//����������
	SPropertyAttr data[1];	//������������

	void hton()
	{
		rpcoin = htonl(rpcoin);
		vit = htonl(vit);
		for(BYTE i = 0; i < num; ++i)
			data[i].hton();
	}

	void ntoh()
	{
		rpcoin = ntohl(rpcoin);
		vit = ntohl(vit);
		for(BYTE i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//��ѯ������Ƭ, MLS_QUERY_CHAIN_SOUL_FRAGMENT
struct SQueryChainSoulFragmentRes
{
	BYTE num;
	SChainSoulFragmentAttr data[1];

	void hton()
	{
		for(BYTE i = 0; i < num; ++i)
			data[i].hton();
	}

	void ntoh()
	{
		for(BYTE i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//���겿������
#define MAX_CHAIN_SOUL_HOLE_NUM 6
struct SChainSoulPosAttr
{
	BYTE pos;		//���겿λ
	BYTE level;		//ǿ���ȼ�
};

//��ѯ���겿��, MLS_QUERY_CHAIN_SOUL_POS
struct SQueryChainSoulPosRes
{
	BYTE core_level;									//���Ĳ����ȼ�
	SChainSoulPosAttr pos[MAX_CHAIN_SOUL_HOLE_NUM];		//�Ǻ��Ĳ�λ�ȼ�
};

//���겿������, MLS_CHAIN_SOUL_POS_UPGRADE
struct SChainSoulPosUpgradeReq
{
	BYTE pos;											//��������
};

struct SChainSoulPosUpgradeRes
{
	BYTE core_level;									//���Ĳ����ȼ�
	SChainSoulPosAttr attr;								//�����󲿼�����
	BYTE num;											//ʣ����Ƭ����
	SChainSoulFragmentAttr data[1];

	void hton()
	{
		for(BYTE i = 0; i < num; ++i)
			data[i].hton();
	}

	void ntoh()
	{
		for(BYTE i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//��������Boss, MLS_ATTACK_WORLD_BOOS
struct SAttackWorldBossReq
{
	UINT32 hurt;

	void hton()
	{
		hurt = htonl(hurt);
	}

	void ntoh()
	{
		hurt = ntohl(hurt);
	}
};

struct SAttackWorldBossRes
{
	UINT32 boss_blood;		//bossѪ��
	UINT32 hurt_sum;		//������˺�
	UINT32 kill_roleId;		//��ɱ�߽�ɫID

	void hton()
	{
		hurt_sum = htonl(hurt_sum);
		boss_blood = htonl(boss_blood);
		kill_roleId = htonl(kill_roleId);
	}

	void ntoh()
	{
		hurt_sum = ntohl(hurt_sum);
		boss_blood = ntohl(boss_blood);
		kill_roleId = ntohl(kill_roleId);
	}
};

//��ѯ����Boss���а�MLS_QUERY_WORLD_BOSS_RANK
struct SWorldBossRankItem
{
	UINT32 id;			//��ɫID
	UINT32 hurt_sum;	//������˺�
	char   nick[33];	//�ǳ�

	void hton()
	{
		id = htonl(id);
		hurt_sum = htonl(hurt_sum);
	}

	void ntoh()
	{
		id = ntohl(id);
		hurt_sum = ntohl(hurt_sum);
	}
};

struct SQueryWorldBossRankRes
{
	UINT32 boss_blood;			// bossѪ��
	UINT32 kill_roleId;			// ��ɱ�߽�ɫID
	UINT32 number;				// ��������
	BYTE num;					// ��������
	SWorldBossRankItem data[1];	// ��������

	void hton()
	{
		boss_blood = htonl(boss_blood);
		kill_roleId = htonl(kill_roleId);
		number = htonl(number);
		for(BYTE i = 0; i < num; ++i)
			data[i].hton();
	}

	void ntoh()
	{
		boss_blood = ntohl(boss_blood);
		kill_roleId = ntohl(kill_roleId);
		number = ntohl(number);
		for(BYTE i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//������Boss�����и���, MLS_RESURRECTION_IN_WORLDBOSS
struct SResurrectionInWorldBossRes
{
	UINT32 rpcoin;					//����
	UINT32 consume_count;			// ���Ѹ������

	void hton()
	{
		rpcoin = htonl(rpcoin);
		consume_count = htonl(consume_count);
	}

	void ntoh()
	{
		rpcoin = ntohl(rpcoin);
		consume_count = ntohl(consume_count);
	}
};

//��ѯ����Boss��Ϣ, MLS_QUERY_WORLDBOSS_INFO
struct SQueryWorldBossInfoRes
{
	UINT32 resurrection_time;		// ����ʱ��
	UINT32 consume_count;			// ���Ѹ������

	void hton()
	{
		resurrection_time = htonl(resurrection_time);
		consume_count = htonl(consume_count);
	}

	void ntoh()
	{
		resurrection_time = htonl(resurrection_time);
		consume_count = ntohl(consume_count);
	}
};

//��ȡ������Ϣ, MLS_GET_LADDER_ROLE_INFO
#define LADDER_MATCHTING_MAX_NUM 5

struct SLadderRoleInfoRes
{
	UINT32 rank;						//����
	UINT16 refresh_count;				//ˢ�´���
	UINT16 eliminate_count;				//����cd����
	UINT16 challenge_count;				//��ս����
	UINT16 win_in_row;					//��ʤ����
	UINT32 cooling_time;				//��ս��ȴʱ��

	void hton()
	{
		rank = htonl(rank);
		refresh_count = htons(refresh_count);
		eliminate_count = htons(challenge_count);
		challenge_count = htons(challenge_count);
		win_in_row = htons(win_in_row);
		cooling_time = htonl(cooling_time);
	}

	void ntoh()
	{
		rank = ntohl(rank);
		refresh_count = ntohs(refresh_count);
		eliminate_count = ntohs(challenge_count);
		challenge_count = ntohs(challenge_count);
		win_in_row = ntohs(win_in_row);
		cooling_time = ntohl(cooling_time);
	}
};

//��������CD, MLS_ELIMINATE_LADDER_CD
struct SEliminateLadderCDTimeRes
{
	UINT32 rpcoin;						//����
	UINT16 eliminate_count;				//����cd����

	void hton()
	{
		rpcoin = htonl(rpcoin);
		eliminate_count = htons(eliminate_count);
	}

	void ntoh()
	{
		rpcoin = ntohl(rpcoin);
		eliminate_count = ntohs(eliminate_count);
	}
};

//ƥ���������, MLS_MATCHING_LADDER
struct SMatchingLadderItem
{
	UINT32 roleId;		//��ɫID
	UINT32 rank;		//����
	bool robot;			//�Ƿ��ǻ�����
	BYTE level;			//�ȼ�
	BYTE actortype;		//ְҵ
	UINT32 cp;			//ս��
	char nick[33];		//�ǳ�

	void hton()
	{
		cp = htonl(cp);
		rank = htonl(rank);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		cp = ntohl(cp);
		rank = ntohl(rank);
		roleId = ntohl(roleId);
	}
};

struct SMatchingLadderRes
{
	UINT32 rpcoin;						//����
	UINT16 refresh_count;				//ƥ�����
	BYTE num;
	SMatchingLadderItem data[1];

	void hton()
	{
		for(int i=0;i<num;i++)
			data[i].hton();
		rpcoin = htonl(rpcoin);
		refresh_count = htons(refresh_count);
	}

	void ntoh()
	{
		for(int i=0;i<num;i++)
			data[i].ntoh();
		rpcoin = ntohl(rpcoin);
		refresh_count = ntohs(refresh_count);
	}
};

//��ս�������, MLS_CHALLENGE_LADDER
struct SChallengeLadderReq
{
	UINT32 targetId;					//Ŀ��ID
	UINT32 target_rank;					//Ŀ������

	void hton()
	{
		targetId = htonl(targetId);
		target_rank = htonl(target_rank);
	}

	void ntoh()
	{
		targetId = ntohl(targetId);
		target_rank = ntohl(target_rank);
	}
};

struct SChallengeLadderRes
{
	UINT16 challenge_count;				//��ս����
	SMatchingLadderItem player_info;	//�����Ϣ

	void hton()
	{
		challenge_count = htons(challenge_count);
	}

	void ntoh()
	{
		challenge_count = ntohs(challenge_count);
	}
};

//���������ս
//struct SFinishLadderChallengeReq
//{
//	BYTE finishCode;					//�����(0-�ɹ�������-ʧ��)
//};
//
//struct SFinishLadderChallengeRes
//{
//	UINT32 rank;						//����
//	UINT16 win_in_row;					//��ʤ����
//	UINT32 cooling_time;				//��ս��ȴʱ��
//	
//	void hton()
//	{
//		rank = htonl(rank);
//		win_in_row = htons(win_in_row);
//		cooling_time = htonl(cooling_time);
//	}
//
//	void ntoh()
//	{
//		rank = ntohl(rank);
//		win_in_row = ntohs(win_in_row);
//		cooling_time = ntohl(cooling_time);
//	}
//};

//ˢ���������У�MLS_REFRESH_LADDER_RANK
struct SRefreshLadderRankRes
{
	BYTE num;
	SMatchingLadderItem data[1];

	void hton()
	{
		for(int i=0;i<num;i++)
			data[i].hton();
	}

	void ntoh()
	{
		for(int i=0;i<num;i++)
			data[i].ntoh();
	}
};

struct SRoleOccu
{
	UINT32 roleId;			//��ɫid
	BYTE occuId;			//ְҵid

	void hton()
	{
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		roleId = ntohl(roleId);
	}
};

//��ѯ���н�ɫ��MLS_QUERY_ALL_ROLES
struct SQueryAllRolesRes
{
	BYTE num;
	SRoleOccu data[1];

	void hton()
	{
		for(int i=0;i<num;i++)
			data[i].hton();
	}

	void ntoh()
	{
		for(int i=0;i<num;i++)
			data[i].ntoh();
	}
};

//��ѯӵ�еĻ�������ͣ�MLS_QUERY_CAN_FINISH_ACTIVITY_INST
struct SActivityInstItem
{
	UINT32 instId;			// ����id
	BYTE num;				// ʣ�����

	void hton()
	{
		instId = htonl(instId);
	}

	void ntoh()
	{
		instId = ntohl(instId);
	}
};

struct SQueryHasActivityInstTypeRes
{
	BYTE num;
	SActivityInstItem data[1];

	void hton()
	{
		for(int i=0;i<num;i++)
			data[i].hton();
	}

	void ntoh()
	{
		for(int i=0;i<num;i++)
			data[i].ntoh();
	}
};

//��ѯ�����ɫ, MLS_QUERY_WEEKFREE_ACTOR
struct SQueryWeekFreeActorRes
{
	BYTE actorType;
};

//��ѯ���������������, MLS_QUERY_COMPMODE_ENTER_NUM
struct SQueryCompEnterNumRes
{
	UINT16 enter_num;		// �������
	UINT16 buy_num;			// vip�������

	void hton()
	{
		buy_num = htons(buy_num);
		enter_num = htons(enter_num);
	}

	void ntoh()
	{
		buy_num = ntohs(buy_num);
		enter_num = ntohs(enter_num);
	}
};

//���򾺼�ģʽ�������, MLS_BUY_COMPMODE_ENTER_NUM
struct SBuyCompEnterNumReq
{
	BYTE buy_mode;			// ����ģʽ��1��ʾ����һ�Σ�2��ʾ�������У�
};

struct SBuyCompEnterNumRes
{
	UINT16 buy_num;			// vip�������
	UINT32 rpcoin;			// ����

	void hton()
	{
		buy_num = htons(buy_num);
		rpcoin = htonl(rpcoin);
	}

	void ntoh()
	{
		buy_num = ntohs(buy_num);
		rpcoin = htonl(rpcoin);
	}
};

//��ѯ1v1ģʽ�������, MLS_QUERY_1V1_ENTER_NUM
struct SQuery1V1EnterNumRes
{
	UINT16 enter_num;		// �������
	UINT16 buy_num;			// �������

	void hton()
	{
		buy_num = htons(buy_num);
		enter_num = htons(enter_num);
	}

	void ntoh()
	{
		buy_num = ntohs(buy_num);
		enter_num = ntohs(enter_num);
	}
};

//����1vs1ģʽ�������, MLS_BUY_1V1_ENTER_NUM
struct SBuy1V1EnterNumReq
{
	UINT16 buy_num;

	void hton()
	{
		buy_num = htons(buy_num);
	}

	void ntoh()
	{
		buy_num = ntohs(buy_num);
	}
};

struct SBuy1V1EnterNumRes
{
	UINT16 buy_num;			// vip�������
	UINT32 rpcoin;			// ����

	void hton()
	{
		buy_num = htons(buy_num);
		rpcoin = htonl(rpcoin);
	}

	void ntoh()
	{
		buy_num = ntohs(buy_num);
		rpcoin = htonl(rpcoin);
	}
};

//��Ϣ��ʾ, MLS_MESSAGETIPS_NOTIFY
struct SMessageTipsItem
{
	BYTE type;					// ��Ϣ����
	UINT16 title;				// ��Ϣ���ⳤ��
	UINT16 content;				// ��Ϣ���ݳ���
	char data[1];				// title + content ������β����

	void hton()
	{
		title = htons(title);
		content = htons(content);
	}

	void ntoh()
	{
		title = ntohs(title);
		content = htons(content);
	}
};

struct SMessageTipsNotify
{
	BYTE num;					// ��Ϣ����
	char data[1];				// ��Ϣ���ݣ����SMessageTipsItem����
};

///////////
// add by hxw 20151014
///���й�����������
const int RANK_REWARD_TYPE_NUM = 4;

///���н�����ȡ��ѯ
struct SQueryRankRewardGet
{
	UINT16 rankid;	//����ID
	BOOL bGet;	//�Ƿ��Ѿ���ȡ
	void hton()
	{
		rankid = htons(rankid);	
	}
	void ntoh()
	{
		rankid = ntohs(rankid);	
	}
};

///������ͼ���в�ѯ
const int MAX_INS_RANK_NUM = 10;
struct SInsnumRank
{
	UINT16	rankid;		//����ID
	UINT16	num;		//��ͼ����,����ǵȼ����У��򷵻صȼ�
	char	nick[33];	//����ǳ�[33];
	BYTE	occuId;		//ְҵid	
	void hton()
	{
		rankid = htons(rankid);	
		num = htons(num);	
	}
	void ntoh()
	{
		rankid = ntohs(rankid);	
		num = ntohs(num);	
	}

};

//��ѯ������ս�������а�
struct SQueryInsnumRanksInfo{
	SInsnumRank m_vInsRanks[MAX_INS_RANK_NUM];
	void hton()
	{
		for(int i=0;i<MAX_INS_RANK_NUM;i++)
			m_vInsRanks[i].hton();
	}
	void ntoh()
	{
		for(int i=0;i<MAX_INS_RANK_NUM;i++)
			m_vInsRanks[i].ntoh();
	}
};

///��ȡ���н���
struct SGetRankRewardRes
{
	UINT32 rpcoin;			//����
	UINT32 vit;				//����
	SBigNumber exp;			//����
	SBigNumber gold;		//���
	BYTE num;				//����������
	SPropertyAttr data[1];	//������������

	void hton()
	{
		rpcoin = htonl(rpcoin);
		vit = htonl(vit);
		for(BYTE i = 0; i < num; ++i)
			data[i].hton();
	}

	void ntoh()
	{
		rpcoin = ntohl(rpcoin);
		vit = ntohl(vit);
		for(BYTE i = 0; i < num; ++i)
			data[i].ntoh();
	}
};
//end


//add by hxw 20151015

const int STRONGER_REWARD_NUM = 20;
//��ȡǿ�߽�����Ϣ����,������ȡ����ID
struct SQueryStrongerInfos
{
	UINT16 rewards[STRONGER_REWARD_NUM];
	void hton()
	{
		for(int i=0;i<STRONGER_REWARD_NUM;i++)
			rewards[i] = htons(rewards[i]);
	}
	void ntoh()
	{
		for(int i=0;i<STRONGER_REWARD_NUM;i++)
			rewards[i] = ntohs(rewards[i]);
	}
};


//end at 20151015
//end
//add by hxw 20151020
//��ѯ���߽�����ȡ״��
const int ONLINE_NUM = 10;
struct SQueryOnlineInfos
{
	UINT16 rewards[ONLINE_NUM];//���ص�������ȡ�����߽���
	UINT32 time;//���ص�������ʱ�� ��λ��
	void hton()
	{
		for(int i=0;i<ONLINE_NUM;i++)
			rewards[i] = htons(rewards[i]);
		time = htons(time);
	}
	void ntoh()
	{
		for(int i=0;i<ONLINE_NUM;i++)
			rewards[i] = ntohs(rewards[i]);
		time = ntohs(time);
	}
};


//end at 20151015

////add by hxw 20151021

struct SGetStrongerReq
{	
	UINT16 rewardId; //����ID
	void hton()	
	{
		rewardId = htons(rewardId);
	}
	void ntoh() 
	{		
		rewardId = ntohs(rewardId);
	}
};

struct SGetOnlineRewardReq
{
	UINT16 rewardId; //����ID
	void hton()	
	{
		rewardId = htons(rewardId);
	}
	void ntoh() 
	{		
		rewardId = ntohs(rewardId);
	}
};
//end

#pragma pack(pop)

#endif	//_SGSMLogic_h__
