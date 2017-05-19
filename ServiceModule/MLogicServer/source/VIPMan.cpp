#include <set>
#include "VIPMan.h"
#include "ServiceErrorCode.h"
#include "DBSClient.h"
#include "RoleInfos.h"
#include "ConfigFileMan.h"

static VIPMan* sg_VIPMan = NULL;
void InitVIPMan()
{
	ASSERT(sg_VIPMan == NULL);
	sg_VIPMan = new VIPMan();
	ASSERT(sg_VIPMan != NULL);
}

VIPMan* GetVIPMan()
{
	return sg_VIPMan;
}

void DestroyVIPMan()
{
	VIPMan* pVIPMan = sg_VIPMan;
	sg_VIPMan = NULL;
	if (pVIPMan != NULL)
		delete pVIPMan;
}

VIPMan::VIPMan()
{

}

VIPMan::~VIPMan()
{

}

int VIPMan::QueryRoleNotReceiveVIPReward(UINT32 roleId, BYTE vipLv, SQueryNotReceiveVIPRewardRes* pVIPLv)
{
	std::set<BYTE> vips;
	for (BYTE i = 1; i <= vipLv; ++i)
		vips.insert(i);

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select vip from vip where actorid=%u", roleId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows > 0 && nCols == 1)
	{
		DBS_ROW row = NULL;
		while ((row = DBS_FetchRow(result)) != NULL)
		{
			BYTE vip = (BYTE)atoi(row[0]);
			std::set<BYTE>::iterator it = vips.find(vip);
			if (it != vips.end())
				vips.erase(it);
		}
	}
	DBS_FreeResult(result);

	BYTE idx = 0;
	pVIPLv->num = (BYTE)vips.size();
	std::set<BYTE>::iterator it = vips.begin();
	for (; it != vips.end(); ++it)
		pVIPLv->vipLv[idx++] = *it;

	return retCode;
}

void VIPMan::RecordRoleReceiveVIPReward(UINT32 roleId, BYTE vipLv)
{
	char pszNowDate[32] = { 0 };
	GetLocalStrTime(pszNowDate);
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "insert into vip(actorid, vip, reward, tts) values(%u, %d, %d, '%s')", roleId, vipLv, 1, pszNowDate);
	GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
}

BOOL VIPMan::IsReceivedVIPReward(UINT32 roleId, BYTE vipLv)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select vip from vip where actorid=%u and vip=%d", roleId, vipLv);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return false;

	UINT32 nRows = DBS_NumRows(result);
	DBS_FreeResult(result);

	return nRows > 0 ? true : false;
}