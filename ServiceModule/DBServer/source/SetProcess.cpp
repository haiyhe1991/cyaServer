#include "SetProcess.h"

SetProcess::SetProcess()
{
}

SetProcess::~SetProcess()
{
}

/*	Req: Table Name + KeyValue + [Field + Value]...*/
/// 修改一条记录的一个字段, 返回修改记录的数目
INT SetProcess::MemSet(DBTcpSession* mySession, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != mySession && NULL != msgData && 0 < dataLen);
	char* strTable = ParseTableName(msgData, dataLen);
	char* strKey = ParseKeyValue(msgData, dataLen);
	KeyLine* pLine = new KeyLine(strKey);
	if (NULL == pLine)
	{
		return ResponseMsg(mySession, 1, 0, 0);
	}

	UINT16 filed_num = ParseReqFileds(msgData, dataLen, pLine);
	MemTable* table_val = GetTableExisted(strTable);
	if (NULL == strTable || NULL == strKey || 0 >= filed_num || NULL == table_val)
	{
		pLine->ReleaseRef();
		return ResponseMsg(mySession, DBS_ERR_PARAM);
	}

	KeyLine* key_val = table_val->GetLine(strKey);
	if (NULL == key_val)
	{
		pLine->ReleaseRef();
		return ResponseMsg(mySession, DBS_ERR_PARAM);
	}

	const std::vector<FieldValue*>& pValues = pLine->GetFileds();
	std::vector<FieldValue*>::const_iterator itr = pValues.begin();
	for (itr; pValues.end() != itr; ++itr)
	{
		UINT16 valLen = 0;
		const char* valval = (*itr)->GetFiledValue(valLen);
		key_val->SetFiledValue((*itr)->GetFiledName(), valval, valLen - 1);
	}

	pLine->ReleaseRef();
	return ResponseMsg(mySession, DBS_OK, 1, 0);
}

INT SetProcess::ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows/* = 0*/, UINT32 fields/* = 0*/)
{
	char strFetchRow[MAX_PACKET] = { 0 };
	SDBSResultRes* pHead = (SDBSResultRes*)strFetchRow;
	pHead->fields = fields;
	pHead->rows = rows;
	pHead->retCode = error;
	pHead->hton();
	return sessionSit->OnSubmitCmdPacket(strFetchRow, sizeof(SDBSResultRes) - sizeof(char), true);
}

/// msgData: CmdType + NameLen + Name
char* SetProcess::ParseTableName(const void* msgData, UINT dataLen)
{
	if (dataLen <= sizeof(UINT16))
		return NULL;
	return (char*)msgData + sizeof(UINT16);
}

//msgData: CmdType + TableNameLen + TableName + KeyValueLen + KeyValue
char* SetProcess::ParseKeyValue(const void* msgData, UINT dataLen)
{
	char* TableName = ParseTableName(msgData, dataLen);
	if (NULL == TableName)
		return NULL;
	UINT16 TableNameLen = strlen(TableName) + sizeof(char);
	if (TableNameLen + sizeof(UINT16) * 2 >= dataLen)
		return NULL;
	return TableName + TableNameLen + sizeof(UINT16);
}

//msgData: CmdType + TableNameLen + TableName + KeyValueLen + KeyValue + [FieldNameLen + FieldName + FieldValueLen + FieldValue]
UINT16 SetProcess::ParseReqFileds(const void* msgData, UINT dataLen, KeyLine* pLine)
{
	ASSERT(NULL != msgData && 0 < dataLen && NULL != pLine);
	if (NULL == msgData || 0 >= dataLen || NULL == pLine)
	{
		return 0;
	}
	UINT16 TableNameLen = strlen(ParseTableName(msgData, dataLen)) + sizeof(char);
	char* KeyValue = ParseKeyValue(msgData, dataLen);
	UINT16 KeyValueLen = strlen(KeyValue) + sizeof(char);
	UINT16 wOffset = TableNameLen + KeyValueLen + sizeof(UINT);
	UINT16 field_num = 0;
	while (wOffset < dataLen)
	{
		char nameBuf[128] = { 0 };
		UINT16 nameLen = ntohs(*(UINT16*)((char*)msgData + wOffset));
		wOffset += sizeof(UINT16);
		memcpy(nameBuf, (char*)msgData + wOffset, nameLen);
		nameBuf[nameLen] = '\0';
		wOffset += nameLen;

		char valueBuf[MAX_PACKET] = { 0 };
		UINT16 valLen = ntohs(*(UINT16*)((char*)msgData + wOffset));
		wOffset += sizeof(UINT16);
		memcpy(valueBuf, (char*)msgData + wOffset, valLen);
		valueBuf[valLen] = '\0';
		wOffset += valLen;

		FieldValue* pFiled = new FieldValue(nameBuf, valueBuf, valLen);
		if (NULL != pFiled)
		{
			pLine->AddFiled(pFiled);
			field_num++;
		}
	}

	return field_num;
}

MemTable* SetProcess::GetTableExisted(const std::string& strTableName)
{
	SQLTable* redis_table = SQLTable::FetchTables();
	if (NULL == redis_table)
		return NULL;

	return redis_table->FetchTable(strTableName);
}
