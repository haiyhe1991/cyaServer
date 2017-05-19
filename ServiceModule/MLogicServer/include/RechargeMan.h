#ifndef __RECHARGE_MAN_H__
#define __RECHARGE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
充值管理
*/

#include <vector>
#include "cyaTypes.h"
#include "ServiceMLogic.h"
#include "DBSSDK.h"

class RechargeMan
{
	RechargeMan(const RechargeMan&);
	RechargeMan& operator = (const RechargeMan&);

public:
	RechargeMan();
	~RechargeMan();

	struct SOrderItem
	{
		std::string orderid;
		UINT16 rechargeId;
	};

	//连接充值数据服务器
	BOOL ConnectRechargeDBServer();
	//断开充值数据服务器
	void DisConnectRechargeDBServer();

public:
	//生成充值订单
	int GenerateRechargeOrder(UINT32 userId, UINT32 roleId, UINT16 partId, UINT16 rechargeId, UINT32 rmb, char* pszOrder);
	//获取充值信息
	int GetRechargeInfo(UINT32 userId, UINT32 roleId, const char* pszOrder, UINT16& rechargeId, UINT32& rmb, BYTE& status);

	//查询首次充值赠送
	void QueryRechargeFirstGive(UINT32 userId, UINT32 roleId, SQueryRechargeFirstGiveRes* pRechargeRes);
	//记录充值信息
	void InputRechargeRecord(UINT32 userId, UINT32 roleId, const char* nick, UINT16 cashId, UINT32 rmb, UINT32 token, UINT32 give);
	//该id是否首次充值
	BOOL IsFirstRecharge(UINT32 userId, UINT32 roleId, UINT16 cashId);

	//查询平台首次充值赠送
	void QueryPlatRechargeFirstGive(UINT32 userId, UINT32 roleId, SQueryRechargeFirstGiveRes* pRechargeRes);
	//该id是否首次充值
	BOOL IsFirstRechargeId(UINT32 userId, UINT32 roleId, UINT16 rechargeId);
	//更新本区订单信息
	int UpdateLocalPartOrderInfo(const char* pszOrder, BYTE status, UINT32 token, UINT32 give);
	//是否领取过的订单
	int CheckLocalOrderStatus(const char* pszOrder);
	//获取未领取充值代币充值id
	int GetNotReceiveTokenRechargeId(UINT32 roleId, std::vector<SOrderItem>& orderItemVec);

private:
	const char* GetRechargeOrderTableName(const char* pszOrder, std::string& strTabName);
	unsigned int BKDRHash2(const char* str)
	{
		unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
		unsigned int hash = 0;

		while (*str)
		{
			hash = hash * seed + (*str++);
			hash &= 0xFFFFFF;
		}

		return hash;//(hash & 0x7FFFFFFF);
	}


private:
	DBSHandle m_rechargeHandle;		//充值DBServer
};

RechargeMan* GetRechargeMan();
BOOL ConnectRechargeDBServer();
void DisConnectRechargeDBServer();

#endif	//_RechargeMan_h__