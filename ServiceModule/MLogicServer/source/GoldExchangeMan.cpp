#include "RoleInfos.h"
#include "GoldExchangeMan.h"
#include "DBSClient.h"
#include "ConfigFileMan.h"

static GoldExchangeMan* sg_exchangeMan = NULL;
void InitGoldExchangeMan()
{
	ASSERT(sg_exchangeMan == NULL);
	sg_exchangeMan = new GoldExchangeMan();
	ASSERT(sg_exchangeMan != NULL);
	sg_exchangeMan->Start();
}

GoldExchangeMan* GetGoldExchangeMan()
{
	return sg_exchangeMan;
}

void DestroyGoldExchangeMan()
{
	GoldExchangeMan* pExchangeMan = sg_exchangeMan;
	sg_exchangeMan = NULL;
	if (pExchangeMan != NULL)
	{
		pExchangeMan->Stop();
		delete pExchangeMan;
	}
}

GoldExchangeMan::GoldExchangeMan()
{

}

GoldExchangeMan::~GoldExchangeMan()
{

}

void GoldExchangeMan::Start()
{
	DWORD n = GetConfigFileMan()->GetRefreshDBInterval();
	m_timer.Start(GoldExchangeTimer, this, n * 1000);
}

void GoldExchangeMan::Stop()
{
	m_timer.Stop();
}

int GoldExchangeMan::QueryRoleGoldExchange(UINT32 roleId, UINT16& exchangedTimes)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select exchangetimes from goldexchange where actorid=%u", roleId);
	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 1)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	DBS_ROW row = DBS_FetchRow(result);
	exchangedTimes = (UINT16)atoi(row[0]);
	DBS_FreeResult(result);

	return retCode;
}

void GoldExchangeMan::RefeshRoleGoldExchange(UINT32 roleId, UINT16 nTimes)
{
	LTMSEL nowMsel = GetMsel();
	CXTAutoLock lock(m_locker);
	std::map<UINT32/*½ÇÉ«id*/, SGoldExchangeUpdate>::iterator it = m_goldExchangeMap.find(roleId);
	if (it != m_goldExchangeMap.end())
	{
		it->second.nTimes = nTimes;
		it->second.tts = nowMsel;
	}
	else
	{
		SGoldExchangeUpdate exchange;
		exchange.nTimes = nTimes;
		exchange.tts = nowMsel;
		m_goldExchangeMap.insert(std::make_pair(roleId, exchange));
	}
}

void GoldExchangeMan::ClearGoldExchangeRecord()
{
	GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), "delete from goldexchange");
}

void GoldExchangeMan::GoldExchangeTimer(void* param, TTimerID /*id*/)
{
	GoldExchangeMan* pThis = (GoldExchangeMan*)param;
	ASSERT(pThis != NULL);
	if (pThis)
		pThis->OnGoldExchange();
}

void GoldExchangeMan::OnGoldExchange()
{
	if (m_goldExchangeMap.empty())
		return;

	std::map<UINT32/*½ÇÉ«id*/, SGoldExchangeUpdate> goldExchangeMap;
	{
		CXTAutoLock lock(m_locker);
		if (m_goldExchangeMap.empty())
			return;
		goldExchangeMap.insert(m_goldExchangeMap.begin(), m_goldExchangeMap.end());
		m_goldExchangeMap.clear();
	}

	std::map<UINT32/*½ÇÉ«id*/, SGoldExchangeUpdate>::iterator it = goldExchangeMap.begin();
	for (; it != goldExchangeMap.end(); ++it)
		OnRefreshDBGoldExchange(it->first, it->second.nTimes, it->second.tts);
}

void GoldExchangeMan::OnRefreshDBGoldExchange(UINT32 roleId, UINT16 nTimes, LTMSEL tts)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	char szDate[32] = { 0 };
	MselToLocalStrTime(tts, szDate);
	sprintf(szSQL, "update goldexchange set exchangetimes=%d, tts='%s' where actorid=%u", nTimes, szDate, roleId);

	UINT32 affectRows = 0;
	const char* dbName = GetConfigFileMan()->GetDBName();
	GetDBSClient()->Update(dbName, szSQL, &affectRows);
	if (affectRows > 0)
		return;

	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "insert into goldexchange values(%u, %d, '%s')", roleId, nTimes, szDate);
	GetDBSClient()->Insert(dbName, szSQL, NULL);
}