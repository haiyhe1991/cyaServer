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

	/// 返回0成功
	UINT64 SqlConnect(const char* dbHost, const char* dbUser, const char* dbPwd, const char* dbName, UINT dbPort = 3306);
	/// 关闭连接
	void SqlClose();
	/// 有效性测试
	bool ValidSession() const;

	/// 获取查询结果，外面锁保证数据准确
	bool SelectInit(const char* strCmd, DWORD CmdLen, std::queue<MYSQL_RES*>& m_handles);
	void ReleaseHandle(MYSQL_RES* theHandle);
	/// Query的字段
	INT FetchFields(MYSQL_RES* theHandle, LineFieldName* pLine);
	/// return: field 数量
	DWORD FetchRow(MYSQL_RES* theRes, TableLineValue* pLine, INT keyPos);
	/// 拼接字符串
	DWORD EscapeString(char *to, const char *from, DWORD length);
	DWORD HexString(char *to, const char *from, DWORD from_length);

	/// 返回被最新的UPDATE, DELETE或INSERT查询影响的行数
	UINT64 UpdateRow(const char* StrCmd, DWORD CmdLen);
	UINT64 DeleteRow(const char* StrCmd, DWORD CmdLen);
	UINT64 InsertRow(const char* StrCmd, DWORD CmdLen, DWORD& id);
	/// 最后插入的ID
	DWORD LastInsertID();
	/// 返回结果的行数
	UINT64 RowsNum(MYSQL_RES* theHandle);
	/// Ping服务器，可以重连
	INT SessionPing();
	/// 超时时间
	INT GetWaitTimeout();
	/// 设置超时时间
	void SetWaitTimeout();
private:
	SqlSingle& operator=(const SqlSingle&);
	SqlSingle(const SqlSingle&);

	inline bool ValidClient() const;
	///影响表格的操作, UPDATE, DELETE或INSERT
	UINT64 AffectRow(const char* StrCmd, DWORD CmdLen);
private:
	bool m_connect;
	MYSQL* m_mysqlClient;
	CXTLocker m_sqlLocker;
};

#endif