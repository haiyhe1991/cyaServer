#ifndef __SERVICE_MAU_H__
#define __SERVICE_MAU_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#if !defined(SGS_UNREAL_BUILD)
#include "cyaTypes.h"
#endif //SGS_UNREAL_BUILD

/***********************************************************************************
��ɫ��������ݽṹ
***********************************************************************************/
#pragma pack(push, 1)

//��������, MAU_CREATE_TABLE
struct SCreateTabelReq	
{
	char nick[33];	//��ɫ�ǳ�
};

//��������, MAU_JOIN_TABLE
struct SJoinTableReq
{
	char nick[33];	//��ɫ�ǳ�
	UINT32 joinId;	//����Ŀ��id(����id, channelΪCHAT_WORLD_CHANNELʱ��ֵ��Ч)

	void hton()
	{
		joinId = htonl(joinId);
	}

	void ntoh()
	{
		joinId = ntohl(joinId);
	}
};

//��������, MAU_JOIN_TABLE
struct SJoinTableInfo
{
	UINT32 userId;	//��ǰ�û�id
	UINT32 roleId;	//��ɫid
	char nick[33];	//��ɫ�ǳ�
	UINT32 joinId;	//����Ŀ��id(����id, channelΪCHAT_WORLD_CHANNELʱ��ֵ��Ч)
	BYTE tableId;	//����id
	BYTE ready;		//׼����0��׼����1׼����

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

//��������, MAU_JOIN_TABLE
struct SJoinTableRes
{
	BYTE rolesCount;	//��ɫ����
	SJoinTableInfo roles[1];
};

//��������, MAU_JOIN_TABLE
struct SReadyTableReq
{
	UINT32 joinId;	//����Ŀ��id(����id, channelΪCHAT_WORLD_CHANNELʱ��ֵ��Ч)
	BYTE ready;		//׼����0��׼����1׼����

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
	BYTE tableId;	//����id
	BYTE ready;		//׼����0��׼����1׼����
};

//�뿪����, MAU_LEAVE_TABLE
struct SLeaveTableReq
{
	UINT32 joinId;	//�뿪��Ƶ��srcId, channelΪCHAT_WORLD_CHANNELʱ��ֵ��Ч

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