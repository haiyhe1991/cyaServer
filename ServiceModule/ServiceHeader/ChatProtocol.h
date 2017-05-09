#ifndef __CHAT_PROTOCOL_H__
#define __CHAT_PROTOCOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"
#include "ServiceCmdCode.h"
#include "ServiceErrorCode.h"

#pragma pack(push, 1)

struct SLinkerJoinChannelReq	//进入聊天频道
{
	BYTE linkerId;	//所在link服务器编号
	UINT32 userId;	//当前用户id
	UINT32 roleId;	//角色id
	char nick[33];	//角色昵称
	BYTE channel;	//加入频道类型
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

struct SLinkerLeaveChannelReq	//离开聊天频道
{
	UINT32 userId;	//当前用户id
	UINT32 roleId;	//角色id
	BYTE   channel;		//离开频道类型
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

struct SLinkerChatMessageSend		//发送聊天信息
{
	UINT32 userId;	//当前用户id
	UINT32 roleId;	//角色id
	BYTE channel;		//频道类型
	UINT32 targetId;	//聊天对象目标(个人或公会)
	UINT16 msgBytes;	//信息字节数
	char  msg[1];		//消息

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		targetId = htonl(targetId);
		msgBytes = htons(msgBytes);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		targetId = ntohl(targetId);
		msgBytes = ntohs(msgBytes);
	}
};

struct SLinkerChatRecvObj
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