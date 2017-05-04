#ifndef __LCS_PROTOCOL_H__
#define __LCS_PROTOCOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"

#define LCS_SQL_BUF_SIZE 1024

#pragma pack(push, 1)

//linker信息
struct SLinkerInfo
{
	BYTE linkerId;	//linker id
	UINT32 dwIp;	//linker服务器ip
	UINT16 port;	//linker服务器端口
	UINT32 conns;	//linker服务器当前连接数
	BYTE  status;	//linker服务器当前状态
};

//linker通知验证服务器用户验证通过, LICENCE_LINKER_USER_CHECK
struct SLinkerNotifyUserCheck
{
	UINT32 userId;	//账号id
	void hton()
		{	userId = htonl(userId);	}
	void ntoh()
		{	userId = ntohl(userId);	}
};

//验证服务器通知linker用户下线, LICENCE_NOTIFY_USER_OFFLINE
struct SLinkerNotifyUserOffline
{
	UINT32 userId;	//账号id
	BYTE   linkerId;	//linker id
	void hton()
		{	userId = htonl(userId);	}
	void ntoh()
		{	userId = ntohl(userId);	}
};

/// Linker登录验证服务器, LICENCE_LINKER_LOGIN
struct SLinkerLoginReq
{
	UINT16 partitionId;	//分区id
	BYTE linkerId;	//link服务器编号
	UINT32 linkerIp;	//link服务器IP
	UINT16 linkerPort;	//link服务器端口
	char   linkerToken[33];	//验证码
	UINT32 subareIp;	//分区管理服务对外IP
	UINT16 subarePort;	//分区管理服务对外端口

	void hton()
	{
		partitionId = htons(partitionId);
		linkerIp = htonl(linkerIp);
		linkerPort = htons(linkerPort);
		subareIp = htonl(subareIp);
		subarePort = htons(subarePort);
	}

	void ntoh()
	{
		partitionId = ntohs(partitionId);
		linkerIp = ntohl(linkerIp);
		linkerPort = ntohs(linkerPort);
		subareIp = ntohl(subareIp);
		subarePort = ntohs(subarePort);
	}
};

//上报linker当前连接数, LICENCE_LINKER_REPORT_CONNECTIONS
struct SLinkerReportConnections	
{
	UINT32 connections;	//link服务器当前连接数
	UINT32 tokens;		//token数

	void hton()
	{
		connections = htonl(connections);
		tokens = htonl(tokens);
	}

	void ntoh()
	{
		connections = ntohl(connections);
		tokens = ntohl(tokens);
	}
};

//获取上报linker连接数时间间隔(单位:秒), LICENCE_GET_REPORT_TIMER
struct SLinkerGetReportTimerRes
{
	UINT32 timerSec;	//秒
	void hton()
		{	timerSec = htonl(timerSec);		}

	void ntoh()
		{	timerSec = ntohl(timerSec);		}
};


//查询分区状态, LICENCE_LINKER_QUERY_PARTITION_STATUS
#define PARTITION_STATUS_OK		0	//正常
#define PARTITION_STATUS_MAINTAIN	1	//维护
struct SPartitionStatusQueryRes
{
	BYTE status;	//0-正常,1-维护
};

//从linker服务器中获取用户token请求, LICENCE_LINKER_GET_TOKEN
struct SLinkerGetUserTokenReq
{
	UINT32 userId;		//账号id
	char username[33];	//账号名

	void hton()
		{	userId = htonl(userId);	}

	void ntoh()
		{	userId = ntohl(userId);	}
};

struct SLinkerGetUserTokenRes
{
	UINT32 userId;		//账号id
	char token[33];		//token

	void hton()
		{	userId = htonl(userId);	}

	void ntoh()
		{	userId = ntohl(userId);	}
};

//查询已解锁职业
struct SLinkerQueryUnlockJobsReq
{
	UINT32 userId;
	UINT32 roleId;
	BYTE type;
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

struct SLinkerQueryUnlockJobsRes
{
	UINT32 userId;
	UINT32 roleId;
	UINT16 num;		//解锁数量
	UINT16 data[1];	//解锁职业类型

	void hton()
	{	
		for(UINT16 i = 0; i < num; ++i)
			data[i]= htons(data[i]);
		num = htons(num);
		userId = htonl(userId);
		roleId = htonl(roleId);	
	}

	void ntoh()
	{
		num = ntohs(num);
		for(UINT16 i = 0; i < num; ++i)
			data[i]= ntohs(data[i]);
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

#define REPORT_ROLE_CREATE	0 /// 角色创建
#define REPORT_ROLE_DELETE	1 /// 角色删除
/// LinkS向验证服务器汇报角色创建或者删除
/// 客户端请求角色创建或者删除
/// 发往角色服务器的角色创建或者删除, LICENCE_LINKER_ROLE_CHANGE
struct SRoleOperReq	
{
	UINT32 userId;		/// 账户ID
	//UINT32 roleId;		/// 角色ID
	BYTE model;			/// 模式 0:创建,1:删除

	void hton()
	{	
		userId = htonl(userId);
		//roleId = htonl(roleId);
	}

	void ntoh()
	{	
		userId = ntohl(userId);
		//roleId = ntohl(roleId);
	}
};

#pragma pack(pop)

#endif	//_LCSProtocol_h__