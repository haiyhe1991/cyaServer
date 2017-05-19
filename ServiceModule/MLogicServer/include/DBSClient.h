#ifndef __DBS_CLIENT_H__
#define __DBS_CLIENT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
数据库操作管理类
*/

#include <vector>
#include "DBSSDK.h"
#include "LCSProtocol.h"
#include "cyaLocker.h"
#include "cyaRefCount.h"

class DBSClient
{
private:
	class DBSConnHandler : public BaseRefCount1
	{
	public:
		DBSConnHandler(DBSClient* parent);
		~DBSConnHandler();

		int  Connect();
		void DisConnect();

		DBSHandle GetDBSHandle() const
		{
			return m_dbsHandle;
		}

	private:
		DBSClient* m_parent;
		DBSHandle m_dbsHandle;
	};

public:
	DBSClient();
	~DBSClient();

	int Query(const char* dbName, const char* sql, DBS_RESULT& result, int nSql = -1);
	int Insert(const char* dbName, const char* sql, UINT32* newId, int nSql = -1);
	int Del(const char* dbName, const char* sql, UINT32* affectRows = NULL, int nSql = -1);
	int Update(const char* dbName, const char* sql, UINT32* affectRows = NULL, int nSql = -1);

public:
	int  Connect();
	void DisConnect();
	DBSConnHandler* GetDBSConnectionHandler();

private:
	CXTLocker m_dbsHandlerVecLocker;
	std::vector<DBSConnHandler*> m_dbsHandlerVec;
};

int InitDBSClient();
DBSClient* GetDBSClient();
void DestroyDBSClient();


#endif