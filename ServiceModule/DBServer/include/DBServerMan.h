#ifndef __DBSERVER_MAN_H__
#define __DBSERVER_MAN_H__

#include <map>

#include "TcpSessionMaker.h"
#include "cyaTcp.h"
#include "SqlMan.h"
#include "SyncServer.h"

class DBServerMan
{
public:
	DBServerMan();
	~DBServerMan();

	BOOL InitializeDBS();
	void FinishDBS();

	/// �����ȡһ�����ݿ�����
	DBSessionHandle FetchDBSessionHandle(const char* dbName);
	/// Insert��������ʹ��һ������
	DBSessionHandle FetchDBSessionInsertHandle(const char* dbName);
private:
	BOOL m_manstart;
	DBSessionMaker* m_tcpmaker;
	ICyaTcpServeMan* m_tcpman;
	SyncServer* m_syncser;

	/// ������ݿ�
	typedef std::map<std::string, DBSession*> DB_SESSIONS;
	DB_SESSIONS m_db_sessins;
};

DBServerMan& GetDBServerManObj();

#endif