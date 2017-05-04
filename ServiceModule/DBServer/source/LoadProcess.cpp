#include "LoadProcess.h"
#include "cyaTcp.h"

LoadProcess::LoadProcess()
{

}

LoadProcess::~LoadProcess()
{

}

char* LoadProcess::ParseCommand(const void* msgData, UINT& dataLen)
{
	char* TableName = ParseTableName(msgData, dataLen);
	if (NULL == TableName)
		return NULL;
	UINT16 TableNameLen = strlen(TableName) + sizeof(char);
	if (TableNameLen + sizeof(UINT16) >= dataLen)
		return NULL;
	dataLen = ntohs(*(UINT16*)(TableName + TableNameLen));

	return TableName + TableNameLen + sizeof(UINT16);
}

char* LoadProcess::ParseTableName(const void* msgData, UINT dataLen)
{
	if (dataLen <= sizeof(UINT16))
		return NULL;
	return (char*)msgData + sizeof(UINT16);
}

INT LoadProcess::ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows/* = 0*/, UINT32 fields/* = 0*/)
{
	char strFetchRow[MAX_PACKET] = { 0 };
	SDBSResultRes* pHead = (SDBSResultRes*)strFetchRow;
	pHead->fields = fields;
	pHead->rows = rows;
	pHead->retCode = error;
	pHead->hton();
	return sessionSit->OnSubmitCmdPacket(strFetchRow, sizeof(SDBSResultRes) - sizeof(char), true);
}

MemTable* LoadProcess::FetchTable(const std::string& strTableName)
{
	SQLTable* redis_table = SQLTable::FetchTables();
	if (NULL == redis_table)
		return NULL;
	if (redis_table->TalbeIsExisted(strTableName))
	{
		return redis_table->FetchTable(strTableName);
	}

	MemTable* table_val = new MemTable(strTableName);
	if (NULL != table_val)
	{
		redis_table->AddTable(table_val);
	}

	return table_val;
}

/*	Req : Command + TableName///Table里面有字段要求*/
INT LoadProcess::MemLoad(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	ASSERT(NULL != mySession && NULL != sqlSession && NULL != msgData && 0 < dataLen);

	UINT cmdLen = dataLen;
	UINT talLen = dataLen;
	char* strCmd = ParseCommand(msgData, cmdLen);
	char* strTable = ParseTableName(msgData, talLen);
	if (NULL == strCmd || NULL == strTable)
	{
		return ResponseMsg(mySession, DBS_ERR_PARAM);
	}

	MemTable* table_val = FetchTable(strTable);
	if (NULL == table_val)
	{
		return ResponseMsg(mySession, DBS_ERR_TABLE);
	}

	std::queue<MYSQL_RES*> handles;
	if (!sqlSession->SelectInit(strCmd, cmdLen, handles) || handles.empty())
	{
		return ResponseMsg(mySession, DBS_ERR_SQL);
	}

	MYSQL_RES* theSqlHandle = NULL;
	{
		CXTAutoLock locke(m_handleLocker);
		theSqlHandle = handles.front();
		handles.pop();
	}
	LineFieldName* pLineField = new LineFieldName();
	if (NULL == pLineField || 0 == LoadFields(sqlSession, theSqlHandle, table_val, pLineField))
	{
		pLineField->ReleaseRef();
		return ResponseMsg(mySession, DBS_ERR_SQL);
	}

	INT row_num = LoadRows(sqlSession, theSqlHandle, table_val, pLineField);

	for (UINT16 i = 0; i<handles.size(); ++i)
	{
		CXTAutoLock locke(m_handleLocker);
		theSqlHandle = handles.front();
		handles.pop();
		row_num += LoadRows(sqlSession, theSqlHandle, table_val, pLineField);
	}
	pLineField->ReleaseRef();
	return ResponseMsg(mySession, 0, row_num, 0);
}

INT LoadProcess::LoadRows(SqlSingle* sqlSession, MYSQL_RES* theSqlHandle, MemTable* table_val, LineFieldName* pLineField)
{
	INT KeyPos = 0;
	std::string strKey = pLineField->OnKeyFieldNameFirst(KeyPos);

	INT row_num = 0;
	while (true)
	{
		TableLineValue* pLine = new TableLineValue();
		if (NULL == pLine)
			continue;
		KeyLine* pKeyLine = new KeyLine();
		if (NULL == pKeyLine)
		{
			pLine->ReleaseRef();
			continue;
		}

		/// 读取一行
		DWORD field_num = sqlSession->FetchRow(theSqlHandle, pLine, KeyPos);	/// 有设置KeyValue
		if (0 == field_num)
		{
			pLine->ReleaseRef();
			pKeyLine->ReleaseRef();
			return row_num;
		}

		table_val->DelKey(pLine->Key());	/// 存在的行先删除
		pKeyLine->SetKeyName(pLine->Key());

		for (UINT16 i = 0; i < field_num; ++i)
		{
			StrValue* pVal_1 = pLine->FetchValueByIndx(i);
			UINT16 valLen = 0;
			const char* pVal_2 = pVal_1->FetchValue(valLen);
			valLen -= sizeof(char);	/// 长度减一，不然后面长度又会加一
			char* pMen = NULL;
			if (pLineField->OnFieldType(i) == MYSQL_TYPE_VARCHAR ||
				pLineField->OnFieldType(i) == MYSQL_TYPE_VAR_STRING ||
				pLineField->OnFieldType(i) == MYSQL_TYPE_STRING ||
				pLineField->OnFieldType(i) == MYSQL_TYPE_DATE ||		/// 时间日期当字符串处理
				pLineField->OnFieldType(i) == MYSQL_TYPE_TIME ||
				pLineField->OnFieldType(i) == MYSQL_TYPE_DATETIME ||
				pLineField->OnFieldType(i) == MYSQL_TYPE_YEAR ||
				pLineField->OnFieldType(i) == MYSQL_TYPE_TIMESTAMP)
			{
				pMen = (char*)CyaTcp_Alloc(valLen + 2);
				pMen[0] = '\'';
				memcpy(pMen + sizeof(char), pVal_2, valLen);
				pMen[valLen + sizeof(char)] = '\'';
				valLen += sizeof(char) * 2;
			}
			else
			{
				pMen = (char*)CyaTcp_Alloc(valLen + 2);
				memcpy(pMen, pVal_2, valLen);
			}


			FieldValue* pVal = new FieldValue(pLineField->OnFieldName(i), pMen, valLen);
			if (pVal != NULL)
			{
				pKeyLine->AddFiled(pVal);
			}
			if (pMen)
			{
				CyaTcp_Free(pMen);
			}
		}

		table_val->AddLine(pKeyLine);
		pLine->ReleaseRef();
		++row_num;
	}
	return row_num;
}

/// 先存字段名
INT LoadProcess::LoadFields(SqlSingle* sqlSession, MYSQL_RES* theSqlHandle, MemTable* table_val, LineFieldName* pLineField)
{
	UINT16 field_num = sqlSession->FetchFields(theSqlHandle, pLineField);
	if (0 == field_num)
		return 0;

	std::map<std::string, INT> vecfileds;
	for (UINT16 i = 0; i < field_num; ++i)
	{
		//vecfileds.push_back(pLineField->OnFieldName(i));
		vecfileds.insert(std::make_pair(pLineField->OnFieldName(i), pLineField->OnFieldType(i)));
	}

	INT key_Indx = 0;
	table_val->LoadAllField(vecfileds, pLineField->OnKeyFieldNameFirst(key_Indx));

	return field_num;
}