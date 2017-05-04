#ifndef __SERVICE_EXT_MAN_H__
#define __SERVICE_EXT_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "ServiceCommon.h"
#include "ServiceManageLogic.h"

#pragma pack(push, 1)

//管理服务器, MANS_USER_LOGIN_CONFIRM
struct SExtManLoginReq
{
	char account[33];
	char pwd[33];
};

struct SExtManLoginRes
{
	UINT32 loginToken;	//验证码
	char   key[33];		//解码key
	void hton()
		{	loginToken = htonl(loginToken);	}
	void ntoh()
		{	loginToken = ntohl(loginToken);	}
};

//查询玩家账户分区, LCS_QUERY_ACCOUNT_PARTITION
struct SExtManQueryAccountPartitionReq
{
	UINT16 from;	//开始位置
	BYTE   num;		//数量
	void hton()
		{	from = htons(from);		}
	void ntoh()
		{	from = ntohs(from);		}
};

struct SAccountPartitionInfo
{
	UINT16 id;
	char   name[33];
	void hton()
		{	id = htons(id);		}
	void ntoh()
		{	id = ntohs(id);		}
};

struct SExtManQueryAccountPartitionRes
{
	UINT16 total;	//用户区总数
	BYTE   retNum;	//当前返回数量
	SAccountPartitionInfo userPartitions[1];
	void hton()
	{
		total = htons(total);
		for(BYTE i = 0; i < retNum; ++i)
			userPartitions[i].hton();
	}
	void ntoh()
	{
		total = ntohs(total);
		for(BYTE i = 0; i < retNum; ++i)
			userPartitions[i].ntoh();
	}
};

//查询玩家账户用户数据, LICENCE_QUERY_USERS
struct SExtManQueryPlayerAccountReq
{
	UINT16 userPartId;	//用户分区id
	UINT32 from;	//开始位置
	UINT16 num;		//查询数量
	void hton()
	{
		userPartId = htonl(userPartId);
		from = htonl(from);
		num = htons(num);
	}
	void ntoh()
	{
		userPartId = ntohl(userPartId);
		from = ntohl(from);
		num = ntohs(num);
	}
};

struct SAccountnfo
{
	UINT32 userId;
	char username[33];
	char ltts[20];
	char regtts[20];
	UINT32 visitNum;

	void hton()
	{
		userId = htonl(userId);
		visitNum = htonl(visitNum);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		visitNum = ntohl(visitNum);
	}
};

struct SExtManQueryPlayerAccountRes
{
	UINT32 total;		//总数
	UINT16 retNum;		//返回数量
	SAccountnfo users[1];	//用户数据
	void hton()
	{
		total = htonl(total);
		retNum = htons(retNum);
		for(UINT16 i = 0; i < retNum; ++i)
			users[i].hton();
	}
	void ntoh()
	{
		total = ntohl(total);
		retNum = ntohs(retNum);
		for(UINT16 i = 0; i < retNum; ++i)
			users[i].ntoh();
	}
};

//查询游戏分区, LCS_QUERY_GAME_PARTITION
struct SExtManQueryGamePartitionReq
{
	UINT16 from;
	BYTE   num;

	void hton()
		{	from = htons(from);	}
	void ntoh()
		{	from = ntohs(from);	}
};

struct SGamePartitionInfo
{
	UINT16 partId;	//分区编号
	char   name[33];	//分区名称
	UINT32 onlinePlayer;	//分区在线人数
	BYTE   status;	//分区当前状态
	void hton()
	{
		partId = htons(partId);
		onlinePlayer = htonl(onlinePlayer);
	}
	void ntoh()
	{
		partId = ntohs(partId);
		onlinePlayer = ntohl(onlinePlayer);
	}
};

struct SExtManQueryGamePartitionRes
{
	UINT16 total;
	BYTE   retNum;
	SGamePartitionInfo gameParts[1];
	void hton()
	{
		total = htons(total);
		for(BYTE i = 0; i < retNum; ++i)
			gameParts[i].hton();
	}
	void ntoh()
	{
		total = ntohs(total);
		for(BYTE i = 0; i < retNum; ++i)
			gameParts[i].ntoh();
	}
};

//增加分区, LCS_ADD_PARTITION
struct SExtAddPartitionReq
{
	char name[33];	//名字
	char lic[33];	//验证码
	BYTE isRecommend;	//是否推荐分区
};

struct SExtAddPartitionRes
{
	UINT16 newPartId;	//新id
	void hton()
		{	newPartId = htons(newPartId);	}
	void ntoh()
		{	newPartId = ntohs(newPartId);	}
};

//删除分区, LCS_DEL_PARTITION
struct SExtDelPartitionReq
{
	UINT16 partitionId;	//分区id
	void hton()
		{	partitionId = htons(partitionId);	}
	void ntoh()
		{	partitionId = ntohs(partitionId);	}
};

//修改分区名字, LCS_MODIFY_PARTITION
struct SExtModPartitionReq	//请求
{
	UINT16 partitionId;	//分区id
	char newName[33];	//新名称
	void hton()
		{	partitionId = htons(partitionId);	}
	void ntoh()
		{	partitionId = ntohs(partitionId);	}
};

///设置推荐分区, LCS_SET_RECOMMEND_PARTITION
struct SExtSetRecommendPartitionReq //请求
{
	UINT16 partitionId;	//分区id
	void hton()
		{	partitionId = htons(partitionId);	}
	void ntoh()
		{	partitionId = ntohs(partitionId);	}
};

//停止/启动分区服务器, LCS_START_LINKER_SERVE/LCS_STOP_LINKER_SERVE
struct SExtCtrlPartitionReq		//请求
{
	UINT16 partitionId;	//分区id
	void hton()
		{	partitionId = htons(partitionId);	}
	void ntoh()
		{	partitionId = ntohs(partitionId);	}
};

//查询玩家列表, LGS_QUERY_PLAYER_LIST
struct SExtQueryPlayerListReq
{
	UINT32 from;
	UINT16 num;
	void hton()
	{
		from = htonl(from);
		num = htons(num);
	}
	void ntoh()
	{
		from = ntohl(from);
		num = ntohs(num);
	}
};

struct SPlayerListInfo
{
	UINT32 userId;			//所属账号id
	UINT32 roleId;			//角色id
	char   nick[33];		//昵称
	BYTE   level;			//等级
	SBigNumber exp;			//经验
	SBigNumber gold;		//金币
	UINT32 rpcoin;			//代币
	char   createtts[20];	//创建时间
	char   lvtts[20];		//最后访问时间
	UINT32 vistNum;			//访问次数
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		rpcoin = htonl(rpcoin);
		vistNum = htonl(vistNum);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		rpcoin = ntohl(rpcoin);
		vistNum = ntohl(vistNum);
	}
};

struct SExtQueryPlayerListRes
{
	UINT32 total;		//总数
	UINT16 retNum;		//当前返回数量
	SPlayerListInfo data[1];	//数据
	void hton()
	{
		for(UINT16 i = 0; i < retNum; ++i)
			data[i].hton();

		total = htonl(total);
		retNum = htons(retNum);
	}
	void ntoh()
	{
		total = ntohl(total);
		retNum = ntohs(retNum);

		for(UINT16 i = 0; i < retNum; ++i)
			data[i].ntoh();
	}
};

//发送用户邮件
struct SExtSendEmailReq
{
	UINT32 userId;
	UINT32 roleId;
	char szCaption[33];	//标题
	char szBody[128];		//正文
	SBigNumber gold;	//金币
	UINT32 rpcoin;	//代币
	BYTE   attachmentsNum;	//附件个数
	SPropertyAttr data[1];
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		rpcoin = htonl(rpcoin);
		for(BYTE i = 0; i < attachmentsNum; ++i)
			data[i].hton();
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		rpcoin = ntohl(rpcoin);
		for(BYTE i = 0; i < attachmentsNum; ++i)
			data[i].ntoh();
	}
};

#pragma pack(pop)

#endif	//_SGSExtMan_h__