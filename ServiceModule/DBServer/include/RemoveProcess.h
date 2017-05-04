#ifndef __REMOVE_PROCESS_H__
#define __REMOVE_PROCESS_H__

#include "TcpSession.h"
#include "SqlSingle.h"
#include "cyaLocker.h"
#include "DBTable.h"

class RemoveProcess
{
public:
	RemoveProcess();
	~RemoveProcess();

	/// 只删除一条记录
	INT MemRemove(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
private:
	char* ParseTableName(const void* msgData, UINT dataLen);
	char* ParseKeyValue(const void* msgData, UINT dataLen);
	char* ParseKeyName(const void* msgData, UINT dataLen);

	/// 表须Load过的
	MemTable* GetTableExisted(const std::string& strTableName);
	/// 回复(错误)消息
	INT ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows = 0, UINT32 fields = 0);
};


#endif