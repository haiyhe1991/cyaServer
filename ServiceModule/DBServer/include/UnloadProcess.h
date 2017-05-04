#ifndef __UNLOAD_PROCESS_H__
#define __UNLOAD_PROCESS_H__

#include "TcpSession.h"
#include "SqlSingle.h"

class UnloadProcess
{
public:
	UnloadProcess();
	~UnloadProcess();

	INT MemUnLoad(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
private:
	char* ParseTableName(const void* msgData, UINT dataLen);
	INT ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows = 0, UINT32 fields = 0);
	/// Load¹ýµÄ±í
	MemTable* GetTableExisted(const std::string& strTableName);
};

#endif