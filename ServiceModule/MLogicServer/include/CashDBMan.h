#ifndef __CASH_DB_MAN_H__
#define __CASH_DB_MAN_H__

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

class CashDBMan
{

public:
	CashDBMan();
	~CashDBMan();


	//连接充值数据服务器
	BOOL ConnectCashDBServer();
	//断开充值数据服务器
	void DisConnectCashDBServer();

	//设置代币
	INT SetCash(const UINT32 userID, const UINT32 cash, const UINT32 cashcount) const;

	//减少代币
	INT SubCash(const UINT32 userID, const UINT32 addCash) const;

	//增加代币
	INT AddCash(const UINT32 userID, const UINT32 addCash) const;

	//获取代币
	INT GetCash(const UINT32 userID, UINT32& cash, UINT32& cashcount);

	//充值记录
	INT SaveCashInfo(const UINT32 userID, UINT8 cashID, UINT32 rmb, UINT32 token, const UINT32 roleID, const char* nick = NULL);

private:
	DBSHandle m_CashDBHandle;		//充值DBServer
	UINT16 m_partID;
};

CashDBMan* GetCashDBMan();
BOOL ConnectCashDBServer();
void DisConnectCashDBServer();

#endif