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
	/// return: ErrCode + Row数量 + Field数量 + Fields + Rows
	/// Fields: FieldNameLen + FiledName
	/// Rows:  FieldValueLen + FieldValue
	INT DbQuery(DBTcpSession* mySession, SqlSingle* sqlSession, const void* msgData, UINT dataLen);
private:
	/// 解析数据库表名
	char* ParseCommand(const void* msgData, UINT& dataLen);

	INT QueryResProcess(DBTcpSession* mySession, SqlSingle* sqlSession, std::queue<MYSQL_RES*>& handles);

	/// 返回偏移量，0则不继续取行
	INT FieldProcess(Transporter* pts, SqlSingle* sqlSession, MYSQL_RES* theSqlHandle);
	INT RowProcess(Transporter* pts, SqlSingle* sqlSession, MYSQL_RES* theSqlHandle, bool LastHandle);

	/// 直接回复错误消息
	INT ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows = 0, UINT32 fields = 0);

private:
	CXTLocker m_handleLocker;
};


#endif