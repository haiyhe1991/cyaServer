#ifndef __MAU_PROTOCOL_H__
#define __MAU_PROTOCOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#include "cyaTypes.h"
#include "ServiceCmdCode.h"
#include "ServiceErrorCode.h"

#pragma pack(push, 1)

struct SLinkerCreateMauTabelReq	//进入牌桌频道
{
	BYTE linkerId;	//所在link服务器编号
	UINT32 userId;	//当前用户id
	UINT32 roleId;	//角色id
	char nick[33];	//角色昵称

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

struct SLinkerJoinMauTabelReq	//进入牌桌频道
{
	BYTE linkerId;	//所在link服务器编号
	UINT32 userId;	//当前用户id
	UINT32 roleId;	//角色id
	char nick[33];	//角色昵称
	UINT32 joinId;	//加入目标id(公会id, channel为CHAT_WORLD_CHANNEL时该值无效)

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

struct SLinkerReadyTabelReq	//离开牌桌频道
{
	UINT32 userId;	//当前用户id
	UINT32 roleId;	//角色id
	UINT32 joinId;	//离开的频道id, channel为CHAT_WORLD_CHANNEL时该值无效
	BYTE ready;

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

struct SLinkerLeaveMauTabelReq	//离开牌桌频道
{
	UINT32 userId;	//当前用户id
	UINT32 roleId;	//角色id
	UINT32 joinId;	//离开的频道id, channel为CHAT_WORLD_CHANNEL时该值无效

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

struct SLinkerMauBasicSend		//发送聊天信息
{
	UINT32 userId;	//当前用户id
	UINT32 roleId;	//角色id
	UINT32 targetId;	//聊天对象目标(个人或公会)

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

struct SLinkerMauRecvObj
{
	UINT32 recvUserId;	//接收者用户id
	void hton()
	{
		recvUserId = htonl(recvUserId);
	}

	void ntoh()
	{
		recvUserId = ntohl(recvUserId);
	}
};

struct SLinkerChatUserExitReq
{
	UINT32 userId;	//用户id
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

//sizeof(UINT16) + n* sizeof(SLinkerChatRecvObj) + data

#pragma pack(pop)

#endif