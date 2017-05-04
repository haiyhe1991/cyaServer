#include "DBServerMan.h"

#include "cyaIpCvt.h"
#include "cyaLocker.h"
#include "cyaLog.h"
#include "DBConfig.h"

DBServerMan::DBServerMan()
	:m_manstart(FALSE)
	, m_tcpmaker(NULL)
	, m_tcpman(NULL)
	, m_syncser(NULL)
{

}

DBServerMan::~DBServerMan()
{
	FinishDBS();
}

BOOL DBServerMan::InitializeDBS()
{
	if (NULL == m_tcpmaker)
	{
		m_tcpmaker = new DBSessionMaker();
	}

	if (m_tcpmaker && NULL == m_tcpman)
	{
		CyaTcp_MakeServeMan(m_tcpman, m_tcpmaker);
	}

	INT count_ = GetDBConfigObj().DBS_DB_Count();
	for (INT i = 0; i < count_; ++i)
	{
		DBSession* theDb = new DBSession();
		if (NULL != theDb)
		{
			if (0 < theDb->DBS_Connect(GetDBConfigObj().DBS_SQL_Addr(),
				GetDBConfigObj().DBS_SQL_User(),
				GetDBConfigObj().DBS_SQL_Pswd(),
				GetDBConfigObj().DBS_DB_Name(i)))
			{
				VERIFY(m_db_sessins.insert(std::make_pair(GetDBConfigObj().DBS_DB_Name(i), theDb)).second);
				LogInfo(("连接到(DB：%s)%s:%d", GetDBConfigObj().DBS_DB_Name(i),
					GetDBConfigObj().DBS_SQL_Addr(),
					GetDBConfigObj().DBS_SQL_Port()));
			}
		}
	}
	if (m_db_sessins.empty())
		LogInfo(("没能连接到数据库"));

	if (m_tcpmaker && m_tcpman)
	{
		IPPADDR ipd(GetDWordIP(GetDBConfigObj().DBS_Addr()), GetDBConfigObj().DBS_Port());
		if (!m_manstart && 0 == m_tcpman->StartServe(ipd, true))
		{
			m_manstart = TRUE;
			m_syncser = new SyncServer();	/// 开启数据同步
			m_syncser->OnInitialize();
		}
		if (!m_manstart)
		{
			LogInfo(("开启DBServer失败"));
			return FALSE;
		}

		return !m_db_sessins.empty();
	}
	return FALSE;
}

void DBServerMan::FinishDBS()
{
	if (m_syncser)
	{
		m_syncser->OnUnInitialize();
		delete m_syncser;
		m_syncser = NULL;
	}
	if (m_tcpman)
	{
		if (m_manstart)
		{
			m_tcpman->StopServe();
			m_manstart = FALSE;
		}
		delete m_tcpman;
		m_tcpman = NULL;
	}
	if (m_tcpmaker)
	{
		delete m_tcpmaker;
		m_tcpmaker = NULL;
	}
	DB_SESSIONS::iterator itr = m_db_sessins.begin();
	for (itr; m_db_sessins.end() != itr; ++itr)
	{
		itr->second->DBS_Close();
		delete itr->second;
	}
	m_db_sessins.clear();
}

DBSessionHandle DBServerMan::FetchDBSessionHandle(const char* dbName)
{
	DB_SESSIONS::iterator itr = m_db_sessins.find(dbName);
	if (m_db_sessins.end() != itr)
	{
		return itr->second->FetchDBSessionHandle();
	}
	LogDebug(("不存在的数据库名"));
	return NULL;
}

DBSessionHandle DBServerMan::FetchDBSessionInsertHandle(const char* dbName)
{
	DB_SESSIONS::iterator itr = m_db_sessins.find(dbName);
	if (m_db_sessins.end() != itr)
	{
		return itr->second->FetchDBSessionInsertHandle();
	}
	LogDebug(("不存在的数据库名"));
	return NULL;
}

DBServerMan& GetDBServerManObj()
{
	BEGIN_LOCAL_SAFE_STATIC_OBJ(DBServerMan, dbsMan);
	END_LOCAL_SAFE_STATIC_OBJ(dbsMan);
}
