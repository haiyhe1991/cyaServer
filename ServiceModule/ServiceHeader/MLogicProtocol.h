#ifndef __MLOGIC_PROTOCOL_H__
#define __MLOGIC_PROTOCOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "ServiceMLogic.h"

#pragma pack(push, 1)

//查询角色外形信息(linker发送数据包)
struct SLinkerQueryRoleAppearanceReq	//linker->MLogicServer
{
	UINT32 userId;		//账号id
	UINT32 roleId;		//角色Id
	UINT32 playerId;	//玩家角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		playerId = htonl(playerId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		playerId = ntohl(playerId);
	}
};

//查询角色详细信息(linker发送数据包)
struct SLinkerQueryRoleExplicitReq	//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色Id
	UINT32 playerId;	//玩家角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		playerId = htonl(playerId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		playerId = ntohl(playerId);
	}
};

//进入角色请求(linker发送数据包)
struct SLinkerEnterRoleReq	//linker->MLogicServer
{
	UINT32 userId;	//账户id;
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//退出角色请求(linker发送数据包)
struct SLinkerLeaveRoleReq	//linker->MLogicServer
{
	UINT32 userId;	//账户id;
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询背包(linker发送数据包)
struct SLinkerQueryBackpackReq	//linker->MLogicServer
{
	UINT32 userId;	//账户id
	UINT32 roleId;	//角色id
	BYTE from;	//开始位置;
	BYTE num;	//数量;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询成就(linker发送数据包)
struct SLinkerQueryAchievementReq	//linker->MLogicServer
{
	UINT32 userId;							//用户ID
	UINT32 roleId;							//角色ID
	BYTE type;								//1成长之路，2冒险历程，3完美技巧ID
	UINT16 start_pos;						//开始查询位置

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		start_pos = htons(start_pos);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		start_pos = ntohs(start_pos);
	}
};

//创建角色(linker发送数据包)
struct SLinkerCreateRoleReq	//linker->MLogicServer
{
	UINT32 userId;		//账号id
	BYTE occuId;		//职业id;
	BYTE sex;			//性别
	char   nick[33];	//创建昵称[33];
	void hton()
	{
		userId = htonl(userId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
	}
};

//查询已接受任务(linker发送数据包)
struct SLinkerQueryAcceptTaskReq
{
	UINT32 userId;		//账号id
	UINT32 roleId;		//角色id;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询好友(linker发送数据包)
struct SLinkerQueryFriendsReq	//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE from;		//开始位置;
	BYTE num;		//数量;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询好友申请
struct SLinkerQueryFriendApplyReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE from;		//开始位置;
	BYTE num;		//数量;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//添加好友(linker发送数据包)
struct SLinkerAddFriendReq	//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	char nick[33];		//好友昵称[33];
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(userId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//好友申请确认
struct SLinkerFriendApplyConfirmReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT32 friendUserId;	//好友账号id
	UINT32 friendRoleId;	//好友角色id
	BYTE   status;			//0-拒绝, 1-同意
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(userId);
		friendUserId = htonl(friendUserId);
		friendRoleId = htonl(friendRoleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		friendUserId = ntohl(friendUserId);
		friendRoleId = ntohl(friendRoleId);
	}
};

//获取推荐好友列表
struct SLinkerGetRecommendFriendsReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(userId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//搜索玩家
struct SLinkerPlayerSearchReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	char playerNick[33];
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(userId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//删除好友(linker发送数据包)
struct SLinkerDelFriendReq	//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	char nick[33];		//删除好友昵称[33];
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(userId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询已完成副本信息(linker发送数据包)
struct SLinkerQueryFinishedInstReq	//linker->MLogicServer
{
	UINT32 roleId;	//角色id
	UINT16 from;	//开始位置;
	BYTE   num; 	//查询数量;
	void hton()
	{
		roleId = htonl(roleId);
		from = htons(from);
	}

	void ntoh()
	{
		roleId = ntohl(roleId);
		from = ntohs(from);
	}
};

//完成新手引导(linker发送数据包)
struct SLinkerFinishHelpReq		//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 helpId;	//引导id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		helpId = htons(helpId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		helpId = ntohs(helpId);
	}
};

//查询已完成新手引导信息(linker发送数据包)
struct SLinkerQueryFinishedHelpReq	//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//出售物品申请(linker发送数据包)
struct SLinkerSellPropertyReq		//linker->MLogicServer
{
	UINT32 userId;
	UINT32 roleId;	//角色id
	UINT32 id;	//物品唯一ID;
	BYTE   num;	//物品数量;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		id = htonl(id);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		id = ntohl(id);
	}
};

//丢弃道具申请(linker发送数据包)
struct SLinkerDropPropertyReq		//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE from;      //0-背包, 1-仓库
	UINT32 id;      //道具唯一ID;
	BYTE num; 	    //丢弃数量数量;
	void hton()
	{
		id = htonl(id);
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		id = ntohl(id);
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//放弃任务(linker发送数据包)
struct SLinkerDropTaskReq		//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 taskId;	//任务id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		taskId = htons(taskId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		taskId = ntohs(taskId);
	}
};

//更新角色位置信息(linker发送数据包)
struct SLinkerUpdateRolePosReq	//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	char xyz[16];	//xyz坐标[16];
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//购买背包或仓库存储空间(linker发送数据包)
struct SLinkerBuyStorageSpaceReq		//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE buyType;   //0.背包  1.仓库
	BYTE num;		//购买的数量		
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//接受任务(linker发送数据包)
struct SLinkerAcceptTaskReq	//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 taskId;	//任务id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		taskId = htons(taskId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		taskId = ntohs(taskId);
	}
};

//收取邮件附件(linker发送数据包)
struct SLinkerReceiveEmailAttachmentsReq	//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT32 emailId;	//邮件id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		emailId = htonl(emailId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		emailId = ntohl(emailId);
	}
};

//查询邮件列表
struct SLinkerQueryEmailListReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 from;	//开始位置;
	BYTE num;	//数量;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		from = htons(from);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		from = ntohs(from);
	}
};

//查询邮件内容
struct SLinkerQueryEmailContentReq	//请求
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT32 emailId; //邮件ID
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		emailId = htonl(emailId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		emailId = ntohl(emailId);
	}
};

//删除邮件
struct SLinkerDelEmailReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE num;	//删除邮件个数
	UINT32 emailIds[1];	//删除邮件id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		for (BYTE i = 0; i < num; ++i)
			emailIds[i] = htonl(emailIds[i]);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		for (BYTE i = 0; i < num; ++i)
			emailIds[i] = ntohl(emailIds[i]);
	}
};

//穿戴装备(linker发送数据包)
struct SLinkerLoadEquipmentReq	//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT32 id;	//道具唯一ID;
	BYTE   pos;	//位置;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		id = htonl(id);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		id = ntohl(id);
	}
};

//卸载装备(linker发送数据包)
struct SLinkerUnLoadEquipmentReq	//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT32 id;	//道具唯一ID;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		id = htonl(id);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		id = ntohl(id);
	}
};

//使用道具(linker发送数据包)
struct SLinkerUsePropertyReq	//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT32 id;	//道具唯一ID;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		id = htonl(id);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		id = ntohl(id);
	}
};

//领取成就奖励(linker发送数据包)
struct SLinkerReceiveRewardReq	//linker->MLogicServer
{
	UINT32 userId;							//用户ID
	UINT32 roleId;							//角色ID
	UINT32 achievement_id;					//成就ID

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		achievement_id = htonl(achievement_id);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		achievement_id = ntohl(achievement_id);
	}
};

//兑换道具(linker发送数据包)
struct SLinkerCashPropertyReq		//linker->MLogicServer
{
	UINT32 userId;		//账号id
	UINT32 roleId;		//角色id
	UINT16 targetId;	//兑换目标ID;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		targetId = htons(targetId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		targetId = ntohs(targetId);
	}
};

//删除角色(linker发送数据包)
struct SLinkerDelRoleReq	//linker->MLogicServer
{
	UINT32 userId;		//账号id
	UINT32 roleId;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//账号退出(linker发送数据包)
struct SLinkerUserExitReq	//linker->MLogicServer
{
	UINT32 userId;		//账号id
	void hton()
	{
		userId = htonl(userId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
	}
};

//用户提交定时任务
struct SLinkerPersonalTimerTaskReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 taskId;	//定时任务编号
	void hton()
	{
		taskId = htons(taskId);
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		taskId = ntohs(taskId);
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询角色技能
struct SLinkerQuerySkillsReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询角色连招
struct SLinkerQueryCombosReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//技能升级
struct SLinkerSkillUpgradeReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 skillId;	//要升级的技能id
	BYTE   step;	//升级级数
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		skillId = htons(skillId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		skillId = ntohs(skillId);
	}
};

//改变阵营
struct SLinkerChangeFactionReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE factionId;	//阵营id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//装载技能
struct SLinkerLoadSkillReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 skillId;	//技能id
	BYTE   key;		//对应客户端key
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		skillId = htons(skillId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		skillId = ntohs(skillId);
	}
};

//取消技能
struct SLinkerCancelSkillReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 skillId;	//技能id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		skillId = htons(skillId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		skillId = ntohs(skillId);
	}
};

//更新已接任务目标
struct SLinkerUpdateTaskTargetReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 taskId;		//任务id
	UINT32 target[MAX_TASK_TARGET_NUM];
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		taskId = htons(taskId);
		for (BYTE i = 0; i < MAX_TASK_TARGET_NUM; ++i)
			target[i] = htonl(target[i]);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		taskId = ntohs(taskId);
		for (BYTE i = 0; i < MAX_TASK_TARGET_NUM; ++i)
			target[i] = ntohl(target[i]);
	}
};

//提交任务完成
struct SLinkerFinishTaskReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 taskId;	//任务id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		taskId = htons(taskId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		taskId = ntohs(taskId);
	}
};

//领取任务奖励, MLS_RECEIVE_TASK_REWARD
struct SLinkerReceiveTaskRewardReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 taskId;	//任务id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		taskId = htons(taskId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		taskId = ntohs(taskId);
	}
};

//查询已完成任务历史记录
struct SLinkerQueryFinishedTaskReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 from;	//从多少开始
	UINT16 num;		//查多少条

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		from = htons(from);
		num = htons(num);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		from = ntohs(from);
		num = ntohs(num);
	}
};

//获取日常任务
struct SLinkerGetDailyTaskReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//装备合成
struct SLinkerEquipComposeReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 composeId;	//合成id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		composeId = htons(composeId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		composeId = ntohs(composeId);
	}
};

//装备位强化
struct SLinkerEquipPosStrengthenReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE pos;	//装备位
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//装备位升星
struct SLinkerEquipPosRiseStarReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE pos;	//装备位
	SRiseStarAttachParam param;	//升星附加参数
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//装备位镶嵌宝石, MLS_EQUIP_POS_INLAID_GEM
struct SLinkerEquipPosInlaidGemReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE   pos;	//装备位;
	UINT32 gemId;		//宝石唯一ID;
	BYTE   holeSeq;		//宝石孔号(0-3)

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		gemId = htonl(gemId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		gemId = ntohl(gemId);
	}
};

//装备位取出宝石, MLS_EQUIP_POS_REMOVE_GEM
struct SLinkerEquipPosRemoveGemReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE   pos;	//装备位;
	BYTE   holeSeq;	//取出目标装备宝石孔号;  //0-3;

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//获取装备位属性
struct SLinkerGetEquipPosAttrReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//宝石合成, MLS_GEM_COMPOSE
struct SLinkerGemComposeReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE num;	//材料数量
	SConsumeProperty material[1];
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		for (BYTE i = 0; i < num; ++i)
			material[i].hton();
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		for (BYTE i = 0; i < num; ++i)
			material[i].ntoh();
	}
};

//物品回购
struct SLinkerPropBuyBackReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT32 sellId;	//出售id
	BYTE   num;		//回购数量
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		sellId = htonl(sellId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		sellId = ntohl(sellId);
	}
};

//查询角色出售物品记录
struct SLinkerQueryRoleSellPropRcdReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT32 from;	//从第几条开始
	BYTE   num;		//查多少条
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		from = htonl(from);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		from = ntohl(from);
	}
};

//背包整理
struct SLinkerBackpackCleanupReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//充值
struct SLinkerRechargeReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 cashId;	//充值id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		cashId = htons(cashId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		cashId = ntohs(cashId);
	}
};

//获取角色占领副本
struct SLinkerGetRoleOccupyInstReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//设置角色职业称号id
struct SLinkerSetRoleTitleReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 titleId;	//称号id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		titleId = htons(titleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		titleId = ntohs(titleId);
	}
};

//商城购买物品
struct SLinkerMallBuyGoodsReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT32 buyId;	//商品id
	UINT16 num;		//购买次数
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		buyId = htonl(buyId);
		num = htons(num);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		buyId = ntohl(buyId);
		num = ntohs(num);
	}
};

//查询角色时装
struct SLinkerQueryRoleDressReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//穿带时装
struct SLinkerWearDressReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 dressId;		//时装id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		dressId = htons(dressId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		dressId = ntohs(dressId);
	}
};

//进入副本(linker发送数据包)
struct SLinkerEnterInstReq	//linker->MLogicServer
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 instId;	// 副本ID;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
	}
};

//获取副本区域怪掉落
struct SLinkerFetchInstAreaDropsReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 instId;	//副本ID;
	BYTE   areaId;	//区域id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
	}
};

//拾取副本怪掉落
struct SLinkerPickupInstMonsterDropReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 instId;	//副本id
	BYTE   areaId;	//区域id
	BYTE   dropId;	//掉落编号(SMonsterDropPropItem:id)
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
	}
};

//拾取副本宝箱
struct SLinkerPickupInstBoxReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 instId;	//副本id
	BYTE   boxId;	//宝箱id
	BYTE   id;		//宝箱中物品编号
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
	}
};

//拾取破碎物掉落
struct SLinkerPickupBrokenDropReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 instId;	//副本id
	BYTE   dropId;	//掉落编号(SMonsterDropPropItem:id)
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
	}
};

//杀怪得经验
struct SLinkerKillMonsterExpReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 instId;		//副本id
	BYTE   areaId;		//区域id
	UINT16 monsterId;	//怪id
	BYTE   monsterLv;	//怪等级

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
		monsterId = htons(monsterId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
		monsterId = ntohs(monsterId);
	}
};

//完成某副本(linker发送数据包)
struct SLinkerFinishInstReq	//linker->MLogicServer
{
	UINT32 userId;			//账号id
	UINT32 roleId;			//角色id
	UINT16 instId;			//副本ID;
	UINT32 score;			//评分
	BYTE   star;			//评星
	UINT16 finCode;			//完成码(0-成功，其他-失败)

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
		userId = htonl(userId);
		roleId = htonl(roleId);
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
		for (BYTE i = 0; i < chest_num; ++i)
			data[i].hton();
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
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
		for (BYTE i = 0; i < chest_num; ++i)
			data[i].ntoh();
	}
};

//获取机器人数据
struct SLinkerFetchRobotReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE rule;		//匹配规则
	UINT32 targetId;//目标角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		targetId = htonl(targetId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		targetId = ntohl(targetId);
	}
};

//获取单人随机副本传送阵进入人数
struct SLinkerGetPersonalTransferEnterReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 transferId;	//传送阵id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		transferId = htons(transferId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		transferId = ntohs(transferId);
	}
};

//查询装备碎片
struct SLinkerQueryEquipFragmentReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//角色复活
struct SLinkerReviveRoleReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 reviveId;	//复活id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		reviveId = htons(reviveId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		reviveId = ntohs(reviveId);
	}
};

//获取已打开的单人随机副本
struct SLinkerGetOpenPersonalRandInstReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询金币兑换信息
struct SLinkerQueryGoldExchangeReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//金币兑换
struct SLinkerGoldExchangeReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询未领取奖励的VIP等级
struct SLinkerQueryNotReceiveVIPRewardReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//领取VIP奖励, MLS_RECEIVE_VIP_REWARD
struct SLinkerReceiveVIPRewardReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE   vipLv;	//vip等级
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询已充值且有首充赠送的充值id
struct SLinkerQueryRechargeFirstGiveReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询角色拥有魔导器
struct SLinkerQueryRoleMagicsReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//获得魔导器
struct SLinkerGainMagicReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 instId;	//副本id
	UINT16 magicId;	//魔导器id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
		magicId = htons(magicId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
		magicId = ntohs(magicId);
	}
};

//副本扫荡
struct SLinkerInstSweepReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 instId;	//副本id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		instId = htons(instId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		instId = ntohs(instId);
	}
};

//查询当前签到
struct SLinkerQueryOnlineSignInReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//上线签到
struct SLinkerOnlineSignInReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询已经领取活动列表
struct SLinkerQueryReceivedActivityReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//领取活动奖励
struct SLinkerReceiveActivityReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT32 activityId;	//活动id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		activityId = htonl(activityId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		activityId = ntohl(activityId);
	}
};

//查询当前开放章节
struct SLinkerQueryCurOpenChapterReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//领取兑换码奖励
struct SLinkerReceiveCashCodeRewardReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	char cashCode[33];	//兑换码
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询解锁内容
struct SLinkerQueryUnlockContentReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE type;		//解锁类型

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//内容解锁, MLS_CONTENT_UNLOCK
struct SLinkerContentUnlockReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE type;		//解锁类型
	UINT16 unlockId;	//解锁id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		unlockId = htons(unlockId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		unlockId = ntohs(unlockId);
	}
};

//获取轮盘奖品
struct SLinkerGetRoulettePropReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//抽取轮盘奖品
struct SLinkerExtractRouletteRewardReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//获取充值订单
struct SLinkerFetchRechargeOrderReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 partId;	//分区id
	UINT16 rechargeId;	//充值id
	UINT32 rmb;			//人民币
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		partId = htons(partId);
		rechargeId = htons(rechargeId);
		rmb = htonl(rmb);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		partId = ntohs(partId);
		rechargeId = ntohs(rechargeId);
		rmb = ntohl(rmb);
	}
};

//获取充值返回代币
struct SLinkerGetRechargeTokenReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	char   orderNO[65]; //订单号

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//刷新轮盘大奖
struct SLinkerUpdateRouletteBigRewardReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//获取轮盘大奖
struct SLinkerFetchRouletteBigRewardReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//获取活动副本当天挑战次数
struct SLinkerGetActiveInstChallengeTimesReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

/* zpy 体力兑换 */
//查询体力兑换信息
struct SLinkerQueryVitExchangeReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//体力兑换
struct SLinkerVitExchangeReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

/* zpy 查询角色每日签到 */
struct SLinkerQueryRoleDailySignReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

/* zpy 每日签到 */
struct SLinkerRoleDailySignReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询炼魂碎片
struct SLinkerQueryChainSoulFragmentReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询炼魂部件
struct SLinkerQueryChainSoulPosReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//炼魂部件升级
struct SLinkerChainSoulPosUpgradeReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE pos;		//升级部件

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//add by hxw 20151028
//查询等级排行榜信息
struct SLinkerQueryLvInfoReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};
//end

/* by hxw 20151006 */
//查询等级排行的奖励信息 返回名次 和 是否获取奖励
struct SLinkerQueryLvRankRewardReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};

//查询副本挑战次数排行的奖励信息 返回名次 和 是否获取奖励
struct SLinkerQueryInsnumRankRewardReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};

//查询副本挑战次数排行信息 返回 排行榜中的信息
struct SLinkerQueryInsnumRankInfoReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};

/* by hxw 20151006 获取LV排行奖励*/
struct SLinkerGetLvRankRewardReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};

//获取副本挑战排行奖励请求
struct SLinkerGetInsnumRankRewardReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};

// add by hxw 20151015 强者奖励相关
//获取领取过的强者奖励请求
struct SLinkerQueryStrongerInfoReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};
//获取强者奖励
struct SLinkerGetStrongerReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 rewardId; //奖励ID
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		rewardId = htons(rewardId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		rewardId = ntohs(rewardId);
	}
};
//end at 20151015

//add by hxw 20151020 获取在线奖励信息
struct SLinkerQueryOnlineInfoReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	void hton()	{ userId = htonl(userId); roleId = htonl(roleId); }
	void ntoh() { userId = ntohl(userId);	roleId = ntohl(roleId); }
};
// 获取在线奖励
struct SLinkerGetOnlineRewardReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 rewardId; //奖励ID
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		rewardId = htons(rewardId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		rewardId = ntohs(rewardId);
	}
};
//end at 20151020

//攻击世界Boos
struct SLinkerAttackWorldBossReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT32 hurt;	//伤害数量

	void hton()
	{
		hurt = htonl(hurt);
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		hurt = ntohl(hurt);
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

//查询世界Boss排行榜
struct SLinkerQueryWorldBossRankReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 被世界Boss杀死
struct SLinkerWasKilledByWorldBossReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 在世界Boss副本中复活
struct SLinkerResurrectionInWorldBossReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 查询世界Boss信息
struct SLinkerQueryWorldBossInfoReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 查询天梯信息
struct SLinkerLadderRoleInfoReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 天梯匹配玩家
struct SLinkerMatchingLadderReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 刷新天梯排名信息
struct SLinkerRefreshLadderRankReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 天梯挑战玩家
struct SLinkerChallengeLadderReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT32 targetId;					//目标ID
	UINT32 target_rank;					//目标排名

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		targetId = htonl(targetId);
		target_rank = htonl(target_rank);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		targetId = ntohl(targetId);
		target_rank = ntohl(target_rank);
	}
};

// 消除天梯CD
struct SLinkerEliminateLadderCDTimeReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 完成天梯挑战
struct SLinkerFinishLadderChallengeReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE finishCode;					//完成码(0-成功，其他-失败)

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 查询所有角色
struct SLinkerQueryAllRolesReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 查询拥有的活动副本类型
struct SLinkerHasActivityInstTypeReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 查询周免角色
struct SLinkerQueryQueryWeekFreeActorReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 查询竞技模式进入次数
struct SLinkerQueryCompEnterNumReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 购买竞技模式进入次数
struct SLinkerBuyCompEnterNumReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	BYTE buy_mode;

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 查询1v1模式进入次数
struct SLinkerQuery1V1EnterNumReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

// 购买1v1模式进入次数
struct SLinkerBuy1V1EnterNumReq
{
	UINT32 userId;	//账号id
	UINT32 roleId;	//角色id
	UINT16 buy_num;

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		buy_num = htons(buy_num);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		buy_num = ntohs(buy_num);
	}
};

#pragma pack(pop)

#endif