#include "UnlockContentMan.h"
#include "DBSClient.h"
#include "ServiceProtocol.h"
#include "ServiceErrorCode.h"
#include "ConfigFileMan.h"
#include "cyaMaxMin.h"

static UnlockContentMan* sg_pUnlockMan = NULL;
void InitUnlockContentMan()
{
	ASSERT(sg_pUnlockMan == NULL);
	sg_pUnlockMan = new UnlockContentMan();
	ASSERT(sg_pUnlockMan != NULL);
}

UnlockContentMan* GetUnlockContentMan()
{
	return sg_pUnlockMan;
}

void DestroyUnlockContentMan()
{
	UnlockContentMan* pUnlockMan = sg_pUnlockMan;
	sg_pUnlockMan = NULL;
	if (pUnlockMan)
		delete pUnlockMan;
}

UnlockContentMan::UnlockContentMan()
{

}

UnlockContentMan::~UnlockContentMan()
{

}

int UnlockContentMan::QueryUnlockContent(UINT32 userId, UINT32 roleId, BYTE type, SQueryUnlockContentRes* pUnlockRes)
{
	pUnlockRes->num = 0;

	UINT16 maxNum = (MAX_RES_USER_BYTES - member_offset(SQueryUnlockContentRes, num, data)) / sizeof(UINT16);
	char szSQL[1024] = { 0 };
	if (type == ESGS_UNLOCK_ROLE_JOB)
		sprintf(szSQL, "select jobs from joblock where accountid=%u", userId);
	else if (type == ESGS_UNLOCK_STAGE)
		sprintf(szSQL, "select stages from actorlock where actorid=%u", roleId);
	else if (type == ESGS_UNLOCK_SKILL)
		sprintf(szSQL, "select skills from actorlock where actorid=%u", roleId);
	else if (type == ESGS_UNLOCK_LEVEL)
		sprintf(szSQL, "select levels from actorlock where actorid=%u", roleId);
	else
		return MLS_INVALID_PACKET;

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols <= 0)
	{
		DBS_FreeResult(result);
		return MLS_OK;
	}

	DBS_ROWLENGTH pRowBytes = NULL;
	DBS_ROW row = DBS_FetchRow(result, &pRowBytes);
	if (pRowBytes[0] <= 0)
	{
		DBS_FreeResult(result);
		return MLS_OK;
	}

	BYTE* p = (BYTE*)row[0];
	UINT16 nNum = *(UINT16*)p;
	UINT16 n = min(maxNum, nNum);
	UINT16* pID = (UINT16*)(p + sizeof(UINT16));
	for (UINT16 i = 0; i < n; ++i)
		pUnlockRes->data[i] = pID[i];
	pUnlockRes->num = n;
	DBS_FreeResult(result);
	return MLS_OK;
}

int UnlockContentMan::UnlockContent(UINT32 userId, UINT32 roleId, BYTE type, UINT16 unlockId)
{
	char szDate[32] = { 0 };
	GetLocalStrTime(szDate);
	char szSQL[1024 * 8] = { 0 };
	if (type == ESGS_UNLOCK_ROLE_JOB)
		sprintf(szSQL, "select jobs from joblock where accountid=%u", userId);
	else if (type == ESGS_UNLOCK_STAGE)
		sprintf(szSQL, "select stages from actorlock where actorid=%u", roleId);
	else if (type == ESGS_UNLOCK_SKILL)
		sprintf(szSQL, "select skills from actorlock where actorid=%u", roleId);
	else if (type == ESGS_UNLOCK_LEVEL)
		sprintf(szSQL, "select levels from actorlock where actorid=%u", roleId);
	else
		return MLS_INVALID_PACKET;

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	BOOL isUnlocked = false;
	BOOL isHasRecord = true;
	std::vector<UINT16> vec;
	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols <= 0)
	{
		isHasRecord = false;
	}
	else
	{
		DBS_ROWLENGTH pRowBytes = NULL;
		DBS_ROW row = DBS_FetchRow(result, &pRowBytes);
		if (pRowBytes[0] > 0)
		{
			BYTE* p = (BYTE*)row[0];
			UINT16 nNum = *(UINT16*)p;
			UINT16* pID = (UINT16*)(p + sizeof(UINT16));
			for (UINT16 i = 0; i < nNum; ++i)
			{
				if (unlockId == pID[i])
				{
					isUnlocked = true;
					break;
				}
				vec.push_back(pID[i]);
			}
		}
	}

	DBS_FreeResult(result);

	if (isUnlocked)
		return MLS_OK;

	vec.push_back(unlockId);
	memset(szSQL, 0, sizeof(szSQL));

	int n = 0;
	char szContent[4 * 1024] = { 0 };
	UINT16 si = (UINT16)vec.size();
	n += DBS_EscapeString(szContent + n, (const char*)&si, sizeof(UINT16));
	if (si > 0)
		DBS_EscapeString(szContent + n, (const char*)vec.data(), si * sizeof(UINT16));

	if (type == ESGS_UNLOCK_ROLE_JOB)
	{
		if (isHasRecord)
			sprintf(szSQL, "update joblock set jobs='%s', tts='%s' where accountid=%u", szContent, szDate, userId);
		else
			sprintf(szSQL, "insert into joblock(accountid, jobs, tts) values(%u, '%s', '%s')", userId, szContent, szDate);
	}
	else if (type == ESGS_UNLOCK_STAGE)
	{
		if (isHasRecord)
			sprintf(szSQL, "update actorlock set stages='%s', stages_tts='%s' where actorid=%u", szContent, szDate, roleId);
		else
			sprintf(szSQL, "insert into actorlock(actorid, stages, stages_tts) values(%u, '%s', '%s')", roleId, szContent, szDate);
	}
	else if (type == ESGS_UNLOCK_SKILL)
	{
		if (isHasRecord)
			sprintf(szSQL, "update actorlock set skills='%s', skills_tts where actorid=%u", szContent, szDate, roleId);
		else
			sprintf(szSQL, "insert into actorlock(actorid, skills, skills_tts) values(%u, '%s', '%s')", roleId, szContent, szDate);
	}
	else if (type == ESGS_UNLOCK_LEVEL)
	{
		if (isHasRecord)
			sprintf(szSQL, "update actorlock set levels='%s', levels_tts='%s' where actorid=%u", szContent, szDate, roleId);
		else
			sprintf(szSQL, "insert into actorlock(actorid, levels, levels_tts) values(%u, '%s', '%s')", roleId, szContent, szDate);
	}
	else
		ASSERT(false);

	if (isHasRecord)
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	else
		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);

	return MLS_OK;
}
