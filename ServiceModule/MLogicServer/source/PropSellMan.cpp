#include "PropSellMan.h"
#include "cyaTime.h"
#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "GameCfgFileMan.h"
#include "cyaMaxMin.h"

static PropSellMan* sg_propSellMan = NULL;
void InitPropSellMan()
{
	sg_propSellMan = new PropSellMan();
	ASSERT(sg_propSellMan != NULL);
}

PropSellMan* GetPropSellMan()
{
	return sg_propSellMan;
}

void DestroyPropSellMan()
{
	PropSellMan* propSellMan = sg_propSellMan;
	sg_propSellMan = NULL;
	if (propSellMan != NULL)
		delete propSellMan;
}

PropSellMan::PropSellMan()
{

}

PropSellMan::~PropSellMan()
{

}

void PropSellMan::InputPropSellRecord(UINT32 roleId, SPropSellInfo& sellInfo)
{
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select sells from sellhistory where actorid=%u", roleId);
	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 1)
	{
		DBS_FreeResult(result);
		memset(szSQL, 0, sizeof(szSQL));
		sprintf(szSQL, "insert into sellhistory(actorid, sells) values(%u, '", roleId);

		BYTE num = 1;
		sellInfo.prop.id = 1;
		int n = (int)strlen(szSQL);
		n += DBS_EscapeString(szSQL + n, (const char*)&num, sizeof(BYTE));
		DBS_EscapeString(szSQL + n, (const char*)&sellInfo, sizeof(SPropSellInfo));
		strcat(szSQL, "')");
		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}
	else
	{
		DBS_ROWLENGTH pRowLen = NULL;
		DBS_ROW row = DBS_FetchRow(result, &pRowLen);
		ASSERT(row != NULL);

		BYTE* pData = (BYTE*)row[0];
		int nDataLen = pRowLen[0];
		BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SPropSellInfo));
		nCount = min(nCount, *(BYTE*)pData);
		SPropSellInfo* pSellIfo = (SPropSellInfo*)(pData + sizeof(BYTE));
		BYTE maxSells = GetGameCfgFileMan()->GetMaxSaveSellProps();

		memset(szSQL, 0, sizeof(szSQL));
		strcpy(szSQL, "update sellhistory set sells='");
		int n = (int)strlen(szSQL);

		UINT32 sellId = 0;
		for (BYTE i = 0; i < nCount; ++i)
		{
			if (pSellIfo[i].prop.id > sellId)
				sellId = pSellIfo[i].prop.id;
		}
		sellInfo.prop.id = ++sellId;

		if (nCount >= maxSells)
		{
			n += DBS_EscapeString(szSQL + n, (const char*)&nCount, sizeof(BYTE));
			n += DBS_EscapeString(szSQL + n, (const char*)(pSellIfo + 1), sizeof(SPropSellInfo)* (nCount - 1));	//Ä¨µôµÚÒ»¸ö
			n += DBS_EscapeString(szSQL + n, (const char*)&sellInfo, sizeof(SPropSellInfo));
		}
		else
		{
			BYTE newCount = SGSU8Add(nCount, 1);
			n += DBS_EscapeString(szSQL + n, (const char*)&newCount, sizeof(BYTE));
			n += DBS_EscapeString(szSQL + n, (const char*)pSellIfo, sizeof(SPropSellInfo)* nCount);
			n += DBS_EscapeString(szSQL + n, (const char*)&sellInfo, sizeof(SPropSellInfo));
		}

		DBS_FreeResult(result);

		char szId[64] = { 0 };
		sprintf(szId, "' where actorid=%u", roleId);
		strcat(szSQL, szId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}
}

int PropSellMan::QueryPropSellRecord(UINT32 roleId, UINT32 from, BYTE num, SQueryRoleSellPropRcdRes* pSellRcdRes, BYTE maxNum)
{
	pSellRcdRes->num = 0;
	pSellRcdRes->total = 0;

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select sells from sellhistory where actorid=%u", roleId);
	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 1)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	DBS_ROWLENGTH pRowLen = NULL;
	DBS_ROW row = DBS_FetchRow(result, &pRowLen);
	ASSERT(row != NULL);

	BYTE* pData = (BYTE*)row[0];
	int nDataLen = pRowLen[0];
	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SPropSellInfo));
	nCount = min(nCount, *(BYTE*)pData);
	SPropSellInfo* pSellIfo = (SPropSellInfo*)(pData + sizeof(BYTE));

	pSellRcdRes->total = nCount;
	for (BYTE i = 0; i < nCount; ++i)
	{
		memcpy(&pSellRcdRes->sells[pSellRcdRes->num], &pSellIfo[i], sizeof(SPropSellInfo));
		pSellRcdRes->sells[pSellRcdRes->num].hton();
		if (++pSellRcdRes->num >= maxNum)
			break;
	}
	DBS_FreeResult(result);
	return retCode;
}

int PropSellMan::GetPropSellInfo(UINT32 roleId, UINT32 sellId, SPropertyAttr& propAttr)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select sells from sellhistory where actorid=%u", roleId);
	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 1)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	DBS_ROWLENGTH pRowLen = NULL;
	DBS_ROW row = DBS_FetchRow(result, &pRowLen);
	ASSERT(row != NULL);

	retCode = MLS_NO_PROP_SELL_RCD;
	BYTE* pData = (BYTE*)row[0];
	int nDataLen = pRowLen[0];
	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SPropSellInfo));
	nCount = min(nCount, *(BYTE*)pData);
	SPropSellInfo* pSellIfo = (SPropSellInfo*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
	{
		if (pSellIfo[i].prop.id == sellId)
		{
			memcpy(&propAttr, &pSellIfo[i].prop, sizeof(SPropertyAttr));
			retCode = MLS_OK;
			break;
		}
	}
	DBS_FreeResult(result);
	return retCode;
}

int PropSellMan::DelPropSellInfo(UINT32 roleId, UINT32 sellId)
{
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select sells from sellhistory where actorid=%u", roleId);
	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 1)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	DBS_ROWLENGTH pRowLen = NULL;
	DBS_ROW row = DBS_FetchRow(result, &pRowLen);
	ASSERT(row != NULL);

	retCode = MLS_NO_PROP_SELL_RCD;
	BYTE* pData = (BYTE*)row[0];
	int nDataLen = pRowLen[0];
	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SPropSellInfo));
	nCount = min(nCount, *(BYTE*)pData);
	if (nCount <= 0)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	std::vector<SPropSellInfo> vec;
	SPropSellInfo* pSellIfo = (SPropSellInfo*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
	{
		if (vec[i].prop.id == sellId)
			continue;
		vec.push_back(pSellIfo[i]);
	}
	DBS_FreeResult(result);

	memset(szSQL, 0, sizeof(szSQL));
	strcpy(szSQL, "update sellhistory set sells='");
	int n = (int)strlen(szSQL);

	BYTE si = (BYTE)vec.size();
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(BYTE));
	if (si > 0)
		n += DBS_EscapeString(szSQL + n, (const char*)vec.data(), sizeof(SPropSellInfo)* si);

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	return GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

int PropSellMan::GetPropSellInfo(UINT32 roleId, UINT32 sellId, SPropertyAttr& propAttr, std::vector<SPropSellInfo>& sellVec)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select sells from sellhistory where actorid=%u", roleId);
	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 1)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	DBS_ROWLENGTH pRowLen = NULL;
	DBS_ROW row = DBS_FetchRow(result, &pRowLen);
	ASSERT(row != NULL);

	retCode = MLS_NO_PROP_SELL_RCD;
	BYTE* pData = (BYTE*)row[0];
	int nDataLen = pRowLen[0];
	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SPropSellInfo));
	nCount = min(nCount, *(BYTE*)pData);
	SPropSellInfo* pSellIfo = (SPropSellInfo*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
	{
		sellVec.push_back(pSellIfo[i]);
		if (pSellIfo[i].prop.id == sellId)
		{
			memcpy(&propAttr, &pSellIfo[i].prop, sizeof(SPropertyAttr));
			retCode = MLS_OK;
		}
	}
	DBS_FreeResult(result);
	return retCode;
}

int PropSellMan::DelPropSellInfo(UINT32 roleId, UINT32 sellId, BYTE num, std::vector<SPropSellInfo>& sellVec)
{
	std::vector<SPropSellInfo>::iterator it = sellVec.begin();
	for (; it != sellVec.end(); ++it)
	{
		if (it->prop.id == sellId)
		{
			if (it->prop.num <= num)
				sellVec.erase(it);
			else
				it->prop.num -= num;
			break;
		}
	}

	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	strcpy(szSQL, "update sellhistory set sells='");
	int n = (int)strlen(szSQL);

	BYTE si = (BYTE)sellVec.size();
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(BYTE));
	if (si > 0)
		n += DBS_EscapeString(szSQL + n, (const char*)sellVec.data(), sizeof(SPropSellInfo)* si);

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	return GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}