#ifndef __QUERY_PROCESS_H__
#define __QUERY_PROCESS_H__

#include "TcpSession.h"
#include "SqlSingle.h"
#include "cyaLocker.h"
#include "Transporter.h"

class QueryProcess
{
public:
	QueryProcess();
	~QueryProcess();

	/// request: cmd
	/// return: ErrCode + Row���� + Field���� + Fields + Rows
	/// Fields: FieldNameLen + FiledName
	/// Rows:  FieldValueLen + FieldValue
	INT DbQuery(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
private:
	/// �������ݿ����
	char* ParseCommand(const void* msgData, UINT& dataLen);

	INT QueryResProcess(DBTcpSession* mySession, SqlSingle* sqlSession, std::queue<MYSQL_RES*>& handles);

	/// ����ƫ������0�򲻼���ȡ��
	INT FieldProcess(Transporter* pts, SqlSingle* sqlSession, MYSQL_RES* theSqlHandle);
	INT RowProcess(Transporter* pts, SqlSingle* sqlSession, MYSQL_RES* theSqlHandle, bool LastHandle);

	/// ֱ�ӻظ�������Ϣ
	INT ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows = 0, UINT32 fields = 0);

private:
	CXTLocker m_handleLocker;
};


#endif