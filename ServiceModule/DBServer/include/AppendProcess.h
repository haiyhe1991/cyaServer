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

	/// ����һ����¼�Ķ���ֶ�
	INT MemAppend(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);

private:
	/// �������ݿ����
	char* ParseTableName(const void* msgData, UINT dataLen);
	/// ��ȡ������ֶ�
	UINT16 ParseReqFields(const void* msgData, UINT dataLen, KeyLine* pLine);
	/// ��ȡLoad���ı�
	MemTable* GetTableExisted(const std::string& strTableName);

	/// ��ϲ�����������
	UINT16 ComposeInsertCmd(KeyLine* pLine, char* SqlCmd, const std::string& keyName, const std::map<std::string, INT>& fileds, char* strTable);
	/// �ظ�(����)��Ϣ
	INT ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows = 0, UINT32 fields = 0);
	INT ResponseAppendMsg(DBTcpSession* sessionSit, const char* keyName, UINT16 error, UINT32 rows, UINT32 fields, UINT32 dwID);
};

#endif