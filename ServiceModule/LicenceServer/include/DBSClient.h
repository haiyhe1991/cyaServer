#ifndef __DBS_CLIENT_H__
#define __DBS_CLIENT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
数据库操作类
*/

#include <vector>
#include "DBSSdk.h"
#include "LCSProtocol.h"
#include "cyaLocker.h"
#include "cyaRefCount.h"

enum enDBClientType
{
	DBCLI_VERIFY = 0,
	DBCLI_CASH,
};

class DBSClient
{
private:
	class DBSConnHandler : public BaseRefCount1
	{
	public:
		DBSConnHandler(DBSClient* parent, char* ip, int port);
		~DBSConnHandler();

		int  Connect();
		void DisConnect();

		DBSHandle GetDBSHandle() const
		{
			return m_dbsHandle;
		}

	private:
		char    m_IP[20];
		int     m_Port;
		DBSClient* m_parent;
		DBSHandle m_dbsHandle;
	};

public:
	DBSClient(const char* ip, int port);
	~DBSClient();

public:
	int  Connect();
	void DisConnect();
	DBSConnHandler* GetDBSConnectionHandler();

	int Insert(const char* dbName, const char* sql, UINT32* newId, int nSql = -1);
	int Query(const char* dbName, const char* sql, DBS_RESULT& result, int nSql = -1);
	int Del(const char* dbName, const char* sql, UINT32* affectRows = NULL, int nSql = -1);
	int Update(const char* dbName, const char* sql, UINT32* affectRows = NULL, int nSql = -1);

private:
	char    m_dbIP[20];
	int     m_dbPort;
	CXTLocker m_dbsHandlerVecLocker;
	std::vector<DBSConnHandler*> m_dbsHandlerVec;
};

BOOL InitDBSClient();
DBSClient* GetDBSClient(int type);
void DestroyDBSClient();

#endif