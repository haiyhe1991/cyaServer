#include "DailyTaskMan.h"
#include "DBSClient.h"
#include "RoleInfos.h"
#include "ConfigFileMan.h"

static DailyTaskMan* sg_dailyTaskMan = NULL;
void InitDailyTaskMan()
{
	ASSERT(sg_dailyTaskMan == NULL);
	sg_dailyTaskMan = new DailyTaskMan();
}

DailyTaskMan* GetDailyTaskMan()
{
	return sg_dailyTaskMan;
}

void DestroyDailyTaskMan()
{
	DailyTaskMan* pDailyTaskMan = sg_dailyTaskMan;
	sg_dailyTaskMan = NULL;
	if (pDailyTaskMan)
		delete pDailyTaskMan;
}

DailyTaskMan::DailyTaskMan()
{

}

DailyTaskMan::~DailyTaskMan()
{

}

BOOL DailyTaskMan::IsFinishedDailyTask(UINT32 roleId, UINT16 taskId)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select count(id) from dailytask where actorid=%u and taskid=%d", roleId, taskId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return false;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows == 0 || nCols == 0)
	{
		return false;
		DBS_FreeResult(result);
	}

	DBS_ROW row = DBS_FetchRow(result);
	int n = atoi(row[0]);
	DBS_FreeResult(result);
	return n > 0 ? true : false;
}

void DailyTaskMan::FinishedDailyTask(UINT32 roleId, UINT16 taskId)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	char szDate[32] = { 0 };
	GetLocalStrTime(szDate);
	sprintf(szSQL, "insert into dailytask(actorid, taskid, tts) values(%u, %d, '%s')", roleId, taskId, szDate);
	GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
}

void DailyTaskMan::ClearDailyTaskFinishRecord()
{
	GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), "delete from dailytask");
}