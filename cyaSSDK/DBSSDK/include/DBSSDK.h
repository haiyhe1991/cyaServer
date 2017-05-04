#ifndef __DBSSDK_H__
#define __DBSSDK_H__

#include "DBSSDKExports.h"
#include "DBSProtocol.h"

typedef char** DBS_ROW;
typedef char** DBS_FIELD;
typedef void*  DBS_RESULT;
typedef void*  DBSHandle;
typedef unsigned int* DBS_ROWLENGTH;
typedef unsigned int* DBS_FIELDLENGTH;

///库初始化
DBSSDK_API void DBS_Init();

///连接服务器
///@dbsIp[IN] : 服务器IP 
///@dbsPort[IN] : 服务器端口
///@connTimeout[IN] : 连接超时(ms)
///@heartbeatInterval[IN] : 心跳间隔(ms)
///@writeTimeout[IN] : 发送超时(ms)
///@readTimeout[IN] : 接收超时(ms)
DBSSDK_API DBSHandle DBS_Connect(const char* dbsIp, int dbsPort,
	unsigned int connTimeout = 5000,
	unsigned int heartbeatInterval = 10 * 1000,
	unsigned int writeTimeout = 0,
	unsigned int readTimeout = 0);

///mysql操作
///@handle[IN] : DBS_Connect返回
///@cmdType[IN] : 操作类型
///@dbName[IN] : 数据库名
///@sql[IN] : sql语句等
///@result[OUT] : 数据结果集,必须由DBS_FreeResult释放结果集
///@sqlLen[IN] : sql语句长度
DBSSDK_API int  DBS_ExcuteSQL(DBSHandle handle, DBS_MSG_TYPE cmdType, const char* dbName, const char* sql, DBS_RESULT& result, int sqlLen = -1);

///@handle[IN] : DBS_Connect返回
///@dbName[IN] : 数据库名
///@tableName[IN] : 表名
///@sql[IN] : sql语句等
///@result[OUT] : 数据结果集,必须由DBS_FreeResult释放结果集
///@sqlLen[IN] : sql语句长度
DBSSDK_API int  DBS_Load(DBSHandle handle, const char* dbName, const char* tableName, const char* sql, DBS_RESULT& result, int sqlLen = -1);

///@handle[IN] : DBS_Connect返回
///@dbName[IN] : 数据库名
///@tableName[IN] : 表名
///@pValsArray[IN] : 主键值数组
///@arrCount[IN] : 数组元素个数
///@result[OUT] : 数据结果集,必须由DBS_FreeResult释放结果集
DBSSDK_API int  DBS_Get(DBSHandle handle, const char* dbName, const char* tableName, const SDBSValue* pValsArray, int arrCount, DBS_RESULT& result);

///@handle[IN] : DBS_Connect返回
///@dbName[IN] : 数据库名
///@tableName[IN] : 表名
///@keyFieldValue[IN] : 主键值
///@pKeyValueArray[IN] : 更新字段数组
///@arrCount[IN] : 数组元素个数
///@result[OUT] : 数据结果集,必须由DBS_FreeResult释放结果集
DBSSDK_API int  DBS_Set(DBSHandle handle, const char* dbName, const char* tableName, const char* keyFieldValue, const SDBSKeyValue* pKeyValueArray, int arrCount, DBS_RESULT& result);

///@handle[IN] : DBS_Connect返回
///@dbName[IN] : 数据库名
///@tableName[IN] : 表名
///@pKeyValArray[IN] : (字段,值, 字段,值, 字段,值 ...)
///@arrCount[IN] : 数组元素个数
///@result[OUT] : 数据结果集,必须由DBS_FreeResult释放结果集
DBSSDK_API int  DBS_Append(DBSHandle handle, const char* dbName, const char* tableName, const SDBSKeyValue* pKeyValArray, int arrCount, DBS_RESULT& result);

///@handle[IN] : DBS_Connect返回
///@dbName[IN] : 数据库名
///@tableName[IN] : 表名
///@pKeyVal[IN] : 删除项(只能按主键删)
///@result[OUT] : 数据结果集,必须由DBS_FreeResult释放结果集
DBSSDK_API int  DBS_Remove(DBSHandle handle, const char* dbName, const char* tableName, const SDBSKeyValue* pKeyVal, DBS_RESULT& result);

///@handle[IN] : DBS_Connect返回
///@dbName[IN] : 数据库名
///@tableName[IN] : 表名
///@result[OUT] : 数据结果集,必须由DBS_FreeResult释放结果集
DBSSDK_API int  DBS_UnLoad(DBSHandle handle, const char* dbName, const char* tableName, DBS_RESULT& result);

///断开连接
///@handle[IN] : DBS_Connect返回
DBSSDK_API void DBS_DisConnect(DBSHandle handle);

///库清理
DBSSDK_API void DBS_Cleanup();

///释放结果集
DBSSDK_API void	DBS_FreeResult(DBS_RESULT result);

///获取记录行数
DBSSDK_API UINT32 DBS_NumRows(DBS_RESULT result);

///影响表行数
DBSSDK_API UINT32 DBS_AffectedRows(DBS_RESULT result);

///获取字段个数
DBSSDK_API UINT16 DBS_NumFields(DBS_RESULT result);

///获取字段
DBSSDK_API DBS_FIELD DBS_FetchField(DBS_RESULT result, DBS_FIELDLENGTH* ppFieldsBytes = NULL);

///获取行
DBSSDK_API DBS_ROW DBS_FetchRow(DBS_RESULT result, DBS_ROWLENGTH* ppRowBytes = NULL);

//二进制->字符
DBSSDK_API unsigned int DBS_EscapeString(char* to, const char* from, unsigned int from_length);


#endif	