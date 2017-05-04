#include "AES.h"
#include "PartitionInfoMan.h"
#include "ServiceErrorCode.h"
#include "LicenceSession.h"
#include "DBSClient.h"
#include "cyaLog.h"
#include "ConfigFileMan.h"
#include "cyaIpCvt.h"
#include "UserInfoMan.h"

#define MAX_NO_LINKER_HEARTBEAT_TIMES  3
#define MAX_ONCE_RET_PARTITION_NUM     15

static PartitionInfoMan* sg_partitionMan = NULL;
BOOL InitPartitionInfoMan()
{
	ASSERT(sg_partitionMan == NULL);
	sg_partitionMan = new PartitionInfoMan();
	ASSERT(sg_partitionMan != NULL);
	return sg_partitionMan->LoadPartitionInfo();
}

PartitionInfoMan* GetPartitionInfoMan()
{
	return sg_partitionMan;
}

void DestroyPartitionInfoMan()
{
	PartitionInfoMan* partitionMan = sg_partitionMan;
	sg_partitionMan = NULL;
	if (partitionMan != NULL)
	{
		partitionMan->UnLoadPartitionInfo();
		delete partitionMan;
	}
}

PartitionInfoMan::PartitionInfoMan()
{

}

PartitionInfoMan::~PartitionInfoMan()
{

}

BOOL PartitionInfoMan::LoadPartitionInfo()
{
	/*
	从数据服务器中取出分区列表
	*/
	UINT32 from = 0;
	UINT32 totals = 0;
	BOOL isSuccess = true;

#define MAX_PARTITIONS_PER_PAGE 1024

	while (true)
	{
		DBS_RESULT result = NULL;
		char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
		sprintf(szSQL, "select id, name, status, isrecomment, accounts from game_partition limit %u, %u", from, MAX_PARTITIONS_PER_PAGE);
		int retCode = GetDBSClient(DBCLI_VERIFY)->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
		if (retCode != DBS_OK)
		{
			isSuccess = false;
			break;
		}

		UINT32 nRows = DBS_NumRows(result);
		UINT16 nCols = DBS_NumFields(result);
		if (nRows <= 0 || nCols < 5)
		{
			DBS_FreeResult(result);
			isSuccess = false;
			break;
		}

		totals += nRows;

		DBS_ROW row = NULL;
		while ((row = DBS_FetchRow(result)) != NULL)
		{
			PartitionSite* site = new PartitionSite();
			ASSERT(site != NULL);

			site->SetName(row[1]);
			site->SetStatus((BYTE)atoi(row[2]));
			site->SetRecommendFlag(!!atoi(row[3]));
			site->SetRegistAccountNum(_atoi64(row[4]));
			m_partitionInfoMap.insert(std::make_pair((UINT16)atoi(row[0]), site));
		}
		DBS_FreeResult(result);

		if (nRows < MAX_PARTITIONS_PER_PAGE)
			break;
		from += nRows;
	}

#undef MAX_PARTITIONS_PER_PAGE

	LogInfo(("共载入%u条分区信息!", totals));

	DWORD timerInterval = GetConfigFileMan()->GetLinkerHeartbeatInterval();
	if (timerInterval > 0)
	{
		m_linkerHeartTimer.Start(LinkerHeartTimer, this, timerInterval);
		LogInfo(("启动对linker上报连接数的检测,时间间隔%u秒", timerInterval / 1000));
	}
	return isSuccess;
}

void PartitionInfoMan::UnLoadPartitionInfo()
{
	if (m_linkerHeartTimer.IsStarted())
		m_linkerHeartTimer.Stop();

	CXTAutoLock lock(m_partitionInfoMapLocker);
	std::map<UINT16, PartitionSite*>::iterator it = m_partitionInfoMap.begin();
	for (; it != m_partitionInfoMap.end(); ++it)
	{
		it->second->DelAllLinkers();
		it->second->ReleaseRef();
	}
	m_partitionInfoMap.clear();
}

#if SEPARATOR("For LinkServerMsg or LinkSession")
int PartitionInfoMan::LinkerLogin(UINT16 partitionId, BYTE linkerId, LicenceSession* session, UINT32 linkerIp, UINT16 linkerPort)
{
	PartitionSite* p = GetPartitionSite(partitionId);
	if (p == NULL)
		return LCS_NOT_EXIST_PARTITION;

	AutoRefCountObjTmpl<PartitionSite> pAutoObj(p);
	return pAutoObj->LinkerLogin(linkerId, session, linkerIp, linkerPort);
}

int PartitionInfoMan::LinkerLogout(UINT16 partitionId, BYTE linkerId)
{
	PartitionSite* p = GetPartitionSite(partitionId);
	if (p == NULL)
		return LCS_NOT_EXIST_PARTITION;

	AutoRefCountObjTmpl<PartitionSite> pAutoObj(p);
	return pAutoObj->LinkerLogout(linkerId);
}

int PartitionInfoMan::GetPartitionStatus(UINT16 partitionId, BYTE& status)
{
	CXTAutoLock lock(m_partitionInfoMapLocker);
	std::map<UINT16, PartitionSite*>::iterator it = m_partitionInfoMap.find(partitionId);
	if (it == m_partitionInfoMap.end())
		return LCS_NOT_EXIST_PARTITION;
	status = it->second->GetStatus();
	return LCS_OK;
}

int PartitionInfoMan::UpdateLinkerConnections(UINT16 partitionId, BYTE linkerId, UINT32 connections, UINT32 tokens)
{
	PartitionSite* p = GetPartitionSite(partitionId);
	if (p == NULL)
		return LCS_NOT_EXIST_PARTITION;

	AutoRefCountObjTmpl<PartitionSite> pAutoObj(p);
	return pAutoObj->UpdateLinkerConnections(linkerId, connections, tokens);
}

int PartitionInfoMan::GetPartitionLinkerAddr(UINT16 partitionId, BYTE linkerId, UINT32& linkerIp, UINT16& linkerPort)
{
	PartitionSite* p = GetPartitionSite(partitionId);
	if (p == NULL)
		return LCS_NOT_EXIST_PARTITION;

	AutoRefCountObjTmpl<PartitionSite> pAutoObj(p);
	return pAutoObj->GetLinkerAddr(linkerId, linkerIp, linkerPort);
}

int PartitionInfoMan::NotifyUserFromLinkerOffline(UINT16 partitionId, BYTE exceptLinkerId, UINT32 userId)
{
	PartitionSite* p = GetPartitionSite(partitionId);
	if (p == NULL)
		return LCS_NOT_EXIST_PARTITION;

	AutoRefCountObjTmpl<PartitionSite> pAutoObj(p);
	return pAutoObj->NotifyUserOffline(exceptLinkerId, userId);
}
#endif

#if SEPARATOR("For UserMsg or UserSession")
int PartitionInfoMan::GetMinPayloadLinker(UINT16 partitionId, BYTE& linkerId)
{
	PartitionSite* p = GetPartitionSite(partitionId);
	if (p == NULL)
		return LCS_NOT_EXIST_PARTITION;

	AutoRefCountObjTmpl<PartitionSite> pAutoObj(p);
	return pAutoObj->GetMinPayloadLinker(linkerId);
}

int PartitionInfoMan::UserEnterLinkerGetToken(UINT16 partitionId, BYTE linkerId, UINT32 userId, const char* username)
{
	PartitionSite* p = GetPartitionSite(partitionId);
	if (p == NULL)
		return LCS_NOT_EXIST_PARTITION;

	AutoRefCountObjTmpl<PartitionSite> pAutoObj(p);
	return pAutoObj->EnterLinkerGetToken(linkerId, userId, username);
}

int PartitionInfoMan::GetPartitionInfoTable(UINT16 from, UINT16 num, SUserQueryPartitionRes* pQueryRes)
{
	pQueryRes->num = 0;
	pQueryRes->totals = 0;
	if (num <= 0)
		return LCS_OK;

	UINT16 maxNum = (UINT16)((MAX_RES_USER_BYTES - member_offset(SUserQueryPartitionRes, totals, data)) / sizeof(SPartitionInfo));

	CXTAutoLock lock(m_partitionInfoMapLocker);
	if (m_partitionInfoMap.empty())
		return LCS_OK;
	pQueryRes->totals = m_partitionInfoMap.size();

	int start = from;
	std::map<UINT16/*分区id*/, PartitionSite*>::iterator it = m_partitionInfoMap.begin();
	while (start-- > 0)
	{
		++it;
		if (it == m_partitionInfoMap.end())
			return LCS_OK;
	}
	for (; it != m_partitionInfoMap.end(); ++it)
	{
		PartitionSite* pSite = it->second;
		ASSERT(pSite != NULL);
		strcpy(pQueryRes->data[pQueryRes->num].name, pSite->GetName());
		pQueryRes->data[pQueryRes->num].isRecommend = pSite->IsRecommend() ? 1 : 0;
		pQueryRes->data[pQueryRes->num].status = pSite->GetStatus();
		pQueryRes->data[pQueryRes->num].partitionId = it->first;
		++pQueryRes->num;
		if (pQueryRes->num >= num || pQueryRes->num >= maxNum)
			break;
	}

	return LCS_OK;
}

UINT16 PartitionInfoMan::GetRecommendPartitionId()
{
	UINT16 id = 0;
	CXTAutoLock lock(m_partitionInfoMapLocker);
	std::map<UINT16, PartitionSite*>::iterator it = m_partitionInfoMap.begin();
	for (; it != m_partitionInfoMap.end(); ++it)
	{
		if (it->second->IsRecommend())
		{
			id = it->first;
			break;
		}
	}
	return id;
}

void PartitionInfoMan::GetMinAccountPartition(UINT16& partId, UINT32& accountNum)
{
	partId = 0;
	accountNum = 0;
	{
		CXTAutoLock lock(m_partitionInfoMapLocker);
		if (m_partitionInfoMap.empty())
			return;

		std::map<UINT16, PartitionSite*>::iterator it = m_partitionInfoMap.begin();
		partId = it->first;
		accountNum = it->second->GetRegistAccountNum();
		for (; it != m_partitionInfoMap.end(); ++it)
		{
			PartitionSite* pSite = it->second;
			if (!pSite->HasLinkerLogin())
				continue;

			UINT32 nCount = pSite->GetRegistAccountNum();
			if (nCount < accountNum)
			{
				partId = it->first;
				accountNum = nCount;
			}
		}

		//m_partitionInfoMap[partId]->SetRegistAccountNum(++accountNum);
	}

	//刷新分区注册用户数量
	//  char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
	//  sprintf(szSQL, "update game_partition set accounts=%u where id=%d", accountNum, partId);
	//  UINT32 affectRows = 0;
	//  GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
	//  ASSERT(affectRows == 1);
}

void PartitionInfoMan::UpdatePartitionUsers(UINT16 partId)
{
	UINT32 accountNum = 0;
	{
		CXTAutoLock lock(m_partitionInfoMapLocker);
		std::map<UINT16, PartitionSite*>::iterator it = m_partitionInfoMap.find(partId);
		if (it == m_partitionInfoMap.end())
			return;
		accountNum = it->second->GetRegistAccountNum();
		it->second->SetRegistAccountNum(++accountNum);
	}

	//刷新分区注册用户数量
	char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
	sprintf(szSQL, "update game_partition set accounts=%u where id=%d", accountNum, partId);
	UINT32 affectRows = 0;
	GetDBSClient(DBCLI_VERIFY)->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
	ASSERT(affectRows == 1);
}

int PartitionInfoMan::QueryUnlockJobs(UINT16 partId, UINT32 userId)
{
	PartitionSite* p = GetPartitionSite(partId);
	if (p == NULL)
		return LCS_NOT_EXIST_PARTITION;

	AutoRefCountObjTmpl<PartitionSite> pAutoObj(p);
	return pAutoObj->QueryUnlockJobs(userId);
}

int PartitionInfoMan::GetPartitionName(UINT16 partitionId, char* pszName)
{
	PartitionSite* p = GetPartitionSite(partitionId);
	if (p == NULL)
		return LCS_NOT_EXIST_PARTITION;

	AutoRefCountObjTmpl<PartitionSite> pAutoObj(p);
	strcpy(pszName, pAutoObj->GetName());
	return LCS_OK;
}
#endif

#if SEPARATOR("For ExtServeLogicMan")
int PartitionInfoMan::StartPartitionLinkerServe(UINT16 partitionId)
{
	PartitionSite* p = GetPartitionSite(partitionId);
	if (p == NULL)
		return LCS_NOT_EXIST_PARTITION;

	AutoRefCountObjTmpl<PartitionSite> pAutoObj(p);
	return pAutoObj->StartAllLinkerServe();
}

int PartitionInfoMan::StopPartitionLinkerServe(UINT16 partitionId)
{
	PartitionSite* p = GetPartitionSite(partitionId);
	if (p == NULL)
		return LCS_NOT_EXIST_PARTITION;

	AutoRefCountObjTmpl<PartitionSite> pAutoObj(p);
	return pAutoObj->StopAllLinkerServe();
}

//GM管理
int PartitionInfoMan::QueryGamePartition(UINT16 from, BYTE num, SExtManQueryGamePartitionRes* pRes)
{
	BYTE idx = 0;
	UINT16 n = from;
	BYTE maxNum = (MAX_RES_INNER_USER_BYTES - member_offset(SExtManQueryGamePartitionRes, total, gameParts)) / sizeof(SGamePartitionInfo);

	{
		CXTAutoLock lock(m_partitionInfoMapLocker);
		std::map<UINT16/*分区id*/, PartitionSite*>::iterator it = m_partitionInfoMap.begin();
		while (n-- && it != m_partitionInfoMap.end())
			++it;

		BYTE idx = 0;
		for (; it != m_partitionInfoMap.end(); ++it)
		{
			PartitionSite* pSite = it->second;
			if (pSite == NULL)
				continue;

			pRes->gameParts[idx].partId = it->first;
			strcpy(pRes->gameParts[idx].name, pSite->GetName());
			pRes->gameParts[idx].hton();

			idx++;
			if (idx >= num || idx >= maxNum)
				break;
		}
	}

	pRes->retNum = idx;
	return LCS_OK;
}

int PartitionInfoMan::AddPartition(const char* name, const char* lic, BYTE isRecommend, UINT16& newPartId)
{
	/*
	写数据库
	*/
	char buf[sizeof(SLinkerInfo) + sizeof(BYTE)] = { 0 };
	char szBuf[64] = { 0 };
	DBS_EscapeString(szBuf, (const char*)buf, sizeof(SLinkerInfo) + sizeof(BYTE));
	char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
	sprintf(szSQL, "insert into tpartition(name, connvc, linker, mansdp, status, isrecomment) values('%s', '%s', '%s', '%s', %d, %d)",
		name, lic, szBuf, "0.0.0.0:0", 1, isRecommend);
	UINT32 newId = 0;
	int r = GetDBSClient(DBCLI_VERIFY)->Insert(GetConfigFileMan()->GetDBName(), szSQL, &newId);
	if (r != LCS_OK)
		return r;

	PartitionSite* site = new PartitionSite();
	ASSERT(site != NULL);
	site->SetName(name);
	site->SetRecommendFlag(isRecommend);
	site->SetStatus(1);
	CXTAutoLock lock(m_partitionInfoMapLocker);
	std::map<UINT16, PartitionSite*>::iterator it = m_partitionInfoMap.find(newId);
	if (it != m_partitionInfoMap.end())
	{
		delete site;
		return LCS_EXISTED_PARTITION;
	}
	m_partitionInfoMap.insert(std::make_pair(newId, site));
	newPartId = (UINT16)newId;
	return LCS_OK;
}

int PartitionInfoMan::ModifyPartitionName(UINT16 partitionId, const char* newName)
{
	/*
	修改数据库
	*/
	char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
	sprintf(szSQL, "update tpartition set name='%s' where id=%d", newName, partitionId);
	int retCode = GetDBSClient(DBCLI_VERIFY)->Update(GetConfigFileMan()->GetDBName(), szSQL);
	if (retCode != DBS_OK)
		return retCode;

	CXTAutoLock lock(m_partitionInfoMapLocker);
	std::map<UINT16, PartitionSite*>::iterator it = m_partitionInfoMap.find(partitionId);
	if (it == m_partitionInfoMap.end())
		return LCS_NOT_EXIST_PARTITION;
	it->second->SetName(newName);
	return LCS_OK;
}

int PartitionInfoMan::SetRecommendPartition(UINT16 partitionId)
{
	/*
	修改数据库
	*/

	char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
	sprintf(szSQL, "update tpartition set isrecomment=1 where id=%d", partitionId);
	int retCode = GetDBSClient(DBCLI_VERIFY)->Update(GetConfigFileMan()->GetDBName(), szSQL);
	if (retCode != DBS_OK)
		return retCode;

	CXTAutoLock lock(m_partitionInfoMapLocker);
	std::map<UINT16, PartitionSite*>::iterator it = m_partitionInfoMap.find(partitionId);
	if (it == m_partitionInfoMap.end())
		return LCS_NOT_EXIST_PARTITION;
	it->second->SetRecommendFlag(true);
	return LCS_OK;
}

int PartitionInfoMan::DelPartition(UINT16 partitionId)
{
	/*
	修改数据库
	*/
	char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
	sprintf(szSQL, "delete from tpartition where id=%d", partitionId);
	int retCode = GetDBSClient(DBCLI_VERIFY)->Del(GetConfigFileMan()->GetDBName(), szSQL);
	if (retCode != DBS_OK)
		return retCode;

	PartitionSite* site = NULL;
	{
		CXTAutoLock lock(m_partitionInfoMapLocker);
		std::map<UINT16, PartitionSite*>::iterator it = m_partitionInfoMap.find(partitionId);
		if (it == m_partitionInfoMap.end())
			return LCS_NOT_EXIST_PARTITION;

		site = it->second;
		m_partitionInfoMap.erase(it);
	}

	if (site != NULL)
	{
		site->DelAllLinkers();
		site->ReleaseRef();
	}
	return LCS_OK;
}
#endif

#if SEPARATOR("Inner Process")
BOOL PartitionInfoMan::IsRecommendPartition(UINT16 partitionId)
{
	PartitionSite* p = GetPartitionSite(partitionId);
	if (p == NULL)
		return false;
	AutoRefCountObjTmpl<PartitionSite> pAutoObj(p);
	return pAutoObj->IsRecommend();
}

PartitionInfoMan::PartitionSite* PartitionInfoMan::GetPartitionSite(UINT16 partitionId)
{
	PartitionSite* p = NULL;
	CXTAutoLock lock(m_partitionInfoMapLocker);
	std::map<UINT16, PartitionSite*>::iterator it = m_partitionInfoMap.find(partitionId);
	if (it != m_partitionInfoMap.end())
	{
		p = it->second;
		p->AddRef();
	}
	return p;
}

void PartitionInfoMan::LinkerHeartTimer(void* param, TTimerID id)
{
	PartitionInfoMan* pThis = (PartitionInfoMan*)param;
	ASSERT(pThis != NULL);
	pThis->OnHeartCheck();
}

void PartitionInfoMan::OnHeartCheck()
{
	std::vector<PartitionSite*> partVec;
	{
		CXTAutoLock lock(m_partitionInfoMapLocker);
		std::map<UINT16, PartitionSite*>::iterator it = m_partitionInfoMap.begin();
		for (; it != m_partitionInfoMap.end(); ++it)
		{
			PartitionSite* pSite = it->second;
			pSite->AddRef();
			partVec.push_back(pSite);
		}
	}

	int nSize = (int)partVec.size();
	for (int i = 0; i < nSize; ++i)
	{
		partVec[i]->CheckHeartbeat();
		partVec[i]->ReleaseRef();
	}
}
#endif

PartitionInfoMan::PartitionSite::PartitionSite()
{
	m_name = "";
	m_accountNum = 0;
	m_status = 0;
	m_isRecommend = false;
	m_linkersMap.clear();
}

PartitionInfoMan::PartitionSite::~PartitionSite()
{
	m_name = "";
	m_accountNum = 0;
	m_status = 0;
	m_isRecommend = false;
	m_linkersMap.clear();
}

int PartitionInfoMan::PartitionSite::LinkerLogin(BYTE linkerId, LicenceSession* session, UINT32 linkerIp, UINT16 linkerPort)
{
	CXTAutoLock lock(m_linkersMapLocker);
	std::map<BYTE, LinkerInfo>::iterator it = m_linkersMap.find(linkerId);
	if (it != m_linkersMap.end())
		return LCS_LINKER_LOGIN_AGAIN;

	LinkerInfo linker;
	linker.linkerIp = linkerIp;
	linker.linkerPort = linkerPort;
	linker.session = session;
	m_linkersMap.insert(std::make_pair(linkerId, linker));
	return LCS_OK;
}

int PartitionInfoMan::PartitionSite::LinkerLogout(BYTE linkerId)
{
	CXTAutoLock lock(m_linkersMapLocker);
	std::map<BYTE, LinkerInfo>::iterator it = m_linkersMap.find(linkerId);
	if (it != m_linkersMap.end())
		m_linkersMap.erase(it);
	return LCS_OK;
}

int PartitionInfoMan::PartitionSite::UpdateLinkerConnections(BYTE linkerId, UINT32 connections, UINT32 tokens)
{
	CXTAutoLock lock(m_linkersMapLocker);
	std::map<BYTE, LinkerInfo>::iterator it = m_linkersMap.find(linkerId);
	if (it == m_linkersMap.end())
		return LCS_LINKER_NOT_LOGIN;

	it->second.connections = connections;
	it->second.tokens = tokens;
	return LCS_OK;
}

int PartitionInfoMan::PartitionSite::StartAllLinkerServe()
{
	CXTAutoLock lock(m_linkersMapLocker);
	std::map<BYTE, LinkerInfo>::iterator it = m_linkersMap.begin();
	for (; it != m_linkersMap.end(); ++it)
	{
		if (it->second.session == NULL)
			continue;
		it->second.session->ControlLinkerServe(true);
	}
	return LCS_OK;
}

int PartitionInfoMan::PartitionSite::StopAllLinkerServe()
{
	CXTAutoLock lock(m_linkersMapLocker);
	std::map<BYTE, LinkerInfo>::iterator it = m_linkersMap.begin();
	for (; it != m_linkersMap.end(); ++it)
	{
		if (it->second.session == NULL || it->second.status == 1)
			continue;
		it->second.session->ControlLinkerServe(false);
	}
	return LCS_OK;
}


int PartitionInfoMan::PartitionSite::GetMinPayloadLinker(BYTE& linkerId)
{
	linkerId = 0;
	int minConns = 0;
	CXTAutoLock lock(m_linkersMapLocker);
	std::map<BYTE, LinkerInfo>::iterator it = m_linkersMap.begin();
	for (; it != m_linkersMap.end(); ++it)
	{
		if (it->second.session == NULL || it->second.status == 1)
			continue;

		if (minConns == 0)
		{
			minConns = it->second.connections;
			linkerId = it->first;
		}
		if (it->second.connections < minConns)
		{
			minConns = it->second.connections;
			linkerId = it->first;
		}
		if (minConns < 1)
			break;
	}
	return linkerId == 0 ? LCS_LINKER_NOT_LOGIN : LCS_OK;
}

int PartitionInfoMan::PartitionSite::EnterLinkerGetToken(BYTE linkerId, UINT32 userId, const char* username)
{
	CXTAutoLock lock(m_linkersMapLocker);
	std::map<BYTE, LinkerInfo>::iterator it = m_linkersMap.find(linkerId);
	if (it == m_linkersMap.end())
		return LCS_LINKER_NOT_LOGIN;

	if (it->second.session != NULL || it->second.status == 0)
		return it->second.session->UserEnterLinkerGetToken(userId, username);

	return LCS_LINKER_NOT_LOGIN;
}

int PartitionInfoMan::PartitionSite::GetLinkerAddr(BYTE linkerId, UINT32& linkerIp, UINT16& linkerPort)
{
	CXTAutoLock lock(m_linkersMapLocker);
	std::map<BYTE, LinkerInfo>::iterator it = m_linkersMap.find(linkerId);
	if (it == m_linkersMap.end())
		return LCS_LINKER_NOT_LOGIN;

	if (it->second.session != NULL && it->second.status == 0)
	{
		linkerIp = it->second.linkerIp;
		linkerPort = it->second.linkerPort;
		return LCS_OK;
	}
	return LCS_LINKER_NOT_LOGIN;
}

int PartitionInfoMan::PartitionSite::NotifyUserOffline(BYTE exceptLinkerId, UINT32 userId)
{
	CXTAutoLock lock(m_linkersMapLocker);
	std::map<BYTE, LinkerInfo>::iterator it = m_linkersMap.begin();
	for (; it != m_linkersMap.end(); ++it)
	{
		if (it->first == exceptLinkerId || it->second.session == NULL)
			continue;
		it->second.session->NotifyUserOffline(userId);
	}
	return LCS_OK;
}

int PartitionInfoMan::PartitionSite::QueryUnlockJobs(UINT32 userId)
{
	CXTAutoLock lock(m_linkersMapLocker);
	if (m_linkersMap.empty())
		return LCS_LINKER_NOT_LOGIN;

	LicenceSession* pLinkerSession = m_linkersMap.begin()->second.session;
	ASSERT(pLinkerSession != NULL);
	return pLinkerSession->GetUnlockJobs(userId);
}

void PartitionInfoMan::PartitionSite::DelAllLinkers()
{
	CXTAutoLock lock(m_linkersMapLocker);
	for (std::map<BYTE, LinkerInfo>::iterator it = m_linkersMap.begin(); it != m_linkersMap.end(); ++it)
	{
		LicenceSession* session = it->second.session;
		ASSERT(session != NULL);
		if (session == NULL)
			continue;

		ASSERT(session->GetSessionType() == LicenceSession::linker_session);
		session->SetId(0);
		session->GetServeMan()->CloseSession(session);
	}
	m_linkersMap.clear();
}

void PartitionInfoMan::PartitionSite::CheckHeartbeat()
{
	if (m_linkersMap.empty())
		return;

	DWORD tickNow = GetTickCount();
	DWORD timerPeriod = GetConfigFileMan()->GetLinkerHeartbeatInterval() * MAX_NO_LINKER_HEARTBEAT_TIMES;

	CXTAutoLock lock(m_linkersMapLocker);
	if (m_linkersMap.empty())
		return;

	for (std::map<BYTE, LinkerInfo>::iterator it = m_linkersMap.begin(); it != m_linkersMap.end();)
	{
		LicenceSession* session = it->second.session;
		ASSERT(session != NULL && session->GetSessionType() == LicenceSession::linker_session);
		DWORD lastTick = session->GetSessionTick();
		int interval = tickNow - lastTick;
		if (interval < 0)
		{
			session->SetSessionTick(tickNow);
			++it;
			continue;
		}
		if (interval < timerPeriod)
		{
			++it;
			continue;
		}

		BYTE linkerId = it->first;
		session->SetId(0);
		GetUserInfoMan()->PartitionLinkerBroken(session->GetPartitionId(), linkerId);

		LogInfo(("%u秒内未收到Linker[%d]心跳信息,将其连接断开", timerPeriod / 1000, linkerId));

		m_linkersMap.erase(it++);
		session->GetServeMan()->CloseSession(session);
	}
}
