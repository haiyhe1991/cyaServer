#include "cyaLog.h"
#include "RankDBSClient.h"
#include "RankConfigFileMan.h"

static RankDBSClient* sg_dbsRankClient = NULL;
BOOL InitRankDBSClient()
{
	ASSERT(sg_dbsRankClient == NULL);
	sg_dbsRankClient = new RankDBSClient();
	ASSERT(sg_dbsRankClient != NULL);
	return sg_dbsRankClient->Connect() == LCS_OK ? true : false;
}

RankDBSClient* GetRankDBSClient()
{
	return sg_dbsRankClient;
}

void DestroyRankDBSClient()
{
	RankDBSClient* dbsClient = sg_dbsRankClient;
	sg_dbsRankClient = NULL;
	if (dbsClient != NULL)
	{
		dbsClient->DisConnect();
		delete dbsClient;
	}
}

BOOL ConnectRankDBSClient()
{
	if (sg_dbsRankClient == NULL)
	{
		LogDebug(("连接数据服务器[%s:%d]失败, 数据库客户端不存在!", GetRankConfigFileMan()->GetDBSIp(), GetRankConfigFileMan()->GetDBSPort()));
		return false;
	}
	else
	{
		return sg_dbsRankClient->Connect() == LCS_OK ? true : false;
	}
}

void DisConnectRankDBSClient()
{
	if (sg_dbsRankClient != NULL)
	{
		sg_dbsRankClient->DisConnect();
	}
}

RankDBSClient::RankDBSClient()
{
	DBS_Init();
}

RankDBSClient::~RankDBSClient()
{
	DBS_Cleanup();
}

int RankDBSClient::Connect()
{
	CXTAutoLock lock(m_dbsHandlerVecLocker);
	if (0 < m_dbsHandlerVec.size())
		return RANK_OK;

	int initConnections = GetRankConfigFileMan()->GetInitDBConnections();
	for (int i = 0; i < initConnections; ++i)
	{
		DBSConnHandler* handler = new DBSConnHandler(this);
		ASSERT(handler != NULL);
		int r = handler->Connect();
		if (r != RANK_OK)
		{
			delete handler;
			LogDebug(("连接数据服务器[%s:%d]失败, err:%d!", GetRankConfigFileMan()->GetDBSIp(), GetRankConfigFileMan()->GetDBSPort(), r));
			return r;
		}
		m_dbsHandlerVec.push_back(handler);
	}

	LogDebug(("连接数据服务器[%s:%d]成功!", GetRankConfigFileMan()->GetDBSIp(), GetRankConfigFileMan()->GetDBSPort()));
	return RANK_OK;
}

void RankDBSClient::DisConnect()
{
	CXTAutoLock lock(m_dbsHandlerVecLocker);
	for (int i = 0; i < (int)m_dbsHandlerVec.size(); ++i)
	{
		DBSConnHandler* handler = m_dbsHandlerVec[i];
		ASSERT(handler != NULL);
		handler->DisConnect();
		handler->ReleaseRef();
	}
	m_dbsHandlerVec.clear();
}

int RankDBSClient::Insert(const char* dbName, const char* sql, UINT32* newId, int nSql/* = -1*/)
{
	DBSConnHandler* dbsHandler = GetDBSConnectionHandler();
	if (dbsHandler == NULL)
		return RANK_DBS_READ_FAILED;

	DBS_RESULT result;
	{
		AutoRefCountObjTmpl<DBSConnHandler> pDBSHandleObj(dbsHandler);
		int ret = DBS_ExcuteSQL(pDBSHandleObj->GetDBSHandle(), DBS_MSG_INSERT, dbName, sql, result, nSql);
		if (ret != DBS_OK)
			return ret;
	}

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows > 0 && nCols > 0 && newId != NULL)
	{
		DBS_ROW rows = DBS_FetchRow(result);
		*newId = _atoi64(rows[0]);
	}
	DBS_FreeResult(result);
	return LCS_OK;
}

int RankDBSClient::Query(const char* dbName, const char* sql, DBS_RESULT& result, int nSql/* = -1*/)
{
	DBSConnHandler* dbsHandler = GetDBSConnectionHandler();
	if (dbsHandler == NULL)
		return RANK_DBS_READ_FAILED;

	AutoRefCountObjTmpl<DBSConnHandler> pDBSHandleObj(dbsHandler);
	return DBS_ExcuteSQL(pDBSHandleObj->GetDBSHandle(), DBS_MSG_QUERY, dbName, sql, result, nSql);
}

int RankDBSClient::Del(const char* dbName, const char* sql, UINT32* affectRows /*= NULL*/, int nSql /*= -1*/)
{
	DBSConnHandler* dbsHandler = GetDBSConnectionHandler();
	if (dbsHandler == NULL)
		return RANK_DBS_READ_FAILED;

	DBS_RESULT result = NULL;
	AutoRefCountObjTmpl<DBSConnHandler> pDBSHandleObj(dbsHandler);
	int r = DBS_ExcuteSQL(pDBSHandleObj->GetDBSHandle(), DBS_MSG_DELETE, dbName, sql, result, nSql);
	if (r != DBS_OK)
		return r;

	if (affectRows != NULL)
		*affectRows = DBS_AffectedRows(result);
	DBS_FreeResult(result);
	return DBS_OK;
}

int RankDBSClient::Update(const char* dbName, const char* sql, UINT32* affectRows /*= NULL*/, int nSql /*= -1*/)
{
	DBSConnHandler* dbsHandler = GetDBSConnectionHandler();
	if (dbsHandler == NULL)
		return RANK_DBS_READ_FAILED;

	DBS_RESULT result = NULL;
	AutoRefCountObjTmpl<DBSConnHandler> pDBSHandleObj(dbsHandler);
	int r = DBS_ExcuteSQL(pDBSHandleObj->GetDBSHandle(), DBS_MSG_UPDATE, dbName, sql, result, nSql);
	if (r != DBS_OK)
		return r;

	if (affectRows != NULL)
		*affectRows = DBS_AffectedRows(result);
	DBS_FreeResult(result);
	return DBS_OK;
}

RankDBSClient::DBSConnHandler* RankDBSClient::GetDBSConnectionHandler()
{
	int idx = 0;
	int minUsers = -1;
	DBSConnHandler* handler = NULL;

	CXTAutoLock lock(m_dbsHandlerVecLocker);
	for (int i = 0; i < (int)m_dbsHandlerVec.size(); ++i)
	{
		int count = m_dbsHandlerVec[i]->GetRefCount();
		if (minUsers < 0)
		{
			minUsers = count;
			idx = i;
		}
		else if (count < minUsers)
		{
			minUsers = count;
			idx = i;
		}

		if (minUsers <= 1)
		{
			handler = m_dbsHandlerVec[i];
			break;
		}
	}

	if (handler != NULL)
	{
		handler->AddRef();
		return handler;
	}

	if (m_dbsHandlerVec.size() >= GetRankConfigFileMan()->GetMaxDBConnections())
	{
		m_dbsHandlerVec[idx]->AddRef();
		return m_dbsHandlerVec[idx];
	}

	handler = new DBSConnHandler(this);
	ASSERT(handler != NULL);
	int r = handler->Connect();
	if (r == LCS_OK)
	{
		m_dbsHandlerVec.push_back(handler);
		handler->AddRef();
		return handler;
	}
	else
		delete handler;

	m_dbsHandlerVec[idx]->AddRef();
	return m_dbsHandlerVec[idx];
}

RankDBSClient::DBSConnHandler::DBSConnHandler(RankDBSClient* parent)
: m_parent(parent)
, m_dbsHandle(NULL)
{

}

RankDBSClient::DBSConnHandler::~DBSConnHandler()
{

}

int  RankDBSClient::DBSConnHandler::Connect()
{
	m_dbsHandle = DBS_Connect(GetRankConfigFileMan()->GetDBSIp(), GetRankConfigFileMan()->GetDBSPort());
	return m_dbsHandle == NULL ? RANK_DBS_LOGIN_FAILED : LCS_OK;
}

void RankDBSClient::DBSConnHandler::DisConnect()
{
	if (m_dbsHandle != NULL)
	{
		DBS_DisConnect(m_dbsHandle);
		m_dbsHandle = NULL;
	}
}