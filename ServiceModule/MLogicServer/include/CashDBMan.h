#ifndef __CASH_DB_MAN_H__
#define __CASH_DB_MAN_H__

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

class CashDBMan
{

public:
	CashDBMan();
	~CashDBMan();


	//���ӳ�ֵ���ݷ�����
	BOOL ConnectCashDBServer();
	//�Ͽ���ֵ���ݷ�����
	void DisConnectCashDBServer();

	//���ô���
	INT SetCash(const UINT32 userID, const UINT32 cash, const UINT32 cashcount) const;

	//���ٴ���
	INT SubCash(const UINT32 userID, const UINT32 addCash) const;

	//���Ӵ���
	INT AddCash(const UINT32 userID, const UINT32 addCash) const;

	//��ȡ����
	INT GetCash(const UINT32 userID, UINT32& cash, UINT32& cashcount);

	//��ֵ��¼
	INT SaveCashInfo(const UINT32 userID, UINT8 cashID, UINT32 rmb, UINT32 token, const UINT32 roleID, const char* nick = NULL);

private:
	DBSHandle m_CashDBHandle;		//��ֵDBServer
	UINT16 m_partID;
};

CashDBMan* GetCashDBMan();
BOOL ConnectCashDBServer();
void DisConnectCashDBServer();

#endif