#include "MsgProcess.h"
#include "SyncHelper.h"


#include "QueryProcess.h"
#include "AppendProcess.h"
#include "RemoveProcess.h"
#include "LoadProcess.h"
#include "UnloadProcess.h"
#include "SetProcess.h"
#include "GetProcess.h"

#define ADD_CMD_HANDLER(cmd, handler)				\
do													\
{													\
	VERIFY(m_cmdMap.insert(std::make_pair(cmd, &MsgProcess::handler)).second);	\
} while(0)

MsgProcess::MsgProcess()
	: m_heartTimer(HeartTimerCallback, this, CLEAR_SESSION_IDLE)
{

}

MsgProcess::~MsgProcess()
{
	FinishMsgPro();
}

BOOL MsgProcess::InitializeMsgPro()
{
	OnInitCmdHandle();
	if (ENABLE_HEART_BEAT && !m_heartTimer.IsStarted())
		return m_heartTimer.Start();
	return TRUE;
}

void MsgProcess::FinishMsgPro()
{
	if (!m_cmdMap.empty() || !m_heartMap.empty())
	{
		CXTAutoLock lock(m_handleLocker);
		m_cmdMap.clear();
		m_heartMap.clear();
	}
	if (m_heartTimer.IsStarted())
		m_heartTimer.Stop(TRUE);
}

void MsgProcess::OnInitCmdHandle()
{
	m_cmdMap.clear();
	ADD_CMD_HANDLER(DBS_MSG_LOAD, OnMemLoad);
	ADD_CMD_HANDLER(DBS_MSG_GET, OnMemGet);
	ADD_CMD_HANDLER(DBS_MSG_SET, OnMemSet);
	ADD_CMD_HANDLER(DBS_MSG_APPEND, OnMemAppend);
	ADD_CMD_HANDLER(DBS_MSG_REMOVE, OnMemRemove);
	ADD_CMD_HANDLER(DBS_MSG_ULOAD, OnMemUnLoad);

	ADD_CMD_HANDLER(DBS_MSG_QUERY, OnDbQuery);
	ADD_CMD_HANDLER(DBS_MSG_UPDATE, OnDbUpdate);
	ADD_CMD_HANDLER(DBS_MSG_INSERT, OnDbInsert);
	ADD_CMD_HANDLER(DBS_MSG_DELETE, OnDbDelete);

	ADD_CMD_HANDLER(DBS_MSG_HEART, OnSessionHeart);
}

INT MsgProcess::MessageProxy(DBTcpSession* sessionSit, const void* msgData, INT msgLen)
{
	UINT16* cmdType = (UINT16*)msgData;
	return OnMsgRequest(sessionSit, ntohs(*cmdType), cmdType + 1, msgLen - sizeof(UINT16));
}

INT MsgProcess::SessionRegist(DBTcpSession* sessionSit)
{
	CXTAutoLock locke(m_handleLocker);
	VERIFY(m_heartMap.insert(std::make_pair(sessionSit, GetTickCount())).second);
	return 0;
}

void MsgProcess::SessionUnRegist(DBTcpSession* sessionSit)
{
	CXTAutoLock locke(m_handleLocker);
	HeartSessinMap::iterator itr = m_heartMap.find(sessionSit);
	if (m_heartMap.end() != itr)
		m_heartMap.erase(itr);
}

MsgProcess::CmdHandleMap::iterator MsgProcess::FindCmdHandle(INT cmdType)
{
	//	CXTAutoLock locke(m_handleLocker);
	return m_cmdMap.find(cmdType);
}

INT MsgProcess::OnMsgRequest(DBTcpSession* mySession, UINT16 cmdType, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != msgData && dataLen > sizeof(UINT16));
	char* dbName = ParseDBName(msgData, dataLen);
	if (NULL == dbName)
		return ResponseMsg(mySession, DBS_ERR_PARAM);

	SqlSingle* sqlSession = NULL;
	if (DBS_MSG_INSERT == cmdType)
		sqlSession = GetDBServerManObj().FetchDBSessionInsertHandle(dbName);
	else
		sqlSession = GetDBServerManObj().FetchDBSessionHandle(dbName);
	ASSERT(NULL != sqlSession && sqlSession->ValidSession());
	if (NULL == sqlSession && sqlSession->ValidSession())
		return ResponseMsg(mySession, DBS_ERROR);

	if (NULL != sqlSession)
	{
		CmdHandleMap::iterator itr = FindCmdHandle(cmdType);
		UINT16 dbNameLen = strlen(dbName) + sizeof(char);
		if (m_cmdMap.end() != itr && NULL != mySession)
			return (this->*(itr->second))(mySession, sqlSession, dbName + dbNameLen, dataLen - dbNameLen - sizeof(UINT16));
	}

	return ResponseMsg(mySession, DBS_ERR_COMMAND);
}

/*	Req : Command + TableName///Table里面有字段要求*/
/// 同步所有字段
INT MsgProcess::OnMemLoad(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != msgData && dataLen > sizeof(UINT16));
	LoadProcess loadPro;
	return loadPro.MemLoad(mySession, sqlSession, msgData, dataLen);
}

/*Req--TableName*/
INT MsgProcess::OnMemUnLoad(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != msgData && dataLen > sizeof(UINT16));
	UnloadProcess UnloadPro;
	return UnloadPro.MemUnLoad(mySession, sqlSession, msgData, dataLen);
}


/*Req--TableName + KeyValue*/
///获取多条记录全部字段
INT MsgProcess::OnMemGet(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != sqlSession && NULL != mySession);
	GetProcess GetPro;
	return GetPro.MemGet(mySession, msgData, dataLen);
}

/*	Req: Table Name + KeyValue + [Field + Value]...*/
/// 修改一条记录的一个字段, 返回修改记录的数目
INT MsgProcess::OnMemSet(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != sqlSession && NULL != mySession);
	SetProcess SetPro;
	return SetPro.MemSet(mySession, msgData, dataLen);
}

/* 	Req: Talbe Name + [Filed + Value]...*/
/// 添加一条条记录的多个字段
INT MsgProcess::OnMemAppend(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != msgData && dataLen > sizeof(UINT16));
	AppendProcess AppendPro;
	return AppendPro.MemAppend(mySession, sqlSession, msgData, dataLen);
}

/* Req: TableName + KeyName + KeyValue */
/// 只删除一条记录
INT MsgProcess::OnMemRemove(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != msgData && dataLen > sizeof(UINT16));
	RemoveProcess RevProc;
	return RevProc.MemRemove(mySession, sqlSession, msgData, dataLen);
}

/* cmd*/
/// return ErrCode + Row数量 + Field数量 + Fields + Rows
/// Fields: FieldNameLen + FiledName
/// Rows:  FieldValueLen + FieldValue
INT MsgProcess::OnDbQuery(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != msgData && dataLen > sizeof(UINT16));
	QueryProcess QuePro;
	return QuePro.DbQuery(mySession, sqlSession, msgData, dataLen);
}

INT MsgProcess::OnDbUpdate(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != msgData && dataLen > sizeof(UINT16));
	return OnDbCommonAffect(mySession, sqlSession, msgData, dataLen, &SqlSingle::UpdateRow);
}

char* ParseCmd(const void* msgData, UINT& dataLen)
{
	ASSERT(NULL != msgData && dataLen > sizeof(UINT16));
	if (dataLen <= sizeof(UINT16))
		return NULL;

	dataLen = ntohs(*(UINT16*)msgData);
	return (char*)msgData + sizeof(UINT16);
}

INT MsgProcess::OnDbInsert(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	ASSERT(0 < dataLen && NULL != sqlSession && sqlSession->ValidSession());
	UINT msgLen = dataLen - sizeof(UINT16);
	char* strCmd = ParseCmd(msgData, msgLen);
	char strFetchRow[MAX_COM_RES_LEN] = { 0 };
	SDBSResultRes* pHead = (SDBSResultRes*)strFetchRow;
	pHead->fields = 0;
	if (NULL == strCmd)
	{
		pHead->rows = 0;
		pHead->retCode = DBS_ERR_PARAM;
		pHead->hton();
		return mySession->OnSubmitCmdPacket(strFetchRow, sizeof(SDBSResultRes) - sizeof(char), true);
	}
	else
	{
		DWORD id = 0;
		pHead->rows = sqlSession->InsertRow(strCmd, msgLen, id);
		if (pHead->rows > 0)
		{
			pHead->fields = 1;
			pHead->retCode = 0;
		}
		else
		{
			pHead->retCode = DBS_ERR_SQL;
		}


		UINT16 offset = 0;
		char idValue[64] = { 0 };
		char resBuf[128] = { 0 };
		memset(resBuf, 0, 128);
		DBS_PRINTF(idValue, sizeof(idValue), "%s", "newid");
		UINT16 idlen = strlen(idValue);
		UINT16* pLen = (UINT16*)(resBuf + offset);
		*pLen = htons(idlen + 1);
		offset += sizeof(UINT16);
		memcpy(resBuf + offset, idValue, idlen);
		offset += idlen + sizeof(char);
		resBuf[offset] = '\0';

		DBS_PRINTF(idValue, sizeof(idValue), "%lu", id);
		idlen = strlen(idValue);
		pLen = (UINT16*)(resBuf + offset);
		*pLen = htons(idlen + 1);
		offset += sizeof(UINT16);
		memcpy(resBuf + offset, idValue, idlen);
		offset += idlen + sizeof(char);
		resBuf[offset] = '\0';
		memcpy(pHead->data, resBuf, offset);
		pHead->hton();
		return mySession->OnSubmitCmdPacket(strFetchRow, sizeof(SDBSResultRes) - sizeof(char) + offset, true);
	}
	// 	pHead->hton();
	// 	return mySession->OnSubmitCmdPacket(strFetchRow, sizeof(SDBSResultRes)-sizeof(char), true);
}

INT MsgProcess::OnDbDelete(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != msgData && dataLen > sizeof(UINT16));
	return OnDbCommonAffect(mySession, sqlSession, msgData, dataLen, &SqlSingle::DeleteRow);
}

INT MsgProcess::OnDbCommonAffect(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData,
	UINT dataLen, SqlRowAffect affectFun)
{
	ASSERT(0 < dataLen && NULL != sqlSession && sqlSession->ValidSession());
	UINT msgLen = dataLen - sizeof(UINT16);
	char* strCmd = ParseCmd(msgData, msgLen);
	char strFetchRow[MAX_COM_RES_LEN] = { 0 };
	SDBSResultRes* pHead = (SDBSResultRes*)strFetchRow;
	pHead->fields = 0;
	if (NULL == strCmd)
	{
		pHead->rows = 0;
		pHead->retCode = DBS_ERR_PARAM;
	}
	else
	{
		pHead->rows = (sqlSession->*affectFun)(strCmd, msgLen);
		if (pHead->rows >= 0)
		{
			pHead->retCode = 0;
		}
		else
		{
			pHead->retCode = DBS_ERR_SQL;
		}
	}
	pHead->hton();
	return mySession->OnSubmitCmdPacket(pHead, sizeof(SDBSResultRes) - sizeof(char), true);
}

INT MsgProcess::OnSessionHeart(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != sqlSession && dataLen && NULL != msgData);
	bool ValidSessin = false;
	HeartSessinMap::iterator itr = m_heartMap.end();
	{
		CXTAutoLock locke(m_handleLocker);
		itr = m_heartMap.find(mySession);

		if (m_heartMap.end() != itr)
		{
			ValidSessin = true;
			itr->second = GetTickCount();
		}
	}

	return -1;
}

INT MsgProcess::SessinClearThWorker()
{
	CXTAutoLock locke(m_handleLocker);
	HeartSessinMap::iterator itr = m_heartMap.begin();
	for (itr; m_heartMap.end() != itr;)
	{
		if (HeartTimeout(itr->second))
		{
			DBTcpSession* session = (DBTcpSession*)itr->first;
			if (NULL != session)
				session->GetServeMan()->CloseSession(session);
			m_heartMap.erase(itr++);
		}
		else
		{
			++itr;
		}
	}
	return 0;
}

void MsgProcess::HeartTimerCallback(void* param, TTimerID id)
{
	MsgProcess* pThis = (MsgProcess*)param;
	if (pThis)
	{
		pThis->SessinClearThWorker();
	}
}

bool MsgProcess::HeartTimeout(DWORD sessionTime)
{
	return GetTickCount() - sessionTime>CLEAR_SESSION_IDLE;
}

char* MsgProcess::ParseDBName(const void* msgData, UINT dataLen)
{
	ASSERT(dataLen > sizeof(UINT16));
	if (dataLen <= sizeof(UINT16))
		return NULL;
	return (char*)msgData + sizeof(UINT16);
}

INT MsgProcess::ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows/* = 0*/, UINT32 fields/* = 0*/)
{
	char strFetchRow[MAX_PACKET] = { 0 };
	SDBSResultRes* pHead = (SDBSResultRes*)strFetchRow;
	pHead->fields = fields;
	pHead->rows = rows;
	pHead->retCode = error;
	pHead->hton();
	return sessionSit->OnSubmitCmdPacket(strFetchRow, sizeof(SDBSResultRes) - sizeof(char), true);
}

MsgProcess& GetMsgProObj()
{
	BEGIN_LOCAL_SAFE_STATIC_OBJ(MsgProcess, msgPro);
	END_LOCAL_SAFE_STATIC_OBJ(msgPro);
}
