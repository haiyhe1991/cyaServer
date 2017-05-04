#ifndef __SERVICE_CHAT_H__
#define __SERVICE_CHAT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if !defined(SERVICE_UNREAL_BUILD)
	#include "cyaTypes.h"
#endif	//SERVICE_UNREAL_BUILD

/***********************************************************************************
							����������ݽṹ
***********************************************************************************/
#define MAX_CHAT_MSG_BYTES	1024		//������Ϣ��󳤶�Ϊ1024�ֽ�

#pragma pack(push, 1)

enum EChatChannelType	//Ƶ������
{
	CHAT_WORLD_CHANNEL = 1,		//����Ƶ��
	CHAT_TEAM_CHANNEL = 2,		//����Ƶ��
	CHAT_UNIONS_CHANNEL = 3,	//����Ƶ��
	CHAT_PRIVATE_CHANNEL = 4,	//˽��Ƶ��
};

//��������Ƶ��, CHAT_JOIN_CHANNEL
struct SJoinChannelReq
{
	char nick[33];	//��ɫ�ǳ�
	BYTE channel;	//����Ƶ������
	UINT32 joinId;	//����Ŀ��id(����id, channelΪCHAT_WORLD_CHANNELʱ��ֵ��Ч)

	void hton()
		{	joinId = htonl(joinId);		}

	void ntoh()
		{	joinId = ntohl(joinId);		}
};

//�뿪����Ƶ��, CHAT_LEAVE_CHANNEL
struct SLeaveChannelReq
{
	BYTE channel;	//�뿪Ƶ������
	UINT32 joinId;	//�뿪��Ƶ��srcId, channelΪCHAT_WORLD_CHANNELʱ��ֵ��Ч

	void hton()
		{	joinId = htonl(joinId);		}

	void ntoh()
		{	joinId = ntohl(joinId);		}
};

//����������Ϣ, CHAT_SEND_MESSAGE
struct SChatMessageSend
{
	BYTE   channel;		//Ƶ������
	UINT32 targetId;	//�������Ŀ��(���˻򹫻�)
	UINT16 msgBytes;	//��Ϣ�ֽ���
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

//�յ�������Ϣ, CHAT_RECV_MESSAGE
struct SChatMessageRecv
{
	BYTE channel;			//Ƶ������
	UINT32 sendSourceId;	//����Դid
	char   nick[33];		//����Դ�ǳ�
	UINT32 sendTargetId;	//����Ŀ��id(channel==CHAT_WORLD_CHANNELʱΪ�Լ���ǰ��ɫid, channel==CHAT_UNIONS_CHANNELʱΪ����Ƶ��id)
	UINT16 msgBytes;	//��Ϣ�ֽ���
	char   msg[1];			//��Ϣ

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

//�����������֪ͨ, CHAT_USER_OFFLINE_NOTIFY
struct SChatTargetOfflineNotify
{
	BYTE   channel;		//Ƶ������
	UINT32 targetId;	//�������Ŀ��(���˻򹫻�)
	UINT16 msgBytes;	//��Ϣ�ֽ���
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