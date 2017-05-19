#ifndef __RECHARGE_MAN_H__
#define __RECHARGE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��ֵ����
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

	//���ӳ�ֵ���ݷ�����
	BOOL ConnectRechargeDBServer();
	//�Ͽ���ֵ���ݷ�����
	void DisConnectRechargeDBServer();

public:
	//���ɳ�ֵ����
	int GenerateRechargeOrder(UINT32 userId, UINT32 roleId, UINT16 partId, UINT16 rechargeId, UINT32 rmb, char* pszOrder);
	//��ȡ��ֵ��Ϣ
	int GetRechargeInfo(UINT32 userId, UINT32 roleId, const char* pszOrder, UINT16& rechargeId, UINT32& rmb, BYTE& status);

	//��ѯ�״γ�ֵ����
	void QueryRechargeFirstGive(UINT32 userId, UINT32 roleId, SQueryRechargeFirstGiveRes* pRechargeRes);
	//��¼��ֵ��Ϣ
	void InputRechargeRecord(UINT32 userId, UINT32 roleId, const char* nick, UINT16 cashId, UINT32 rmb, UINT32 token, UINT32 give);
	//��id�Ƿ��״γ�ֵ
	BOOL IsFirstRecharge(UINT32 userId, UINT32 roleId, UINT16 cashId);

	//��ѯƽ̨�״γ�ֵ����
	void QueryPlatRechargeFirstGive(UINT32 userId, UINT32 roleId, SQueryRechargeFirstGiveRes* pRechargeRes);
	//��id�Ƿ��״γ�ֵ
	BOOL IsFirstRechargeId(UINT32 userId, UINT32 roleId, UINT16 rechargeId);
	//���±���������Ϣ
	int UpdateLocalPartOrderInfo(const char* pszOrder, BYTE status, UINT32 token, UINT32 give);
	//�Ƿ���ȡ���Ķ���
	int CheckLocalOrderStatus(const char* pszOrder);
	//��ȡδ��ȡ��ֵ���ҳ�ֵid
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
	DBSHandle m_rechargeHandle;		//��ֵDBServer
};

RechargeMan* GetRechargeMan();
BOOL ConnectRechargeDBServer();
void DisConnectRechargeDBServer();

#endif	//_RechargeMan_h__