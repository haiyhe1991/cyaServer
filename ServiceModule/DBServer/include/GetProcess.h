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

	/// ��ȡ "������¼" ��ȫ���ֶ�
	INT MemGet(DBTcpSession* mySession, const void* msgData, UINT dataLen);
private:
	/// ��ȡ����
	char* ParseTableName(const void* msgData, UINT16& dataLen);
	/// ��ȡ��������м�¼
	UINT16 GetRequestKeys(const void* msgData, UINT dataLen, std::vector<std::string>& vecKeys);
	/// �õ�������ֶ�
	INT GetRequestFields(MemTable* table_val, const std::string& keyValue, std::vector<std::string>& vecFileds);

	void DeleteNotExistedKeys(MemTable* table_val, std::vector<std::string>& reqKeys);

	/// ��ȡLoad��һ����
	MemTable* GetTableExisted(const std::string& strTableName);

	/// �ظ������ֶ���
	void ResponseFieldsName(Transporter* pts, const std::vector<std::string>& reqFields, UINT16 rowNum);
	/// �ظ���
	void ResponseRow(Transporter* pts, const std::string& strKey, MemTable* table_val, bool Last);

	/// �ظ�(����)��Ϣ
	INT ResponseMsg(DBTcpSession* sessionSit, UINT16 error, UINT32 rows = 0, UINT32 fields = 0);

	INT SendPacket(DBTcpSession* mySession, void* msgData, INT msgLen, bool Last);
private:
	char m_res_buf[MAX_PACKET];
	UINT16 m_res_len;
};

#endif