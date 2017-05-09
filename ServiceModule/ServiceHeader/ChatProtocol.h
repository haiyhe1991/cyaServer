#ifndef __CHAT_PROTOCOL_H__
#define __CHAT_PROTOCOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"
#include "ServiceCmdCode.h"
#include "ServiceErrorCode.h"

#pragma pack(push, 1)

struct SLinkerJoinChannelReq	//��������Ƶ��
{
	BYTE linkerId;	//����link���������
	UINT32 userId;	//��ǰ�û�id
	UINT32 roleId;	//��ɫid
	char nick[33];	//��ɫ�ǳ�
	BYTE channel;	//����Ƶ������
	UINT32 joinId;	//����Ŀ��id(����id, channelΪCHAT_WORLD_CHANNELʱ��ֵ��Ч)

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

struct SLinkerLeaveChannelReq	//�뿪����Ƶ��
{
	UINT32 userId;	//��ǰ�û�id
	UINT32 roleId;	//��ɫid
	BYTE   channel;		//�뿪Ƶ������
	UINT32 joinId;	//�뿪��Ƶ��id, channelΪCHAT_WORLD_CHANNELʱ��ֵ��Ч

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

struct SLinkerChatMessageSend		//����������Ϣ
{
	UINT32 userId;	//��ǰ�û�id
	UINT32 roleId;	//��ɫid
	BYTE channel;		//Ƶ������
	UINT32 targetId;	//�������Ŀ��(���˻򹫻�)
	UINT16 msgBytes;	//��Ϣ�ֽ���
	char  msg[1];		//��Ϣ

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
	UINT32 recvUserId;	//�������û�id
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
	UINT32 userId;	//�û�id
	UINT32 roleId;	//��ɫid
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