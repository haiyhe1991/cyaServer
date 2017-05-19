#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "cyaLog.h"

static DBSClient* sg_dbsClient = NULL;
int InitDBSClient()
{
	ASSERT(sg_dbsClient == NULL);
	sg_dbsClient = new DBSClient();
	ASSERT(sg_dbsClient != NULL);
	return sg_dbsClient->Connect();
}

DBSClient* GetDBSClient()
{
	return sg_dbsClient;
}

void DestroyDBSClient()
{
	DBSClient* dbsClient = sg_dbsClient;
	sg_dbsClient = NULL;

	if (dbsClient != NULL)
	{
		dbsClient->DisConnect();
		delete dbsClient;
	}
}

DBSClient::DBSClient()
{
	DBS_Init();
}

DBSClient::~DBSClient()
{
	DBS_Cleanup();
}

int  DBSClient::Connect()
{
	int initConnections = GetConfigFileMan()->GetInitDBConnections();
	for (int i = 0; i < initConnections; ++i)
	{
		DBSConnHandler* handler = new DBSConnHandler(this);
		ASSERT(handler != NULL);
		int r = handler->Connect();
		if (r != DBS_OK)
		{
			delete handler;
			LogInfo(("连接数据服务器[%s:%d]失败, err:%d!", GetConfigFileMan()->GetDBSIp(), GetConfigFileMan()->GetDBSPort(), r));
			return r;
		}
		m_dbsHandlerVec.push_back(handler);
	}

	LogInfo(("连接数据服务器[%s:%d]成功!", GetConfigFileMan()->GetDBSIp(), GetConfigFileMan()->GetDBSPort()));
	return MLS_OK;
}

void DBSClient::DisConnect()
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

int DBSClient::Query(const char* dbName, const char* sql, DBS_RESULT& result, int nSql/* = -1*/)
{
	DBSConnHandler* dbsHandler = GetDBSConnectionHandler();
	if (dbsHandler == NULL)
		return MLS_READ_DBS_FAILED;

	AutoRefCountObjTmpl<DBSConnHandler> pDBSHandleObj(dbsHandler);
	return DBS_ExcuteSQL(pDBSHandleObj->GetDBSHandle(), DBS_MSG_QUERY, dbName, sql, result, nSql);
}

int DBSClient::Insert(const char* dbName, const char* sql, UINT32* newId, int nSql/* = -1*/)
{
	DBSConnHandler* dbsHandler = GetDBSConnectionHandler();
	if (dbsHandler == NULL)
		return LCS_READ_DBS_FAILED;

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
		DBS_ROW row = DBS_FetchRow(result);
		*newId = _atoi64(row[0]);
	}
	DBS_FreeResult(result);

	return MLS_OK;
}

int DBSClient::Del(const char* dbName, const char* sql, UINT32* affectRows/* = NULL*/, int nSql/* = -1*/)
{
	DBSConnHandler* dbsHandler = GetDBSConnectionHandler();
	if (dbsHandler == NULL)
		return MLS_READ_DBS_FAILED;

	DBS_RESULT result = NULL;
	AutoRefCountObjTmpl<DBSConnHandler> pDBSHandleObj(dbsHandler);
	int r = DBS_ExcuteSQL(pDBSHandleObj->GetDBSHandle(), DBS_MSG_DELETE, dbName, sql, result, nSql);
	if (r != DBS_OK)
		return r;

	if (affectRows != NULL)
		*affectRows = DBS_AffectedRows(result);
	DBS_FreeResult(result);
	return MLS_OK;
}

int DBSClient::Update(const char* dbName, const char* sql, UINT32* affectRows /*= NULL*/, int nSql/* = -1*/)
{
	DBSConnHandler* dbsHandler = GetDBSConnectionHandler();
	if (dbsHandler == NULL)
		return MLS_READ_DBS_FAILED;

	DBS_RESULT result = NULL;
	AutoRefCountObjTmpl<DBSConnHandler> pDBSHandleObj(dbsHandler);
	int r = DBS_ExcuteSQL(pDBSHandleObj->GetDBSHandle(), DBS_MSG_UPDATE, dbName, sql, result, nSql);
	if (r != DBS_OK)
		return r;

	if (affectRows != NULL)
		*affectRows = DBS_AffectedRows(result);
	DBS_FreeResult(result);
	return MLS_OK;
}

DBSClient::DBSConnHandler* DBSClient::GetDBSConnectionHandler()
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

	if (m_dbsHandlerVec.size() >= GetConfigFileMan()->GetMaxDBConnections())
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

DBSClient::DBSConnHandler::DBSConnHandler(DBSClient* parent)
: m_parent(parent)
, m_dbsHandle(NULL)
{

}

DBSClient::DBSConnHandler::~DBSConnHandler()
{

}

int  DBSClient::DBSConnHandler::Connect()
{
	m_dbsHandle = DBS_Connect(GetConfigFileMan()->GetDBSIp(), GetConfigFileMan()->GetDBSPort());
	return m_dbsHandle == NULL ? LCS_DBS_LOGIN_FAILED : LCS_OK;
}

void DBSClient::DBSConnHandler::DisConnect()
{
	if (m_dbsHandle != NULL)
	{
		DBS_DisConnect(m_dbsHandle);
		m_dbsHandle = NULL;
	}
}