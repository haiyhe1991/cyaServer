#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "cyaLog.h"

static DBSClient* sg_dbsClient = NULL;
static DBSClient* sg_dbsClient2 = NULL;
static DBSClient* sg_dbsClient3 = NULL; // debug

BOOL InitDBSClient()
{
	DBS_Init();

	ASSERT(sg_dbsClient == NULL);
	sg_dbsClient = new DBSClient(GetConfigFileMan()->GetDBSIp(), GetConfigFileMan()->GetDBSPort());
	ASSERT(sg_dbsClient != NULL);
	if (LCS_OK != sg_dbsClient->Connect())
		return false;

	ASSERT(sg_dbsClient2 == NULL);
	sg_dbsClient2 = new DBSClient(GetConfigFileMan()->GetCashDbIp(), GetConfigFileMan()->GetCashDbPort());
	ASSERT(sg_dbsClient2 != NULL);
	if (LCS_OK != sg_dbsClient2->Connect())
		return false;

	ASSERT(sg_dbsClient3 == NULL);
	sg_dbsClient3 = new DBSClient(GetConfigFileMan()->GetActionDbIp(), GetConfigFileMan()->GetActionDbPort());
	ASSERT(sg_dbsClient3 != NULL);
	return sg_dbsClient3->Connect() == LCS_OK ? true : false;
}

DBSClient* GetDBSClient(int type)
{
	if (DBCLI_VERIFY == type)
		return sg_dbsClient;
	else if (DBCLI_CASH == type)
		return sg_dbsClient2;
	else
		return sg_dbsClient3;
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

	dbsClient = sg_dbsClient2;
	sg_dbsClient2 = NULL;
	if (dbsClient != NULL)
	{
		dbsClient->DisConnect();
		delete dbsClient;
	}

	dbsClient = sg_dbsClient3;
	sg_dbsClient3 = NULL;
	if (dbsClient != NULL)
	{
		dbsClient->DisConnect();
		delete dbsClient;
	}

	DBS_Cleanup();
}

DBSClient::DBSClient(const char* ip, int port) : m_dbPort(port)
{
	sprintf(m_dbIP, "%s", ip);
	m_dbIP[15] = 0;
	//DBS_Init();
}

DBSClient::~DBSClient()
{
	//DBS_Cleanup();
}

int  DBSClient::Connect()
{
	int initConnections = GetConfigFileMan()->GetInitDBConnections();
	for (int i = 0; i < initConnections; ++i)
	{
		DBSConnHandler* handler = new DBSConnHandler(this, m_dbIP, m_dbPort);
		ASSERT(handler != NULL);
		int r = handler->Connect();
		if (r != LCS_OK)
		{
			delete handler;
			LogInfo(("连接数据服务器[%s:%d]失败, err:%d!", m_dbIP, m_dbPort, r));
			return r;
		}
		m_dbsHandlerVec.push_back(handler);
	}

	LogInfo(("连接数据服务器[%s:%d]成功!", m_dbIP, m_dbPort));
	return LCS_OK;
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
		DBS_ROW rows = DBS_FetchRow(result);
		*newId = _atoi64(rows[0]);
	}
	DBS_FreeResult(result);
	return LCS_OK;
}

int DBSClient::Query(const char* dbName, const char* sql, DBS_RESULT& result, int nSql/* = -1*/)
{
	DBSConnHandler* dbsHandler = GetDBSConnectionHandler();
	if (dbsHandler == NULL)
		return MLS_READ_DBS_FAILED;

	AutoRefCountObjTmpl<DBSConnHandler> pDBSHandleObj(dbsHandler);
	return DBS_ExcuteSQL(pDBSHandleObj->GetDBSHandle(), DBS_MSG_QUERY, dbName, sql, result, nSql);
}

int DBSClient::Del(const char* dbName, const char* sql, UINT32* affectRows /*= NULL*/, int nSql /*= -1*/)
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

int DBSClient::Update(const char* dbName, const char* sql, UINT32* affectRows /*= NULL*/, int nSql /*= -1*/)
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

	handler = new DBSConnHandler(this, m_dbIP, m_dbPort);
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

DBSClient::DBSConnHandler::DBSConnHandler(DBSClient* parent, char* ip, int port)
	: m_parent(parent), m_dbsHandle(NULL), m_Port(port)
{
	sprintf(m_IP, "%s", ip);
	m_IP[15] = 0;
}

DBSClient::DBSConnHandler::~DBSConnHandler()
{

}

int  DBSClient::DBSConnHandler::Connect()
{
	m_dbsHandle = DBS_Connect(m_IP, m_Port);
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