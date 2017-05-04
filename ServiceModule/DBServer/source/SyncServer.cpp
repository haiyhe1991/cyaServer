#include "SyncServer.h"
#include "DBConfig.h"
#include "DBTable.h"
#include "DBServerMan.h"

const DWORD DefualtDelayIdleTick = 5 * 60 * 1000;

SyncServer::SyncServer()
	: m_syncDelayThread(INVALID_OSTHREAD)
	, m_syncDelayThreadID(INVALID_OSTHREADID)
	, m_syncDelayExit(false)
	, m_delayIdleTick(DefualtDelayIdleTick)
{
	OSSemaInit(&m_syncExitSema);
}

SyncServer::~SyncServer()
{
	OnUnInitialize();
	OSSemaDestroy(&m_syncExitSema);
}

void SyncServer::OnInitialize(DWORD delayIdleTick/* = 5*60*1000*/)
{
	SetSyncDelayIdleTick(delayIdleTick);
	if (delayIdleTick != 0)
	{
		OSCreateThread(&m_syncDelayThread, &m_syncDelayThreadID, SyncThreadRoutine, this, 0);
	}
}

void SyncServer::OnUnInitialize()
{
	if (INVALID_OSTHREAD != m_syncDelayThread)
	{
		m_syncDelayExit = true;
		OSSemaPost(&m_syncExitSema);
		OSCloseThread(m_syncDelayThread);
		m_syncDelayThread = INVALID_OSTHREAD;
		m_syncDelayThreadID = INVALID_OSTHREADID;
	}
}

void SyncServer::SetSyncDelayIdleTick(DWORD delayIdleTick)
{
	m_delayIdleTick = delayIdleTick;
}

DWORD SyncServer::GetDefaultSyncDelayIdleTick() const
{
	return m_delayIdleTick;
}

int SyncServer::SyncThreadRoutine(void* pParam)
{
	SyncServer* pThis = (SyncServer*)pParam;
	if (NULL != pThis)
		return pThis->SyncThreadWoker();
	return 0;
}

int SyncServer::SyncThreadWoker()
{
	while (!m_syncDelayExit)
	{
		OSSemaWait(&m_syncExitSema, m_delayIdleTick);
		for (INT i = 0; i<GetDBConfigObj().DBS_DB_Count(); ++i)
		{
			EnforceSync(GetDBConfigObj().DBS_DB_Name(i));
		}
	}
	return 0;
}

int SyncServer::EnforceSync(const char* dbName)
{
	UINT16 table_num = 0;
	SQLTable* theTable = SQLTable::FetchTables();
	const std::map<std::string, MemTable*>& all_table = theTable->GetAllTable();
	std::map<std::string, MemTable*>::const_iterator itr = all_table.begin();
	for (itr; all_table.end() != itr; ++itr)
	{
		SqlSingle* sqlSession = GetDBServerManObj().FetchDBSessionInsertHandle(dbName);
		SyncHelper::FetchSyncHelper()->EnforceSync(itr->second, sqlSession);
		++table_num;
	}
	return table_num;
}

