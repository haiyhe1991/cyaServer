#include "DBSHelper.h"
#include "cyaSock.h"
#include "cyaIpCvt.h"
#include "DBGlobalObjMan.h"
#include "cyaMaxMin.h"
#include "DBSResult.h"

#define MAX_PACKET_LEN			64*1024
#define MAX_DBS_PACKET_LEN		16*1024
#define MAX_NO_HEARTBEAT_TIMES	3

DBSHelper::DBSHelper(const char* dbsIp,
					int dbsPort,
					unsigned int connTimeout/* = 5000*/,
					unsigned int heartbeatInterval /*= 10*1000*/,
					unsigned int writeTimeout/* = 0*/,
					unsigned int readTimeout/* = 0*/)
		: m_dbsIp(dbsIp)
		, m_dbsPort(dbsPort)
		, m_connTimeout(connTimeout)
		, m_writeTimeout(writeTimeout)
		, m_readTimeout(readTimeout)
		, m_heartbeatInterval(heartbeatInterval)
		, m_seq(0)
		, m_recvHeartTick(0)
		, m_localSession(NULL)
		, m_reconnect(false)
		, m_heartbeatData(NULL)
		, m_heartbeatDataLen(0)
		, m_reconnectTimer(ReconnectTimer, this, 5 * 1000)
{

}

DBSHelper::~DBSHelper()
{
	if (m_heartbeatData != NULL)
	{
		free(m_heartbeatData);
		m_heartbeatData = NULL;
	}
}

int  DBSHelper::Connect()
{
	SOCKET sock = TCPNewClientByTime(GetDWordIP(m_dbsIp.c_str()), m_dbsPort, m_connTimeout);
	if (!SockValid(sock))
		return DBC_CONNECT_TIMEOUT;

	{
		CXTAutoLock lock(m_locker);
		GetDBGlobalObjMan()->GetDBSSessionMan()->TrustSession(sock, this, true);
		if (m_localSession == NULL)
			return DBC_SERVER_BROKEN;
		m_reconnect = true;
	}

	m_recvHeartTick = GetTickCount();
	//m_heartbeatTimer.Start();
	return DBS_OK;
}

void DBSHelper::DisConnect()
{
	if (m_reconnectTimer.IsStarted())
		m_reconnectTimer.Stop();

	if (m_heartbeatTimer.IsStarted())
		m_heartbeatTimer.Stop();

	if (m_localSession == NULL)
		return;

	m_reconnect = false;
	CXTAutoLock lock(m_locker);
	if (m_localSession != NULL)
	{
		GetDBGlobalObjMan()->GetDBSSessionMan()->CloseSession(m_localSession, true);
		m_localSession = NULL;
	}
}

int DBSHelper::ExcuteSQL(DBS_MSG_TYPE cmdType, const char* dbName, const char* sql, DBS_RESULT& result, int sqlLen /*= -1*/)
{
	result = NULL;
	if (m_localSession == NULL)
		return DBC_SERVER_BROKEN;

	if (sql == NULL || dbName == NULL)
		return DBC_INVALID_PARAM;

	int nDBName = (int)strlen(dbName);
	int nSql = sqlLen > 0 ? sqlLen : (int)strlen(sql);
	if (nDBName <= 0 || nSql <= 0 || nSql >= 0xFFFF)
		return DBC_INVALID_PARAM;

	int nTotalLen = sizeof(UINT16) + sizeof(UINT16) + nDBName + 1 + sizeof(UINT16) + nSql + 1;
	BYTE* p = NULL;
	BYTE buf[MAX_PACKET_LEN] = { 0 };
	if (nTotalLen > MAX_PACKET_LEN)
		p = (BYTE*)CyaTcp_Alloc(nTotalLen);
	else
		p = buf;
	ASSERT(p != NULL);

	//cmd
	int offset = 0;
	*(UINT16*)(p + offset) = htons((UINT16)cmdType);
	offset += sizeof(UINT16);

	//dbName
	offset += OnFillData(p, offset, dbName, nDBName);

	//sql
	offset += OnFillData(p, offset, sql, nSql);

	return OnExcuteCmd(p, nTotalLen, nTotalLen > MAX_PACKET_LEN ? true : false, result);
}

int  DBSHelper::Load(const char* dbName, const char* tableName, const char* sql, DBS_RESULT& result, int sqlLen/* = -1*/)
{
	result = NULL;
	if (m_localSession == NULL)
		return DBC_SERVER_BROKEN;

	if (sql == NULL || tableName == NULL || dbName == NULL)
		return DBC_INVALID_PARAM;

	int nDBName = (int)strlen(dbName);
	int nTableName = (int)strlen(tableName);
	int nSql = sqlLen > 0 ? sqlLen : (int)strlen(sql);

	if (nDBName <= 0 || nSql <= 0 || nTableName <= 0 || nSql >= 0xFFFF)
		return DBC_INVALID_PARAM;

	int nTotalLen = sizeof(UINT16) + sizeof(UINT16) + nDBName + 1 + sizeof(UINT16) + nTableName + 1 + sizeof(UINT16) + nSql + 1;
	BYTE* p = NULL;
	BYTE buf[MAX_PACKET_LEN] = { 0 };
	if (nTotalLen > MAX_PACKET_LEN)
		p = (BYTE*)CyaTcp_Alloc(nTotalLen);
	else
		p = buf;
	ASSERT(p != NULL);

	//cmd
	int offset = 0;
	*(UINT16*)(p + offset) = htons((UINT16)DBS_MSG_LOAD);
	offset += sizeof(UINT16);

	//dbName
	offset += OnFillData(p, offset, dbName, nDBName);

	//tableName
	offset += OnFillData(p, offset, tableName, nTableName);

	//sql
	offset += OnFillData(p, offset, sql, nSql);

	return OnExcuteCmd(p, nTotalLen, nTotalLen > MAX_PACKET_LEN ? true : false, result);
}

int  DBSHelper::Get(const char* dbName, const char* tableName, const SDBSValue* pValsArray, int arrCount, DBS_RESULT& result)
{
	result = NULL;
	if (m_localSession == NULL)
		return DBC_SERVER_BROKEN;

	if (dbName == NULL || tableName == NULL || pValsArray == NULL || arrCount <= 0)
		return DBC_INVALID_PARAM;

	int nDBName = (int)strlen(dbName);
	int nTableName = (int)strlen(tableName);
	if (nTableName <= 0)
		return DBC_INVALID_PARAM;

	int nTotalLen = 0;
	int nKeyLen = 0;
	for (int i = 0; i < arrCount; ++i)
		nKeyLen += sizeof(UINT16) + pValsArray[i].len + 1;

	if (nKeyLen <= 0)
		return DBC_INVALID_PARAM;

	nTotalLen += sizeof(UINT16) + sizeof(UINT16) + nDBName + 1 + sizeof(UINT16) + nTableName + 1 + nKeyLen;
	BYTE* p = NULL;
	BYTE buf[MAX_PACKET_LEN] = { 0 };
	if (nTotalLen > MAX_PACKET_LEN)
		p = (BYTE*)CyaTcp_Alloc(nTotalLen);
	else
		p = buf;
	ASSERT(p != NULL);

	//cmd
	int offset = 0;
	*(UINT16*)(p + offset) = htons((UINT16)DBS_MSG_GET);
	offset += sizeof(UINT16);

	//dbName
	offset += OnFillData(p, offset, dbName, nDBName);

	//tableName
	offset += OnFillData(p, offset, tableName, nTableName);

	//val
	for (int i = 0; i < arrCount; ++i)
		offset += OnFillData(p, offset, pValsArray[i].pValue, pValsArray[i].len);

	return OnExcuteCmd(p, nTotalLen, nTotalLen > MAX_PACKET_LEN ? true : false, result);
}

int  DBSHelper::Set(const char* dbName, const char* tableName, const char* keyFieldValue, const SDBSKeyValue* pKeyValueArray, int arrCount, DBS_RESULT& result)
{
	result = NULL;
	if (m_localSession == NULL)
		return DBC_SERVER_BROKEN;

	if (dbName == NULL || tableName == NULL || keyFieldValue == NULL || pKeyValueArray == NULL || arrCount <= 0)
		return DBC_INVALID_PARAM;

	int nTableName = (int)strlen(tableName);
	int nDBName = (int)strlen(dbName);
	int nKeyFieldValue = (int)strlen(keyFieldValue);
	if (nTableName <= 0 || nKeyFieldValue <= 0 || nKeyFieldValue >= 0xFFFF)
		return DBC_INVALID_PARAM;

	int nFieldValues = 0;
	for (int i = 0; i < arrCount; ++i)
		nFieldValues += sizeof(UINT16) + pKeyValueArray[i].key.len + 1 + sizeof(UINT16) + pKeyValueArray[i].val.len + 1;

	if (nFieldValues <= 0)
		return DBC_INVALID_PARAM;

	int nTotalLen = sizeof(UINT16) +
		sizeof(UINT16) + nDBName + 1 +           //hhy改以前是nTableName
		sizeof(UINT16) + nTableName + 1 +
		sizeof(UINT16) + nKeyFieldValue + 1 +
		nFieldValues;
	BYTE* p = NULL;
	BYTE buf[MAX_PACKET_LEN] = { 0 };
	if (nTotalLen > MAX_PACKET_LEN)
		p = (BYTE*)CyaTcp_Alloc(nTotalLen);
	else
		p = buf;
	ASSERT(p != NULL);

	//cmd
	int offset = 0;
	*(UINT16*)(p + offset) = htons((UINT16)DBS_MSG_SET);
	offset += sizeof(UINT16);

	//dbName
	offset += OnFillData(p, offset, dbName, nDBName);

	//tableName
	offset += OnFillData(p, offset, tableName, nTableName);

	//keyFieldValue
	offset += OnFillData(p, offset, keyFieldValue, nKeyFieldValue);

	//keyVal
	for (int i = 0; i < arrCount; ++i)
	{
		offset += OnFillData(p, offset, pKeyValueArray[i].key.pKey, pKeyValueArray[i].key.len);
		offset += OnFillData(p, offset, pKeyValueArray[i].val.pValue, pKeyValueArray[i].val.len);
	}

	return OnExcuteCmd(p, nTotalLen, nTotalLen > MAX_PACKET_LEN ? true : false, result);
}

int  DBSHelper::Append(const char* dbName, const char* tableName, const SDBSKeyValue* pKeyValueArray, int arrCount, DBS_RESULT& result)
{
	result = NULL;
	if (m_localSession == NULL)
		return DBC_SERVER_BROKEN;

	if (tableName == NULL || pKeyValueArray == NULL)
		return DBC_INVALID_PARAM;

	int nTableName = (int)strlen(tableName);
	int nDBName = (int)strlen(dbName);
	if (nTableName <= 0)
		return DBC_INVALID_PARAM;

	int nFieldValues = 0;
	for (int i = 0; i < arrCount; ++i)
		nFieldValues += sizeof(UINT16) + pKeyValueArray[i].key.len + 1 + sizeof(UINT16) + pKeyValueArray[i].val.len + 1;


	if (nFieldValues <= 0)
		return DBC_INVALID_PARAM;

	int nTotalLen = sizeof(UINT16) + sizeof(UINT16) + nDBName + 1 + sizeof(UINT16) + nTableName + 1 + nFieldValues;
	BYTE* p = NULL;
	BYTE buf[MAX_PACKET_LEN] = { 0 };
	if (nTotalLen > MAX_PACKET_LEN)
		p = (BYTE*)CyaTcp_Alloc(nTotalLen);
	else
		p = buf;
	ASSERT(p != NULL);

	//cmd
	int offset = 0;
	*(UINT16*)(p + offset) = htons((UINT16)DBS_MSG_APPEND);
	offset += sizeof(UINT16);

	//dbName
	offset += OnFillData(p, offset, dbName, nDBName);

	//tableName
	offset += OnFillData(p, offset, tableName, nTableName);

	//keyVal
	for (int i = 0; i < arrCount; ++i)
	{
		offset += OnFillData(p, offset, pKeyValueArray[i].key.pKey, pKeyValueArray[i].key.len);
		offset += OnFillData(p, offset, pKeyValueArray[i].val.pValue, pKeyValueArray[i].val.len);
	}
	return OnExcuteCmd(p, nTotalLen, nTotalLen > MAX_PACKET_LEN ? true : false, result);
}

int  DBSHelper::Remove(const char* dbName, const char* tableName, const SDBSKeyValue* pKeyVal, DBS_RESULT& result)
{
	result = NULL;
	if (m_localSession == NULL)
		return DBC_SERVER_BROKEN;

	if (dbName == NULL || tableName == NULL || pKeyVal == NULL)
		return DBC_INVALID_PARAM;

	int nTableName = (int)strlen(tableName);
	int nDBName = (int)strlen(dbName);
	if (nTableName <= 0)
		return DBC_INVALID_PARAM;
	int nKeyVal = sizeof(UINT16) + pKeyVal->key.len + 1 + sizeof(UINT16) + pKeyVal->val.len + 1;
	if (nKeyVal <= 0)
		return DBC_INVALID_PARAM;

	BYTE* p = NULL;
	int nTotalLen = sizeof(UINT16) + sizeof(UINT16) + nDBName + 1 + sizeof(UINT16) + nTableName + 1 + nKeyVal;
	BYTE buf[MAX_PACKET_LEN] = { 0 };
	if (nTotalLen > MAX_PACKET_LEN)
		p = (BYTE*)CyaTcp_Alloc(nTotalLen);
	else
		p = buf;
	ASSERT(p != NULL);

	//cmd
	int offset = 0;
	*(UINT16*)(p + offset) = htons((UINT16)DBS_MSG_REMOVE);
	offset += sizeof(UINT16);

	//dbName
	offset += OnFillData(p, offset, dbName, nDBName);

	//tableName
	offset += OnFillData(p, offset, tableName, nTableName);

	//keyVal
	offset += OnFillData(p, offset, pKeyVal->key.pKey, pKeyVal->key.len);
	offset += OnFillData(p, offset, pKeyVal->val.pValue, pKeyVal->val.len);

	return OnExcuteCmd(p, nTotalLen, nTotalLen > MAX_PACKET_LEN ? true : false, result);
}

int  DBSHelper::UnLoad(const char* dbName, const char* tableName, DBS_RESULT& result)
{
	result = NULL;
	if (m_localSession == NULL)
		return DBC_SERVER_BROKEN;

	if (tableName == NULL || dbName == NULL)
		return DBC_INVALID_PARAM;

	int nTableName = (int)strlen(tableName);
	int nDBName = (int)strlen(dbName);
	if (nTableName <= 0 || nTableName >= 0xFFFF)
		return DBC_INVALID_PARAM;

	int nTotalLen = sizeof(UINT16) + sizeof(UINT16) + nDBName + 1 + sizeof(UINT16) + nTableName + 1;
	BYTE* p = NULL;
	BYTE buf[MAX_PACKET_LEN] = { 0 };
	if (nTotalLen > MAX_PACKET_LEN)
		p = (BYTE*)CyaTcp_Alloc(nTotalLen);
	else
		p = buf;
	ASSERT(p != NULL);

	//cmd
	int offset = 0;
	*(UINT16*)(p + offset) = htons((UINT16)DBS_MSG_ULOAD);
	offset += sizeof(UINT16);

	//dbName
	offset += OnFillData(p, offset, dbName, nDBName);

	//tableName
	offset += OnFillData(p, offset, tableName, nTableName);

	return OnExcuteCmd(p, nTotalLen, nTotalLen > MAX_PACKET_LEN ? true : false, result);
}

int	  DBSHelper::OnExcuteCmd(const BYTE* payload, int payloadBytes, BOOL needFree, DBS_RESULT& result)
{
	DBSHelper::DataBlock* blk = CyaTcp_New(DBSHelper::DataBlock);
	if (blk == NULL)
	{
		ASSERT(false);
		return DBC_ALLOC_FAILED;
	}
	AutoRefCountObjTmpl<DBSHelper::DataBlock> pAutoObj(blk);

	long seq = 0;
	int sendRet = 0;
	SDBSProtocolHead header;

	{
		CXTAutoLock lock(m_locker);
		if (m_localSession == NULL)
			return DBC_SERVER_BROKEN;

		seq = OnGetCmdSeq();
		BOOL bRet = false;
		bRet = m_dataBlocks.insert(std::make_pair(seq, blk)).second;
		ASSERT(bRet);

		if (payloadBytes > MAX_DBS_PACKET_LEN/*MAX_PACKET_LEN*/)
		{
			int offset = 0;
			while (payloadBytes - offset > 0)
			{
				SockBufVec bufs[2];
				int pduLen = min(payloadBytes - offset, MAX_DBS_PACKET_LEN/*MAX_PACKET_LEN*/);
				BOOL isLastPacket = (payloadBytes - (offset + pduLen)) > 0 ? false : true;
				bufs[0].buf = &header;
				bufs[0].bufBytes = sizeof(SDBSProtocolHead);
				bufs[1].buf = (void*)(payload + offset);
				bufs[1].bufBytes = pduLen;
				OnMakeDBSHeader(&header, (UINT16)pduLen, seq, isLastPacket);
				sendRet = GetDBGlobalObjMan()->GetDBSSessionMan()->SendSessionDataVec(m_localSession, bufs, 2);
				if (sendRet != 0)
					break;
				offset += pduLen;
			}
		}
		else
		{
			SockBufVec bufs[2];
			OnMakeDBSHeader(&header, (UINT16)payloadBytes, seq, true);
			bufs[0].buf = &header;
			bufs[0].bufBytes = sizeof(SDBSProtocolHead);
			bufs[1].buf = (void*)payload;
			bufs[1].bufBytes = payloadBytes;
			sendRet = GetDBGlobalObjMan()->GetDBSSessionMan()->SendSessionDataVec(m_localSession, bufs, 2);
		}

		if (sendRet != 0)
			m_dataBlocks.erase(seq);
	}

	if (needFree)
		CyaTcp_Free((void*)payload);

	if (sendRet != 0)
		return DBC_SERVER_BROKEN;

	BOOL ret = pAutoObj->Wait(m_readTimeout > 0 ? m_readTimeout : INFINITE);

	{
		CXTAutoLock lock(m_locker);
		m_dataBlocks.erase(seq);
	}

	if (!ret || m_localSession == NULL)
		return m_readTimeout > 0 ? DBC_RECV_TIMEOUT : DBC_SERVER_BROKEN;

	if (NULL == pAutoObj->DataPtr())
		return DBC_SERVER_BROKEN;

	UINT16 retCode = ntohs((*(UINT16*)pAutoObj->DataPtr()));
	if (retCode != DBS_OK)
		return retCode;

	void* p = pAutoObj->Drop();	//放弃对_data的释放操作
	result = CyaTcp_NewEx(DBSResult, (p, pAutoObj->Length()));
	if (result == NULL)
		return DBC_ALLOC_FAILED;
	return DBS_OK;
}

int   DBSHelper::OnFillData(BYTE* pDest, int offset, const void* pSrc, int srcLen)
{
	int n = offset;

	//2字节pSrc长度
	*(UINT16*)(pDest + n) = htons((UINT16)(srcLen + 1));
	n += sizeof(UINT16);

	//pSrc数据
	if (pSrc != NULL && srcLen > 0)
		memcpy(pDest + n, pSrc, srcLen);
	n += srcLen;
	pDest[n] = '\0';
	n += 1;

	return n - offset;
}

void DBSHelper::SetLocalSession(DBSSession* session)
{
	m_localSession = session;
}

void DBSHelper::NotifySessionBroken()
{
	{
		CXTAutoLock lock(m_locker);
		m_localSession = NULL;
		DataBlockMap::iterator it = m_dataBlocks.begin();
		for (; it != m_dataBlocks.end(); ++it)
			it->second->PostSema();
	}

	if (m_reconnect)
	{
		if (!m_reconnectTimer.IsStarted())
			m_reconnectTimer.Start();
	}
	else
		delete this;
}

long DBSHelper::OnGetCmdSeq()
{
	InterlockedIncrement(&m_seq);
	if (m_seq <= 0)
		m_seq = 1;
	return m_seq;
}

void DBSHelper::InputResponseData(unsigned int seq, const void* pData, int dataBytes, BOOL isOver)
{
	if (pData == NULL || dataBytes <= 0)
		return;

	if (seq == 0)
	{
		m_recvHeartTick = GetTickCount();
		CyaTcp_Free((void*)pData);
		return;
	}

	DBSHelper::DataBlock* blk = NULL;

	{
		CXTAutoLock lock(m_locker);
		DataBlockMap::iterator it = m_dataBlocks.find(seq);
		if (it != m_dataBlocks.end())
		{
			blk = it->second;
			blk->AddRef();
		}
	}

	if (blk == NULL)
	{
		CyaTcp_Free((void*)pData);
		ASSERT(false);
		return;
	}

	AutoRefCountObjTmpl<DBSHelper::DataBlock> pAutoObj(blk);
	pAutoObj->Input(pData, dataBytes, isOver);
}

void  DBSHelper::OnMakeDBSHeader(SDBSProtocolHead* pHeader, UINT16 nPayLoadBytes, DWORD seq, BOOL isOver)
{
	pHeader->id = 0x1020;
	pHeader->pktLen = nPayLoadBytes;
	pHeader->pktOver = isOver ? 1 : 0;
	pHeader->seq = seq;
	pHeader->ntoh();
}

void DBSHelper::ReconnectTimer(void* param, TTimerID id)
{
	DBSHelper* pThis = (DBSHelper*)param;
	ASSERT(pThis != NULL);
	pThis->OnReconnect();
}

void DBSHelper::OnReconnect()
{
	int r = Connect();
	if (r == 0)
		m_reconnectTimer.Stop(false);
}

void DBSHelper::HeartbeatTimer(void* param, TTimerID id)
{
	DBSHelper* pThis = (DBSHelper*)param;
	ASSERT(pThis != NULL);
	pThis->OnHeartbeat();
}

void DBSHelper::OnHeartbeat()
{
	if (m_localSession == NULL)
	{
		m_heartbeatTimer.Stop(false);
		return;
	}

	DWORD tickNow = GetTickCount();
	int tickInterval = tickNow - m_recvHeartTick;
	if (tickInterval < 0)
	{
		m_recvHeartTick = tickNow;
		return;
	}

	if (tickInterval > m_heartbeatInterval*MAX_NO_HEARTBEAT_TIMES)	//broken ???
	{
		{
			CXTAutoLock lock(m_locker);
			m_reconnect = true;
			if (m_localSession != NULL)
			{
				GetDBGlobalObjMan()->GetDBSSessionMan()->CloseSession(m_localSession, true);
				m_localSession = NULL;
			}
		}

		m_heartbeatTimer.Stop(false);
		return;
	}

	CXTAutoLock lock(m_locker);
	if (m_localSession != NULL)
		GetDBGlobalObjMan()->GetDBSSessionMan()->SendSessionData(m_localSession, m_heartbeatData, m_heartbeatDataLen, false);
}
