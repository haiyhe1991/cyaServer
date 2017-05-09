#ifndef __SERVICE_MLOGIC_H__
#define __SERVICE_MLOGIC_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "ServiceCommon.h"

/***********************************************************************************
							主逻辑服务器相关数据结构
***********************************************************************************/
#define MAX_UTF8_NICK_LEN			128		//昵称最大长度(utf8)
#define MAX_UTF8_EMAIL_CAPTION_LEN	256		//邮件标题(utf8)
#define MAX_UTF8_EMAIL_BODY_LEN		1024	//邮件正文(utf8)
#define MAX_HAS_DRESS_NUM			1000	//最多拥有时装套数
#define MAX_TASK_TARGET_NUM			5		//查询已接受任务, MLS_QUERY_ACCEPTED_TASK

#pragma pack(push, 1)

//宝箱类型
enum ESGSChestConditionType
{
	ESGS_CONDITION_CHEST_SENIOR = 1,		//高级宝箱
	ESGS_CONDITION_CHEST_LOWER = 2,			//低级宝箱
};

//奖励条件类型
enum ESGSRewardConditionType
{
	ESGS_CONDITION_SURPLUS_TIME = 1,		//剩余时间
	ESGS_CONDITION_RESIST_NUMBER,			//抵挡次数
	ESGS_CONDITION_KILL_NUMBER,				//击杀数量
	ESGS_CONDITION_HURT_EVERY,				//每造成伤害
	ESGS_CONDITION_RANKING,					//排名
	ESGS_CONDITION_CHEST,					//宝箱
	ESGS_CONDITION_LEVEL_GAP,				//等级差
	ESGS_CONDITION_CP_GAP,					//战力差
	ESGS_CONDITION_LAST_STRAW,				//最后一击
	ESGS_CONDITION_HURT_EXTREMELY,			//伤害万分比
	ESGS_CONDITION_HIGH_LADDER_RANK,		//天梯排名
	ESGS_CONDITION_HIGH_LADDER_CONTINUOUS_VICTORY_COUNT,	//天梯连胜次数
	ESGS_CONDITION_MAN_MACHINE_TO_WAR,		//人机对战
	ESGS_CONDITION_ONE_ON_ONE,				//1v1
	ESGS_CONDITION_COMPETITIVE_RANKING,		//竞技排名（大于等于）
};

//物品类型
enum ESGSPropType
{
	ESGS_PROP_EQUIP = 1,		//装备
	ESGS_PROP_GEM = 2,			//宝石
	ESGS_PROP_MATERIAL = 3,		//材料
	ESGS_PROP_FRAGMENT = 4,		//碎片
	ESGS_PROP_DRESS = 5,		//时装
	ESGS_PROP_GOLD = 6,			//游戏币
	ESGS_PROP_TOKEN = 7,		//代币
	ESGS_PROP_VIT = 8,			//体力
	ESGS_PROP_EXP = 9,			//经验
	ESGS_PROP_DROP_ID = 10,		//掉落ID
	ESGS_PROP_MAGIC = 11,		//魔导器
	ESGS_PROP_CHAIN_SOUL_FRAGMENT = 12, //炼魂碎片
};

//装备类型
enum ESGSEquipType
{  
	ESGS_EQUIP_WEAPON = 1,			// 武器 
	ESGS_EQUIP_ARMOR = 2,			// 盔甲
	ESGS_EQUIP_HAND	= 3,			// 护手
	ESGS_EQUIP_SHOES = 4,			// 鞋子 
	ESGS_EQUIP_RING	= 5,			// 戒指 
	ESGS_EQUIP_NECKLACE	= 6			// 项链
};

//装备附加属性类型
enum ESGSEquipAttachType
{
	ESGS_EQUIP_ATTACH_HP = 1,			//hp
	ESGS_EQUIP_ATTACH_MP = 2,			//mp
	ESGS_EQUIP_ATTACH_ATTACK = 3,		//攻击
	ESGS_EQUIP_ATTACH_DEF = 4,			//防御
	ESGS_EQUIP_ATTACH_CRIT = 5,			//暴击
	ESGS_EQUIP_ATTACH_TENACITY = 6,		//韧性
};

//装备品质类型
enum ESGSEquipQualityType
{
	ESGS_EQUIP_QUALITY_WHITE = 1,	//白
	ESGS_EQUIP_QUALITY_BLUE = 2,	//蓝
	ESGS_EQUIP_QUALITY_PRUPLE = 3,	//紫
	ESGS_EQUIP_QUALITY_ORANGE = 4,	//橙
	ESGS_EQUIP_QUALITY_GOLD = 5,	//金
};

//材料类型
enum ESGSMaterialType
{
	ESGS_MATERIAL_STRENGTHEN_STONE = 31002,		//强化石(强化)
	ESGS_MATERIAL_VEINS_STAR_STONE = 31001,		//纹星石(升星)
	ESGS_MATERIAL_BRILLIANT_STONE = 31102,		//辉煌石(升星辅助,提高成功率)
	ESGS_MATERIAL_SHINE_STONE = 31101,			//光芒石(升星辅助,防破损)
	ESGS_MATERIAL_REVIVE_COIN = 31801,			//复活币
	ESGS_MATERIAL_VIT_POSION_SMALL = 31802,		//体力药水(小)
	ESGS_MATERIAL_VIT_POSION_MID = 31803,		//体力药水(中)
	ESGS_MATERIAL_VIT_POSION_BIG = 31804,		//体力药水(大)
	ESGS_MATERIAL_SWEEP_SCROLL = 31805,			//扫荡卷轴
	ESGS_MATERIAL_BACKPACK_EXTEND = 31806,		//背包扩展
	ESGS_MATERIAL_LUCKY_COIN = 31807,			//幸运币
};
//是否体力药水
#define IS_VIT_POSION(id)	(id == ESGS_MATERIAL_VIT_POSION_BIG || id == ESGS_MATERIAL_VIT_POSION_MID || id == ESGS_MATERIAL_VIT_POSION_SMALL)

//任务目标类型
enum ESGSTaskTargetType
{
	ESGS_TASK_TARGET_PASS_STAGE = 1,			//通关
	ESGS_TASK_TARGET_KILL_MONSTER = 2,			//杀怪
	ESGS_TASK_TARGET_DESTROY_SCENE = 3,			//破坏场景物
	ESGS_TASK_TARGET_FETCH_PROP = 4,			//获取道具
	ESGS_TASK_TARGET_LIMIT_TIME = 5,			//限时通关
	ESGS_TASK_TARGET_GOT_AWAY = 6,				//逃离
	ESGS_TASK_TARGET_ESCORT = 7,				//护送
	ESGS_TASK_TARGET_FIND_NPC = 8,				//找到NPC
	ESGS_TASK_TARGET_PASS_NO_HURT = 9,			//无伤通关
	ESGS_TASK_TARGET_PASS_NO_BLOOD = 10,		//不使用血瓶通关
	ESGS_TASK_TARGET_PASS_NO_DIE = 11,			//不死通关
	ESGS_TASK_TARGET_BLOCK_TIMES = 12,			//格挡多少次
	ESGS_TASK_TARGET_COMBO_NUM_REACH = 13,		//达到足够多的连击数
	ESGS_TASK_TARGET_KEEP_BLOOD = 14,			//血量保持在多少以下/上通关（25%/50%/75%)
	ESGS_TASK_TARGET_CASH = 15,					//兑换
	ESGS_TASK_TARGET_COMPOSE = 16,				//合成
	ESGS_TASK_TARGET_STRENGTHEN = 17,			//强化
	ESGS_TASK_TARGET_RISE_STAR = 18,			//升星
	ESGS_TASK_TARGET_ROLE_LV_REACH	= 19,		//角色等级达到
	ESGS_TASK_TARGET_KILL_MONSTER_LIMIT_TIME = 20,	//限时杀怪
	ESGS_TASK_TARGET_FETCH_PROP_LIMIT_TIME = 21,	//限时获取道具（复数）
	ESGS_TASK_TARGET_DESTROY_SCENE_LIMIT_TIME = 22,	//限时破坏场景物（复数）
	ESGS_TASK_TARGET_NO_USE_SPECIAL_SKILL = 23,		//不使用特殊技能通关
	ESGS_TASK_TARGET_KILL_ALL_MONSTER = 24,			//杀死所有怪
	ESGS_TASK_TARGET_DESTROY_ALL_SCENE = 25,		//破坏所有场景物
	ESGS_TASK_TARGET_PASS_STAGE_NUM = 26,			//通关任意普通副本n次
	ESGS_TASK_TARGET_PASS_RAND_STAGE_NUM = 27,		//通关随机普通副本n次
	ESGS_TASK_TARGET_LOGIN_RECEIVE_VIT = 28,		//登陆领取体力
	ESGS_TASK_TARGET_OPEN_CHEST_BOX_NUM = 29,		//手动开启宝箱
};

//系统任务类型
enum ESGSSystaskType
{
	ESGS_SYS_MAIN_TASK = 1,		//主线
	ESGS_SYS_BRANCH_TASK = 2,	//支线
	ESGS_SYS_DAILY_TASK = 3,	//日常
	ESGS_SYS_REWARD_TASK = 4,	//赏金
	ESGS_SYS_RAND_TASK = 5,		//随机
};

//奖励类型
// enum ESGSRewardType
// {
// 	ESGS_REWARD_GOLD = 0,		//游戏币
// 	ESGS_REWARD_TOKEN = 1,		//代币
// 	ESGS_REWARD_VIT = 2,		//体力
// 	ESGS_REWARD_EXP = 3,		//经验
// 	ESGS_REWARD_EQUIP = 4,		//装备
// 	ESGS_REWARD_GEM = 5,		//宝石
// 	ESGS_REWARD_MATERIAL = 6,	//材料
// 	ESGS_REWARD_FRAGMENT = 7,	//碎片
// };

//宝石类型
enum ESGSGemType
{
	ESGS_GEM_HP = 1,			//HP宝石
	ESGS_GEM_MP = 2,			//MP宝石
	ESGS_GEM_ATTACK = 3,		//攻击宝石
	ESGS_GEM_DEF = 4,			//防御宝石
	ESGS_GEM_CRIT = 5,			//暴击宝石
	ESGS_GEM_TENACITY = 6,		//韧性宝石
};

//宝石孔编号
enum ESGSGemHoleCode
{
	ESGS_GEM_HOLE_0 = 0,	//1号孔
	ESGS_GEM_HOLE_1 = 1,	//2号孔
	ESGS_GEM_HOLE_2 = 2,	//3号孔
	ESGS_GEM_HOLE_3 = 3,	//4号孔
};

#define IsValidGemHole(hole) ((hole) >= ESGS_GEM_HOLE_0 && (hole) <= ESGS_GEM_HOLE_3)

//装备属性类型
enum ESGSEquipAttrType
{
	ESGS_INLAID_GEM = 1,	//镶嵌宝石
};

//游戏类型
enum ESGSGameMode
{
	ESGS_GAME_CHALLENGE = 1,	//挑战
	ESGS_GAME_EXPLORE = 2,		//探索
	ESGS_GAME_SPEED = 3,		//竞速
};

//副本类型
enum ESGSInstMode
{
	ESGS_INST_MAIN = 1,			//主线
	ESGS_INST_BRANCH = 2,		//支线
	ESGS_INST_FIND_CHEST = 3,	//找宝箱（活动）
	ESGS_INST_GUARD_ICON = 4,	//守卫圣像（活动）
	ESGS_INST_WORLD_BOSS = 5,	//世界Boss
	ESGS_INST_MAN_MACHINE_TO_WAR = 6,	//人机对战
	ESGS_INST_ONE_VS_ONE = 7,			//1v1
	ESGS_INST_HIGH_LADDER = 8,			//天梯
	ESGS_INST_TEAM = 9,					//组队副本
	ESGS_INST_TRAINING = 10,			//练习模式
	ESGS_INST_ACTIVITY_INST = 11,		//活动副本（活动）
	ESGS_INST_COMPETITIVEMODE = 12,		//竞技模式
};


//系统公告类型
enum ESGSNoticeType
{
	ESGS_NOTICE_PERSONAL_RAND_INST_OPEN = 1,	//单人随机副本开启
	ESGS_NOTICE_PERSONAL_RAND_INST_CLOSE = 2,	//单人随机副本关闭
	ESGS_NOTICE_FRAGMENT_COMPOSE = 3,			//碎片合成
	ESGS_NOTICE_SYSTEM = 4,						//系统公告
	ESGS_NOTICE_WORLDBOSS_END = 5,				//世界Boss结束
};

//怪类型
enum ESGSMonsterType
{
	ESGS_MONSTER_COMMON = 1,		//普通
	ESGS_MONSTER_CREAM = 2,			//精英
	ESGS_MONSTER_SMALL_BOSS = 3,	//小boss
	ESGS_MONSTER_BIG_BOSS = 4,		//大boss
	ESGS_MONSTER_RAGE = 5,			//狂暴
	ESGS_MONSTER_MEAT = 6,			//肉盾
	ESGS_MONSTER_WORLDBOSS = 10,	//世界Boss
};

//邮件类型
enum ESGSEmailType
{
	ESGS_EMAIL_SYSTEM = 0,			//系统邮件
	ESGS_EMAIL_INST_DROP = 1,		//副本掉落
	ESGS_EMAIL_TASK_REWARD = 2,		//任务奖励
	ESGS_EMAIL_FIRST_RECHARGE = 3,	//首充赠送
	ESGS_EMAIL_VIP_REWARD = 4,		//VIP奖励
	ESGS_EMAIL_SIGN_IN_REWARD = 5,	//签到奖励
	ESGS_EMAIL_ACTIVITY_REWARD = 6,	//活动奖励
	ESGS_EMAIL_GIFTBAG_REWARD = 7,	//礼包奖励
	ESGS_EMAIL_NEW_ACTOR_REWARD = 8,	//新角色奖励
	ESGS_EMAIL_EXTRACT_REWARD = 9,	//抽奖
	ESGS_EMAIL_RECHARGE_RETURN = 10,	//充值返还
	ESGS_EMAIL_WORLD_BOSS_REWARD = 11,	//世界Boss奖励
	ESGS_EMAIL_ACHIEVEMENT_REWARD = 12,	//成就奖励
	ESGS_EMAIL_LIMITEDTIME_ACTIVITY_REWARD = 13,	//限时活动奖励
	ESGS_EMAIL_LADDER_REWARD = 14,	//天梯奖励
	ESGS_EMAIL_COMPETITIVE_REWARD = 15,	//竞技模式奖励
};

//装备合成条件限制类型
enum ESGSEquipComposeConditionType
{
	ESGS_EQUIP_COMPOSE_ROLE_LEVEL = 1,		//角色等级
	ESGS_EQUIP_COMPOSE_DATE_TIME = 2,		//时间
	ESGS_EQUIP_COMPOSE_MAGIC = 3,			//魔导器 
	ESGS_EQUIP_COMPOSE_PROP = 4,			//物品
};

//解锁类型
enum ESGSUnlockType
{
	ESGS_UNLOCK_ROLE_JOB = 1,	//角色职业
	ESGS_UNLOCK_STAGE = 2,		//关卡
	ESGS_UNLOCK_SKILL = 3,		//技能
	ESGS_UNLOCK_LEVEL = 4,		//等级
};

//充值状态
enum ESGSRechargeStatus
{
	ESGS_WAIT_CONFIRM = 0,	//等待确认
	ESGS_SYS_CONFIRMED = 1,	//系统确认
	ESGS_MANUAL_CONFIRMED = 2,	//手动确认
};

//人物属性点
struct SRoleAttrPoint
{
	UINT32 u32HP;
	UINT32 u32MP;
	UINT32 crit;			//暴击
	UINT32 tenacity;		//韧性
	UINT32 attack;			//攻击
	UINT32 def;				//防御

	SRoleAttrPoint()
	{
		u32HP = 0;
		u32MP = 0;
		crit = 0;			//暴击
		tenacity = 0;		//韧性
		attack = 0;	//元素攻击
		def = 0;	//元素抗性
	}
};

//消费/产出货币类型
enum ESGSConsumeProduceType
{
	ESGS_GOLD = 0,
	ESGS_TOKEN = 1
};

struct SGSPosition	//位置
{
	UINT16 owncopy;		//所在副本;
	BYTE   monsterArea;	//刷怪区域点
	BYTE   reserve[13];	//保留
	void hton()
		{	owncopy = htons(owncopy);	}
	void ntoh()
		{	owncopy = ntohs(owncopy);	}
};

struct SBasicAttr	//物品基本属性
{
	UINT16 valType;	//属性值类型;
	UINT32 val;		//属性值;
	BYTE   type;	//属性类型;
	BYTE   source;	//属性来源;
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

//装备位属性
#define MAX_GEM_HOLE_NUM 4
struct SEquipPosAttr
{
	BYTE pos;		//装备位
	BYTE lv;		//强化等级
	BYTE star;		//星级
	UINT16 gem[MAX_GEM_HOLE_NUM];	//镶嵌宝石id

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

struct SMonsterDropPropItem	//掉落物品信息
{
	BYTE id;		//编号
	BYTE type;		//类型(ESGSPropType)
	UINT16 propId;	//物品id
	UINT32 num;		//物品数量
	
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
	BYTE id;		//物品编号
	BYTE type;		//类型(ESGSPropType)
	UINT16 propId;	//物品id
	UINT32 num;		//物品数量
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
	UINT16 propId;	//物品ID;
	BYTE   type;	//物品类型 ESGSPropType
	UINT32 num;		//物品数量

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

//#define MAX_PROP_ATTR_NUM			10	//道具属性个数
struct SPropertyAttr		//-道具属性
{
	UINT32 id;		//唯一ID;
	UINT16 propId;	//物品ID;
	BYTE   type;	//物品类型 ESGSPropType
	BYTE   suitId;  //套装id;(装备才有的属性)
	BYTE   level;	//物品等级;(宝石,装备才有的属性)
	BYTE   grade;	//物品品级(若type==ESGS_PROP_GEM,则表示宝石类型)
	BYTE   star;	//物品星级;装备才有的属性
	BYTE   num;		//物品数量;
	BYTE   pos;		//物品位置(装备才有的属性, >0表示装备位置, ESGSEquipType)
	//SBasicAttr basicAttr[MAX_PROP_ATTR_NUM]; //属性,道具属性[10];
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

//装备碎片
struct SEquipFragmentAttr
{
	UINT16 fragmentId;	//碎片id
	UINT16 num;			//数量
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

//炼魂碎片
struct SChainSoulFragmentAttr
{
	UINT32 fragmentId;	//碎片id
	UINT32 num;			//数量
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

//消耗物品
struct SConsumeProperty
{
	UINT32 id;	//唯一id
	BYTE   num;	//数量
	void hton()
		{	id = htonl(id);		}
	void ntoh()
		{	id = ntohl(id);		}
};

// 查询分区角色信息, MLS_QUERY_ROLES
struct SQueryRolesReq	//请求
{
	UINT32 userId;	//账户ID
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

struct SQueryRoleAttr	//查询角色属性
{
	UINT32 id;			//角色ID;
	BYTE occuId;		//职业id;
	BYTE sex;			//性别
	UINT16 dress;		//角色时装;
	UINT16 mainWeapon;	//角色主手武器;
	UINT16 subWeapon;	//角色副手武器;
	char   nick[33];	//昵称;
	BYTE   level;		//等级;
	SBigNumber gold;	//游戏币;
	UINT32	rpcoin;		//代币;
	UINT32  cashs;		//冲值金额
	SBigNumber exp;		//经验;
	UINT32	cashcount;	//冲值总金额
	UINT16	owncopy;	//所在副本;
	char lastEnterTime[SGS_DATE_TIME_LEN];	//最后一次进入游戏时间(1970-01-01 00:00:00)

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

struct SQueryRolesRes	//响应
{
	BYTE rolesCount;	//角色个数
	SQueryRoleAttr roles[1]; //角色信息;
};

//进入角色, MLS_ENTER_ROLE
struct SEnterRoleReq	//user->linker
{
	UINT32 roleId;	//角色id
	void hton()
		{	roleId = htonl(roleId);	}
	void ntoh()
		{	roleId = ntohl(roleId);	}
};

struct SSkillMoveInfo	//技能/连招信息
{
	UINT16 id;		//技能/连招编号
	BYTE   key;		//客户端绑定key(0-未绑定,1-已绑定)
	BYTE   level;	//技能等级

	void hton()
		{	id = htons(id);		}

	void ntoh()
		{	id = ntohs(id);		}
};

struct SEnterRoleRes
{
	UINT32 userId;	//账户ID;
	UINT32 roleId;	//角色ID;
	char   nick[33];//昵称[33];
	UINT16 titleId;	//称号id;
	BYTE   roleLevel;	//角色等级
	SBigNumber gold;// 游戏币
	SBigNumber exp;		//经验
	UINT32 cash;		//现金
	BYTE occuId;	//职业id
	BYTE sex;		//性别
	UINT32 cashcount;	//充值总额
	UINT32 guildId;		//公会ID
	UINT32 hp;			//生命
	UINT32 mp;			//魔力
	UINT32 vit;		//体力
	UINT16 ap;			//剩余属性点数(数据库里是int)
	UINT32 attack;       //攻击
	UINT32 def;          //防御
	BYTE packSize;		//背包大小
	BYTE wareSize;		//仓库大小
	BYTE unReadEmails;	//未读邮件数量;
	UINT32 bt;			//当前战斗力;
	SGSPosition pos;	//所在位置
	BYTE factionId;	//阵营id
	BYTE skillNum;	//技能/连招个数
	SSkillMoveInfo data[1];	//数据
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

//查询背包, MLS_QUERY_BACKPACK
struct SQueryBackpackReq	//--请求包
{
	BYTE from;	//开始位置;
	BYTE num;	//数量;
};

struct SQueryBackpackRes	//--返回包
{
	BYTE total;		//背包道具总数量;
	BYTE curNum;	//当前放回数量;
	SPropertyAttr props[1];	////array<道具>     返回的道具信息;
};

//查询角色外形信息	MLS_QUERY_ROLE_APPEARANCE
struct SQueryRoleAppearanceReq
{
	UINT32 roleId;	//角色Id
	void hton()
		{	roleId = htonl(roleId);	}

	void ntoh()
		{	roleId = ntohl(roleId);	}
};

struct SQueryRoleAppearanceRes
{
	UINT32 roleId;		//角色Id
	char   nick[33];	//昵称[33];
	BYTE   level;		//角色等级
	UINT16 dressId;		//时装id
	UINT16 titleId;		//穿戴称号id
	BYTE   num;			//已经装备装备数量
	SPropertyAttr props[1];	//已经装备装备;
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

//查询角色详细信息	MLS_QUERY_ROLE_EXPLICIT
struct SQueryRoleExplicitReq
{
	UINT32 roleId;	//角色Id
	void hton()
		{	roleId = htonl(roleId);	}

	void ntoh()
		{	roleId = ntohl(roleId);	}
};

struct SQueryRoleExplicitRes
{
	UINT32 roleId;			//角色Id
	char   roleNick[33];	//角色昵称[33];
	BYTE   roleLevel;		//角色等级
	UINT16 titleId;		//称号id;
	SBigNumber gold;	//游戏币
	UINT32 guildId;		//公会ID;
	char   guildNick[33];	//公会名称[33];
	SBigNumber exp;		//经验
	UINT32 cash;		//现金
	BYTE occuId;	//职业id
	BYTE sex;		//性别
	UINT32 cashcount;	//充值总额
	UINT32 hp;			//生命
	UINT32 mp;			//魔力
	UINT32 vit;		//体力
	UINT16 ap;			//剩余属性点数(数据库里是int)
	UINT32 attack;       //攻击
	UINT32 def;          //防御
	BYTE packSize;		//背包大小
	BYTE wareSize;		//仓库大小
	BYTE unReadEmails;	//未读邮件数量;
	SGSPosition pos;	//所在位置
	UINT32 cp;			//当前战斗力;
	BYTE   num;			//技能数量
	SSkillMoveInfo skills[1];	//技能数据;
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

//查询成就, MLS_QUERY_ACHIEVEMENT
struct SQueryAchievementReq	//请求
{
	BYTE type;								//1成长之路，2冒险历程，3完美技巧ID
	UINT16 start_pos;						//开始查询位置

	void hton()
	{
		start_pos = htons(start_pos);
	}

	void ntoh()
	{
		start_pos = ntohs(start_pos);
	}
};

struct SAchievementInfo		//成就信息
{
	UINT32 id;								//成就ID
	UINT32 point;							//完成点数
	BYTE status;							//成就状态，1表示正在进行，2表示已达成，3表示已领取

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

struct SQueryAchievementRes		//返回
{
	bool done;								//是否接收完所有数据
	UINT32 point;							//成就点数
	UINT16 num;								//成就数量
	SAchievementInfo data[1];				//成就信息

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

//创建角色, MLS_CREATE_ROLE
struct SCreateRoleReq	//请求
{
	BYTE occuId;		//职业id;
	BYTE sex;			//性别
	char nick[33];	//创建昵称[33];
};

struct SCreateRoleRes	//回复
{
	UINT32 id;			//角色ID;
	BYTE occuId;		//职业id;
	BYTE sex;			//性别
	UINT16 dress;		//角色时装;
	UINT16 mainWeapon;		//角色主手武器;
	UINT16 subWeapon;		//角色副手武器;
	char   nick[33];	//昵称[33];
	BYTE   level;		//等级;
	SBigNumber gold;	//游戏币;
	UINT32	rpcoin;		//代币;
	SBigNumber exp;		//经验;
	UINT16 owncopy;		//所在副本;

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

//查询好友, MLS_QUERY_FRIEND
struct SQueryFriendsReq
{
	BYTE from;	//开始位置;
	BYTE num;//数量;
}; 

struct SFriendInfo	//好友信息
{
	UINT32 userId;		//账户ID;
	UINT32 roleId;		//角色ID;
	BYTE   occuId;		//职业id;
	BYTE   sex;			//性别
	char   roleNick[33];//角色昵称[33];
	BYTE   roleLevel;	//角色等级;
	UINT32 guildId;		//所在公会ID;
	char   guildName[33];	//所在公会名称[33];
	UINT32 cp;				//战力
	BYTE   online;		//是否在线;
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
	BYTE total;				//好友总数;
	BYTE num;				//返回好友信息数;
	SFriendInfo friends[1];	//好友列表;
};

//查询好友申请, MLS_QUERY_FRIEND_APPLY
struct SQueryFriendApplyReq
{
	BYTE from;	//开始位置;
	BYTE num;	//数量;
};

struct SQueryFriendApplyRes
{
	BYTE total;				//好友总数;
	BYTE num;				//返回好友信息数;
	SFriendInfo data[1];	//好友申请列表;
};

//添加好友, MLS_ADD_FRIEND
struct SAddFriendReq
{
	char nick[33];	//目标好友昵称[33];
};

//好友申请/同意/拒绝通知, MLS_FRIEND_APPLY_NOTIFY/MLS_AGREE_FRIEND_APPLY_NOTIFY/MLS_REFUSE_FRIEND_APPLY_NOTIFY(服务器推送)
struct SFriendApplyNotify
{
	SFriendInfo friendInfo;
	void hton()
		{	friendInfo.hton();	}

	void ntoh()
		{	friendInfo.ntoh();	}
};

//好友申请确认, MLS_FRIEND_APPLY_CONFIRM
struct SFriendApplyConfirmReq
{
	UINT32 friendUserId;	//好友账号id
	UINT32 friendRoleId;	//好友角色id
	BYTE   status;			//0-拒绝, 1-同意
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

//获取推荐好友列表, MLS_GET_RECOMMEND_FRIEND_LIST
struct SGetRecommendFriendsRes
{
	BYTE num;				//返回好友信息数;
	SFriendInfo data[1];	//好友申请列表;
};

//搜索玩家, MLS_SEARCH_PLAYER
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

//删除好友, MLS_DEL_FRIEND
struct SDelFriendReq	//请求
{
	char nick[33];	//好友角色昵称[33];
};

//好友上下线通知, MLS_FRIEND_ONLINE_NOTIFY/MLS_FRIEND_OFFLINE_NOTIFY(服务器推送)
struct SFriendOnOffLineNotify
{
	UINT32 friendRoleId;
	void hton()
		{	friendRoleId = htonl(friendRoleId);		}
	void ntoh()
		{	friendRoleId = ntohl(friendRoleId);		}
};

//好友删除通知, MLS_FRIEND_DEL_NOTIFY(服务器推送)
struct SFriendDelNotify
{
	UINT32 friendRoleId;
	void hton()
		{	friendRoleId = htonl(friendRoleId);		}
	void ntoh()
		{	friendRoleId = ntohl(friendRoleId);		}
};

struct SAcceptTaskInfo
{
	UINT16 taskId;		//任务id
	BYTE   isFinished;	//任务是否完成(1-完成, 0-未完成)
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
	BYTE num;					//任务数量;
	SAcceptTaskInfo tasks[1];	//任务
};

//更新已接任务目标, MLS_UPDATE_TASK_TARGET(该操作无返回)
struct SUpdateTaskTargetReq
{
	UINT16 taskId;		//任务id
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

//接受任务, MLS_ACCEPT_TASK
struct SAcceptTaskReq
{
	UINT16 taskId;	//任务id
	void hton()
	{	taskId = htons(taskId);	}

	void ntoh()
	{	taskId = ntohs(taskId);	}
};

struct SAcceptTaskRes
{
	UINT16 taskId;	//任务id
	void hton()
		{	taskId = htons(taskId);	}

	void ntoh()
		{	taskId = ntohs(taskId);	}
};

//完成任务, MLS_COMMIT_TASK_FINISH(该次操作无返回)
struct SFinishTaskReq
{
	UINT16 taskId;	//任务id
	void hton()
		{	taskId = htons(taskId);		}

	void ntoh()
		{	taskId = ntohs(taskId);		}
};

//完成任务失败通知, MLS_RECEIVE_FIN_TASK_FAIL_NOTIFY
struct SFinishTaskFailNotify
{
	UINT16 taskId;		//任务id;
	UINT32 target[MAX_TASK_TARGET_NUM];	//任务目标当前状态
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

//领取任务奖励, MLS_RECEIVE_TASK_REWARD
struct SReceiveTaskRewardReq
{
	UINT16 taskId;		//任务id;
	void hton()
		{	taskId = htons(taskId);		}

	void ntoh()
		{	taskId = ntohs(taskId);		}
};

struct SReceiveTaskRewardRes
{
	UINT16 taskId;		//任务id;
	SBigNumber exp;		//新拥有总的经验(当前拥有+本次所得)
	SBigNumber gold; 	//新拥有总的游戏币(当前拥有+本次所得)
	UINT32 rpcoin;     	//新拥有总的代币(当前拥有+本次所得)
	BYTE isUpgrade;		//角色是否升级(0-否, 1-是)
	BYTE modifyNum;	   	//背包更新道具数量; 
	SPropertyAttr modifyProps[1];	//array<道具>  背包修改信息;
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

//放弃任务, MLS_DROP_TASK
struct SDropTaskReq
{
	UINT16 taskId;	//任务id
	void hton()
		{	taskId = htons(taskId);	}

	void ntoh()
		{	taskId = ntohs(taskId);	}
};

struct SDropTaskRes
{
	UINT16 taskId;	//任务id
	void hton()
		{	taskId = htons(taskId);	}

	void ntoh()
		{	taskId = ntohs(taskId);	}
};

//查询已完成任务历史记录, MLS_QUERY_FIN_TASK_HISTORY
struct SQueryFinishedTaskReq
{
	UINT16 from;	//从多少开始
	UINT16 num;		//查多少条

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

//获取日常任务
struct SGetDailyTaskRes
{
	BYTE num;
	UINT16 dailyTasks[1];
};

//完成某新手引导, MLS_FINISH_ROLE_HELP
struct SFinishHelpReq
{
	UINT16 helpId;	//引导id

	void hton()
		{	helpId = htons(helpId);		}

	void ntoh()
		{	helpId = ntohs(helpId);		}
};

//查询已完成新手引导信息, MLS_QUERY_FINISHED_HELP
struct SQueryFinishedHelpRes
{
	UINT16 num;			//引导id数量;
	UINT16 data[1];	//新手引导ID;

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

//查询已完成副本信息, MLS_QUERY_FINISHED_INST
struct SQueryFinishedInstReq
{
	UINT16 from;	//开始位置;
	BYTE   num; 	//查询数量;
	void hton()
		{	from = htons(from);	}

	void ntoh()
		{	from = ntohs(from);	}
};

struct SFinishedInstInfo//完成副本历史信息
{
	UINT32	id;			//记录ID;
	UINT16  instId;		//副本ID;
	BYTE    star;		//副本评星;
	UINT16  complatedNum;		//副本完成次数
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
	UINT16 total;		//总数;
	BYTE   curNum; 		//当前数量;
	SFinishedInstInfo instInfos[1];	//array<完成副本信息>   副本信息;
	void hton()
		{	total = htons(total);	}

	void ntoh()
		{	total = ntohs(total);	}
};

//保存属性点, MLS_SAVE_ROLE_ATTR
struct SSaveRoleAttrValReq
{
	UINT16 str;				//力量
	UINT16 dex;				//敏捷
	UINT16 intelligence;	//智慧
	UINT16 sp;				//精神
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
	UINT32 hp;		//生命
	UINT32 mp;			//魔力
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

//出售物品, MLS_SELL_PROPERTY
struct SSellPropertyReq
{
	UINT32 id;	//物品唯一ID;
	BYTE   num;	//物品数量;
	void hton()
		{	id = htonl(id);	}

	void ntoh()
		{	id = ntohl(id);	}
};

struct SSellPropertyRes
{
	UINT32 sellId;		//出售记录id
	SBigNumber gold;	//当前金币总量;
	char szSellDate[SGS_DATE_TIME_LEN];	//出售时间
	BYTE num;		//背包格子变化数量
	SPropertyAttr data[1];	//背包格子变化数据
	void hton()
		{	sellId = htonl(sellId);		}

	void ntoh()
		{	sellId = ntohl(sellId);		}
};

//兑换道具, MLS_CASH_PROPERTY
struct SCashPropertyReq
{
	UINT16 targetId;	//兑换目标ID;
	void hton()
		{	targetId = htons(targetId);	}
	void ntoh()
		{	targetId = ntohs(targetId);	}
};

struct SCashPropertyRes
{
	UINT16 targetId;	//兑换目标ID;
	BYTE   packNum;		//背包更新数;
	SPropertyAttr packs[1];	//array<道具> 背包更新的道具信息;

	void hton()
		{	targetId = htons(targetId);	}
	void ntoh()
		{	targetId = ntohs(targetId);	}
};

//丢弃道具, MLS_DROP_PROPERTY
struct SDropPropertyReq
{
	BYTE from;     //0-背包, 1-仓库
	UINT32 id;     //道具唯一ID;
	BYTE num; 	   //丢弃数量数量;
	void hton()
		{	id = htonl(id);	}

	void ntoh()
		{	id = ntohl(id);	}
};

//购买背包或仓库存储空间, MLS_BUY_STORAGE_SPACE
struct SBuyStorageSpaceReq
{
	BYTE buyType;   //0.背包  1.仓库
	BYTE num;		//购买的数量		
};

struct SBuyStorageSpaceRes
{
	BYTE buyType;   //0.背包  1.仓库
	UINT32 spend;	// 购买消耗资金(代币或游戏币);
	BYTE newNum;	// 购买后背包或仓库数量;
	void hton()
		{	spend = htonl(spend);	}

	void ntoh()
		{	spend = ntohl(spend);	}
};

//进入副本, MLS_ENTER_INST
struct SEnterInstReq
{
	UINT16 instId;	// 副本ID;
	void hton()
		{	instId = htons(instId);	}

	void ntoh()
		{	instId = ntohs(instId);	}
};

struct SEnterInstRes
{
	UINT32 restVit;		//剩余体力
	BYTE   boxNum;		//宝箱数量
	BYTE   brokenNum;	//破碎物掉落数量
	char   data[1];		//宝箱数据, (宝箱ID(BYTE) + 宝箱物品数量(BYTE) + n * sizeof(SBoxDropPropItem)) * boxNum  + brokenNum * sizeof(SMonsterDropPropItem)
	void hton()
		{	restVit = htonl(restVit);	}

	void ntoh()
		{	restVit = ntohl(restVit);	}
};

//获取副本区域掉落, MLS_FETCH_INST_AREA_DROPS
struct SFetchInstAreaDropsReq
{
	UINT16 instId;	//副本ID;
	BYTE   areaId;	//区域id
	void hton()
		{	instId = htons(instId);	}

	void ntoh()
		{	instId = ntohs(instId);	}
};

struct SFetchInstAreaDropsRes
{
	BYTE   monsterDrops;	//小怪掉落数量
	BYTE   creamDrops;		//精英怪掉落数量
	BYTE   bossDrops;		//boss怪掉落数量
	SMonsterDropPropItem data[1];	//掉落数据(monsterDrops + creamDrops + bossDrops个SMonsterDropPropItem)
};

//拾取副本怪掉落, MLS_PICKUP_INST_MONSTER_DROP
//无MLS_PICKUP_INST_MONSTER_DROP命令码返回, 可能会触发角色数据更新通知
struct SPickupInstMonsterDropReq
{
	UINT16 instId;	//副本id
	BYTE   areaId;	//区域id
	BYTE   dropId;	//掉落编号(SMonsterDropPropItem:id)
	void hton()
		{	instId = htons(instId);		}
	void ntoh()
		{	instId = ntohs(instId);		}
};

//拾取副本宝箱, MLS_PICKUP_INST_BOX
//无MLS_PICKUP_INST_BOX命令码返回, 可能会触发角色数据更新通知
struct SPickupInstBoxReq
{
	UINT16 instId;	//副本id
	BYTE   boxId;	//宝箱id
	BYTE   id;		//宝箱中物品编号
	void hton()
		{	instId = htons(instId);		}
	void ntoh()
		{	instId = ntohs(instId);		}
};

//拾取破碎物掉落, MLS_PICKUP_BROKEN_DROP
//无MLS_PICKUP_BROKEN_DROP命令码返回, 可能会触发角色数据更新通知
struct SPickupBrokenDropReq
{
	UINT16 instId;	//副本id
	BYTE   dropId;	//掉落编号(SMonsterDropPropItem:id)
	void hton()
		{	instId = htons(instId);		}
	void ntoh()
		{	instId = ntohs(instId);		}
};

//杀怪得经验, MLS_KILL_MONSTER_EXP
struct SKillMonsterExpReq
{
	UINT16 instId;		//副本id
	BYTE   areaId;		//区域id
	UINT16 monsterId;	//怪id
	BYTE   monsterLv;	//怪等级

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

//完成副本, MLS_FINISH_INST
struct SChestItem
{
	BYTE type;			//宝箱类型
	UINT16 num;			//宝箱数量

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
	UINT16 instId;	//副本ID
	UINT32 score;	//评分
	BYTE   star;	//评星
	UINT16 finCode;	//完成码(0-成功，1-平局，其他-失败)

	/* zpy 成就系统新增 */
	UINT32 continuous;		//连击次数
	bool speed_evaluation;	//速度评价
	bool hurt_evaluation;	//伤害评价

	/* zpy 限时活动新增 */
	UINT32 surplus_time;	//剩余时间
	UINT32 resist_number;	//抵挡波数
	UINT32 kill_number;		//击杀怪物数量
	INT32 level_gap;		//等级差
	INT32 exp_gap;			//经验差
	INT32 cp_gap;			//战力差
	UINT32 hurt_sum;		//伤害总量
	BYTE chest_num;			//宝箱数量
	SChestItem data[1];		//宝箱信息

	void hton()
	{	
		instId = htons(instId);
		score = htonl(score);
		finCode = htons(finCode);

		/* zpy 成就系统新增 */
		continuous = htonl(continuous);

		/* zpy 限时活动新增 */
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

		/* zpy 成就系统新增 */
		continuous = ntohl(continuous);

		/* zpy 限时活动新增 */
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
	UINT32 rpcoin;			//代币
	UINT32 vit;				//体力
	SBigNumber exp;			//经验
	SBigNumber gold;		//金币
	BYTE   num;				//背包更新数
	SPropertyAttr data[1];	//背包更新数据

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

//副本扫荡, MLS_INST_SWEEP
struct SInstSweepReq
{
	UINT16 instId;	//副本id
	void hton()
		{	instId = htons(instId);	}
	void ntoh()
		{	instId = ntohs(instId);	}
};

struct SInstSweepRes
{
	UINT32 remainVit;		//剩余体力点;
	SBigNumber exp;			//新拥有总的经验(当前拥有+本次所得)
	SBigNumber gold; 		//新拥有的游戏币;
	UINT32 rpcoin;        	//新拥有的代币;
	BYTE num;	   			//背包格子变化数量; 
	SPropertyAttr data[1];	//背包格子变化;
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

//查询邮件列表, MLS_QUERY_EMAILS_LIST
struct SQueryEmailListReq
{
	UINT16 from;	//开始位置;
	BYTE num;	//数量;
	void hton()
		{	from = htons(from);	}
	void ntoh()
		{	from = ntohs(from);	}
};

struct SEmailListGenInfo
{
	UINT32 emailId;			//邮件id
	BYTE   type;			//邮件类型
	BYTE   isRead;			//是否读取(0-未读取, 1-已读取)
	BYTE   isGetAttachment;	//是否提取附件(0-未提取, 1-已提取)
	char   caption[33];		//邮件标题[33];
	char   senderNick[33];	//发件人
	char   sendTime[20];	//发送时间
	void hton()
		{	emailId = htonl(emailId);	}
	void ntoh()
		{	emailId = ntohl(emailId);	}
};

struct SQueryEmailListRes
{
	//UINT16 total;		//邮件总数;
	BYTE retNum;	//当前返回数量;
	SEmailListGenInfo emails[1];	//array<邮件信息>       返回邮件数据;
};

//查询邮件内容, MLS_QUERY_EMAIL_CONTENT
struct SQueryEmailContentReq	//请求
{
	UINT32 emailId; //邮件ID
	void hton()
		{	emailId = htonl(emailId);	}
	void ntoh()
		{	emailId = ntohl(emailId);	}
};

struct SEmailPropGenInfo
{
	BYTE propType;	//物品类型
	UINT16 propId;	//物品id
	BYTE   num;		//数量
	void hton()
		{	propId = htons(propId);		}
	void ntoh()
		{	propId = ntohs(propId);	}
};

struct SQueryEmailContentRes	//内容返回
{
	UINT32 emailId;		//邮件ID;
	SBigNumber gold;	//金币
	UINT32 rpcoin;		//代币
	SBigNumber exp;		//经验
	UINT32 vit;			//体力
	BYTE   propNum;	//物品个数
	UINT16 textLen;	//文字字节数量
	char   data[1];	//propNum个SEmailPropGenInfo + textLen个字节文字
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

//收取邮件附件, MLS_RECEIVE_EMAIL_ATTACHMENT
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
	UINT32 emailId;		//邮件id;
	SBigNumber gold;	//角色拥有的游戏币;
	UINT32 rpcoin;		//角色拥有的代币;
	SBigNumber exp;		//经验
	UINT32 vit;			//体力
	BYTE propsNum;		//本邮件得到的道具数量;
	SPropertyAttr props[1];	//array<道具>   背包修改的道具信息;
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

//删除邮件, MLS_DEL_EMAIL
struct SDelEmailReq
{
	BYTE num;	//删除邮件个数
	UINT32 emailIds[1];	//删除邮件id
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

//装载装备, MLS_LOAD_EQUIPMENT
struct SLoadEquipmentReq
{
	UINT32 id;	//道具唯一ID;
	BYTE   pos;	//位置;          //装备目标位置
	void hton()
		{	id = htonl(id);	}

	void ntoh()
		{	id = ntohl(id);	}
};

struct SEquipmentInfo
{
	UINT32  id;		//道具唯一ID;
	BYTE	pos;	//道具更改后位置;
	void hton()
		{	id = htonl(id);	}

	void ntoh()
		{	id = ntohl(id);	}
};

//卸载装备, MLS_UNLOAD_EQUIPMENT
struct SUnLoadEquipmentReq
{
	UINT32 id;	//道具唯一ID;
	void hton()
		{	id = htonl(id);	}

	void ntoh()
		{	id = ntohl(id);	}
};

//使用道具, MLS_USE_PROPERTY
struct SUsePropertyReq
{
	UINT32 id;	//道具唯一ID;
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
	SBigNumber gold;						//游戏币
	SBigNumber exp;							//经验
	UINT32 vit;								//体力
	UINT32 rpcoin;							//代币
	BYTE deduct_num;						//扣除背包物品数量
	SPropertyDeduct data[1];				//扣除背包物品

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

//领取成就奖励, MLS_RECEIVE_REWARD
struct SReceiveRewardReq
{
	UINT32 achievement_id;					//成就ID

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
	SBigNumber gold;						//游戏币
	SBigNumber exp;							//经验
	UINT32 vit;								//体力
	UINT32 rpcoin;							//代币
	UINT32 point;							//成就点
	BYTE num;								//背包格子变化数量
	SPropertyAttr data[1];					//背包格子变化数据

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

//删除角色, MLS_DEL_ROLE
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

//查询公会名称, MLS_QUERY_GUILD_NAME
struct SQueryGuildNameReq
{
	UINT32 guildId;		//公会ID
	void hton()
		{	guildId = htonl(guildId);	}

	void ntoh()
		{	guildId = ntohl(guildId);	}
};

struct SQueryGuildNameRes
{
	UINT32 guildId;			//公会ID
	char   guildName[33];	//公会名称[33];
	void hton()
		{	guildId = htonl(guildId);	}

	void ntoh()
		{	guildId = ntohl(guildId);	}
};

//查询副本完成排行信息, MLS_QUERY_INST_RANK
struct SQueryInstRankReq
{
	UINT16 instId;	//副本id
	void hton()
		{	instId = htons(instId);	}
	void ntoh()
		{	instId = ntohs(instId);	}
};

struct SInstRankInfo
{
	UINT32 roleId;		//角色id
	char   roleNick[33];//角色昵称
	UINT32 score;	//评分
	char   tts[20];		//完成时间1970-01-01 00:00:00

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
	UINT16 instId;	//副本id
	BYTE   num;		//排行数量
	SInstRankInfo ranks[1];	//排行数据(由高到低)

	void hton()
		{	instId = htons(instId);	}
	void ntoh()
		{	instId = ntohs(instId);	}
};

//用户提交定时任务, MLS_USER_COMMIT_TIMER_TASK
struct SPersonalTimerTaskReq
{
	UINT16 taskId;	//定时任务编号
	void hton()
		{	taskId = htons(taskId);	}
	void ntoh()
		{	taskId = ntohs(taskId);	}
};

struct SPersonalTimerTaskRes
{
	UINT16 taskId;	//定时任务编号
	UINT32 newId;	//新的定时任务id
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

//查询角色技能, MLS_QUERY_ROLE_SKILLS
struct SQuerySkillsRes
{
	BYTE skillNum;	//技能数量
	SSkillMoveInfo skills[1];	//技能数组
};

//技能升级, MLS_SKILL_UPGRADE
struct SSkillUpgradeReq
{
	UINT16 skillId;	//要升级的技能id
	BYTE   step;	//升级级数
	void hton()
		{	skillId = htons(skillId);	}
	void ntoh()
		{	skillId = ntohs(skillId);	}
};

struct SSkillUpgradeRes
{
	SSkillMoveInfo newSkill;	//升级后技能数据
	SBigNumber     gold;		//当前金币数量
	void hton()
		{	newSkill.hton();	}
	void ntoh()
		{	newSkill.ntoh();	}
};

//改变阵营, MLS_CHANGE_FACTION
struct SChangeFactionReq
{
	BYTE factionId;	//阵营id
};

//装载技能, MLS_LOAD_SKILL
struct SLoadSkillReq
{
	UINT16 skillId;	//技能id
	BYTE   key;		//对应客户端key
	void hton()
		{	skillId = htons(skillId);	}
	void ntoh()
		{	skillId = ntohs(skillId);	}
};

//取消技能, MLS_CANCEL_SKILL
struct SCancelSkillReq
{
	UINT16 skillId;	//技能id
	void hton()
		{	skillId = htons(skillId);	}
	void ntoh()
		{	skillId = ntohs(skillId);	}
};


//装备位置升星参数
struct SRiseStarAttachParam		//升星附加参数
{
	BYTE   decDamagedChance;	//是否降低破损率(0-否,1-是)
	BYTE   addSuccedChance;		//是否增加成功率(0-否,1-是)
};

//装备合成, MLS_EQUIPMENT_COMPOSE
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
	SBigNumber gold;		//当前金币数
	bool replace;			//是否替换装备位置装备
	SPropertyAttr equip;	//被替换的装备信息
	BYTE num;				//背包格子更新数量
	SPropertyAttr data[1];	//背包格子更新数据

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

//装备位强化,MLS_EQUIP_POS_STRENGTHEN
struct SEquipPosStrengthenReq
{
	BYTE pos;	//装备位
};

struct SEquipPosStrengthenRes
{
	BYTE newStrengthenLv;		//新的强化等级
	SBigNumber gold;			//剩余金币
	BYTE num;					//消耗背包石头格子数量
	SConsumeProperty consumeStones[1];	//消耗背包石头数据

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

//装备位升星, MLS_EQUIP_POS_RISE_STAR
struct SEquipPosRiseStarReq
{
	BYTE pos;	//装备位
	SRiseStarAttachParam param;	//升星附加参数
};

struct SEquipPosRiseStarRes
{
	BYTE newStarLv;		//新的星级
	SBigNumber gold;	//剩余金币
	BYTE num;		//消耗背包石头格子数量
	SConsumeProperty consumeStones[1];	//消耗背包石头数据

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

//装备位镶嵌宝石, MLS_EQUIP_POS_INLAID_GEM
struct SEquipPosInlaidGemReq
{
	BYTE   pos;	//装备位;
	UINT32 gemId;		//宝石唯一ID;
	BYTE   holeSeq;		//宝石孔号(0-3)
	void hton()
		{	gemId = htonl(gemId);	}

	void ntoh()
		{	gemId = ntohl(gemId);	}
};

//装备位取出宝石, MLS_EQUIP_POS_REMOVE_GEM
struct SEquipPosRemoveGemReq
{
	BYTE   pos;	//装备位;
	BYTE   holeSeq;	//取出目标装备宝石孔号;  //0-3;
};

struct SEquipPosRemoveGemRes
{
	SPropertyAttr gemPropAttr;	//宝石放入背包格子数据

	void hton()
		{	gemPropAttr.hton();		}
	void ntoh()
		{	gemPropAttr.ntoh();		}
};

//获取装备位属性
struct SGetEquipPosAttrRes
{
	BYTE num;		//数量
	SEquipPosAttr data[1];	//属性数据

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

//宝石合成, MLS_GEM_COMPOSE
struct SGemComposeReq
{
	BYTE num;	//材料数量
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
	SPropertyAttr composeGemAttr;	//新合成宝石属性
	void hton()
		{	composeGemAttr.hton();	}
	void ntoh()
		{	composeGemAttr.ntoh();	}
};

//副本占领推送通知, MLS_INST_OCCUPY_PUSH_NOTIFY
struct SInstOccupyPushNotify
{
	UINT16 instId;		//副本id
	UINT32 roleId;		//角色id
	char   roleNick[33];//角色昵称
	UINT32 score;		//评分
	char   tts[20];		//完成时间1970-01-01 00:00:00

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

//物品回购, MLS_PROP_BUY_BACK
struct SPropBuyBackReq
{
	UINT32 sellId;	//出售id
	BYTE   num;		//回购数量
	void hton()
		{	sellId = htonl(sellId);		}
	void ntoh()
		{	sellId = ntohl(sellId);		}
};

struct SPropBuyBackRes
{
	SBigNumber gold;	//当前金币总量;
	BYTE propNum;		//背包改变数量
	SPropertyAttr props[1];	//背包改变物品
};

//查询角色出售物品记录, MLS_QUERY_ROLE_SELL_PROP_RCD
struct SQueryRoleSellPropRcdReq
{
	UINT32 from;	//从第几条开始
	BYTE   num;		//查多少条
	void hton()
		{	from = htonl(from);		}

	void ntoh()
		{	from = ntohl(from);		}
};

struct SPropSellInfo
{
	SPropertyAttr prop;
	char sellDate[SGS_DATE_TIME_LEN];	//出售时间

	void hton()
		{	prop.hton();	}

	void ntoh()
		{	prop.ntoh();	}
};

struct SQueryRoleSellPropRcdRes
{
	UINT32 total;	//总数
	BYTE   num;		//返回数量
	SPropSellInfo sells[1];	//出售数据

	void hton()
		{	total = htonl(total);	}

	void ntoh()
		{	total = ntohl(total);	}
};

//体力更新通知, MLS_VIT_RECOVERY_NOTIFY
struct SVitUpdateNotify
{
	UINT32 vit;		//当前体力
	void hton()
		{	vit = htonl(vit);	}
	void ntoh()
		{	vit = ntohl(vit);	}
};

//充值, MLS_DELTA_TOKEN
struct SRechargeReq
{
	UINT16 cashId;	//充值id
	void hton()
		{	cashId = htons(cashId);		}
	void ntoh()
		{	cashId = ntohs(cashId);		}
};

struct SRechargeRes
{
	UINT32 cashcount;	//充值总额(rmb)
	UINT32 rpcoin;		//当前代币总数
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

//获取角色占领副本, MLS_GET_ROLE_OCCUPY_INST
struct SGetRoleOccupyInstRes
{
	UINT16 num;	//占领数量
	UINT16 occupyInstIds[1];	//占领副本id
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

//设置角色职业称号id, MLS_SET_ROLE_TITLE_ID
struct SSetRoleTitleReq
{
	UINT16 titleId;	//称号id
	void hton()
		{	titleId = htons(titleId);	}
	void ntoh()
		{	titleId = ntohs(titleId);	}
};

//商城购买物品, MLS_MALL_BUY_GOODS
struct SMallBuyGoodsReq
{
	UINT32 buyId;	//购买id
	UINT16 num;		//购买次数
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
	UINT32 rpcoin;		//当前剩余代币总数
	BYTE backpackNum;	//背包格子更新数量
	SPropertyAttr props[1];	//背包格子更新数据
	void hton()
		{	rpcoin = htonl(rpcoin);		}
	void ntoh()
		{	rpcoin = ntohl(rpcoin);		}
};

//查询角色时装, MLS_QUERY_ROLE_DRESS
struct SQueryRoleDressRes
{
	UINT16 num;			//数量
	UINT16 dressIds[1];	//时装id
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

//穿带时装, MLS_WEAR_DRESS
struct SWearDressReq
{
	UINT16 dressId;		//时装id
	void hton()
		{	dressId = htons(dressId);	}
	void ntoh()
		{	dressId = ntohs(dressId);	}
};

//金币更新通知, MLS_GOLD_UPDATE_NOTIFY
struct SGoldUpdateNotify
{
	SBigNumber gold;	//当前金币
};

//代币更新通知, MLS_TOKEN_UPDATE_NOTIFY
struct STokenUpdateNotify
{
	UINT32 rpcoin;	//当前代币
	void hton()
		{	rpcoin = htonl(rpcoin);		}
	void ntoh()
		{	rpcoin = ntohl(rpcoin);		}
};

//经验更新通知, MLS_ROLE_EXP_UPDATE_NOTIFY
struct SExpUpdateNotify
{
	SBigNumber exp;	//当前经验
};

//背包更新通知, MLS_ROLE_BACKPACK_UPDATE_NOTIFY
struct SBackpackUpdateNotify
{
	BYTE propNum;
	SPropertyAttr props[1];
};

//碎片更新通知, MLS_FRAGMENT_UPDATE_NOTIFY
struct SFragmentUpdateNotify
{
	SEquipFragmentAttr fragment;
	void hton()
		{	fragment.hton();	}

	void ntoh()
		{	fragment.ntoh();	}
};

//炼魂碎片更新通知, MLS_CHAIN_SOUL_FRAGMENT_UPDATE_NOTIFY
struct SChainSoulFragmentNotify
{
	SChainSoulFragmentAttr fragment;
	void hton()
	{	fragment.hton();	}

	void ntoh()
	{	fragment.ntoh();	}
};

//角色升级通知, MLS_ROLE_UPGRADE_NOTIFY
struct SRoleUpgradeNotify
{
	BYTE roleLevel;		//角色等级
	SBigNumber gold;	//游戏币
	SBigNumber exp;		//经验
	UINT32 hp;			//生命
	UINT32 mp;			//魔力
	UINT32 vit;			//体力
	UINT16 ap;			//剩余属性点数
	UINT32 attack;      //攻击
	UINT32 def;         //防御
	UINT32 cp;			//当前战斗力;
	SGSPosition pos;	//所在位置
	BYTE skillNum;		//技能/连招个数
	SSkillMoveInfo data[1];	//数据
	
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


//获取机器人数据, MLS_FETCH_ROBOT

#define MATCH_ROBOT_BY_CP 1
#define MATCH_ROBOT_BY_LEVEL 2

struct SFetchRobotReq
{
	BYTE rule;			//匹配规则
	UINT32 targetId;	//目标角色id

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
	UINT32 robotId;			//机器人id
	char   robotNick[33];	//机器人昵称
	UINT16 titleId;			//当前装载称号id;
	BYTE   level;			//角色等级
	BYTE   occuId;			//职业id
	BYTE   sex;				//性别
	UINT32 cp;				//战斗力;
	BYTE   factionId;		//阵营id
	UINT16 dressId;			//时装id
	UINT32 guildId;			//所属公会id
	BYTE   vipLevel;		//VIP等级
	BYTE   core_level;		//核炼魂心部件等级
	BYTE   wearEquipNum;	//穿戴装备数量
	BYTE   skillNum;		//拥有已装载技能数量
	BYTE   equipPosNum;		//装备位数量
	BYTE   chainSoulPosNum;	//炼魂部件数量
	char   data[1];			//wearEquipNum个SPropertyAttr + skillNum个SSkillMoveInfo + equipPosNum个SEquipPosAttr + chainSoulPosNum个SChainSoulPosAttr

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

//获取单人随机副本传送阵进入人数, MLS_GET_PERSONAL_TRANSFER_ENTER
struct SGetPersonalTransferEnterReq
{
	UINT16 transferId;	//传送阵id

	void hton()
		{	transferId = htons(transferId);		}
	void ntoh()
		{	transferId = ntohs(transferId);		}
};

struct SGetPersonalTransferEnterRes
{
	UINT16 enterNum;	//进入人数
	UINT32 leftSec;		//剩余时间(秒)
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

//查询装备碎片, MLS_QUERY_EQUIP_FRAGMENT
struct SQueryEquipFragmentRes
{
	BYTE num;
	SEquipFragmentAttr data[1];
};

//角色复活, MLS_ROLE_REVIVE
struct SReviveRoleReq
{
	UINT16 reviveId;	//复活id
	void hton()
		{	reviveId = htons(reviveId);		}
	void ntoh()
		{	reviveId = ntohs(reviveId);		}
};

struct SReviveRoleRes
{
	UINT32 token;	//当前代币
	BYTE   num;		//复活币更新格子数量
	SConsumeProperty data[1];	//消耗复活币格子数据
	void hton()
		{	token = htonl(token);		}
	void ntoh()
		{	token = ntohl(token);		}
};

//获取已打开单人随机副本, MLS_GET_OPEN_PERSONAL_RAND_INST
struct SPersonalRandInstInfo
{
	UINT16 transferId;	//传送阵编号
	UINT16 instId;		//副本id
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

//系统公告, MLS_SYS_NOTICE_NOTIFY
struct SSysNoticeNotify
{
	UINT16 type;	//公告类型(ESGSNoticeType)
	UINT16 len;		//data字节数量
	char data[1];	//数据

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

//查询金币兑换次数, MLS_QUERY_GOLD_EXCHANGE
struct SQueryGoldExchangeRes
{
	UINT16 exchangedTimes;	//当前已兑换次数
	void hton()
		{	exchangedTimes = htons(exchangedTimes);		}
	void ntoh()
		{	exchangedTimes = ntohs(exchangedTimes);		}
};

//金币兑换, MLS_GOLD_EXCHANGE
struct SGoldExchangeRes
{
	UINT32 rpcoin;		//当前代币
	SBigNumber gold;	//当前金币
	UINT16 exchangedTimes;	//当天已兑换次数
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

//查询未领取奖励的VIP等级, MLS_NOT_RECEIVE_VIP_REWARD
struct SQueryNotReceiveVIPRewardRes
{
	BYTE num;
	BYTE vipLv[1];
};

//领取VIP奖励, MLS_RECEIVE_VIP_REWARD
struct SReceiveVIPRewardReq
{
	BYTE vipLv;		//vip等级
};

struct SReceiveVIPRewardRes
{
	UINT32 rpcoin;		//当前代币
	SBigNumber gold;	//当前金币
	BYTE num;			//背包格子变化数量
	SPropertyAttr data[1];	//背包格子变化数据

	void hton()
		{	rpcoin = htonl(rpcoin);		}

	void ntoh()
		{	rpcoin = ntohl(rpcoin);		}
};

//查询已充值且有首充赠送的充值id, MLS_QUERY_FIRST_CASH_GIVE
struct SQueryRechargeFirstGiveRes
{
	BYTE num;			//数量
	UINT16 data[1];		//充值id

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

//查询角色拥有魔导器, MLS_QUERY_MAGICS
struct SQueryRoleMagicsRes
{
	BYTE num;			//数量
	UINT16 data[1];		//魔导器id

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

//获得魔导器, MLC_GAIN_MAGIC
struct SGainMagicReq
{
	UINT16 instId;	//副本id
	UINT16 magicId;	//魔导器id

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

//获得魔导器通知, MLC_GAIN_MAGIC_NOTIFY
struct SGainMagicNotify
{
	UINT16 magicId;	//魔导器id
	void hton()
		{	magicId = htons(magicId);	}
	void ntoh()
		{	magicId = ntohs(magicId);	}
};

//查询当前签到, MLS_QUERY_ONLINE_SIGN_IN
struct SQueryOnlineSignInRes
{
	BYTE continueTimes;	//连续次数
	BYTE sameDayIsSign;	//当天是否签过(1-签过, 0-未签过)
};

//查询已经领取活动列表
struct SQueryReceivedActivityRes
{
	UINT16 num;			//活动数量
	UINT32 data[1];		//活动id列表

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

//领取活动奖励, MLS_RECEIVED_ACTIVITY
struct SReceiveActivityReq
{
	UINT32 activityId;	//活动id

	void hton()
		{	activityId = htonl(activityId);		}
	void ntoh()
		{	activityId = ntohl(activityId);		}
};

//查询当前开放章节, MLS_QUERY_CUR_OPEN_CHAPTER
struct SQueryCurOpenChapterRes
{
	UINT16 chapterId;
	void hton()
		{	chapterId = htons(chapterId);	}
	void ntoh()
		{	chapterId = ntohs(chapterId);	}
};

//随机副本打开, MLS_RAND_INST_OPEN_NOTIFY
struct SRandInstItem
{
	UINT16 transferId;	//当前传送阵编号
	UINT16 instId;		//副本id
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

//随机副本关闭, MLS_RAND_INST_CLOSE_NOTIFY
struct SRandInstCloseNotify
{
	SRandInstItem inst;

	void hton()
		{	inst.hton();	}
	void ntoh()
		{	inst.ntoh();	}
};

//领取兑换码奖励, MLS_RECEIVE_CASH_CODE_REWARD
struct SReceiveCashCodeRewardReq
{
	char cashCode[33];	//兑换码
};

//查询解锁内容, MLS_QUERY_UNLOCK_CONTENT
struct SQueryUnlockContentReq
{
	BYTE type;		//解锁类型(ESGSUnlockType)
};

struct SQueryUnlockContentRes
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 num;		//解锁数量
	UINT16 data[1];	//解锁数据

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

//内容解锁, MLS_CONTENT_UNLOCK
struct SContentUnlockReq
{
	BYTE type;			//解锁类型(ESGSUnlockType)
	UINT16 unlockId;	//解锁id

	void hton()
		{	unlockId = htons(unlockId);		}

	void ntoh()
		{	unlockId = ntohs(unlockId);		}
};

//获取轮盘奖品, MLS_GET_ROULETTE_REMAIN_REWARD
struct SRoulettePropItem
{
	BYTE id;		//轮盘格子id
	BYTE type;		//类型(ESGSPropType)
	UINT16 propId;	//物品id
	UINT32 num;		//物品数量

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
	UINT16 getNum;	//已经抽取次数
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

//抽取轮盘奖励, MLS_EXTRACT_ROULETTE_REWARD
struct SExtractRouletteRewardRes
{
	BYTE id;		//轮盘格子id
	UINT16 getNum;	//已经抽取次数
	UINT32 rpcoin;	//代币
	UINT32 vit;		//体力
	SBigNumber exp;	//经验
	SBigNumber gold;	//金币
	BYTE   num;		//背包更新数
	SPropertyAttr data[1];	//背包更新数据

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

//获取充值订单，MLS_FETCH_RECHARGE_ORDER
struct SFetchRechargeOrderReq
{
	UINT16 rechargeId;	//充值id
	UINT32 rmb;			//人民币
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
	char   orderNO[65]; //订单号
};

//获取充值返回代币, MLS_GET_RECHARGE_TOKEN
struct SGetRechargeTokenReq
{
	char   orderNO[65]; //订单号
};

struct SGetRechargeTokenRes
{
	UINT32 rpcoin;

	void hton()
		{	rpcoin = htonl(rpcoin);		}
	void ntoh()
		{	rpcoin = ntohl(rpcoin);		}
};

//刷新轮盘大奖, MLS_UPDATE_ROULETTE_BIG_REWARD
struct SUpdateRouletteBigRewardRes
{
	UINT32 rpcoin;	//代币
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

//获取轮盘大奖, MLS_FETCH_ROULETTE_BIG_REWARD
struct SFetchRouletteBigRewardRes
{
	UINT32 rpcoin;	//代币
	UINT32 vit;		//体力
	SBigNumber exp;	//经验
	SBigNumber gold;	//金币
	BYTE   num;		//背包更新数
	SPropertyAttr data[1];	//背包更新数据

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

//获取活动副本当天挑战次数, MLS_GET_ACTIVE_INST_CHALLENGE_TIMES
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

//查询体力兑换次数, MLS_QUERY_VIT_EXCHANGE
struct SQueryVitExchangeRes
{
	UINT16 exchangedTimes;	//当前已兑换次数
	void hton()
	{	exchangedTimes = htons(exchangedTimes);		}
	void ntoh()
	{	exchangedTimes = ntohs(exchangedTimes);		}
};

//体力兑换, MLS_VIT_EXCHANGE
struct SVitExchangeRes
{
	UINT32 rpcoin;			//当前代币
	UINT32 vit;				//当前体力
	UINT16 exchangedTimes;	//当天已兑换次数
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

//查询角色每日签到, MLS_QUERY_ROLE_DAILY_SIGN
struct SQueryRoleDailySignRes
{
	BYTE month;             //月份
	bool now_signed;		//今日是否签到
	BYTE num;				//签到次数
	BYTE data[1];			//签到数据
};

//角色每日签到, MLS_ROLE_DAILY_SIGN
struct SRoleDailySignRes
{
	UINT32 rpcoin;			//代币
	UINT32 vit;				//体力
	SBigNumber exp;			//经验
	SBigNumber gold;		//金币
	BYTE num;				//背包更新数
	SPropertyAttr data[1];	//背包更新数据

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

//查询炼魂碎片, MLS_QUERY_CHAIN_SOUL_FRAGMENT
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

//炼魂部件属性
#define MAX_CHAIN_SOUL_HOLE_NUM 6
struct SChainSoulPosAttr
{
	BYTE pos;		//炼魂部位
	BYTE level;		//强化等级
};

//查询炼魂部件, MLS_QUERY_CHAIN_SOUL_POS
struct SQueryChainSoulPosRes
{
	BYTE core_level;									//核心部件等级
	SChainSoulPosAttr pos[MAX_CHAIN_SOUL_HOLE_NUM];		//非核心部位等级
};

//炼魂部件升级, MLS_CHAIN_SOUL_POS_UPGRADE
struct SChainSoulPosUpgradeReq
{
	BYTE pos;											//升级部件
};

struct SChainSoulPosUpgradeRes
{
	BYTE core_level;									//核心部件等级
	SChainSoulPosAttr attr;								//升级后部件属性
	BYTE num;											//剩余碎片数量
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

//攻击世界Boss, MLS_ATTACK_WORLD_BOOS
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
	UINT32 boss_blood;		//boss血量
	UINT32 hurt_sum;		//造成总伤害
	UINT32 kill_roleId;		//击杀者角色ID

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

//查询世界Boss排行榜，MLS_QUERY_WORLD_BOSS_RANK
struct SWorldBossRankItem
{
	UINT32 id;			//角色ID
	UINT32 hurt_sum;	//造成总伤害
	char   nick[33];	//昵称

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
	UINT32 boss_blood;			// boss血量
	UINT32 kill_roleId;			// 击杀者角色ID
	UINT32 number;				// 参与人数
	BYTE num;					// 排行数量
	SWorldBossRankItem data[1];	// 排行数据

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

//在世界Boss副本中复活, MLS_RESURRECTION_IN_WORLDBOSS
struct SResurrectionInWorldBossRes
{
	UINT32 rpcoin;					//代币
	UINT32 consume_count;			// 消费复活次数

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

//查询世界Boss信息, MLS_QUERY_WORLDBOSS_INFO
struct SQueryWorldBossInfoRes
{
	UINT32 resurrection_time;		// 复活时间
	UINT32 consume_count;			// 消费复活次数

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

//获取天梯信息, MLS_GET_LADDER_ROLE_INFO
#define LADDER_MATCHTING_MAX_NUM 5

struct SLadderRoleInfoRes
{
	UINT32 rank;						//排名
	UINT16 refresh_count;				//刷新次数
	UINT16 eliminate_count;				//消除cd次数
	UINT16 challenge_count;				//挑战次数
	UINT16 win_in_row;					//连胜次数
	UINT32 cooling_time;				//挑战冷却时间

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

//消除天梯CD, MLS_ELIMINATE_LADDER_CD
struct SEliminateLadderCDTimeRes
{
	UINT32 rpcoin;						//代币
	UINT16 eliminate_count;				//消除cd次数

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

//匹配天梯玩家, MLS_MATCHING_LADDER
struct SMatchingLadderItem
{
	UINT32 roleId;		//角色ID
	UINT32 rank;		//排名
	bool robot;			//是否是机器人
	BYTE level;			//等级
	BYTE actortype;		//职业
	UINT32 cp;			//战力
	char nick[33];		//昵称

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
	UINT32 rpcoin;						//代币
	UINT16 refresh_count;				//匹配次数
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

//挑战天梯玩家, MLS_CHALLENGE_LADDER
struct SChallengeLadderReq
{
	UINT32 targetId;					//目标ID
	UINT32 target_rank;					//目标排名

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
	UINT16 challenge_count;				//挑战次数
	SMatchingLadderItem player_info;	//玩家信息

	void hton()
	{
		challenge_count = htons(challenge_count);
	}

	void ntoh()
	{
		challenge_count = ntohs(challenge_count);
	}
};

//完成天梯挑战
//struct SFinishLadderChallengeReq
//{
//	BYTE finishCode;					//完成码(0-成功，其他-失败)
//};
//
//struct SFinishLadderChallengeRes
//{
//	UINT32 rank;						//排名
//	UINT16 win_in_row;					//连胜次数
//	UINT32 cooling_time;				//挑战冷却时间
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

//刷新天梯排行，MLS_REFRESH_LADDER_RANK
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
	UINT32 roleId;			//角色id
	BYTE occuId;			//职业id

	void hton()
	{
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		roleId = ntohl(roleId);
	}
};

//查询所有角色，MLS_QUERY_ALL_ROLES
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

//查询拥有的活动副本类型，MLS_QUERY_CAN_FINISH_ACTIVITY_INST
struct SActivityInstItem
{
	UINT32 instId;			// 副本id
	BYTE num;				// 剩余次数

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

//查询周免角色, MLS_QUERY_WEEKFREE_ACTOR
struct SQueryWeekFreeActorRes
{
	BYTE actorType;
};

//查询竞技副本进入次数, MLS_QUERY_COMPMODE_ENTER_NUM
struct SQueryCompEnterNumRes
{
	UINT16 enter_num;		// 进入次数
	UINT16 buy_num;			// vip购买次数

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

//购买竞技模式进入次数, MLS_BUY_COMPMODE_ENTER_NUM
struct SBuyCompEnterNumReq
{
	BYTE buy_mode;			// 购买模式（1表示购买一次，2表示购买所有）
};

struct SBuyCompEnterNumRes
{
	UINT16 buy_num;			// vip购买次数
	UINT32 rpcoin;			// 代币

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

//查询1v1模式进入次数, MLS_QUERY_1V1_ENTER_NUM
struct SQuery1V1EnterNumRes
{
	UINT16 enter_num;		// 进入次数
	UINT16 buy_num;			// 购买次数

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

//购买1vs1模式进入次数, MLS_BUY_1V1_ENTER_NUM
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
	UINT16 buy_num;			// vip购买次数
	UINT32 rpcoin;			// 代币

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

//消息提示, MLS_MESSAGETIPS_NOTIFY
struct SMessageTipsItem
{
	BYTE type;					// 消息类型
	UINT16 title;				// 消息标题长度
	UINT16 content;				// 消息内容长度
	char data[1];				// title + content 包含结尾符号

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
	BYTE num;					// 消息数量
	char data[1];				// 消息数据，存放SMessageTipsItem类型
};

///////////
// add by hxw 20151014
///排行管理类活动的种类
const int RANK_REWARD_TYPE_NUM = 4;

///排行奖励领取查询
struct SQueryRankRewardGet
{
	UINT16 rankid;	//排名ID
	BOOL bGet;	//是否已经领取
	void hton()
	{
		rankid = htons(rankid);	
	}
	void ntoh()
	{
		rankid = ntohs(rankid);	
	}
};

///副本推图排行查询
const int MAX_INS_RANK_NUM = 10;
struct SInsnumRank
{
	UINT16	rankid;		//排名ID
	UINT16	num;		//推图次数,如果是等级排行，则返回等级
	char	nick[33];	//玩家昵称[33];
	BYTE	occuId;		//职业id	
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

//查询副本挑战次数排行榜
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

///获取排行奖励
struct SGetRankRewardRes
{
	UINT32 rpcoin;			//代币
	UINT32 vit;				//体力
	SBigNumber exp;			//经验
	SBigNumber gold;		//金币
	BYTE num;				//背包更新数
	SPropertyAttr data[1];	//背包更新数据

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
//获取强者奖励信息返回,返回领取过的ID
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
//查询在线奖励领取状况
const int ONLINE_NUM = 10;
struct SQueryOnlineInfos
{
	UINT16 rewards[ONLINE_NUM];//返回当天所领取的在线奖励
	UINT32 time;//返回当天在线时间 单位秒
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
	UINT16 rewardId; //奖励ID
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
	UINT16 rewardId; //奖励ID
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
