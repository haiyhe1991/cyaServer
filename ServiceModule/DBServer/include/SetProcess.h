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
	/// �޸�һ����¼�Ķ���ֶ�
	INT MemSet(DBTcpSession* mySession, const void* msgData, UINT dataLen);
private:
	/// �ظ�(����)��Ϣ
	INT ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows = 0, UINT32 fields = 0);

	char* ParseTableName(const void* msgData, UINT dataLen);
	char* ParseKeyValue(const void* msgData, UINT dataLen);
	/// ��ȡ������ֶ�
	UINT16 ParseReqFileds(const void* msgData, UINT dataLen, KeyLine* pLine);
	/// ����Load��
	MemTable* GetTableExisted(const std::string& strTableName);
};

#endif