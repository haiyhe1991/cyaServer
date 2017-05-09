#ifndef __RANK_PROTOCOL_H__
#define __RANK_PROTOCOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "ServiceRank.h"

#pragma pack(push, 1)

//查询玩家的排行信息请求, RANK_QUERY_ROLE_INFO
struct SLinkerGetRankInfoReq
{
	//  BYTE    linkerId;       // 所在link服务器编号
	UINT32  userId;         // 当前用户id
	UINT32  roleId;         // 角色id
	SGetRankInfoReq info;   // 排行查询类型和一次查询结果最大数目

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

//查询当前玩家的某项排行信息, RANK_QUERY_ROLE_RANK
struct SLinkerGetRoleRankInfoReq
{
	//  BYTE    linkerId;           // 所在link服务器编号
	UINT32  userId;             // 当前用户id
	UINT32  roleId;             // 角色id
	SGetRoleRankInfoReq info;   // 查询角色的某项排名(等级/战斗力/副本)

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