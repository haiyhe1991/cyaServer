#ifndef __DB_CONFIG_H__
#define __DB_CONFIG_H__

#include <vector>
#include "cyaLocker.h"

class DBConfig
{
public:
	DBConfig();
	~DBConfig();

	BOOL InitializeConfig();
	void FinishConfig();

	/// DBServer 地址信息
	const char* DBS_Addr() const;
	INT DBS_Port() const;

	/// MYSQL 地址信息
	const char* DBS_SQL_Addr() const;
	INT DBS_SQL_Port() const;
	/// MYSQL 用户名
	const char* DBS_SQL_User() const;
	/// MYSQL 密码
	const char* DBS_SQL_Pswd() const;
	/// MYSQL 数据库名
	const char* DBS_DB_Name(INT Indx) const;
	/// MYSQL 数据库的个数
	INT DBS_DB_Count() const;
	/// MYSQL 连接字符
	const char* DBS_SQL_Charset() const;
private:
	DBConfig(const DBConfig&);
	DBConfig& operator=(const DBConfig&);
private:
	INT m_dbs_port;
	INT m_sql_port;
	INT m_sql_count;
	std::string m_dbs_addr;
	std::string m_sql_addr;
	std::string m_sql_user;
	std::string m_sql_pswd;
	std::string m_sql_charset;
	typedef std::vector<std::string> SQLNames;
	SQLNames m_sql_names;
};

DBConfig& GetDBConfigObj();

#endif