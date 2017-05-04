#include "GetProcess.h"

GetProcess::GetProcess()
	: m_res_len(0)
{

}

GetProcess::~GetProcess()
{

}

/// msgData : NameLen + Name
char* GetProcess::ParseTableName(const void* msgData, UINT16& dataLen)
{
	ASSERT(dataLen > sizeof(UINT16));
	if (dataLen <= sizeof(UINT16))
		return NULL;
	dataLen = ntohs(*(UINT16*)msgData);
	return (char*)msgData + sizeof(UINT16);
}

/// msgData : [KeyValueLen + KeyValue]	/// KeyValue 字符串
/// Key对应的值
UINT16 GetProcess::GetRequestKeys(const void* msgData, UINT dataLen, std::vector<std::string>& vecKeys)
{
	UINT16 wOffset = sizeof(UINT16);	/// Table之后
	while (wOffset < dataLen)	/// key之间用\0分割
	{
		char* strKey = (char*)((char*)msgData + wOffset);
		wOffset += strlen(strKey) + sizeof(char) + sizeof(UINT16);
		vecKeys.push_back(strKey);
	}
	return vecKeys.size();
}

MemTable* GetProcess::GetTableExisted(const std::string& strTableName)
{
	SQLTable* redis_table = SQLTable::FetchTables();
	if (NULL == redis_table)
		return NULL;

	return redis_table->FetchTable(strTableName);
}

void GetProcess::ResponseFieldsName(Transporter* pts, const std::vector<std::string>& reqFields, UINT16 rowNum)
{
	char name_buf[256] = { 0 };
	SDBSResultRes* pRes = (SDBSResultRes*)name_buf;
	pRes->rows = rowNum;
	pRes->fields = reqFields.size();
	pRes->retCode = ((reqFields.size() > 0) && (rowNum > 0)) ? 0 : DBS_ERR_KEY;
	pRes->hton();
	pts->Transport(name_buf, sizeof(SDBSResultRes) - sizeof(char), false);

	UINT16 nameLen = 0;
	for (UINT16 i = 0; i < reqFields.size(); ++i)
	{
		nameLen = reqFields[i].length();
		UINT16 pNameLen = htons(nameLen);
		pts->Transport((char*)(&pNameLen), sizeof(UINT16), false);
		pts->Transport(reqFields[i].c_str(), nameLen, false);
	}

}

void GetProcess::ResponseRow(Transporter* pts, const std::string& strKey, MemTable* table_val, bool Last)
{
	KeyLine* pLine = table_val->GetLine(strKey);
	if (NULL == pLine)
	{
		return;
	}

	const std::vector<FieldValue*>& LineValues = pLine->GetFileds();
	std::vector<FieldValue*>::const_iterator itr = LineValues.begin();
	std::vector<FieldValue*>::const_iterator itre = LineValues.end();
	--itre;
	for (itr; LineValues.end() != itr; ++itr)
	{
		UINT16 valLen = 0;
		const char* pVal = (*itr)->GetFiledValue(valLen);
		const char* pName = (*itr)->GetFiledName();
		INT field_type = table_val->FieldType(pName);
		/// 时间日期当字符串处理
		if (field_type == MYSQL_TYPE_VARCHAR || field_type == MYSQL_TYPE_VAR_STRING || field_type == MYSQL_TYPE_STRING ||
			field_type == MYSQL_TYPE_DATE || field_type == MYSQL_TYPE_TIME || field_type == MYSQL_TYPE_DATETIME ||
			field_type == MYSQL_TYPE_YEAR || field_type == MYSQL_TYPE_TIMESTAMP)
		{
			valLen -= sizeof(char) * 3;
			UINT16 pValLen = htons(valLen);
			pts->Transport((char*)(&pValLen), sizeof(UINT16), false);
			pts->Transport(pVal + sizeof(char), valLen, Last && itre == itr);
		}
		else
		{
			valLen -= sizeof(char);
			UINT16 pValLen = htons(valLen);

			pts->Transport((char*)(&pValLen), sizeof(UINT16), false);
			pts->Transport(pVal, valLen, Last && itre == itr);
		}
	}
}

INT GetProcess::ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows/* = 0*/, UINT32 fields/* = 0*/)
{
	char strFetchRow[MAX_PACKET] = { 0 };
	SDBSResultRes* pHead = (SDBSResultRes*)strFetchRow;
	pHead->fields = fields;
	pHead->rows = rows;
	pHead->retCode = error;
	pHead->hton();
	return sessionSit->OnSubmitCmdPacket(strFetchRow, sizeof(SDBSResultRes) - sizeof(char), true);
}

INT GetProcess::GetRequestFields(MemTable* table_val, const std::string& keyValue, std::vector<std::string>& vecFileds)
{
	ASSERT(NULL != table_val);

	KeyLine* key_val = table_val->GetLine(keyValue);
	if (NULL == key_val)
		return 0;

	const std::vector<FieldValue*> VecFiledValue = key_val->GetFileds();
	if (VecFiledValue.empty())
		return 0;

	std::vector<FieldValue*>::const_iterator itr = VecFiledValue.begin();
	for (itr; VecFiledValue.end() != itr; itr++)
	{
		vecFileds.push_back((*itr)->GetFiledName());
	}
	return vecFileds.size();
}

void GetProcess::DeleteNotExistedKeys(MemTable* table_val, std::vector<std::string>& reqKeys)
{
	ASSERT(NULL != table_val);
	std::vector<std::string> newKeys;
	for (UINT16 i = 0; i < reqKeys.size(); ++i)
	{
		if (table_val->IsExistedKey(reqKeys[i]))
		{
			newKeys.push_back(reqKeys[i]);
		}
	}
	reqKeys = newKeys;
}

/*Req--TableName + [KeyValue]...*/ /// 多条记录
INT GetProcess::MemGet(DBTcpSession* mySession, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != mySession && NULL != msgData && 0 < dataLen);
	UINT16 tableNameLen = dataLen;
	char* strTable = ParseTableName(msgData, tableNameLen);
	std::vector<std::string> reqKeys;	/// Key对应的值
	UINT16 keySize = GetRequestKeys(strTable + tableNameLen, dataLen - tableNameLen, reqKeys);
	if (0 >= keySize)
	{
		return ResponseMsg(mySession, DBS_ERR_PARAM);
	}

	MemTable* table_val = GetTableExisted(strTable);
	if (NULL == table_val)
	{
		return ResponseMsg(mySession, DBS_ERR_TABLE);
	}

	DeleteNotExistedKeys(table_val, reqKeys);
	keySize = reqKeys.size();
	if (reqKeys.empty())
	{
		return ResponseMsg(mySession, DBS_ERR_PARAM);
	}

	std::vector<std::string> reqFields;
	if (0 >= GetRequestFields(table_val, reqKeys[0], reqFields))
	{
		return ResponseMsg(mySession, 0, 0, 0);
	}

	Transporter* pts = new Transporter(mySession);
	if (NULL == pts)
	{
		return 0;
	}
	ResponseFieldsName(pts, reqFields, keySize);

	for (UINT16 i = 0; i < keySize; ++i)
	{
		ResponseRow(pts, reqKeys[i], table_val, i == keySize - 1 ? true : false);
	}
	return 0;
}

INT GetProcess::SendPacket(DBTcpSession* mySession, void* msgData, INT msgLen, bool Last)
{
	if (msgLen > MAX_PACKET)
	{
		if (0 < m_res_len)
		{
			mySession->OnSubmitCmdPacket(m_res_buf, m_res_len, false);
			m_res_len = 0;
		}
		mySession->OnSubmitCmdPacket(msgData, msgLen, Last);
		return msgLen;
	}
	if (NULL == msgData || 0 == msgLen)
	{
		if (Last)
			mySession->OnSubmitCmdPacket(m_res_buf, m_res_len, true);
		else
			return 0;
	}
	if (m_res_len + msgLen > MAX_PACKET)
	{
		mySession->OnSubmitCmdPacket(m_res_buf, m_res_len, false);
		if (Last)
		{
			mySession->OnSubmitCmdPacket(msgData, msgLen, true);
			m_res_len = 0;
			return msgLen;
		}
		memcpy(m_res_buf, msgData, msgLen);
		m_res_len = msgLen;
		return m_res_len;
	}
	memcpy(m_res_buf + m_res_len, msgData, msgLen);
	m_res_len += msgLen;
	if (Last)
	{
		mySession->OnSubmitCmdPacket(m_res_buf, m_res_len, true);
		m_res_len = 0;
	}
	return msgLen;
}
