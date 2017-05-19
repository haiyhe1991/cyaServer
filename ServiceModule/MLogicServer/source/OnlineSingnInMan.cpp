#include "OnlineSingnInMan.h"
#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "RolesInfoMan.h"

static OnlineSingnInMan* sg_pSignInMan = NULL;
void InitOnlineSingnInMan()
{
	ASSERT(sg_pSignInMan == NULL);
	sg_pSignInMan = new OnlineSingnInMan();
	ASSERT(sg_pSignInMan != NULL);
}

OnlineSingnInMan* GetOnlineSingnInMan()
{
	return sg_pSignInMan;
}

void DestroyOnlineSingnInMan()
{
	OnlineSingnInMan* pSignInMan = sg_pSignInMan;
	sg_pSignInMan = NULL;
	if (pSignInMan)
		delete pSignInMan;
}

OnlineSingnInMan::OnlineSingnInMan()
{

}

OnlineSingnInMan::~OnlineSingnInMan()
{

}

int OnlineSingnInMan::QueryRoleSignIn(UINT32 roleId, std::vector<LTMSEL>& signVec)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select signs from signin where actorid=%u", roleId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	if (nRows == 0)
	{
		DBS_FreeResult(result);
		return MLS_OK;
	}

	DBS_ROWLENGTH pRowLen = NULL;
	DBS_ROW row = DBS_FetchRow(result, &pRowLen);
	ASSERT(row != NULL);
	if (row == NULL || pRowLen[0] <= 0)
	{
		DBS_FreeResult(result);
		return MLS_OK;
	}

	BYTE* p = (BYTE*)row[0];
	BYTE n = *p;
	LTMSEL* pMSel = (LTMSEL*)(p + sizeof(BYTE));
	for (BYTE i = 0; i < n; ++i)
		signVec.push_back(pMSel[i]);

	DBS_FreeResult(result);
	return retCode;
}

void OnlineSingnInMan::RefreshSignInInfo(UINT32 userId, UINT32 roleId)
{
	std::vector<LTMSEL> signVec;
	int retCode = GetRolesInfoMan()->GetRoleSignInInfo(userId, roleId, signVec);
	if (retCode != MLS_OK)
		return;

	char pszSigns[GENERIC_SQL_BUF_LEN] = { 0 };
	BYTE si = (BYTE)signVec.size();
	int n = DBS_EscapeString(pszSigns, (const char*)&si, sizeof(BYTE));
	if (si > 0)
		DBS_EscapeString(pszSigns + n, (const char*)signVec.data(), si * sizeof(LTMSEL));

	UINT32 affectRows = 0;
	char szSQL[GENERIC_SQL_BUF_LEN * 2] = { 0 };
	sprintf(szSQL, "update signin set signs='%s' where actorid=%u", pszSigns, roleId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
	if (affectRows == 0)
	{
		memset(szSQL, 0, sizeof(szSQL));
		sprintf(szSQL, "insert into signin(actorid, signs) values(%u, '%s')", roleId, pszSigns);
		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}
}