#include "SqlSingle.h"
#include "cyaLog.h"

#include "DBConfig.h"

SqlSingle::SqlSingle()
	: m_connect(false)
	, m_mysqlClient(NULL)
{
	m_mysqlClient = mysql_init(NULL);
	if (NULL != m_mysqlClient)
	{
		int flg = 1;
		mysql_options(m_mysqlClient, MYSQL_OPT_RECONNECT, &flg);
		mysql_options(m_mysqlClient, MYSQL_SET_CHARSET_NAME, GetDBConfigObj().DBS_SQL_Charset());
	}
}

SqlSingle::~SqlSingle()
{
	SqlClose();
}

bool SqlSingle::ValidClient() const
{
	return (NULL != m_mysqlClient) & m_connect;
}

UINT64 SqlSingle::SqlConnect(const char* dbHost, const char* dbUser, const char* dbPwd, const char* dbName, UINT dbPort /*= 3306*/)
{
	if (NULL == m_mysqlClient || NULL == dbHost)
		return ER_HOSTNAME;

	if (NULL == mysql_real_connect(m_mysqlClient, dbHost, dbUser, dbPwd, dbName, dbPort, NULL, 0))
		return mysql_errno(m_mysqlClient);

	m_connect = true;
	return 0;
}

void SqlSingle::SqlClose()
{
	CXTAutoLock locke(m_sqlLocker);
	if (NULL != m_mysqlClient)
	{
		mysql_close(m_mysqlClient);
		m_mysqlClient = NULL;
		m_connect = false;
		LogDebug(("Sql Session Close"));
	}
}

bool SqlSingle::ValidSession() const
{
	return ValidClient();
}

UINT64 SqlSingle::AffectRow(const char* StrCmd, DWORD CmdLen)
{
	if (!ValidClient() || NULL == StrCmd)
		return 0;
	//	LogDebug(("%s", StrCmd));
	CXTAutoLock locke(m_sqlLocker);
	if (0 != mysql_real_query(m_mysqlClient, StrCmd, CmdLen))
	{
		LogDebug(("%s", StrCmd));
		LogDebug(("mysql_query Error no=%d, %s\n", mysql_errno(m_mysqlClient), mysql_error(m_mysqlClient)));
		return 0;
	}

	return mysql_affected_rows(m_mysqlClient);
}

UINT64 SqlSingle::UpdateRow(const char* StrCmd, DWORD CmdLen)
{
	return AffectRow(StrCmd, CmdLen);
}

UINT64 SqlSingle::DeleteRow(const char* StrCmd, DWORD CmdLen)
{
	return AffectRow(StrCmd, CmdLen);
}

UINT64 SqlSingle::InsertRow(const char* StrCmd, DWORD CmdLen, DWORD& id)
{
	if (!ValidClient() || NULL == StrCmd)
		return 0;
	//	LogDebug(("%s", StrCmd));
	CXTAutoLock locke(m_sqlLocker);
	if (0 != mysql_real_query(m_mysqlClient, StrCmd, CmdLen))
	{
		LogDebug(("%s", StrCmd));
		LogDebug(("mysql_query Error no=%d, %s\n", mysql_errno(m_mysqlClient), mysql_error(m_mysqlClient)));
		return 0;
	}
	id = mysql_insert_id(m_mysqlClient);
	return mysql_affected_rows(m_mysqlClient);
}

UINT64 SqlSingle::RowsNum(MYSQL_RES* theHandle)
{
	ASSERT(theHandle && ValidClient());
	return mysql_num_rows(theHandle);
}

INT SqlSingle::SessionPing()
{
	CXTAutoLock locke(m_sqlLocker);
	return mysql_ping(m_mysqlClient);
}

DWORD SqlSingle::LastInsertID()
{
	CXTAutoLock locke(m_sqlLocker);
	return mysql_insert_id(m_mysqlClient);
}

bool SqlSingle::SelectInit(const char* strCmd, DWORD CmdLen, std::queue<MYSQL_RES*>& handles)
{
	CXTAutoLock locke(m_sqlLocker);
	if (!ValidClient() || NULL == strCmd)
		return false;

	//	LogDebug(("%s", strCmd));
	if (0 != mysql_real_query(m_mysqlClient, strCmd, CmdLen))
	{
		LogDebug(("%s", strCmd));
		LogDebug(("SelectInit Errorno=%d, %s\n", mysql_errno(m_mysqlClient), mysql_error(m_mysqlClient)));
		return false;
	}
	do
	{
		MYSQL_RES* select_res = mysql_store_result(m_mysqlClient);
		if (NULL == select_res)
		{
			if (handles.empty())
			{
				LogDebug(("SelectInit Result Error no=%d, %s\n", mysql_errno(m_mysqlClient), mysql_error(m_mysqlClient)));
			}
		}
		else
		{
			handles.push(select_res);
		}
	} while (!mysql_next_result(m_mysqlClient));

	return !handles.empty();
}

void SqlSingle::ReleaseHandle(MYSQL_RES* theHandle)
{
	mysql_free_result(theHandle);
}

INT SqlSingle::FetchFields(MYSQL_RES* theHandle, LineFieldName* pLine)
{
	if (NULL == theHandle || NULL == pLine)
		return 0;

	MYSQL_FIELD* tableRes = NULL;
	UINT16 counter = 0;
	while (NULL != (tableRes = mysql_fetch_field(theHandle)))
	{
		pLine->AddFieldName(tableRes->name, IS_PRI_KEY(tableRes->flags), counter, tableRes->type);
		counter++;
	}
	return pLine->OnFieldNum();
}

DWORD SqlSingle::FetchRow(MYSQL_RES* theRes, TableLineValue* pLine, INT keyPos)
{
	if (NULL == theRes || NULL == pLine)
		return 0;

	UINT num_col = mysql_num_fields(theRes);
	MYSQL_ROW fetch_row = mysql_fetch_row(theRes);
	if (NULL == fetch_row)
	{
		//mysql_free_result(theRes);
		return 0;
	}

	unsigned long* length = mysql_fetch_lengths(theRes);
	for (UINT i = 0; i<num_col; ++i)
	{
		const char* strRowValue = fetch_row[i] ? fetch_row[i] : "";
		pLine->AddValue(strRowValue, length[i]);
		if (i == keyPos)
			pLine->SetKey(strRowValue);
	}

	return num_col;
}

DWORD SqlSingle::EscapeString(char *to, const char *from, DWORD length)
{
	return mysql_escape_string(to, from, length);
}

DWORD SqlSingle::HexString(char *to, const char *from, DWORD from_length)
{
	return mysql_hex_string(to, from, from_length);
}

INT SqlSingle::GetWaitTimeout()
{
	if (0 != mysql_query(m_mysqlClient, "show variables like 'wait_timeout'"))
		return -1;

	MYSQL_RES* select_res = mysql_store_result(m_mysqlClient);
	if (NULL == select_res)
		return -1;

	MYSQL_ROW fetch_row = NULL;
	while (NULL != (fetch_row = mysql_fetch_row(select_res)))
	{
		if (strcmp("wait_timeout", fetch_row[0]) == 0)
		{
			LogDebug(("SqlSession %s %s", fetch_row[0], fetch_row[1]));
			return atoi(fetch_row[1]);
		}
	}
	return -1;
}

void SqlSingle::SetWaitTimeout()
{
	CXTAutoLock locke(m_sqlLocker);
	mysql_query(m_mysqlClient, "set wait_timeout = 28800");
}

///mysql_affected_rows() ///���ر����µ�UPDATE, DELETE��INSERT��ѯӰ������� ///����ÿ����--SELECT��ѯ
///mysql_errno() ///������������õ�MySQL�����ĳ����š� 
///mysql_error() ///������������õ�MySQL�����ĳ�����Ϣ�� 
///mysql_fetch_field() ///������һ�����ֶε����͡�
///mysql_field_count() ///���������ѯ�Ľ���е�������
///mysql_free_result() ///�ͷ�һ���������ʹ�õ��ڴ档
///mysql_ping() ///���Է������������Ƿ����ڹ�������Ҫʱ�������ӡ�
///mysql_query() ///ִ��ָ��Ϊһ���ս�β���ַ�����SQL��ѯ��
///mysql_store_result() ///����һ�������Ľ�����ϸ��ͻ�.���̼���ȫ�����
///mysql_use_result()	///��ʼ��һ��һ��һ�еؽ�����ϵļ���
///mysql_fetch_row()	///��ȡ��
///mysql_fetch_lengths()	///�ɻ�ù���ÿ��������ֵ�ߴ����Ϣ��
///mysql_hex_string    ///���ڴ���������SQL����еĺϷ�SQL�ַ�����
///mysql_escape_string    ///string ת�壬ʹ֮���԰�ȫ���� mysql_query()��