#include "DBSSDK.h"
#include "GlobalObjMan.h"
#include "DBSHelper.h"
#include "DBSResult.h"

#if defined(WIN32)
#include <WinSock2.h>
#include <mysql/win/mysql-connector-c-noinstall-6.0.2-win32/include/mysql.h>
#include <mysql/win/mysql-connector-c-noinstall-6.0.2-win32/include/mysqld_error.h>
#else
#include <mysql/linux/mysql-connector-c-6.0.2-linux-glibc2.3-x86-64bit/include/mysql.h>
#include <mysql/linux/mysql-connector-c-6.0.2-linux-glibc2.3-x86-64bit/include/mysqld_error.h>
#endif /// defined WIN32

///库初始化
DBSSDK_API void DBS_Init()
{
	InitGlobalObjMan();
}

///连接服务器
DBSSDK_API DBSHandle DBS_Connect(const char* dbsIp, int dbsPort,
								unsigned int connTimeout/* = 5000*/,
								unsigned int heartbeatInterval/* = 10*1000*/,
								unsigned int writeTimeout/* = 0*/,
								unsigned int readTimeout/* = 0*/)
{
	DBSHelper* helper = new DBSHelper(dbsIp, dbsPort, connTimeout, heartbeatInterval, writeTimeout, readTimeout);
	ASSERT(helper != NULL);
	int r = helper->Connect();
	if (r != 0)
	{
		delete helper;
		return NULL;
	}
	return helper;
}

///数据操作
DBSSDK_API int  DBS_ExcuteSQL(DBSHandle handle, DBS_MSG_TYPE cmdType, const char* dbName, const char* sql, DBS_RESULT& result, int sqlLen /*= -1*/)
{
	return ((DBSHelper*)handle)->ExcuteSQL(cmdType, dbName, sql, result, sqlLen);
}

DBSSDK_API int  DBS_Load(DBSHandle handle, const char* dbName, const char* tableName, const char* sql, DBS_RESULT& result, int sqlLen /*= -1*/)
{
	return ((DBSHelper*)handle)->Load(dbName, tableName, sql, result, sqlLen);
}

DBSSDK_API int  DBS_Get(DBSHandle handle, const char* dbName, const char* tableName, const SDBSValue* pValsArray, int arrCount, DBS_RESULT& result)
{
	return ((DBSHelper*)handle)->Get(dbName, tableName, pValsArray, arrCount, result);
}

DBSSDK_API int  DBS_Set(DBSHandle handle, const char* dbName, const char* tableName, const char* keyFieldValue, const SDBSKeyValue* pKeyValueArray, int arrCount, DBS_RESULT& result)
{
	return ((DBSHelper*)handle)->Set(dbName, tableName, keyFieldValue, pKeyValueArray, arrCount, result);
}

DBSSDK_API int  DBS_Append(DBSHandle handle, const char* dbName, const char* tableName, const SDBSKeyValue* pKeyValArray, int arrCount, DBS_RESULT& result)
{
	return ((DBSHelper*)handle)->Append(dbName, tableName, pKeyValArray, arrCount, result);
}

DBSSDK_API int  DBS_Remove(DBSHandle handle, const char* dbName, const char* tableName, const SDBSKeyValue* pKeyVal, DBS_RESULT& result)
{
	return ((DBSHelper*)handle)->Remove(dbName, tableName, pKeyVal, result);
}

DBSSDK_API int  DBS_UnLoad(DBSHandle handle, const char* dbName, const char* tableName, DBS_RESULT& result)
{
	return ((DBSHelper*)handle)->UnLoad(dbName, tableName, result);
}

///断开连接
DBSSDK_API void DBS_DisConnect(DBSHandle handle)
{
	((DBSHelper*)handle)->DisConnect();
}

///清理
DBSSDK_API void DBS_Cleanup()
{
	DestroyGlobalObjMan();
}


///结果集操作
DBSSDK_API void	DBS_FreeResult(DBS_RESULT result)
{
	if (result == NULL)
		return;
	DBSResult* pObj = (DBSResult*)result;
	CyaTcp_Delete(pObj);
}

DBSSDK_API UINT32 DBS_NumRows(DBS_RESULT result)
{
	if (result == NULL)
		return 0;
	return ((DBSResult*)result)->NumRows();
}

DBSSDK_API UINT32 DBS_AffectedRows(DBS_RESULT result)
{
	if (result == NULL)
		return 0;
	return ((DBSResult*)result)->AffectedRows();
}

DBSSDK_API UINT16 DBS_NumFields(DBS_RESULT result)
{
	if (result == NULL)
		return 0;
	return ((DBSResult*)result)->NumFields();
}

DBSSDK_API DBS_FIELD DBS_FetchField(DBS_RESULT result, DBS_FIELDLENGTH* ppFieldsBytes /*= NULL*/)
{
	if (result == NULL)
		return NULL;
	return ((DBSResult*)result)->FetchField(ppFieldsBytes);
}

DBSSDK_API DBS_ROW DBS_FetchRow(DBS_RESULT result, DBS_ROWLENGTH* ppRowBytes /*= NULL*/)
{
	if (result == NULL)
		return NULL;
	return ((DBSResult*)result)->FetchRow(ppRowBytes);
}

DBSSDK_API unsigned int DBS_EscapeString(char* to, const char* from, unsigned int from_length)
{
#if 0
	if (to == NULL || from == NULL || from_length <= 0)
		return 0;

	unsigned int j = 0;
	for (unsigned int i = 0; i < from_length; ++i)
	{
		unsigned char ch = from[i];
		if (ch > 0)
			to[j++] = ch;
		else if (ch == 0)
		{
			to[j++] = '\\';
			to[j++] = '0';
		}
		else
		{
			ASSERT(false);
			return 0;
		}
	}
	return j;
#else
	if (to == NULL || from == NULL || from_length <= 0)
		return 0;

	return mysql_escape_string(to, from, from_length);
#endif
}
