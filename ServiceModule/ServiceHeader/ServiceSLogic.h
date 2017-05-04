#ifndef __SERVICE_S_LOGIC__
#define __SERVICE_S_LOGIC__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if !defined(SERVICE_UNREAL_BUILD)
	#include "cyaTypes.h"
#endif	//SERVICE_UNREAL_BUILD

#pragma pack(push, 1)

//查询等级排行榜, SLS_QUERY_LEVEL_RANK
struct SQueryLevelRankReq
{
	BYTE from;	//开始位置;
	BYTE num;	//数量;
};

struct SLevelRankInfo	//等级排行数据
{
	UINT16 rank;			//排名
	UINT32 roleId;			//角色ID
	char   roleNick[33];    //角色昵称
	BYTE   title;			//角色职业称号
	UINT16 roleType;		//角色类型
	BYTE   roleSex;			//角色性别
	UINT32 cashcount;		//充值总金额
	UINT32 guildId;			//公会ID
	char   guildNick[33];   //公会名称
	BYTE   level;		 	//等级
	UINT16 trend;    		//排名趋势
	void hton()
	{
		rank = htons(rank);
		roleId = htonl(roleId);
		roleType = htons(roleType);
		cashcount = htonl(cashcount);
		guildId = htonl(guildId);
		trend = htons(trend);
	}
	void ntoh()
	{
		rank = ntohs(rank);
		roleId = ntohl(roleId);
		roleType = ntohs(roleType);
		cashcount = ntohl(cashcount);
		guildId = ntohl(guildId);
		trend = ntohs(trend);
	}
};

struct SQueryLevelRankRes	//查询等级排行榜返回包
{
	BYTE total;		//排名信息总数;
	BYTE from;		//开始位置;
	BYTE retNum;	//返回信息数量;
	SLevelRankInfo lvRankInfos[1];	//array<等级排行数据>    返回的信息;
};

// 查询战力排行榜
struct SQueryCPRankReq	//查询战力排行榜数据
{
	BYTE from;	//开始位置;
	BYTE num;	//数量;
};

struct SCPRankInfo	//战斗力排行数据
{
	UINT16 rank;			//排名
	UINT32 roleId;			//角色ID
	char   roleNick[33];	//角色昵称
	BYTE   title;			//角色职业称号
	BYTE   roleSex;			//角色性别
	UINT16 roleType;		//角色类型
	UINT32 cashcount;		//充值总金额
	UINT32 guildId;			//公会ID
	char   guildNick[33];   //公会名称
	UINT32 cp;				//战斗力
	UINT16 trend;			//排名趋势
	void hton()
	{
		rank = htons(rank);
		roleId = htonl(roleId);
		roleType = htons(roleType);
		cashcount = htonl(cashcount);
		guildId = htonl(guildId);
		trend = htons(trend);
		cp = htonl(cp);
	}
	void ntoh()
	{
		rank = ntohs(rank);
		roleId = ntohl(roleId);
		roleType = ntohs(roleType);
		cashcount = ntohl(cashcount);
		guildId = ntohl(guildId);
		trend = ntohs(trend);
		cp = ntohl(cp);
	}
};

struct SQueryCPRankRes	//查询战力排行榜返回包
{
	BYTE total;		//排名信息总数;
	BYTE from;		//开始位置;
	BYTE retNum;	//返回信息数量;
	SCPRankInfo cpRankInfos[1];	//array<战斗力排行数据>  返回的信息;
};

//修改成就数据
struct SModAchievementReq//成就数据修改通知
{
	UINT16 id;		//成就ID;
	UINT32 data[4];	//成就完成数据[4];
	void hton()
	{
		id = htons(id);
		for(int i = 0; i < 4; ++i)
			data[i] = htonl(data[i]);
	}
	void ntoh()
	{
		id = ntohs(id);
		for(int i = 0; i < 4; ++i)
			data[i] = ntohl(data[i]);
	}
};

#pragma pack(pop)

#endif	//_SGSSLogic_h__