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
	explicit DBSession(UINT16 MinKeep = 5, /*���ڱ��ֵ����ݿ���������*/
		UINT16 MaxLink = 11 /*��󱣳ֵ����ݿ���������*/
		);
	~DBSession();

	/// ����������, ������mysql Client
	UINT16 DBS_Connect(const char* dbHost, const char* dbUser, const char* dbPwd, const char* dbName, UINT dbPort = 3306);
	/// �ر���������
	void DBS_Close();

	/// �����ȡһ�����ݿ�����
	DBSessionHandle FetchDBSessionHandle();
	/// Insert��������ʹ��һ������
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