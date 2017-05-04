#ifndef __SET_PROCESS_H__
#define __SET_PROCESS_H__

#include "TcpSession.h"
#include "SqlSingle.h"
#include "cyaLocker.h"
#include "DBTable.h"

class SetProcess
{
public:
	SetProcess();
	~SetProcess();
	/// 修改一条记录的多个字段
	INT MemSet(DBTcpSession* mySession, const void* msgData, UINT dataLen);
private:
	/// 回复(错误)消息
	INT ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows = 0, UINT32 fields = 0);

	char* ParseTableName(const void* msgData, UINT dataLen);
	char* ParseKeyValue(const void* msgData, UINT dataLen);
	/// 获取请求的字段
	UINT16 ParseReqFileds(const void* msgData, UINT dataLen, KeyLine* pLine);
	/// 表须Load过
	MemTable* GetTableExisted(const std::string& strTableName);
};

#endif