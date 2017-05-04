#include "AppendProcess.h"

AppendProcess::AppendProcess()
{

}

AppendProcess::~AppendProcess()
{

}

/* 	Req: Table Name + [字段名 + 字段值 ]*/	/// 字段值由客户端添加 ''
INT AppendProcess::MemAppend(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	char* strTable = ParseTableName(msgData, dataLen);
	KeyLine* pLine = new KeyLine();
	if (NULL == pLine)
	{
		return ResponseMsg(mySession, DBS_ERROR);
	}

	UINT16 tableNameLen = strlen(strTable) + sizeof(char);
	UINT16 filed_num = ParseReqFields(strTable + tableNameLen, dataLen - tableNameLen - sizeof(UINT16), pLine);
	if (NULL == strTable || filed_num <= 0)
	{
		pLine->ReleaseRef();
		return ResponseMsg(mySession, DBS_ERR_PARAM);
	}
	MemTable* table_val = GetTableExisted(strTable);
	if (NULL == table_val)
	{
		pLine->ReleaseRef();
		return ResponseMsg(mySession, DBS_ERR_TABLE);
	}

	std::map<std::string, INT> fileds;
	std::string keyName(table_val->GetAllField(fileds));
	if (keyName.empty())
	{
		pLine->ReleaseRef();
		return ResponseMsg(mySession, DBS_ERR_KEY);
	}

	char SqlCmd[MAX_PACKET] = { 0 };
	UINT16 offset = ComposeInsertCmd(pLine, SqlCmd, keyName, fileds, strTable);

	DWORD id;
	if (0 < sqlSession->InsertRow(SqlCmd, offset, id))
	{
		DBS_PRINTF(SqlCmd, sizeof(SqlCmd), "%ul", id);
		pLine->SetKeyName(SqlCmd);	/// ?
		table_val->AddLine(pLine);
		return ResponseAppendMsg(mySession, keyName.c_str(), DBS_OK, 1, 1, id);
	}

	pLine->ReleaseRef();
	return ResponseMsg(mySession, DBS_ERR_SQL);
}

char* AppendProcess::ParseTableName(const void* msgData, UINT dataLen)
{
	ASSERT(NULL != msgData && sizeof(UINT16) < dataLen);
	if (dataLen <= sizeof(UINT16))
		return NULL;
	return (char*)msgData + sizeof(UINT16);
}

//字段值和字段名均是：{16位：字段长度+字段内容}
UINT16 AppendProcess::ParseReqFields(const void* msgData, UINT dataLen, KeyLine* pLine)
{
	ASSERT(NULL != msgData && dataLen > sizeof(UINT16) && NULL != pLine);
	UINT16 wOffset = 0;
	UINT16 field_num = 0;
	while (wOffset < dataLen)
	{
		char nameBuf[128] = { 0 };
		UINT16 nameLen = ntohs(*(UINT16*)((char*)msgData + wOffset));
		wOffset += sizeof(UINT16);
		memcpy(nameBuf, (char*)msgData + wOffset, nameLen);
		wOffset += nameLen;

		char valueBuf[MAX_PACKET] = { 0 };
		UINT16 valLen = ntohs(*(UINT16*)((char*)msgData + wOffset));
		wOffset += sizeof(UINT16);
		memcpy(valueBuf, (char*)msgData + wOffset, valLen);
		wOffset += valLen;

		FieldValue* pFiled = new FieldValue(nameBuf, valueBuf, valLen - sizeof(char));
		if (NULL != pFiled)
		{
			pLine->AddFiled(pFiled);
			field_num++;
		}
	}

	return field_num;
}

MemTable* AppendProcess::GetTableExisted(const std::string& strTableName)
{
	SQLTable* redis_table = SQLTable::FetchTables();
	if (NULL == redis_table)
		return NULL;

	return redis_table->FetchTable(strTableName);
}

INT AppendProcess::ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows/* = 0*/, UINT32 fields/* = 0*/)
{
	char strFetchRow[MAX_PACKET] = { 0 };
	SDBSResultRes* pHead = (SDBSResultRes*)strFetchRow;
	pHead->fields = fields;
	pHead->rows = rows;
	pHead->retCode = error;
	pHead->hton();
	return sessionSit->OnSubmitCmdPacket(strFetchRow, sizeof(SDBSResultRes) - sizeof(char), true);
}

INT AppendProcess::ResponseAppendMsg(DBTcpSession* sessionSit, const char* keyName, UINT16 error, UINT32 rows, UINT32 fields, UINT32 dwID)
{
	char strFetchRow[MAX_PACKET] = { 0 };
	SDBSResultRes* pHead = (SDBSResultRes*)strFetchRow;
	pHead->fields = fields;
	pHead->rows = rows;
	pHead->retCode = error;
	pHead->hton();
	UINT16 offset = sizeof(SDBSResultRes) - sizeof(char);	/// 关键字长度+关键字名+Id长度+ID
	if (0 != dwID)
	{
		char strID[32] = { 0 };
		DBS_PRINTF(strID, sizeof(strID), "%u", dwID);	/// KeyID
		UINT16 key_len = strlen(keyName);
		UINT16* linLen = (UINT16*)(strFetchRow + offset);	///Line长度
		*linLen = htons(key_len + sizeof(char));
		UINT16 id_len = strlen(strID);
		offset += sizeof(UINT16);
		memcpy(strFetchRow + offset, keyName, key_len);
		strFetchRow[offset + key_len] = '\0';
		offset += key_len + sizeof(char);
		UINT16* idLen = (UINT16*)(strFetchRow + offset);
		*idLen = htons(id_len + sizeof(char));
		offset += sizeof(UINT16);
		memcpy(strFetchRow + offset, strID, id_len);
		strFetchRow[offset + id_len] = '\0';
		offset += id_len + 1;
	}

	sessionSit->OnSubmitCmdPacket(strFetchRow, offset, true);
	return 0;
}

bool field_valid(const std::map<std::string, INT>& fileds, const char* fieldName)
{
	for (UINT16 i = 0; i < fileds.size(); ++i)
	{
		std::map<std::string, INT>::const_iterator itr = fileds.find(fieldName);
		if (fileds.end() != itr)
		{
			return true;
		}
	}
	return false;
}

UINT16 AppendProcess::ComposeInsertCmd(KeyLine* pLine, char* SqlCmd, const std::string& keyName,
	const std::map<std::string, INT>& fileds, char* strTable)
{
	char filed_buf[1024] = { 0 };
	DBS_PRINTF(filed_buf, sizeof(filed_buf), "insert into %s (", strTable);
	UINT16 filed_offset = strlen(filed_buf);
	char value_buf[4096] = { 0 };
	DBS_PRINTF(value_buf, sizeof(value_buf), "%s", ") values(");
	UINT16 value_offset = strlen(value_buf);

	const std::vector<FieldValue*>& pFieldValues = pLine->GetFileds();
	std::vector<FieldValue*>::const_iterator itr = pFieldValues.begin();
	std::vector<FieldValue*>::const_iterator itr_end = pFieldValues.end();
	--itr_end;
	for (itr; pFieldValues.end() != itr; ++itr)
	{
		if (!field_valid(fileds, (*itr)->GetFiledName()) || keyName == (*itr)->GetFiledName())
			continue;
		memcpy(filed_buf + filed_offset, (*itr)->GetFiledName(), strlen((*itr)->GetFiledName()));
		filed_offset += strlen((*itr)->GetFiledName());
		UINT16 valLen = 0;
		const char* pValVal = (*itr)->GetFiledValue(valLen);
		memcpy(value_buf + value_offset, pValVal, valLen - sizeof(char));
		value_offset += valLen - sizeof(char);
		if (itr != itr_end)
		{
			UINT16 len = strlen(", ");
			memcpy(filed_buf + filed_offset, ", ", len);
			filed_offset += len;
			memcpy(value_buf + value_offset, ", ", len);
			value_offset += len;
		}
	}
	memcpy(SqlCmd, filed_buf, filed_offset);
	memcpy(SqlCmd + filed_offset, value_buf, value_offset);
	memcpy(SqlCmd + filed_offset + value_offset, ")", strlen(")"));

	return filed_offset + value_offset + strlen(")");
}

