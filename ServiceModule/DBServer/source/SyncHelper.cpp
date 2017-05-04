#include <vector>
#include "SyncHelper.h"

#ifndef WIN32
	#define DBS_PRINTF snprintf
#else
	#define DBS_PRINTF _snprintf_s
#endif /// WIN32

SyncHelper* SyncHelper::m_helper = NULL;

SyncHelper::SyncHelper()
{

}

SyncHelper::~SyncHelper()
{

}

SyncHelper* SyncHelper::FetchSyncHelper()
{
	if (NULL == m_helper)
		m_helper = new SyncHelper();
	return m_helper;
}

void SyncHelper::DeleteThis()
{
	if (m_helper)
	{
		delete m_helper;
		m_helper = NULL;
	}
}

INT SyncHelper::CreateUpdateSqlCmd(KeyLine* theLine, const std::string& strTable, const std::string& keyName,
									char* cmdBuf, UINT16& cmdLen)
{
	char where_buf[512] = { 0 };
	DBS_PRINTF(where_buf, sizeof(where_buf), "update %s set ", strTable.c_str());
	cmdLen = strlen(where_buf);
	memcpy(cmdBuf, where_buf, cmdLen);

	const std::vector<FieldValue*>& theFields = theLine->GetFileds();
	std::vector<FieldValue*>::const_iterator itr = theFields.begin();
	std::vector<FieldValue*>::const_iterator end_itr = theFields.end();
	--end_itr;

	const char* keyVaLue = NULL;
	UINT16 keyValLen = 0;

	for (itr; theFields.end() != itr; ++itr)
	{
		if ((*itr)->GetFiledName() == keyName)
		{
			keyVaLue = (*itr)->GetFiledValue(keyValLen);
			continue;
		}
		UINT16 fieldLen = 0;
		const char* fieldVal = (*itr)->GetFiledValue(fieldLen);
		if (NULL == fieldVal)
		{
			continue;
		}
		DBS_PRINTF(where_buf, sizeof(where_buf), "%s=", (*itr)->GetFiledName());
		memcpy(cmdBuf + cmdLen, where_buf, strlen(where_buf));
		cmdLen += strlen(where_buf);

		memcpy(cmdBuf + cmdLen, fieldVal, fieldLen - sizeof(char));
		cmdLen += fieldLen - sizeof(char);
		if (itr != end_itr)
		{
			memcpy(cmdBuf + cmdLen, ", ", strlen(", "));
			cmdLen += strlen(", ");
		}
	}
	if (NULL == keyVaLue || 0 == keyValLen)
	{
		cmdBuf[0] = '\0';
		cmdLen = 0;
		return 0;
	}

	DBS_PRINTF(where_buf, sizeof(where_buf), " where %s=", keyName.c_str());
	memcpy(cmdBuf + cmdLen, where_buf, strlen(where_buf));
	cmdLen += strlen(where_buf);
	memcpy(cmdBuf + cmdLen, keyVaLue, keyValLen);
	cmdLen += keyValLen;
	return cmdLen;
}

INT SyncHelper::CreateInsertSqlCmd(KeyLine* theLine, const std::string& strTable, char* cmdBuf, UINT16& cmdLen)
{
	char value_buf[4096] = { 0 };
	char field_buf[4096] = { 0 };
	UINT16 val_len = 0;
	UINT16 fid_len = 0;

	const std::vector<FieldValue*>& theFields = theLine->GetFileds();
	std::vector<FieldValue*>::const_iterator itr = theFields.begin();
	std::vector<FieldValue*>::const_iterator end_itr = theFields.end();
	--end_itr;

	for (itr; theFields.end() != itr; ++itr)
	{
		const char* pStr = (*itr)->GetFiledName();
		UINT16 strLen = strlen(pStr);
		if (NULL == pStr || 0 >= strLen)
		{
			cmdBuf[0] = '\0';
			cmdLen = 0;
			return 0;
		}
		memcpy(field_buf + fid_len, pStr, strLen);
		fid_len += strLen;

		pStr = (*itr)->GetFiledValue(strLen);
		if (strLen <= 0 || NULL == pStr)
		{
			pStr = "";
			strLen = strlen("");
		}
		memcpy(value_buf + val_len, pStr, strLen - sizeof(char));
		val_len += strLen - sizeof(char);

		if (itr != end_itr)
		{
			pStr = ", ";
			strLen = strlen(", ");
			memcpy(field_buf + fid_len, ", ", strLen);
			fid_len += strLen;
			memcpy(value_buf + val_len, pStr, strLen);
			val_len += strLen;
		}

	}
	if (val_len == 0 || fid_len == 0)
	{
		cmdBuf[0] = '\0';
		cmdLen = 0;
		return 0;
	}
	char tmpbuf[4096] = { 0 };
	DBS_PRINTF(tmpbuf, sizeof(tmpbuf), "insert into %s (", strTable.c_str());
	cmdLen = strlen(tmpbuf);
	memcpy(cmdBuf, tmpbuf, cmdLen);
	memcpy(cmdBuf + cmdLen, field_buf, fid_len);
	cmdLen += fid_len;
	DBS_PRINTF(tmpbuf, sizeof(tmpbuf), ") values(");
	memcpy(cmdBuf + cmdLen, tmpbuf, strlen(tmpbuf));
	cmdLen += strlen(tmpbuf);
	memcpy(cmdBuf + cmdLen, value_buf, val_len);
	cmdLen += val_len;
	DBS_PRINTF(tmpbuf, sizeof(tmpbuf), ") ");
	memcpy(cmdBuf + cmdLen, tmpbuf, strlen(tmpbuf));
	cmdLen += strlen(tmpbuf);
	return cmdLen;
}

/// 先更新，失败则插入
int SyncHelper::EnforceSync(MemTable* menTable, SqlSingle* sqlSession)
{
	ASSERT(NULL != menTable && NULL != sqlSession);
	const std::string& strTableName = menTable->GetTableName();
	const std::map<std::string, KeyLine*>& lines = menTable->GetLines();
	std::map<std::string, KeyLine*>::const_iterator itr = lines.begin();
	std::map<std::string, INT> fileds;
	std::string keyName(menTable->GetAllField(fileds));
	char cmd_buf[4096] = { 0 };
	UINT16 cmd_len = 0;
	for (itr; lines.end() != itr; ++itr)
	{
		INT ret = CreateUpdateSqlCmd(itr->second, strTableName, keyName, cmd_buf, cmd_len);
		if (0 >= ret)
			continue;
		if (0>sqlSession->UpdateRow(cmd_buf, cmd_len))
		{
			cmd_len = 0;
			ret = CreateInsertSqlCmd(itr->second, strTableName, cmd_buf, cmd_len);
			if (0 >= ret)
				continue;
			DWORD id;
			sqlSession->InsertRow(cmd_buf, cmd_len, id);
		}
	}
	return 0;
}
