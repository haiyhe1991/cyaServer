#ifndef __SERVICE_MAU_H__
#define __SERVICE_MAU_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#if !defined(SGS_UNREAL_BUILD)
#include "cyaTypes.h"
#endif //SGS_UNREAL_BUILD

/***********************************************************************************
变色龙相关数据结构
***********************************************************************************/
#pragma pack(push, 1)

//创建牌桌, MAU_CREATE_TABLE
struct SCreateTabelReq	
{
	char nick[33];	//角色昵称
};

//进入牌桌, MAU_JOIN_TABLE
struct SJoinTableReq
{
	char nick[33];	//角色昵称
	UINT32 joinId;	//加入目标id(公会id, channel为CHAT_WORLD_CHANNEL时该值无效)

	void hton()
	{
		joinId = htonl(joinId);
	}

	void ntoh()
	{
		joinId = ntohl(joinId);
	}
};

//进入牌桌, MAU_JOIN_TABLE
struct SJoinTableInfo
{
	UINT32 userId;	//当前用户id
	UINT32 roleId;	//角色id
	char nick[33];	//角色昵称
	UINT32 joinId;	//加入目标id(公会id, channel为CHAT_WORLD_CHANNEL时该值无效)
	BYTE tableId;	//牌桌id
	BYTE ready;		//准备（0不准备，1准备）

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		joinId = htonl(joinId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		joinId = ntohl(joinId);
	}
};

//进入牌桌, MAU_JOIN_TABLE
struct SJoinTableRes
{
	BYTE rolesCount;	//角色个数
	SJoinTableInfo roles[1];
};

//进入牌桌, MAU_JOIN_TABLE
struct SReadyTableReq
{
	UINT32 joinId;	//加入目标id(公会id, channel为CHAT_WORLD_CHANNEL时该值无效)
	BYTE ready;		//准备（0不准备，1准备）

	void hton()
	{
		joinId = htonl(joinId);
	}

	void ntoh()
	{
		joinId = ntohl(joinId);
	}
};

struct SReadyTableRes
{
	BYTE tableId;	//牌桌id
	BYTE ready;		//准备（0不准备，1准备）
};

//离开牌桌, MAU_LEAVE_TABLE
struct SLeaveTableReq
{
	UINT32 joinId;	//离开的频道srcId, channel为CHAT_WORLD_CHANNEL时该值无效

	void hton()
	{
		joinId = htonl(joinId);
	}

	void ntoh()
	{
		joinId = ntohl(joinId);
	}
};

#pragma pack(pop)

#endif