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

	/// ֻɾ��һ����¼
	INT MemRemove(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
private:
	char* ParseTableName(const void* msgData, UINT dataLen);
	char* ParseKeyValue(const void* msgData, UINT dataLen);
	char* ParseKeyName(const void* msgData, UINT dataLen);

	/// ����Load����
	MemTable* GetTableExisted(const std::string& strTableName);
	/// �ظ�(����)��Ϣ
	INT ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows = 0, UINT32 fields = 0);
};


#endif