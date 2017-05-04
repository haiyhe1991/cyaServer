#include "RemoveProcess.h"

RemoveProcess::RemoveProcess()
{

}

RemoveProcess::~RemoveProcess()
{

}

MemTable* RemoveProcess::GetTableExisted(const std::string& strTableName)
{
	SQLTable* redis_table = SQLTable::FetchTables();
	if (NULL == redis_table)
		return NULL;

	return redis_table->FetchTable(strTableName);
}

char* RemoveProcess::ParseTableName(const void* msgData, UINT dataLen)
{
	if (dataLen <= sizeof(UINT16))
		return NULL;
	return (char*)msgData + sizeof(UINT16);
}

char* RemoveProcess::ParseKeyValue(const void* msgData, UINT dataLen)
{
	char* KeyName = ParseKeyName(msgData, dataLen);
	UINT16 KeyNameLen = strlen(KeyName) + sizeof(char);
	return KeyName + KeyNameLen + sizeof(UINT16);
}

char* RemoveProcess::ParseKeyName(const void* msgData, UINT dataLen)
{
	char* TableName = ParseTableName(msgData, dataLen);
	UINT16 TableNameLen = strlen(TableName) + sizeof(char);
	if (TableNameLen + sizeof(UINT16) * 3 > dataLen) //UINT16+TableName + UINT16+KeyName + UINT16+KeyValue
		return NULL;
	return TableName + TableNameLen + sizeof(UINT16);
}

INT RemoveProcess::ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows/* = 0*/, UINT32 fields/* = 0*/)
{
	char strFetchRow[MAX_PACKET] = { 0 };
	SDBSResultRes* pHead = (SDBSResultRes*)strFetchRow;
	pHead->fields = fields;
	pHead->rows = rows;
	pHead->retCode = error;
	pHead->hton();
	return sessionSit->OnSubmitCmdPacket(strFetchRow, sizeof(SDBSResultRes) - sizeof(char), true);
}

/* Req: TableName + KeyName + KeyValue */	/// 都是字符串吧
/// 只删除一条记录
INT RemoveProcess::MemRemove(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != mySession && NULL != sqlSession && NULL != msgData && 0 < dataLen);
	char* strTable = ParseTableName(msgData, dataLen);
	char* strKeyName = ParseKeyName(msgData, dataLen);
	char* strKeyValue = ParseKeyValue(msgData, dataLen);
	ASSERT(NULL != strTable && NULL != strKeyValue && NULL != strKeyName);

	MemTable* table_val = GetTableExisted(strTable);
	if (NULL == table_val)
	{
		return ResponseMsg(mySession, DBS_ERR_PARAM);
	}

	KeyLine* key_val = table_val->GetLine(strKeyValue);
	if (NULL == key_val)
	{
		return ResponseMsg(mySession, DBS_OK);
	}

	char tmpSqlBuf[256] = { 0 };
	DBS_PRINTF(tmpSqlBuf, sizeof(tmpSqlBuf), "delete from %s where %s=%s", strTable, strKeyName, strKeyValue);
	if (0 < sqlSession->DeleteRow(tmpSqlBuf, strlen(tmpSqlBuf)))
	{
		table_val->DelKey(strKeyValue);
		return ResponseMsg(mySession, DBS_OK, 1, 0);
	}

	return ResponseMsg(mySession, DBS_ERR_SQL, 0, 0);
}