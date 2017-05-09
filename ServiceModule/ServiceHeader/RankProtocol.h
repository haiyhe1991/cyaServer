#ifndef __RANK_PROTOCOL_H__
#define __RANK_PROTOCOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "ServiceRank.h"

#pragma pack(push, 1)

//��ѯ��ҵ�������Ϣ����, RANK_QUERY_ROLE_INFO
struct SLinkerGetRankInfoReq
{
	//  BYTE    linkerId;       // ����link���������
	UINT32  userId;         // ��ǰ�û�id
	UINT32  roleId;         // ��ɫid
	SGetRankInfoReq info;   // ���в�ѯ���ͺ�һ�β�ѯ��������Ŀ

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		info.hton();
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		info.ntoh();
	}
};

//��ѯ��ǰ��ҵ�ĳ��������Ϣ, RANK_QUERY_ROLE_RANK
struct SLinkerGetRoleRankInfoReq
{
	//  BYTE    linkerId;           // ����link���������
	UINT32  userId;             // ��ǰ�û�id
	UINT32  roleId;             // ��ɫid
	SGetRoleRankInfoReq info;   // ��ѯ��ɫ��ĳ������(�ȼ�/ս����/����)

	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		info.hton();
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		info.ntoh();
	}
};

#pragma pack(pop)

#endif