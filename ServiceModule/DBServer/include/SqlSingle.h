#ifndef __SQL_SINGLE_H__
#define __SQL_SINGLE_H__

#include <queue>
#include <vector>

#if defined(WIN32)
	#include <WinSock2.h>
	#include <win/mysql-connector-c-noinstall-6.0.2-win32/include/mysql.h>
	#include <win/mysql-connector-c-noinstall-6.0.2-win32/include/mysqld_error.h>
    //#pragma comment(lib,"libmysql.lib")
#else
	#include <linux/mysql-connector-c-6.0.2-linux-glibc2.3-x86-64bit/include/mysql.h>
	#include <linux/mysql-connector-c-6.0.2-linux-glibc2.3-x86-64bit/include/mysqld_error.h>
#endif /// defined WIN32

#include "cyaLocker.h"
#include "DBSProtocol.h"

#include "DBTable.h"

class SqlSingle
{
public:
	SqlSingle();
	~SqlSingle();

	/// ����0�ɹ�
	UINT64 SqlConnect(const char* dbHost, const char* dbUser, const char* dbPwd, const char* dbName, UINT dbPort = 3306);
	/// �ر�����
	void SqlClose();
	/// ��Ч�Բ���
	bool ValidSession() const;

	/// ��ȡ��ѯ�������������֤����׼ȷ
	bool SelectInit(const char* strCmd, DWORD CmdLen, std::queue<MYSQL_RES*>& m_handles);
	void ReleaseHandle(MYSQL_RES* theHandle);
	/// Query���ֶ�
	INT FetchFields(MYSQL_RES* theHandle, LineFieldName* pLine);
	/// return: field ����
	DWORD FetchRow(MYSQL_RES* theRes, TableLineValue* pLine, INT keyPos);
	/// ƴ���ַ���
	DWORD EscapeString(char *to, const char *from, DWORD length);
	DWORD HexString(char *to, const char *from, DWORD from_length);

	/// ���ر����µ�UPDATE, DELETE��INSERT��ѯӰ�������
	UINT64 UpdateRow(const char* StrCmd, DWORD CmdLen);
	UINT64 DeleteRow(const char* StrCmd, DWORD CmdLen);
	UINT64 InsertRow(const char* StrCmd, DWORD CmdLen, DWORD& id);
	/// �������ID
	DWORD LastInsertID();
	/// ���ؽ��������
	UINT64 RowsNum(MYSQL_RES* theHandle);
	/// Ping����������������
	INT SessionPing();
	/// ��ʱʱ��
	INT GetWaitTimeout();
	/// ���ó�ʱʱ��
	void SetWaitTimeout();
private:
	SqlSingle& operator=(const SqlSingle&);
	SqlSingle(const SqlSingle&);

	inline bool ValidClient() const;
	///Ӱ����Ĳ���, UPDATE, DELETE��INSERT
	UINT64 AffectRow(const char* StrCmd, DWORD CmdLen);
private:
	bool m_connect;
	MYSQL* m_mysqlClient;
	CXTLocker m_sqlLocker;
};

#endif