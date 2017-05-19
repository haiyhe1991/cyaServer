#include "RoleInfos.h"
#include "VitExchangeMan.h"
#include "DBSClient.h"
#include "ConfigFileMan.h"

static VitExchangeMan* sg_exchangeMan = NULL;
void InitVitExchangeMan()
{
	ASSERT(sg_exchangeMan == NULL);
	sg_exchangeMan = new VitExchangeMan();
	ASSERT(sg_exchangeMan != NULL);
	sg_exchangeMan->Start();
}

VitExchangeMan* GetVitExchangeMan()
{
	return sg_exchangeMan;
}

void DestroyVitExchangeMan()
{
	VitExchangeMan* pExchangeMan = sg_exchangeMan;
	sg_exchangeMan = NULL;
	if (pExchangeMan != NULL)
	{
		pExchangeMan->Stop();
		delete pExchangeMan;
	}
}

VitExchangeMan::VitExchangeMan()
{

}

VitExchangeMan::~VitExchangeMan()
{

}

void VitExchangeMan::Start()
{
	DWORD n = GetConfigFileMan()->GetRefreshDBInterval();
	m_timer.Start(VitExchangeTimer, this, n * 1000);
}

void VitExchangeMan::Stop()
{
	m_timer.Stop();
}

int VitExchangeMan::QueryRoleVitExchange(UINT32 roleId, UINT16& exchangedTimes)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select exchangetimes from vitexchange where actorid=%u", roleId);
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

void VitExchangeMan::RefeshRoleVitExchange(UINT32 roleId, UINT16 nTimes)
{
	LTMSEL nowMsel = GetMsel();
	CXTAutoLock lock(m_locker);
	std::map<UINT32/*½ÇÉ«id*/, SVitExchangeUpdate>::iterator it = m_vitExchangeMap.find(roleId);
	if (it != m_vitExchangeMap.end())
	{
		it->second.nTimes = nTimes;
		it->second.tts = nowMsel;
	}
	else
	{
		SVitExchangeUpdate exchange;
		exchange.nTimes = nTimes;
		exchange.tts = nowMsel;
		m_vitExchangeMap.insert(std::make_pair(roleId, exchange));
	}
}

void VitExchangeMan::ClearVitExchangeRecord()
{
	GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), "delete from vitexchange");
}

void VitExchangeMan::VitExchangeTimer(void* param, TTimerID /*id*/)
{
	VitExchangeMan* pThis = (VitExchangeMan*)param;
	ASSERT(pThis != NULL);
	if (pThis)
		pThis->OnVitExchange();
}

void VitExchangeMan::OnVitExchange()
{
	if (m_vitExchangeMap.empty())
		return;

	std::map<UINT32/*½ÇÉ«id*/, SVitExchangeUpdate> vitExchangeMap;
	{
		CXTAutoLock lock(m_locker);
		if (m_vitExchangeMap.empty())
			return;
		vitExchangeMap.insert(m_vitExchangeMap.begin(), m_vitExchangeMap.end());
		m_vitExchangeMap.clear();
	}

	std::map<UINT32/*½ÇÉ«id*/, SVitExchangeUpdate>::iterator it = vitExchangeMap.begin();
	for (; it != vitExchangeMap.end(); ++it)
		OnRefreshDBVitExchange(it->first, it->second.nTimes, it->second.tts);
}

void VitExchangeMan::OnRefreshDBVitExchange(UINT32 roleId, UINT16 nTimes, LTMSEL tts)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	char szDate[32] = { 0 };
	MselToLocalStrTime(tts, szDate);
	sprintf(szSQL, "update vitexchange set exchangetimes=%d, tts='%s' where actorid=%u", nTimes, szDate, roleId);

	UINT32 affectRows = 0;
	const char* dbName = GetConfigFileMan()->GetDBName();
	GetDBSClient()->Update(dbName, szSQL, &affectRows);
	if (affectRows > 0)
		return;

	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "insert into vitexchange values(%u, %d, '%s')", roleId, nTimes, szDate);
	GetDBSClient()->Insert(dbName, szSQL, NULL);
}