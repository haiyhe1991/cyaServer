#include "DBSClient.h"
#include "RoleInfos.h"
#include "ConfigFileMan.h"
#include "ServiceProtocol.h"
#include "stringHash.h"
#include "cyaLog.h"
#include "CashDBMan.h"

static CashDBMan* sg_pCashDBMan = NULL;

CashDBMan* GetCashDBMan()
{
	return sg_pCashDBMan;
}

BOOL ConnectCashDBServer()
{
	ASSERT(sg_pCashDBMan == NULL);
	sg_pCashDBMan = new CashDBMan();
	ASSERT(sg_pCashDBMan != NULL);
	return sg_pCashDBMan->ConnectCashDBServer();
}

void DisConnectCashDBServer()
{
	CashDBMan* pCashDBMan = sg_pCashDBMan;
	sg_pCashDBMan = NULL;
	if (pCashDBMan)
		pCashDBMan->DisConnectCashDBServer();
}

CashDBMan::CashDBMan()
: m_CashDBHandle(NULL)
{
	m_partID = GetConfigFileMan()->GetPartID();
}

CashDBMan::~CashDBMan()
{

}

BOOL CashDBMan::ConnectCashDBServer()
{
	ASSERT(m_CashDBHandle == NULL);
	const char* pszIp = GetConfigFileMan()->GetCashDBSIp();
	int nPort = GetConfigFileMan()->GetCashDBSPort();
	m_CashDBHandle = DBS_Connect(pszIp, nPort);
	LogInfo(("连接代币数据服务器[%s:%d]%s!", pszIp, nPort, m_CashDBHandle == NULL ? "失败" : "成功"));
	return m_CashDBHandle == NULL ? false : true;
}

void CashDBMan::DisConnectCashDBServer()
{
	if (m_CashDBHandle != NULL)
	{
		DBS_DisConnect(m_CashDBHandle);
		m_CashDBHandle = NULL;
	}
}

//设置代币
INT CashDBMan::SetCash(const UINT32 userID, const UINT32 cash, const UINT32 cashcount) const
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	if (cashcount == -1)
		sprintf(szSQL, "update %s%d set cash=%u where accountid=%u", SGS_CASH_TABLE_NAME_PREFIX, userID%SGS_CASH_TABLE_NUM + 1, cash, userID);
	else
		sprintf(szSQL, "update %s%d set cash=%u, cashcount=%u where accountid=%u", SGS_CASH_TABLE_NAME_PREFIX, userID%SGS_CASH_TABLE_NUM + 1, cash, cashcount, userID);
	DBS_RESULT result = NULL;
	int ret = 0;
	if ((ret = DBS_ExcuteSQL(m_CashDBHandle, DBS_MSG_UPDATE, GetConfigFileMan()->GetCashDBName(), szSQL, result)) != DBS_OK)
	{
		ASSERT(0);
		DBS_FreeResult(result);
		return ret;
	}
	DBS_FreeResult(result);
	return DBS_OK;
}

//减少代币
INT CashDBMan::SubCash(const UINT32 userID, const UINT32 addCash) const
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	DBS_RESULT result = NULL;
	sprintf(szSQL, "update %s%d set cash=-%u where accountid=%u",
		SGS_CASH_TABLE_NAME_PREFIX, userID%SGS_CASH_TABLE_NUM + 1, addCash, userID);
	int ret = 0;
	if ((ret = DBS_ExcuteSQL(m_CashDBHandle, DBS_MSG_UPDATE, GetConfigFileMan()->GetCashDBName(), szSQL, result)) != DBS_OK)
	{
		ASSERT(0);
		DBS_FreeResult(result);
		return ret;
	}
	DBS_FreeResult(result);
	return DBS_OK;
}

//end

//增加代币
INT CashDBMan::AddCash(const UINT32 userID, const UINT32 addCash) const
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	DBS_RESULT result = NULL;
	sprintf(szSQL, "update %s%d set cash=cash+%u where accountid=%u",
		SGS_CASH_TABLE_NAME_PREFIX, userID%SGS_CASH_TABLE_NUM + 1, addCash, userID);
	int ret = 0;
	if ((ret = DBS_ExcuteSQL(m_CashDBHandle, DBS_MSG_UPDATE, GetConfigFileMan()->GetCashDBName(), szSQL, result)) != DBS_OK)
	{
		ASSERT(0);
		DBS_FreeResult(result);
		return ret;
	}
	DBS_FreeResult(result);
	return DBS_OK;
}



//获取代币
INT CashDBMan::GetCash(const UINT32 userID, UINT32& cash, UINT32& cashcount)
{
	if (GetConfigFileMan()->GetCashSaveType() != CASH_SAVE_TYPE_TO_CASHDB)
		return MLS_CASH_SAVE_TYPE_ERROR;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	int num = userID%SGS_CASH_TABLE_NUM + 1;
	sprintf(szSQL, "select cash,cashcount from %s%d where accountid=%u ", SGS_CASH_TABLE_NAME_PREFIX, num, userID);
	DBS_RESULT result = NULL;
	int ret = DBS_ExcuteSQL(m_CashDBHandle, DBS_MSG_QUERY, GetConfigFileMan()->GetCashDBName(), szSQL, result);
	if (ret != DBS_OK)
		return ret;
	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 2)
	{
		DBS_FreeResult(result);

		sprintf(szSQL, "insert into %s%d(accountid)  values(%u)", SGS_CASH_TABLE_NAME_PREFIX, num, userID);
		ret = DBS_ExcuteSQL(m_CashDBHandle, DBS_MSG_INSERT, GetConfigFileMan()->GetCashDBName(), szSQL, result);
		cash = 0;
		cashcount = 0;
		return ret/*MLS_ROLE_NOT_EXIST*/;
	}
	DBS_ROW row = NULL;
	DBS_ROWLENGTH rowValLen = NULL;
	while ((row = DBS_FetchRow(result, &rowValLen)) != NULL)
	{
		cash = _atoi64(row[0]);
		cashcount = _atoi64(row[1]);
	}
	DBS_FreeResult(result);
	return MLS_OK;
}

INT CashDBMan::SaveCashInfo(const UINT32 userID, UINT8 cashID, UINT32 rmb, UINT32 token, const UINT32 roleID, const char* nick)
{
	int num = userID%SGS_CASHLIST_TABLE_NUM + 1;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	char szDate[32];
	GetLocalStrTime(szDate);
	sprintf(szSQL, "insert into %s%d(accountid,cashid,rmb,token,actorid,actornick,tts,areaid)"
		"values(%u,%d,%u,%u,%u,%s,%s,%d", SGS_CASHLIST_TABLE_NAME_PREFIX, num, userID, cashID, rmb, token, roleID, nick, szDate, m_partID);
	GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);

	return 0;
}
