#ifndef __SYNC_HELPER_H__
#define __SYNC_HELPER_H__

#include "DBTable.h"
#include "SqlMan.h"

class SyncHelper
{
public:
	static SyncHelper* FetchSyncHelper();
	void DeleteThis();

	/// 立即同步一张表
	int EnforceSync(MemTable* menTable, SqlSingle* sqlSession);
private:
	SyncHelper();
	~SyncHelper();
private:
	INT CreateUpdateSqlCmd(KeyLine* theLine, const std::string& strTable, const std::string& keyName, char* cmdBuf, UINT16& cmdLen);
	INT CreateInsertSqlCmd(KeyLine* theLine, const std::string& strTable, char* cmdBuf, UINT16& cmdLen);

private:
	static SyncHelper* m_helper;
};

#endif