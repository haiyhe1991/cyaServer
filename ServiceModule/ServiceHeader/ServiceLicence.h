#ifndef __SERVICE_LICENCE_H__
#define __SERVICE_LICENCE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if !defined(SERVICE_UNREAL_BUILD)
	#include "cyaTypes.h"
#endif	//SERVICE_UNREAL_BUILD

/***********************************************************************************
							验证服务器相关数据结构
***********************************************************************************/

#pragma pack(push, 1)

//用户登陆验证服务, LICENCE_USER_LOGIN
struct SUserTokenLoginReq	//请求(模式0)
{
	BYTE mode;			//验证模式0
	char token[33];		//密码
	char loginPlat[17];	//登陆平台
	char devid[65];		//设备号
	char model[17];		//机型
	char os[17];	//操作系统
	char ver[17];		//当前包版本
};

struct SUserPswLoginReq	//请求(模式1)
{
	BYTE mode;			//验证模式1
	char username[33];	//账号
	char password[33];	//密码
	char loginPlat[17];	//登陆平台
	char devid[65];		//设备号
	char model[17];		//机型
	char os[17];		//操作系统
	char ver[17];		//当前包版本
};

struct SUserLoginRes	//响应
{
	UINT32 userId;		//用户id
	char username[33];	//账号名

	void hton()
		{	userId = htonl(userId);	}

	void ntoh()
		{	userId = ntohl(userId);	}
};

//注册用户, LICENCE_USER_REGIST
struct SUserRegistReq
{
	char username[33];	//账号
	char password[33];	//密码
	char devid[65];		//设备号
	char loginPlat[17];	//登陆平台
	char model[17];		//机型
	char os[17];		//操作系统
	char ver[17];		//当前包版本
};

//用户查询分区, LICENCE_USER_QUERY_PARTITION
struct SUserQueryPartitionReq	//请求
{
	UINT16 from;	//开始第几个分区
	UINT16 num;		//查询数量

	void hton()
	{	
		from = htons(from);
		num = htons(num);	
	}

	void ntoh()
	{	
		from = ntohs(from);
		num = ntohs(num);	
	}
};

struct SPartitionInfo	//分区信息
{
	UINT16 partitionId;		//分区id
	char   name[33];		//分区名称
	BYTE   isRecommend;		//是否推荐分区
	BYTE   status;			//分区状态

	void hton()
		{	partitionId = htons(partitionId);	}

	void ntoh()
		{	partitionId = ntohs(partitionId);	}
};

struct SUserQueryPartitionRes	//响应
{
	UINT16 totals;			//分区总数
	UINT16 num;		//返回数量
	SPartitionInfo data[1];	//返回分区信息数组

	void hton()
	{	
		for(UINT16 i = 0; i < num; ++i)
			data[i].hton();

		num = htons(num);
		totals = htons(totals);	
	}

	void ntoh()
	{	
		totals = ntohs(totals);
		num = ntohs(num);
		for(UINT16 i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//用户进入分区, LICENCE_USER_ENTER_PARTITION
struct SUserEnterPartitionReq	//请求
{
	UINT16 partitionId;	//分区id
	UINT32 userId;		//账号id

	void hton()
	{
		partitionId = htons(partitionId);
		userId = htonl(userId);
	}

	void ntoh()
	{	
		partitionId = ntohs(partitionId);
		userId = ntohl(userId);
	}
};

struct SUserEnterPartitionRes	//响应
{
	UINT16 partitionId;	//分区id
	UINT32 userId;		//账号id
	char token[33];		//token
	UINT32 linkerIp;	//分区link服务器IP地址
	UINT16 linkerPort;	//分区link服务器端口

	void hton()
	{
		partitionId = htons(partitionId);
		userId = htonl(userId);
		linkerIp = htonl(linkerIp);
		linkerPort = htons(linkerPort);
	}

	void ntoh()
	{
		partitionId = ntohs(partitionId);
		userId = ntohl(userId);
		linkerIp = ntohl(linkerIp);
		linkerPort = ntohs(linkerPort);
	}
};

//用户查询登陆信息, LICENCE_QUERY_PARTITION_ROLES
struct SUserQueryLoginReq	//请求
{
	UINT32 userId;
	void hton()
		{	userId = htonl(userId);	}

	void ntoh()
		{	userId = ntohl(userId);	}
};

struct SUserQueryLoginRes
{
	UINT16 lastLoginPartId;		//最后一次登陆分区id
	char   name[33];			///最后一次登陆分区名称
	char   lastLoginDate[20];	//最后一次登陆风趣时间
	void hton()
		{	lastLoginPartId = htons(lastLoginPartId);	}

	void ntoh()
		{	lastLoginPartId = ntohs(lastLoginPartId);	}
};

//查询已解锁职业
struct SQueryUnlockJobsReq
{
	UINT32 userId;
	void hton()
		{	userId = htonl(userId);	}

	void ntoh()
		{	userId = ntohl(userId);	}
};

struct SQueryUnlockJobsRes
{
	UINT16 num;		//解锁数量
	UINT16 data[1];	//解锁职业类型

	void hton()
	{
		for(UINT16 i = 0; i < num; ++i)
			data[i] = htons(data[i]);
		num = htons(num);
	}

	void ntoh()
	{
		num = ntohs(num);
		for(UINT16 i = 0; i < num; ++i)
			data[i] = ntohs(data[i]);
	}
};

#pragma pack(pop)

#endif	//_SGSLicence_h__