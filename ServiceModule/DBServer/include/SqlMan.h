#ifndef __SQL_MAN_H__
#define __SQL_MAN_H__

#include "cyaTypes.h"
#include "cyaRefCount.h"
#include "SqlSingle.h"
#include "cyaCoreTimer.h"

typedef SqlSingle* DBSessionHandle;
class DBSession
{
public:
	explicit DBSession(UINT16 MinKeep = 5, /*长期保持的数据库连接数量*/
		UINT16 MaxLink = 11 /*最大保持的数据库连接数量*/
		);
	~DBSession();

	/// 返回连接数, 构造多个mysql Client
	UINT16 DBS_Connect(const char* dbHost, const char* dbUser, const char* dbPwd, const char* dbName, UINT dbPort = 3306);
	/// 关闭所有连接
	void DBS_Close();

	/// 随机获取一个数据库连接
	DBSessionHandle FetchDBSessionHandle();
	/// Insert操作单独使用一个连接
	DBSessionHandle FetchDBSessionInsertHandle();

protected:
	DBSession& operator=(const DBSession&);
	DBSession(const DBSession&);
private:
	static void PingTimerCallBack(void* param, TTimerID id);
	void OnTimer();
private:
	UINT16 m_MinKeep;
	UINT16 m_MaxLink;
	SqlSingle* m_insertSession;

	typedef std::vector<SqlSingle*> SESSION_VEC;
	SESSION_VEC m_sessions;
	CXTLocker m_handleLocker;
	CyaCoreTimer m_pingTimer;
};

#endif