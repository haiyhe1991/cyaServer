#ifndef __APPEND_PROCESS_H__
#define __APPEND_PROCESS_H__

#include "TcpSession.h"
#include "SqlSingle.h"
#include "cyaLocker.h"
#include "DBTable.h"

class AppendProcess
{
public:
	AppendProcess();
	~AppendProcess();

	/// 增加一条记录的多个字段
	INT MemAppend(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);

private:
	/// 解析数据库表名
	char* ParseTableName(const void* msgData, UINT dataLen);
	/// 获取请求的字段
	UINT16 ParseReqFields(const void* msgData, UINT dataLen, KeyLine* pLine);
	/// 获取Load过的表
	MemTable* GetTableExisted(const std::string& strTableName);

	/// 组合插入数据命令
	UINT16 ComposeInsertCmd(KeyLine* pLine, char* SqlCmd, const std::string& keyName, const std::map<std::string, INT>& fileds, char* strTable);
	/// 回复(错误)消息
	INT ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows = 0, UINT32 fields = 0);
	INT ResponseAppendMsg(DBTcpSession* sessionSit, const char* keyName, UINT16 error, UINT32 rows, UINT32 fields, UINT32 dwID);
};

#endif