#ifndef __SERVICE_CHAT_H__
#define __SERVICE_CHAT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if !defined(SERVICE_UNREAL_BUILD)
	#include "cyaTypes.h"
#endif	//SERVICE_UNREAL_BUILD

/***********************************************************************************
							聊天相关数据结构
***********************************************************************************/
#define MAX_CHAT_MSG_BYTES	1024		//聊天信息最大长度为1024字节

#pragma pack(push, 1)

enum EChatChannelType	//频道类型
{
	CHAT_WORLD_CHANNEL = 1,		//世界频道
	CHAT_TEAM_CHANNEL = 2,		//队伍频道
	CHAT_UNIONS_CHANNEL = 3,	//公会频道
	CHAT_PRIVATE_CHANNEL = 4,	//私聊频道
};

//进入聊天频道, CHAT_JOIN_CHANNEL
struct SJoinChannelReq
{
	char nick[33];	//角色昵称
	BYTE channel;	//加入频道类型
	UINT32 joinId;	//加入目标id(公会id, channel为CHAT_WORLD_CHANNEL时该值无效)

	void hton()
		{	joinId = htonl(joinId);		}

	void ntoh()
		{	joinId = ntohl(joinId);		}
};

//离开聊天频道, CHAT_LEAVE_CHANNEL
struct SLeaveChannelReq
{
	BYTE channel;	//离开频道类型
	UINT32 joinId;	//离开的频道srcId, channel为CHAT_WORLD_CHANNEL时该值无效

	void hton()
		{	joinId = htonl(joinId);		}

	void ntoh()
		{	joinId = ntohl(joinId);		}
};

//发送聊天信息, CHAT_SEND_MESSAGE
struct SChatMessageSend
{
	BYTE   channel;		//频道类型
	UINT32 targetId;	//聊天对象目标(个人或公会)
	UINT16 msgBytes;	//信息字节数
	char   msg[1];

	void hton()
	{
		msgBytes = htons(msgBytes);
		targetId = htonl(targetId);
	}

	void ntoh()
	{
		msgBytes = ntohs(msgBytes);
		targetId = ntohl(targetId);
	}
};

//收到聊天信息, CHAT_RECV_MESSAGE
struct SChatMessageRecv
{
	BYTE channel;			//频道类型
	UINT32 sendSourceId;	//发送源id
	char   nick[33];		//发送源昵称
	UINT32 sendTargetId;	//发送目标id(channel==CHAT_WORLD_CHANNEL时为自己当前角色id, channel==CHAT_UNIONS_CHANNEL时为公会频道id)
	UINT16 msgBytes;	//信息字节数
	char   msg[1];			//信息

	void hton()
	{
		msgBytes = htons(msgBytes);
		sendSourceId = htonl(sendSourceId);
		sendTargetId = htonl(sendTargetId);
	}

	void ntoh()
	{
		msgBytes = ntohs(msgBytes);
		sendSourceId = ntohl(sendSourceId);
		sendTargetId = ntohl(sendTargetId);
	}
};

//聊天对象不在线通知, CHAT_USER_OFFLINE_NOTIFY
struct SChatTargetOfflineNotify
{
	BYTE   channel;		//频道类型
	UINT32 targetId;	//聊天对象目标(个人或公会)
	UINT16 msgBytes;	//信息字节数
	char   msg[1];

	void hton()
	{
		msgBytes = htons(msgBytes);
		targetId = htonl(targetId);
	}

	void ntoh()
	{
		msgBytes = ntohs(msgBytes);
		targetId = ntohl(targetId);
	}
};

#pragma pack(pop)

#endif	//__SERVICE_CHAT_H__