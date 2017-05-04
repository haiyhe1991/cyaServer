#include "QueryProcess.h"
#include "DBTable.h"

QueryProcess::QueryProcess()
{

}

QueryProcess::~QueryProcess()
{

}

/// request: cmd
/// return: ErrCode + Row数量 + Field数量 + Fields + Rows
/// Fields: FieldNameLen + FiledName
/// Rows:  FieldValueLen + FieldValue
INT QueryProcess::DbQuery(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen)
{
	ASSERT(0 < dataLen && NULL != sqlSession && sqlSession->ValidSession());

	UINT msgLen = dataLen;
	char* strCmd = ParseCommand(msgData, msgLen);
	if (NULL == strCmd && 0 == msgLen)
		return ResponseMsg(mySession, DBS_ERR_SQL);

	ASSERT(strlen(strCmd) + sizeof(char) == msgLen);
	std::queue<MYSQL_RES*> m_handles;
	if (!sqlSession->SelectInit(strCmd, msgLen, m_handles))
		return ResponseMsg(mySession, DBS_ERR_SQL);

	return QueryResProcess(mySession, sqlSession, m_handles);
}


char* QueryProcess::ParseCommand(const void* msgData, UINT& dataLen)
{
	if (dataLen <= sizeof(UINT16))
		return NULL;
	dataLen = ntohs(*(UINT16*)msgData);
	return (char*)msgData + sizeof(UINT16);
}

INT QueryProcess::ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows/* = 0*/, UINT32 fields/* = 0*/)
{
	char res_buf[128] = { 0 };
	SDBSResultRes* pHead = (SDBSResultRes*)res_buf;
	pHead->fields = fields;
	pHead->rows = rows;
	pHead->retCode = error;
	pHead->hton();
	return sessionSit->OnSubmitCmdPacket(res_buf, sizeof(SDBSResultRes), true);
}

INT QueryProcess::QueryResProcess(DBTcpSession* mySession, SqlSingle* sqlSession, std::queue<MYSQL_RES*>& handles)
{
	if (handles.empty())
		return ResponseMsg(mySession, DBS_ERR_SQL, 0, 0);
	MYSQL_RES* theSqlHandle = NULL;
	{
		CXTAutoLock locke(m_handleLocker);
		theSqlHandle = handles.front();
		handles.pop();
	}

	Transporter* pts = new Transporter(mySession);
	ASSERT(pts);
	if (NULL == pts)
	{
		CXTAutoLock locke(m_handleLocker);
		sqlSession->ReleaseHandle(theSqlHandle);
		for (UINT16 i = 0; i < handles.size(); ++i)
		{
			theSqlHandle = handles.front();
			handles.pop();
			sqlSession->ReleaseHandle(theSqlHandle);
		}
		return 0;
	}

	UINT16 data_offset = FieldProcess(pts, sqlSession, theSqlHandle);
	if (data_offset == 0)
	{
		pts->ReleaseRef();
		CXTAutoLock locke(m_handleLocker);
		sqlSession->ReleaseHandle(theSqlHandle);
		for (UINT16 i = 0; i < handles.size(); ++i)
		{
			theSqlHandle = handles.front();
			handles.pop();
			sqlSession->ReleaseHandle(theSqlHandle);
		}
		return ResponseMsg(mySession, 0, 0, 0);
	}

	RowProcess(pts, sqlSession, theSqlHandle, handles.empty());
	sqlSession->ReleaseHandle(theSqlHandle);

	for (UINT16 i = 0; i < handles.size(); ++i)
	{
		CXTAutoLock locke(m_handleLocker);
		theSqlHandle = handles.front();
		handles.pop();
		RowProcess(pts, sqlSession, theSqlHandle, handles.empty());
		sqlSession->ReleaseHandle(theSqlHandle);
	}
	pts->ReleaseRef();
	return 0;
}

INT QueryProcess::FieldProcess(Transporter* pts, SqlSingle* sqlSession, MYSQL_RES* theSqlHandle)
{
	LineFieldName* pFieldLine = new LineFieldName();
	UINT16 field_num = sqlSession->FetchFields(theSqlHandle, pFieldLine);
	UINT16 row_num = sqlSession->RowsNum(theSqlHandle);
	if (0 == field_num || 0 == row_num)
	{
		pFieldLine->ReleaseRef();
		return 0;
	}
	char res_buf[MAX_PACKET] = { 0 };
	SDBSResultRes* pRes = (SDBSResultRes*)res_buf;
	pRes->retCode = 0;
	pRes->rows = row_num;
	pRes->fields = field_num;
	pRes->hton();
	pts->Transport(res_buf, sizeof(SDBSResultRes) - sizeof(char), false);

	UINT16 data_offset = 0;
	for (UINT16 field_i = 0; field_i < field_num; ++field_i)
	{
		UINT16 len = 0;
		const std::string& pValue = pFieldLine->OnFieldName(field_i);
		len = htons(pValue.length() + sizeof(char));
		data_offset += pts->Transport((char*)(&len), sizeof(UINT16), false);
		data_offset += pts->Transport(pValue.c_str(), pValue.length() + sizeof(char), false);
	}
	pFieldLine->ReleaseRef();

	return data_offset;
}

INT QueryProcess::RowProcess(Transporter* pts, SqlSingle* sqlSession, MYSQL_RES* theSqlHandle, bool LastHandle)
{
	size_t data_offset = 0;
	while (true)
	{
		TableLineValue* pLine = new TableLineValue();
		ASSERT(pLine);
		if (NULL == pLine)
		{
			return 0;
		}
		UINT16 line_fields = sqlSession->FetchRow(theSqlHandle, pLine, 0);
		if (0 == line_fields)
		{
			pLine->ReleaseRef();
			if (LastHandle)
				data_offset += pts->Transport(NULL, 0, true);
			return 0;
		}
		for (UINT16 field_i = 0; field_i < line_fields; ++field_i)
		{
			StrValue* pVal = pLine->FetchValueByIndx(field_i);
			if (NULL != pVal)
			{
				UINT16 field_len = 0, len = 0;
				const char* pField = pVal->FetchValue(field_len);
				len = field_len;
				field_len = htons(field_len);
				data_offset += pts->Transport((char*)(&field_len), sizeof(UINT16), false);
				data_offset += pts->Transport(pField, len, false);
			}
		}
		pLine->ReleaseRef();
	}

	return data_offset;
}