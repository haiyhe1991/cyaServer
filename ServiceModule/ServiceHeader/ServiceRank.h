#ifndef __SERVICE_RANK_H__
#define __SERVICE_RANK_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if !defined(SGS_UNREAL_BUILD)
	#include "cyaTypes.h"
#endif //SGS_UNREAL_BUILD

/***********************************************************************************
排行相关数据结构
***********************************************************************************/
#define MAX_RANK_MSG_BYTES  1024    //排行信息查询响应消息最大长度
#define MAX_RANK_NUM        100     //排行榜显示记录数
#define RANK_NICK_LEN       32

#pragma pack(push, 1)

//排行信息查询类型
enum ERankType
{
	RANK_ACTOR_LEVEL = 1,   // 角色等级
	RANK_ACTOR_CP = 2,   // 角色战斗力
	RANK_ACTOR_DUPLICATE = 3,   // 角色副本
	RANK_ACTOR_POINT = 4,   // 角色成就点数
	RANK_ACTOR_COMP = 5,   // 角色竞技模式
	RANK_ACTOR_TYPE_MAX
};

// 排行信息查询请求, RANK_QUERY_ROLE_INFO
struct SGetRankInfoReq
{
	BYTE    type;               // 排行查询类型
	BYTE    num;                // 一次查询结果最大数目
	UINT32  start;              // 查询开始位置(从第几个开始查询, 值为[0~n]*num)

	void hton(){ start = htonl(start); }
	void ntoh(){ start = ntohl(start); }
};

// 查询角色的某项排名(等级/战斗力/副本), RANK_QUERY_ROLE_RANK
struct SGetRoleRankInfoReq
{
	BYTE    querytype;          // 排行查询类型
	UINT32  roleId;             // 角色ID

	void hton(){ roleId = htonl(roleId); }
	void ntoh(){ roleId = ntohl(roleId); }
};

// 排行信息查询结果
struct SGetRankInfoResult
{
	BYTE    type;               // 排行查询类型
	BYTE    num;                // 一次查询结果的记录数目
};

// 角色等级查询结果信息
struct SRoleLevelAttr
{
	UINT32  roleid;
	UINT16  roleType;                   // 角色类型
	char    roleNick[RANK_NICK_LEN + 1];  // 角色昵称
	BYTE    level;                      // 角色等级

	void hton()
	{
		roleid = htonl(roleid);
		roleType = htons(roleType);
	}
	void ntoh()
	{
		roleid = ntohl(roleid);
		roleType = ntohs(roleType);
	}
};

// 角色战斗力查询结果信息
struct SRoleCPAttr
{
	UINT32  roleid;
	UINT16  roleType;                   // 角色类型
	char    roleNick[RANK_NICK_LEN + 1];  // 角色昵称
	UINT32  cp;                         // 角色战斗力

	void hton()
	{
		roleid = htonl(roleid);
		roleType = htons(roleType);
		cp = htonl(cp);
	}

	void ntoh()
	{
		roleid = ntohl(roleid);
		roleType = ntohs(roleType);
		cp = ntohl(cp);
	}
};

// 角色副本查询结果信息
struct SRoleInstAttr
{
	UINT32  roleid;
	UINT16  roleType;                   // 角色类型
	char    roleNick[RANK_NICK_LEN + 1];  // 角色昵称
	UINT16  instId;                     // 角色副本

	void hton()
	{
		roleid = htonl(roleid);
		roleType = htons(roleType);
		instId = htons(instId);
	}

	void ntoh()
	{
		roleid = ntohl(roleid);
		roleType = ntohs(roleType);
		instId = ntohs(instId);
	}
};

// 角色成就点数查询结果信息
struct SRolePointAttr
{
	UINT32  roleid;
	UINT16  roleType;                   // 角色类型
	char    roleNick[RANK_NICK_LEN + 1];  // 角色昵称
	UINT32  point;                      // 角色成就点数

	void hton()
	{
		roleid = htonl(roleid);
		roleType = htons(roleType);
		point = htonl(point);
	}

	void ntoh()
	{
		roleid = ntohl(roleid);
		roleType = ntohs(roleType);
		point = ntohl(point);
	}
};

// 角色竞技模式查询结果信息
struct SRoleCompAttr
{
	UINT32  roleid;
	UINT16  roleType;                   // 角色类型
	char    roleNick[RANK_NICK_LEN + 1];  // 角色昵称
	UINT32  competitive;                // 角色竞技模式

	void hton()
	{
		roleid = htonl(roleid);
		roleType = htons(roleType);
		competitive = htonl(competitive);
	}

	void ntoh()
	{
		roleid = ntohl(roleid);
		roleType = ntohs(roleType);
		competitive = ntohl(competitive);
	}
};

// 角色等级排名查询结果
struct SRoleLevelRank
{
	BYTE    level;                      // 角色等级
	UINT32  level_rank;                 // 角色等级排名

	void hton()
	{
		level_rank = htonl(level_rank);
	}

	void ntoh()
	{
		level_rank = ntohl(level_rank);
	}
};

// 角色战斗力排名查询结果
struct SRoleCPRank
{
	UINT32  cp;                         // 角色战斗力
	UINT32  cp_rank;                    // 角色战斗力排名

	void hton()
	{
		cp = htonl(cp);
		cp_rank = htonl(cp_rank);
	}

	void ntoh()
	{
		cp = ntohl(cp);
		cp_rank = ntohl(cp_rank);
	}
};

// 角色副本排名查询结果
struct SRoleInstRank
{
	UINT16  instId;                     // 角色副本
	UINT32  inst_rank;                  // 角色副本排名

	void hton()
	{
		instId = htons(instId);
		inst_rank = htonl(inst_rank);
	}

	void ntoh()
	{
		instId = ntohs(instId);
		inst_rank = ntohl(inst_rank);
	}
};

// 角色成就点数排名查询结果
struct SRolePointRank
{
	UINT32  point;                      // 角色成就点数
	UINT32  point_rank;                 // 角色成就点数排名

	void hton()
	{
		point = htonl(point);
		point_rank = htonl(point_rank);
	}

	void ntoh()
	{
		point = ntohl(point);
		point_rank = ntohl(point_rank);
	}
};

// 角色竞技模式排名查询结果
struct SRoleCompRank
{
	UINT32  competitive;                // 角色竞技模式
	UINT32  comp_rank;                  // 角色竞技模式排名

	void hton()
	{
		competitive = htonl(competitive);
		comp_rank = htonl(comp_rank);
	}

	void ntoh()
	{
		competitive = ntohl(competitive);
		comp_rank = ntohl(comp_rank);
	}
};

// 角色等级排行信息查询结果
struct SGetRoleLevelRes
{
	SGetRankInfoResult  result;     // 排行查询类型和记录数目
	SRoleLevelAttr      levels[1];  // 角色等级排行信息
};

// 角色战斗力排行信息查询结果
struct SGetRoleCPRes
{
	SGetRankInfoResult  result;     // 排行查询类型和记录数目
	SRoleCPAttr         cps[1];     // 角色战斗力排行信息
};

// 角色副本排行信息查询结果
struct SGetRoleInstRes
{
	SGetRankInfoResult  result;     // 排行查询类型和记录数目
	SRoleInstAttr       insts[1];   // 角色副本排行信息
};

// 角色成就点数排名查询结果
struct SGetRolePointRes
{
	SGetRankInfoResult  result;     // 排行查询类型和记录数目
	SRolePointAttr      points[1];  // 角色成就点数排行信息
};

// 角色竞技模式排名查询结果
struct SRoleCompetitiveRank
{
	SGetRankInfoResult  result;     // 排行查询类型和记录数目
	SRoleCompAttr       points[1];  // 角色竞技模式排行信息
};

// 角色所有排行信息的查询结果
struct SGetRoleAllRankRes
{
	BYTE    querytype;              // 排行查询类型
	CHAR    data[1];                // 角色所有排行信息
};

//统一写日志的基本信息
struct SGSLogBase
{
	BYTE    level;	        //日志级别(参见roselogex.h的定义)
	BYTE    module;	        //发送日志的服务器(参见roselogex.h的定义)
	LTMSEL  time;           //写日志的时间

	void hton()
	{
		//time = xs_hton(time);
	}

	void ntoh()
	{
		//time = xs_ntoh(time);
	}
};

//统一写日志的日志头
struct SGSLogHead
{
	UINT16      cmdcode;    //命令码(RANK_SEND_LOG_INFO)
	SGSLogBase  base;
	void hton()
	{
		cmdcode = htons(cmdcode);
		//base.hton();
	}

	void ntoh()
	{
		cmdcode = ntohs(cmdcode);
		//base.ntoh();
	}
};

#pragma pack(pop)

#endif