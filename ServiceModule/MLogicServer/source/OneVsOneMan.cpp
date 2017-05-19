#include "OneVsOneMan.h"
#include "DBSClient.h"
#include "ServiceProtocol.h"
#include "RolesInfoMan.h"
#include "ConfigFileMan.h"
#include "GameCfgFileMan.h"


static OneVsOneMan* sg_pOneVsOneMan = NULL;
void InitOneVsOneMan()
{
	ASSERT(sg_pOneVsOneMan == NULL);
	sg_pOneVsOneMan = new OneVsOneMan();
	ASSERT(sg_pOneVsOneMan != NULL);
}

OneVsOneMan* GetOneVsOneMan()
{
	return sg_pOneVsOneMan;
}

void DestroyOneVsOneMan()
{
	OneVsOneMan* pOneVsOneMan = sg_pOneVsOneMan;
	sg_pOneVsOneMan = NULL;
	if (pOneVsOneMan)
		delete pOneVsOneMan;
}

// ÿ������
void OneVsOneMan::ClearEveryday()
{
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), "UPDATE onevsone SET num=0,buyNum=0");
}

// ����1v1����
int OneVsOneMan::EnterOneVsOneInst(UINT32 userId, UINT32 roleId)
{
	// ����vip�ȼ�
	SRoleInfos role_info;
	int retCode = GetRolesInfoMan()->GetRoleInfo(userId, roleId, &role_info);
	if (MLS_OK != retCode)
		return retCode;
	BYTE vipLevel = GetGameCfgFileMan()->CalcVIPLevel(role_info.cashcount);

	// ��ȡ�ɽ����������
	UINT16 enter_times = 0;
	GetGameCfgFileMan()->GetOneVsOneEnterTimes(vipLevel, enter_times);

	// ��ѯ�ѽ������
	int buy_num = 0;
	int enter_num = 0;
	DBS_ROW row_data = NULL;
	DBS_RESULT result = NULL;
	bool need_insert = false;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT num,buyNum FROM onevsone WHERE actorid=%u", roleId);
	retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;
	if ((row_data = DBS_FetchRow(result)) != NULL)
	{
		enter_num = atoi(row_data[0]);
		buy_num = atoi(row_data[1]);
	}
	else
	{
		need_insert = true;
	}
	DBS_FreeResult(result);

	// �ﵽ��������
	if (enter_num >= enter_times + buy_num)
	{
		return MLS_ENTER_ONE_VS_ONE_UPPER;
	}

	// д���¼
	++enter_num;
	if (need_insert)
	{
		sprintf(szSQL, "INSERT INTO onevsone VALUES(%u,%d,0)", roleId, enter_num);
		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}
	else
	{
		sprintf(szSQL, "UPDATE onevsone SET num=%d WHERE actorid=%u", enter_num, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}

	return MLS_OK;
}

// ����1v1�����������
int OneVsOneMan::BuyEnterOneVsOneNum(UINT32 userId, UINT32 roleId, int num, SBuy1V1EnterNumRes *pBuyEnterNum)
{
	// ��ѯ�ѹ������
	int buy_num = 0;
	DBS_ROW row_data = NULL;
	DBS_RESULT result = NULL;
	bool need_insert = false;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT buyNum FROM onevsone WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;
	if ((row_data = DBS_FetchRow(result)) != NULL)
	{
		buy_num = atoi(row_data[0]);
	}
	else
	{
		need_insert = true;
	}
	DBS_FreeResult(result);

	// ִ�й���
	retCode = GetRolesInfoMan()->BuyoneVsOneEnterNum(userId, roleId, buy_num, num, pBuyEnterNum);
	if (retCode != MLS_OK)
		return retCode;

	// д���¼
	if (need_insert)
	{
		sprintf(szSQL, "INSERT INTO onevsone VALUES(%u,0,%u)", roleId, pBuyEnterNum->buy_num);
		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}
	else
	{
		sprintf(szSQL, "UPDATE onevsone SET buyNum=%u WHERE actorid=%u", pBuyEnterNum->buy_num, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}

	return MLS_OK;
}

// ��ѯ1v1�����������
int OneVsOneMan::QueryEnterOneVsOneNum(UINT32 userId, UINT32 roleId, SQuery1V1EnterNumRes *pEnterNum)
{
	pEnterNum->buy_num = 0;
	pEnterNum->enter_num = 0;

	DBS_ROW row_data = NULL;
	DBS_RESULT result = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT num,buyNum FROM onevsone WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;
	if ((row_data = DBS_FetchRow(result)) != NULL)
	{
		pEnterNum->enter_num = atoi(row_data[0]);
		pEnterNum->buy_num = atoi(row_data[1]);
	}
	DBS_FreeResult(result);

	return MLS_OK;
}