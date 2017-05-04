#include "UnloadProcess.h"
#include "SyncHelper.h"

UnloadProcess::UnloadProcess()
{

}

UnloadProcess::~UnloadProcess()
{

}

char* UnloadProcess::ParseTableName(const void* msgData, UINT dataLen)
{
	if (dataLen <= sizeof(UINT16))
		return NULL;
	return (char*)msgData + sizeof(UINT16);
}

INT UnloadProcess::ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows/* = 0*/, UINT32 fields/* = 0*/)
{
	char strFetchRow[MAX_PACKET] = { 0 };
	SDBSResultRes* pHead = (SDBSResultRes*)strFetchRow;
	pHead->fields = fields;
	pHead->rows = rows;
	pHead->retCode = error;
	pHead->hton();
	return sessionSit->OnSubmitCmdPacket(strFetchRow, sizeof(SDBSResultRes) - sizeof(char), true);
}

/*Req-- tableName*/
INT UnloadProcess::MemUnLoad(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	char* strTable = ParseTableName(msgData, dataLen);
	if (NULL == strTable)
		return ResponseMsg(mySession, DBS_ERR_PARAM);
	MemTable* theTable = GetTableExisted(strTable);
	if (NULL == theTable)
		return ResponseMsg(mySession, DBS_ERR_TABLE);

	if (0 < SyncHelper::FetchSyncHelper()->EnforceSync(theTable, sqlSession))
		return ResponseMsg(mySession, DBS_ERROR);
	SQLTable::FetchTables()->DeleteTable(strTable);

	return ResponseMsg(mySession, DBS_OK);
}

MemTable* UnloadProcess::GetTableExisted(const std::string& strTableName)
{
	SQLTable* redis_table = SQLTable::FetchTables();
	if (NULL == redis_table)
		return NULL;

	return redis_table->FetchTable(strTableName);
}
