#include <algorithm>
#include "ActitvityMan.h"
#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "ServiceProtocol.h"
#include "cyaMaxMin.h"
#include "RolesInfoMan.h"

static ActitvityMan* sg_pActitvityMan = NULL;
void InitActitvityMan()
{
	ASSERT(sg_pActitvityMan == NULL);
	sg_pActitvityMan = new ActitvityMan();
	ASSERT(sg_pActitvityMan != NULL);
}

ActitvityMan* GetActitvityMan()
{
	return sg_pActitvityMan;
}

void DestroyActitvityMan()
{
	ActitvityMan* pActitvityMan = sg_pActitvityMan;
	sg_pActitvityMan = NULL;
	if (pActitvityMan)
		delete pActitvityMan;
}

ActitvityMan::ActitvityMan()
{
}

ActitvityMan::~ActitvityMan()
{
}

int ActitvityMan::QueryReceivedActivity(UINT32 roleId, SQueryReceivedActivityRes* pActivityRes)
{
	UINT16 maxNum = (UINT16)((MAX_RES_USER_BYTES - member_offset(SQueryReceivedActivityRes, num, data)) / sizeof(UINT32));

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select activityid from activity where actorid=%u", roleId);
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

	UINT16 n = min(maxNum, (UINT16)nRows);
	DBS_ROW row = NULL;
	while ((row = DBS_FetchRow(result)) != NULL)
	{
		pActivityRes->data[pActivityRes->num] = (UINT32)_atoi64(row[0]);
		if (++pActivityRes->num >= n)
			break;
	}

	DBS_FreeResult(result);
	return retCode;
}

BOOL ActitvityMan::IsReceivedActivity(UINT32 roleId, UINT32 activityId)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select count(id) from activity where actorid=%u and activityid=%u", roleId, activityId);
	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return false;

	UINT32 nRows = DBS_NumRows(result);
	if (nRows == 0)
	{
		DBS_FreeResult(result);
		return false;
	}

	int n = 0;
	DBS_ROW row = DBS_FetchRow(result);
	if (row != NULL)
		n = atoi(row[0]);
	DBS_FreeResult(result);
	return n > 0 ? true : false;
}

void ActitvityMan::RefreshReceivedActivity(UINT32 roleId, UINT32 activityId, const char* pszDesc)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	char szDate[32];
	GetLocalStrTime(szDate);
	sprintf(szSQL, "insert into activity(actorid, activityid, desc, tts) values(%u, %u, '%s', '%s')", roleId, activityId, pszDesc, szDate);
	GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
}

int ActitvityMan::VerifyCashCode(const char* pszCode, UINT32 roleId, UINT16& rewardId)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select rewardid, tts from cashcode where id='%s' and status=0", pszCode);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 2)		//已经领取过的兑换码
	{
		DBS_FreeResult(result);
		return MLS_INVALID_CASH_CODE;
	}

	DBS_ROW row = DBS_FetchRow(result);
	rewardId = atoi(row[0]);
	LTMSEL tts = LocalStrTimeToMSel(row[1]);
	DBS_FreeResult(result);
	if (GetMsel() > tts)	//兑换码过期
		return MLS_CASH_CODE_EXPIRED;

	//玩家是否领取过该类型的兑换码
	UINT32 receiveNum = 0;
	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "select count(*) from cashcode where receiveid=%u and rewardid=%d", roleId, rewardId);
	retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	row = DBS_FetchRow(result);
	if (row != NULL)
		receiveNum = _atoi64(row[0]);
	DBS_FreeResult(result);
	if (receiveNum > 0)
		return MLS_ALREADY_RECEIVED_ACTIVITY;

	//更新领取状态
	char szDate[32] = { 0 };
	GetLocalStrTime(szDate);
	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "update cashcode set receiveid=%u, status=%d, cashtts='%s' where id='%s'", roleId, 1, szDate, pszCode);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	return retCode;
}

int ActitvityMan::GetRouletteReward(UINT32 roleId, SGetRoulettePropRes* pRoulettePropRes)
{
	char szSQL[4 * GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select times, props from roulette where actorid=%u", roleId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 2)
	{
		std::vector<RouletteCfg::SRoulettePropCfgItem> saveDBItemVec;
		GetGameCfgFileMan()->GenerateRouletteReward(pRoulettePropRes, saveDBItemVec);
		ASSERT(pRoulettePropRes->num == saveDBItemVec.size());
		pRoulettePropRes->getNum = 0;
		memset(szSQL, 0, sizeof(szSQL));

		char pszBuf[8 * GENERIC_SQL_BUF_LEN] = { 0 };
		int n = DBS_EscapeString(szSQL, (const char*)&pRoulettePropRes->num, sizeof(BYTE));
		if (pRoulettePropRes->num > 0)
			DBS_EscapeString(szSQL + n, (const char*)saveDBItemVec.data(), pRoulettePropRes->num * sizeof(RouletteCfg::SRoulettePropCfgItem));

		char szDate[32] = { 0 };
		GetLocalStrTime(szDate);
		sprintf(pszBuf, "insert into roulette(actorid, times, props, tts) values(%u, %d, '%s', '%s')", roleId, 0, szSQL, szDate);
		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), pszBuf, NULL);
	}
	else
	{
		DBS_ROW row = DBS_FetchRow(result);
		pRoulettePropRes->getNum = atoi(row[0]);
		BYTE* p = (BYTE*)row[1];
		BYTE nCount = *p;

		RouletteCfg::SRoulettePropCfgItem* pData = (RouletteCfg::SRoulettePropCfgItem*)(p + sizeof(BYTE));
		for (BYTE i = 0; i < nCount; ++i)
		{
			pRoulettePropRes->data[i].id = pData[i].id;
			pRoulettePropRes->data[i].num = pData[i].num;
			pRoulettePropRes->data[i].propId = pData[i].propId;
			pRoulettePropRes->data[i].type = pData[i].type;
		}
		pRoulettePropRes->num = nCount;
	}
	DBS_FreeResult(result);
	return retCode;
}

int ActitvityMan::GetRouletteReward(UINT32 roleId, UINT16& extractNum, std::vector<RouletteCfg::SRoulettePropCfgItem>& itemVec)
{
	char szSQL[4 * GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select times, props from roulette where actorid=%u", roleId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 2)
	{
		DBS_FreeResult(result);
		return MLS_OK;
	}

	DBS_ROW row = DBS_FetchRow(result);
	ASSERT(row != NULL);

	extractNum = atoi(row[0]);
	BYTE* p = (BYTE*)row[1];
	BYTE nCount = *p;
	RouletteCfg::SRoulettePropCfgItem* pData = (RouletteCfg::SRoulettePropCfgItem*)(p + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
		itemVec.push_back(pData[i]);

	DBS_FreeResult(result);
	return MLS_OK;
}

int ActitvityMan::UpdateRoleRouletteReward(UINT32 roleId, UINT16 extractNum, const std::vector<RouletteCfg::SRoulettePropCfgItem>& itemVec)
{
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update roulette set times=%d, props='", extractNum);

	BYTE si = (BYTE)itemVec.size();
	int n = (int)strlen(szSQL);
	n += DBS_EscapeString(szSQL + n, (const char*)&si, sizeof(BYTE));
	if (si > 0)
		DBS_EscapeString(szSQL + n, (const char*)itemVec.data(), si * sizeof(RouletteCfg::SRoulettePropCfgItem));

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	return MLS_OK;
}

int ActitvityMan::FetchRouletteBigReward(UINT32 roleId, UINT16& extractNum, SRoulettePropItem& bigReward)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select props, times from roulette where actorid=%u", roleId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 2)
	{
		DBS_FreeResult(result);
		return MLS_RECEIVED_BIG_REWARD;
	}

	DBS_ROW row = DBS_FetchRow(result);
	if (row == NULL)
	{
		DBS_FreeResult(result);
		return MLS_RECEIVED_BIG_REWARD;
	}

	BYTE* p = (BYTE*)row[0];
	BYTE nCount = *p;
	if (nCount > 1)
	{
		DBS_FreeResult(result);
		return MLS_NOT_FIN_EXTRACT;
	}

	extractNum = atoi(row[1]);
	RouletteCfg::SRoulettePropCfgItem* pData = (RouletteCfg::SRoulettePropCfgItem*)(p + sizeof(BYTE));
	bigReward.id = pData[0].id;
	bigReward.num = pData[0].num;
	bigReward.propId = pData[0].propId;
	bigReward.type = pData[0].type;
	DBS_FreeResult(result);
	return MLS_OK;
}

int ActitvityMan::UpdateRouletteBigReward(UINT32 roleId, const SRoulettePropItem& bigReward)
{
	UINT16 extractNum = 0;
	std::vector<RouletteCfg::SRoulettePropCfgItem> itemVec;
	GetRouletteReward(roleId, extractNum, itemVec);
	if (itemVec.empty())
		return MLS_OK;

	itemVec[0].id = bigReward.id;
	itemVec[0].num = bigReward.num;
	itemVec[0].propId = bigReward.propId;
	itemVec[0].type = bigReward.type;
	itemVec[0].chance = 0;
	return UpdateRoleRouletteReward(roleId, extractNum, itemVec);
}

void ActitvityMan::ClearRouletteReward()
{
	GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), "delete from roulette");
}

/* zpy 查询角色签到信息 */
int ActitvityMan::QueryRoleDailySignFromDB(UINT32 roleId, std::vector<SDBSign> &out_days)
{
	out_days.clear();

	DBS_RESULT result = NULL;
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT userdata FROM dailysign WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK) return retCode;

	DBS_ROWLENGTH row_bytes = 0;
	if (DBS_ROW row = DBS_FetchRow(result, &row_bytes))
	{
		const unsigned int modulo = *row_bytes % sizeof(SDBSign);
		if (modulo == 0)
		{
			unsigned int size = *row_bytes / sizeof(SDBSign);
			SDBSign *pItem = (SDBSign *)row[0];
			for (int i = 0; i < size; ++i)
			{
				out_days.push_back(pItem[i]);
			}
		}
	}
	DBS_FreeResult(result);
	return MLS_OK;
}

/* zpy 写入角色签到信息 */
int ActitvityMan::WriteRoleDailySignToDB(INT32 roleId, std::vector<SDBSign> &days)
{
	if (days.empty()) return MLS_OK;

	UINT32 affectRows = 0;
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	strcpy(szSQL, "update dailysign set userdata='");
	int n = (int)strlen(szSQL);
	DBS_EscapeString(szSQL + n, (const char*)days.data(), days.size() * sizeof(SDBSign));

	char szId[64] = { 0 };
	sprintf(szId, "' where actorid=%u", roleId);
	strcat(szSQL, szId);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
	if (affectRows == 0)
	{
		memset(szSQL, 0, sizeof(szSQL));
		sprintf(szSQL, "INSERT INTO dailysign VALUES(%u,'", roleId);
		n = (int)strlen(szSQL);
		DBS_EscapeString(szSQL + n, (const char*)days.data(), days.size() * sizeof(SDBSign));

		strcat(szSQL, "')");
		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}
	return MLS_OK;
}

/* zpy 清理每日签到 */
void ActitvityMan::ClearRoleDailySign()
{
	GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), "delete from dailysign");
}

/* zpy 查询签到 */
int ActitvityMan::QueryRoleDailySign(UINT32 roleId, SQueryRoleDailySignRes *pQueryRoleSign)
{
	std::vector<SDBSign> days;
	int retCode = QueryRoleDailySignFromDB(roleId, days);
	if (retCode != MLS_OK) return retCode;

	// 获取系统当前时间
	SYSTEMTIME lcTime;
	GetLocalTime(&lcTime);

	pQueryRoleSign->month = lcTime.wMonth;
	pQueryRoleSign->num = days.size();
	for (size_t i = 0; i < days.size(); ++i)
	{
		pQueryRoleSign->data[i] = days[i].day;
	}
	pQueryRoleSign->now_signed = days.empty() ? false : lcTime.wDay == days.back().day;
	return MLS_OK;
}

/* zpy 角色签到 */
int ActitvityMan::OnRoleDailySign(INT32 userID, UINT32 roleId, SRoleDailySignRes *pRoleSign, SReceiveRewardRefresh *pRefresh)
{
	// 判断是否开启每日签到活动
	if (!GetGameCfgFileMan()->IsDailySignValid())
	{
		return MLS_NOT_EXIST_ACTIVITY;
	}

	// 获取系统当前时间
	SYSTEMTIME lcTime;
	GetLocalTime(&lcTime);

	std::vector<SDBSign> days;
	int retCode = QueryRoleDailySignFromDB(roleId, days);
	if (retCode != MLS_OK) return retCode;

	// 是否已签到
	for (size_t i = 0; i < days.size(); ++i)
	{
		if (days[i].day == lcTime.wDay) return MLS_TODAY_HAS_SIGNED;
	}

	// 计算是否连续签到
	bool continuous = false;
	if (!days.empty())
	{
		std::sort(days.begin(), days.end());
		if ((days.back().day == lcTime.wDay - 1) && (days.size() == lcTime.wDay - 1))
		{
			continuous = true;
		}
	}

	// 发放奖励
	retCode = GetRolesInfoMan()->ReceiveDailySignReward(userID, roleId, days.size() + 1, continuous, pRoleSign, pRefresh);
	if (retCode != MLS_OK) return retCode;

	// 写入数据库信息
	SDBSign sign;
	sign.day = lcTime.wDay;
	days.push_back(sign);
	return WriteRoleDailySignToDB(roleId, days);
}


