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

///���ʼ��
DBSSDK_API void DBS_Init();

///���ӷ�����
///@dbsIp[IN] : ������IP 
///@dbsPort[IN] : �������˿�
///@connTimeout[IN] : ���ӳ�ʱ(ms)
///@heartbeatInterval[IN] : �������(ms)
///@writeTimeout[IN] : ���ͳ�ʱ(ms)
///@readTimeout[IN] : ���ճ�ʱ(ms)
DBSSDK_API DBSHandle DBS_Connect(const char* dbsIp, int dbsPort,
	unsigned int connTimeout = 5000,
	unsigned int heartbeatInterval = 10 * 1000,
	unsigned int writeTimeout = 0,
	unsigned int readTimeout = 0);

///mysql����
///@handle[IN] : DBS_Connect����
///@cmdType[IN] : ��������
///@dbName[IN] : ���ݿ���
///@sql[IN] : sql����
///@result[OUT] : ���ݽ����,������DBS_FreeResult�ͷŽ����
///@sqlLen[IN] : sql��䳤��
DBSSDK_API int  DBS_ExcuteSQL(DBSHandle handle, DBS_MSG_TYPE cmdType, const char* dbName, const char* sql, DBS_RESULT& result, int sqlLen = -1);

///@handle[IN] : DBS_Connect����
///@dbName[IN] : ���ݿ���
///@tableName[IN] : ����
///@sql[IN] : sql����
///@result[OUT] : ���ݽ����,������DBS_FreeResult�ͷŽ����
///@sqlLen[IN] : sql��䳤��
DBSSDK_API int  DBS_Load(DBSHandle handle, const char* dbName, const char* tableName, const char* sql, DBS_RESULT& result, int sqlLen = -1);

///@handle[IN] : DBS_Connect����
///@dbName[IN] : ���ݿ���
///@tableName[IN] : ����
///@pValsArray[IN] : ����ֵ����
///@arrCount[IN] : ����Ԫ�ظ���
///@result[OUT] : ���ݽ����,������DBS_FreeResult�ͷŽ����
DBSSDK_API int  DBS_Get(DBSHandle handle, const char* dbName, const char* tableName, const SDBSValue* pValsArray, int arrCount, DBS_RESULT& result);

///@handle[IN] : DBS_Connect����
///@dbName[IN] : ���ݿ���
///@tableName[IN] : ����
///@keyFieldValue[IN] : ����ֵ
///@pKeyValueArray[IN] : �����ֶ�����
///@arrCount[IN] : ����Ԫ�ظ���
///@result[OUT] : ���ݽ����,������DBS_FreeResult�ͷŽ����
DBSSDK_API int  DBS_Set(DBSHandle handle, const char* dbName, const char* tableName, const char* keyFieldValue, const SDBSKeyValue* pKeyValueArray, int arrCount, DBS_RESULT& result);

///@handle[IN] : DBS_Connect����
///@dbName[IN] : ���ݿ���
///@tableName[IN] : ����
///@pKeyValArray[IN] : (�ֶ�,ֵ, �ֶ�,ֵ, �ֶ�,ֵ ...)
///@arrCount[IN] : ����Ԫ�ظ���
///@result[OUT] : ���ݽ����,������DBS_FreeResult�ͷŽ����
DBSSDK_API int  DBS_Append(DBSHandle handle, const char* dbName, const char* tableName, const SDBSKeyValue* pKeyValArray, int arrCount, DBS_RESULT& result);

///@handle[IN] : DBS_Connect����
///@dbName[IN] : ���ݿ���
///@tableName[IN] : ����
///@pKeyVal[IN] : ɾ����(ֻ�ܰ�����ɾ)
///@result[OUT] : ���ݽ����,������DBS_FreeResult�ͷŽ����
DBSSDK_API int  DBS_Remove(DBSHandle handle, const char* dbName, const char* tableName, const SDBSKeyValue* pKeyVal, DBS_RESULT& result);

///@handle[IN] : DBS_Connect����
///@dbName[IN] : ���ݿ���
///@tableName[IN] : ����
///@result[OUT] : ���ݽ����,������DBS_FreeResult�ͷŽ����
DBSSDK_API int  DBS_UnLoad(DBSHandle handle, const char* dbName, const char* tableName, DBS_RESULT& result);

///�Ͽ�����
///@handle[IN] : DBS_Connect����
DBSSDK_API void DBS_DisConnect(DBSHandle handle);

///������
DBSSDK_API void DBS_Cleanup();

///�ͷŽ����
DBSSDK_API void	DBS_FreeResult(DBS_RESULT result);

///��ȡ��¼����
DBSSDK_API UINT32 DBS_NumRows(DBS_RESULT result);

///Ӱ�������
DBSSDK_API UINT32 DBS_AffectedRows(DBS_RESULT result);

///��ȡ�ֶθ���
DBSSDK_API UINT16 DBS_NumFields(DBS_RESULT result);

///��ȡ�ֶ�
DBSSDK_API DBS_FIELD DBS_FetchField(DBS_RESULT result, DBS_FIELDLENGTH* ppFieldsBytes = NULL);

///��ȡ��
DBSSDK_API DBS_ROW DBS_FetchRow(DBS_RESULT result, DBS_ROWLENGTH* ppRowBytes = NULL);

//������->�ַ�
DBSSDK_API unsigned int DBS_EscapeString(char* to, const char* from, unsigned int from_length);


#endif	