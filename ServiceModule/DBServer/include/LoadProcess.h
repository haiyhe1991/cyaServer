#ifndef __LOAD_PROCESS_H__
#define __LOAD_PROCESS_H__

#include "TcpSession.h"
#include "SqlSingle.h"
#include "cyaLocker.h"
#include "DBTable.h"

class LoadProcess
{
public:
	LoadProcess();
	~LoadProcess();

	/// ��Mysql������Load���ڴ����ݿ�, ��SQL����ָ���ֶ�
	INT MemLoad(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
private:
	char* ParseCommand(const void* msgData, UINT& dataLen);
	char* ParseTableName(const void* msgData, UINT dataLen);

	/// �������򴴽���
	MemTable* FetchTable(const std::string& strTableName);

	INT ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows = 0, UINT32 fields = 0);

	INT LoadRows(SqlSingle* sqlSession, MYSQL_RES* theSqlHandle, MemTable* table_val, LineFieldName* pLineField);
	INT LoadFields(SqlSingle* sqlSession, MYSQL_RES* theSqlHandle, MemTable* table_val, LineFieldName* pLineField);
private:
	CXTLocker m_handleLocker;
};

#endif