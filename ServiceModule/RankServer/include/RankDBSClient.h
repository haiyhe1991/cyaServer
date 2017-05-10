#ifndef __RANK_DBS_CLIENT_H__
#define __RANK_DBS_CLIENT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
���ݿ������
*/

#include <vector>
#include "DBSSdk.h"
#include "LCSProtocol.h"
#include "cyaLocker.h"
#include "cyaRefCount.h"

class RankDBSClient
{
private:
	class DBSConnHandler : public BaseRefCount1
	{
	public:
		DBSConnHandler(RankDBSClient* parent);
		~DBSConnHandler();

		int  Connect();
		void DisConnect();

		DBSHandle GetDBSHandle() const
		{
			return m_dbsHandle;
		}

	private:
		RankDBSClient* m_parent;
		DBSHandle m_dbsHandle;
	};

public:
	RankDBSClient();
	~RankDBSClient();

public:
	int  Connect();
	void DisConnect();
	DBSConnHandler* GetDBSConnectionHandler();

	int Insert(const char* dbName, const char* sql, UINT32* newId, int nSql = -1);
	int Query(const char* dbName, const char* sql, DBS_RESULT& result, int nSql = -1);
	int Del(const char* dbName, const char* sql, UINT32* affectRows = NULL, int nSql = -1);
	int Update(const char* dbName, const char* sql, UINT32* affectRows = NULL, int nSql = -1);

private:
	CXTLocker m_dbsHandlerVecLocker;
	std::vector<DBSConnHandler*> m_dbsHandlerVec;
};

BOOL InitRankDBSClient();
RankDBSClient* GetRankDBSClient();
void DestroyRankDBSClient();

// ���ⳤʱ���������ݿ�
BOOL ConnectRankDBSClient();
void DisConnectRankDBSClient();

#endif