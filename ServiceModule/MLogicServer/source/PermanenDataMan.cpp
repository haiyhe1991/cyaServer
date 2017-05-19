#include "cyaTime.h"
#include "PermanenDataMan.h"
#include "DBSClient.h"
#include "RolesInfoMan.h"
#include "ConfigFileMan.h"

static PermanenDataMan* sg_permanentDataMan = NULL;
void InitPermanenDataMan()
{
	ASSERT(sg_permanentDataMan == NULL);
	sg_permanentDataMan = new PermanenDataMan();
	ASSERT(sg_permanentDataMan != NULL);
}

PermanenDataMan* GetPermanenDataMan()
{
	return sg_permanentDataMan;
}

void DestroyPermanenDataMan()
{
	PermanenDataMan* pPermanentDataMan = sg_permanentDataMan;
	sg_permanentDataMan = NULL;
	if (pPermanentDataMan)
		delete pPermanentDataMan;
}

PermanenDataMan::PermanenDataMan()
{
	DWORD interval = GetConfigFileMan()->GetRefreshDBInterval();
	m_refreshDBTimer.Start(RefreshPermanenDataTimer, this, interval * 1000);
}

PermanenDataMan::~PermanenDataMan()
{
	if (m_refreshDBTimer.IsStarted())
		m_refreshDBTimer.Stop();

	CXTAutoLock lock(m_locker);
	std::map<std::string, SUpdatePermanenData>::iterator it = m_updateInfoMap.begin();
	for (; it != m_updateInfoMap.end(); ++it)
		OnRefreshPermanenData(it->second);
	m_updateInfoMap.clear();
}

int PermanenDataMan::InitNewRolePermanenData(UINT32 roleId, const std::vector<UINT16>& magicVec)
{
	BYTE num = (BYTE)magicVec.size();
	char pszInitData[GENERIC_SQL_BUF_LEN] = { 0 };

	int n = DBS_EscapeString(pszInitData, (const char*)&num, sizeof(BYTE));
	if (num > 0)
		DBS_EscapeString(pszInitData + n, (const char*)magicVec.data(), num * sizeof(UINT16));

	char pszDate[32] = { 0 };
	GetLocalStrTime(pszDate);

	char szSQL[GENERIC_SQL_BUF_LEN * 8] = { 0 };
	sprintf(szSQL, "insert into permanent(actorid, helps, magics, help_tts, magic_tts) values(%u, '%s', '%s', '%s', '%s')", roleId, pszInitData, pszInitData, pszDate, pszDate);
	return GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
}

int PermanenDataMan::QueryRoleFinishedHelps(UINT32 roleId, SQueryFinishedHelpRes* pHelpRes)
{
	//查数据库
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select helps from permanent where actorid=%u", roleId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	DBS_ROW row = DBS_FetchRow(result);
	if (row == NULL)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	char* p = row[0];
	BYTE nCount = *(BYTE*)p;
	UINT16* pHelp = (UINT16*)(p + sizeof(BYTE));
	for (UINT16 i = 0; i < nCount; ++i)
		pHelpRes->data[i] = pHelp[i];
	pHelpRes->num = nCount;
	DBS_FreeResult(result);
	return retCode;
}

int PermanenDataMan::QueryRoleMagics(UINT32 roleId, SQueryRoleMagicsRes* pMagicRes)
{
	//查数据库
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select magics from permanent where actorid=%u", roleId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	DBS_ROW row = DBS_FetchRow(result);
	if (row == NULL)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	char* p = row[0];
	BYTE nCount = *(BYTE*)p;
	UINT16* pMagics = (UINT16*)(p + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
		pMagicRes->data[i] = pMagics[i];
	pMagicRes->num = nCount;
	DBS_FreeResult(result);
	return retCode;
}

void PermanenDataMan::InputUpdatePermanenDataInfo(UINT32 userId, UINT32 roleId, UINT16 type)
{
	SUpdatePermanenData item(userId, roleId, type);
	char szKey[64] = { 0 };
	sprintf(szKey, "%u-%u-%d", userId, roleId, type);

	BOOL enforceSyncDB = false;
	{
		CXTAutoLock lock(m_locker);
		std::map<std::string, SUpdatePermanenData>::iterator it = m_updateInfoMap.find(szKey);
		if (it == m_updateInfoMap.end())
			m_updateInfoMap.insert(std::make_pair(szKey, item));
		enforceSyncDB = m_updateInfoMap.size() >= GetConfigFileMan()->GetMaxCacheItems() ? true : false;
	}

	if (enforceSyncDB)
		m_refreshDBTimer.Enforce();
}

void PermanenDataMan::ExitSyncUserPermanenData(UINT32 userId, const std::vector<UINT32>& rolesVec)
{
	BYTE si = (BYTE)rolesVec.size();
	if (si <= 0)
		return;

	std::vector<SUpdatePermanenData> upVec;

	{
		CXTAutoLock lock(m_locker);
		for (BYTE i = 0; i < si; ++i)
		{
			for (int j = PERMANENT_DATA_ID_START; j <= PERMANENT_DATA_ID_END; ++j)
			{
				char szKey[64] = { 0 };
				sprintf(szKey, "%u-%u-%d", userId, rolesVec[i], j);
				std::map<std::string, SUpdatePermanenData>::iterator it = m_updateInfoMap.find(szKey);
				if (it == m_updateInfoMap.end())
					continue;

				upVec.push_back(it->second);
				m_updateInfoMap.erase(it);
			}
		}
	}

	si = upVec.size();
	for (size_t i = 0; i < si; ++i)
		OnRefreshPermanenData(upVec[i]);
}

void PermanenDataMan::RefreshPermanenDataTimer(void* param, TTimerID id)
{
	PermanenDataMan* pThis = (PermanenDataMan*)param;
	if (pThis)
		pThis->OnRefreshPermanenData();
}

void PermanenDataMan::OnRefreshPermanenData()
{
	if (m_updateInfoMap.empty())
		return;

	std::vector<SUpdatePermanenData> upInfoVec;
	{
		CXTAutoLock lock(m_locker);
		std::map<std::string, SUpdatePermanenData>::iterator it = m_updateInfoMap.begin();
		for (; it != m_updateInfoMap.end(); ++it)
			upInfoVec.push_back(it->second);
		m_updateInfoMap.clear();
	}

	size_t si = upInfoVec.size();
	for (size_t i = 0; i < si; ++i)
		OnRefreshPermanenData(upInfoVec[i]);
}

void PermanenDataMan::OnRefreshPermanenData(const SUpdatePermanenData& info)
{
	switch (info.type)
	{
	case PERMANENT_HELP:	//刷新战力
		OnUpdateRoleHelps(info.userId, info.roleId);
		break;
	case PERMANENT_MAGIC:	//刷新背包
		OnUpdateRoleMagics(info.userId, info.roleId);
		break;
	default:
		break;
	}
}

void PermanenDataMan::OnUpdateRoleHelps(UINT32 userId, UINT32 roleId)
{
	std::vector<UINT16> helpsVec;
	int retCode = GetRolesInfoMan()->GetRoleHelps(userId, roleId, helpsVec);
	if (retCode != MLS_OK)
		return;

	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	strcpy(szSQL, "update permanent set helps='");

	int n = (int)strlen(szSQL);
	BYTE si = (BYTE)helpsVec.size();
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(BYTE));
	if (si > 0)
		DBS_EscapeString(szSQL + n, (const char*)helpsVec.data(), si * sizeof(UINT16));

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

void PermanenDataMan::OnUpdateRoleMagics(UINT32 userId, UINT32 roleId)
{
	std::vector<UINT16> magicsVec;
	int retCode = GetRolesInfoMan()->GetRoleMagics(userId, roleId, magicsVec);
	if (retCode != MLS_OK)
		return;

	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	strcpy(szSQL, "update permanent set magics='");

	int n = (int)strlen(szSQL);
	BYTE si = (BYTE)magicsVec.size();
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(BYTE));
	if (si > 0)
		DBS_EscapeString(szSQL + n, (const char*)magicsVec.data(), si * sizeof(UINT16));

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}