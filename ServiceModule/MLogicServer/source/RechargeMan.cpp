#include "RechargeMan.h"
#include "DBSClient.h"
#include "RoleInfos.h"
#include "ConfigFileMan.h"
#include "ServiceProtocol.h"
#include "stringHash.h"
#include "cyaLog.h"

#define SGS_RECHARGE_TABLE_NAME_PREFIX "player_cash_"
#define SGS_RECHARE_ORDER_TABLE_NUM 5

static RechargeMan* sg_pRechargeMan = NULL;
RechargeMan* GetRechargeMan()
{
	return sg_pRechargeMan;
}

BOOL ConnectRechargeDBServer()
{
	ASSERT(sg_pRechargeMan == NULL);
	sg_pRechargeMan = new RechargeMan();
	ASSERT(sg_pRechargeMan != NULL);
	return sg_pRechargeMan->ConnectRechargeDBServer();
}

void DisConnectRechargeDBServer()
{
	RechargeMan* pRechargeMan = sg_pRechargeMan;
	sg_pRechargeMan = NULL;
	if (pRechargeMan)
		pRechargeMan->DisConnectRechargeDBServer();
}

RechargeMan::RechargeMan()
: m_rechargeHandle(NULL)
{

}

RechargeMan::~RechargeMan()
{

}

BOOL RechargeMan::ConnectRechargeDBServer()
{
	ASSERT(m_rechargeHandle == NULL);
	const char* pszIp = GetConfigFileMan()->GetRechargeDBSIp();
	int nPort = GetConfigFileMan()->GetRechargeDBSPort();
	m_rechargeHandle = DBS_Connect(pszIp, nPort);
	LogInfo(("连接充值数据服务器[%s:%d]%s!", pszIp, nPort, m_rechargeHandle == NULL ? "失败" : "成功"));
	return m_rechargeHandle == NULL ? false : true;
}

void RechargeMan::DisConnectRechargeDBServer()
{
	if (m_rechargeHandle != NULL)
	{
		DBS_DisConnect(m_rechargeHandle);
		m_rechargeHandle = NULL;
	}
}

int RechargeMan::GenerateRechargeOrder(UINT32 userId, UINT32 roleId, UINT16 partId, UINT16 rechargeId, UINT32 rmb, char* pszOrder)
{
	SYSTEMTIME nowTM;
	GetLocalTime(&nowTM);
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(pszOrder, "%04d%02d%02d%02d%02d%02d%d%u%d",
		nowTM.wYear, nowTM.wMonth, nowTM.wDay,
		nowTM.wHour, nowTM.wMinute, nowTM.wSecond, nowTM.wMilliseconds, roleId, rechargeId);
	char szDate[32] = { 0 };
	sprintf(szDate, "%04d-%02d-%02d %02d:%02d:%02d", nowTM.wYear, nowTM.wMonth, nowTM.wDay, nowTM.wHour, nowTM.wMinute, nowTM.wSecond);

	sprintf(szSQL, "insert into recharge(orderid, partid, accountid, actorid, rechargeid, rmb, token, give, status, ordertts, isgettoken) values"
		"('%s', %d, %u, %u, %d, %u, %u, %u, %d, '%s', %d)", pszOrder, partId, userId, roleId,
		rechargeId, rmb, 0, 0, ESGS_WAIT_CONFIRM, szDate, 0);
	int retCode = GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	if (retCode != DBS_OK)
		return MLS_GENERATE_ORDER_FAILED;

	std::string strTabName;
	GetRechargeOrderTableName(pszOrder, strTabName);
	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "insert into %s(orderid, accountid, actorid, partid, createtts, status, rechargeid, rmb) values"
		"('%s', %u, %u, %d, '%s', %d, %d, %u)", strTabName.c_str(), pszOrder, userId, roleId, partId,
		szDate, ESGS_WAIT_CONFIRM, rechargeId, rmb);

	DBS_RESULT result = NULL;

	retCode = DBS_ExcuteSQL(m_rechargeHandle, DBS_MSG_INSERT, GetConfigFileMan()->GetRechargeDBName(), szSQL, result);
	if (retCode != DBS_OK)
	{
		memset(szSQL, 0, sizeof(szSQL));
		sprintf(szSQL, "delete from recharge where orderid='%s'", pszOrder);
		GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), szSQL, NULL);
		return MLS_GENERATE_ORDER_FAILED;
	}
	DBS_FreeResult(result);

	return retCode;
}

int RechargeMan::GetRechargeInfo(UINT32 /*userId*/, UINT32 roleId, const char* pszOrder, UINT16& rechargeId, UINT32& rmb, BYTE& status)
{
	std::string strTabName;
	GetRechargeOrderTableName(pszOrder, strTabName);
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select status, rechargeid, rmb from %s where orderid='%s'", strTabName.c_str(), pszOrder);

	DBS_RESULT result = NULL;
	int retCode = DBS_ExcuteSQL(m_rechargeHandle, DBS_MSG_QUERY, GetConfigFileMan()->GetRechargeDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return MLS_GET_ORDER_RESULT_FAILED;

	DBS_ROW row = DBS_FetchRow(result);
	if (row == NULL)
	{
		DBS_FreeResult(result);
		return MLS_INVALID_ORDER;
	}

	status = atoi(row[0]);
	rechargeId = atoi(row[1]);
	rmb = atoi(row[2]);
	DBS_FreeResult(result);
	if (status == ESGS_WAIT_CONFIRM)
		return MLS_ORDER_NOT_CONFIRMED;

	return CheckLocalOrderStatus(pszOrder);
}

int RechargeMan::UpdateLocalPartOrderInfo(const char* pszOrder, BYTE status, UINT32 token, UINT32 give)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	char szDate[32] = { 0 };
	GetLocalStrTime(szDate);
	sprintf(szSQL, "update recharge set status=%d, isgettoken=%d, token=%u, give=%u, oktts='%s' where orderid='%s'", status, 1, token, give, szDate, pszOrder);
	return GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, NULL);
}

/* zpy 2015.12.17 与平台充值合并 */
void RechargeMan::QueryRechargeFirstGive(UINT32 userId, UINT32 roleId, SQueryRechargeFirstGiveRes* pRechargeRes)
{
	QueryPlatRechargeFirstGive(userId, roleId, pRechargeRes);
}

/* zpy 2015.12.17 与平台充值合并 */
void RechargeMan::InputRechargeRecord(UINT32 userId, UINT32 roleId, const char* nick, UINT16 cashId, UINT32 rmb, UINT32 token, UINT32 give)
{
	SYSTEMTIME nowTM;
	GetLocalTime(&nowTM);
	char szDate[32] = { 0 };
	char pszOrder[64] = { 0 };
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(pszOrder, "test%lld", GetMsel());
	sprintf(szDate, "%04d-%02d-%02d %02d:%02d:%02d", nowTM.wYear, nowTM.wMonth, nowTM.wDay, nowTM.wHour, nowTM.wMinute, nowTM.wSecond);
	sprintf(szSQL, "insert into recharge(orderid, partid, accountid, actorid, rechargeid, rmb, token, give, status, ordertts, isgettoken) values"
		"('%s', %d, %u, %u, %d, %u, %u, %u, %d, '%s', %d)", pszOrder, 0, userId, roleId,
		cashId, rmb, token, give, ESGS_SYS_CONFIRMED, szDate, 1);
	GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
}

/* zpy 2015.12.17 与平台充值合并 */
BOOL RechargeMan::IsFirstRecharge(UINT32 userId, UINT32 roleId, UINT16 cashId)
{
	return IsFirstRechargeId(userId, roleId, cashId);
}

BOOL RechargeMan::IsFirstRechargeId(UINT32 userId, UINT32 roleId, UINT16 rechargeId)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };


	//hxw 20151125	
	if (GetConfigFileMan()->GetCashSaveType() == 1)
	{
		sprintf(szSQL, "select count(*) from recharge where actorid=%u and rechargeid=%d and status!=0", roleId, rechargeId);
	}
	else
	{
		sprintf(szSQL, "select count(*) from recharge where accountid=%u and rechargeid=%d and status!=0", userId, rechargeId);
	}
	//end



	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return true;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols <= 0)
	{
		DBS_FreeResult(result);
		return true;
	}

	DBS_ROW row = DBS_FetchRow(result);
	ASSERT(row != NULL);
	UINT32 nCount = (UINT32)_atoi64(row[0]);
	DBS_FreeResult(result);
	return nCount > 0 ? false : true;
}

void RechargeMan::QueryPlatRechargeFirstGive(UINT32 userId, UINT32 roleId, SQueryRechargeFirstGiveRes* pRechargeRes)
{
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SQueryRechargeFirstGiveRes, num, data)) / sizeof(UINT16));
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };

	//hxw 20151125	
	if (GetConfigFileMan()->GetCashSaveType() == 1)
	{
		sprintf(szSQL, "select rechargeid from recharge where actorid=%u and give>0", roleId);
	}
	else
	{
		sprintf(szSQL, "select rechargeid from recharge where accountid=%u and give>0", userId);
	}
	//end


	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 1)
	{
		DBS_FreeResult(result);
		return;
	}

	BYTE idx = 0;
	DBS_ROW row = NULL;
	while ((row = DBS_FetchRow(result)) != NULL)
	{
		pRechargeRes->data[idx] = atoi(row[0]);
		if (++idx >= maxNum)
			break;
	}
	pRechargeRes->num = idx;
	DBS_FreeResult(result);
}

int RechargeMan::CheckLocalOrderStatus(const char* pszOrder)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select isgettoken from recharge where orderid='%s'", pszOrder);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return MLS_INVALID_ORDER;

	DBS_ROW row = DBS_FetchRow(result);
	if (row == NULL)
	{
		DBS_FreeResult(result);
		return MLS_INVALID_ORDER;
	}

	BYTE isGetToken = atoi(row[0]);
	DBS_FreeResult(result);
	return isGetToken ? MLS_ALREADY_GET_ORDER : MLS_OK;
}

const char* RechargeMan::GetRechargeOrderTableName(const char* pszOrder, std::string& strTabName)
{
#if 1
	UINT32 hashNum = BKDRHash2(pszOrder);
	strTabName = SGS_RECHARGE_TABLE_NAME_PREFIX;

	char szBuf[16] = { 0 };
	sprintf(szBuf, "%d", (hashNum % SGS_RECHARE_ORDER_TABLE_NUM) + 1);
	strTabName += szBuf;
	return strTabName.c_str();
#else
	pszOrder;
	strTabName = "player_cash_1";
	return strTabName.c_str();
#endif
}

int RechargeMan::GetNotReceiveTokenRechargeId(UINT32 roleId, std::vector<SOrderItem>& orderItemVec)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select orderid, rechargeid from recharge where actorid=%u and status!=%d and isgettoken=0", roleId, ESGS_WAIT_CONFIRM);
	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows == 0 || nCols < 2)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	DBS_ROW row = NULL;
	while ((row = DBS_FetchRow(result)) != NULL)
	{
		SOrderItem item;
		item.orderid = row[0];
		item.rechargeId = atoi(row[1]);
		orderItemVec.push_back(item);
	}

	DBS_FreeResult(result);
	return retCode;
}