#ifndef __GET_PROCESS_H__
#define __GET_PROCESS_H__

#include "TcpSession.h"
#include "SqlSingle.h"
#include "cyaLocker.h"
#include "DBTable.h"
#include "Transporter.h"

class GetProcess
{
public:
	GetProcess();
	~GetProcess();

	/// 获取 "多条记录" 的全部字段
	INT MemGet(DBTcpSession* mySession, const void* msgData, UINT dataLen);
private:
	/// 获取表名
	char* ParseTableName(const void* msgData, UINT16& dataLen);
	/// 获取请求的所有记录
	UINT16 GetRequestKeys(const void* msgData, UINT dataLen, std::vector<std::string>& vecKeys);
	/// 得到请求的字段
	INT GetRequestFields(MemTable* table_val, const std::string& keyValue, std::vector<std::string>& vecFileds);

	void DeleteNotExistedKeys(MemTable* table_val, std::vector<std::string>& reqKeys);

	/// 获取Load的一个表
	MemTable* GetTableExisted(const std::string& strTableName);

	/// 回复所有字段名
	void ResponseFieldsName(Transporter* pts, const std::vector<std::string>& reqFields, UINT16 rowNum);
	/// 回复行
	void ResponseRow(Transporter* pts, const std::string& strKey, MemTable* table_val, bool Last);

	/// 回复(错误)消息
	INT ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows = 0, UINT32 fields = 0);

	INT SendPacket(DBTcpSession* mySession, void* msgData, INT msgLen, bool Last);
private:
	char m_res_buf[MAX_PACKET];
	UINT16 m_res_len;
};

#endif