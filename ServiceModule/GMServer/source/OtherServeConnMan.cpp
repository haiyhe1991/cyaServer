#include "OtherServeConnMan.h"
#include "ServiceProtocol.h"
#include "AES.h"
#include "cyaIpcvt.h"
#include "cyaLog.h"
#include "cyaFdk.h"
#include <fstream>
#include "cyaLog.h"
#include "ServiceErrorCode.h"
#include "GMCfgMan.h"
#include "ServeConnMan.h"
#include "ServiceCommon.h"
#include "gb_utf8.h"


/*****************��ǰDBδ����������**********************/


#define IS_RESULT_NULL() (m_update.GetNewDBSResult().size() ==0?1:0) //db ��ѯ����Ƿ�Ϊ��


COherServeConnMan* g_otherServeConnObj = NULL;

const char* account_tb[] = { "player_account_1", "player_account_2", "player_account_3", "player_account_4", "player_account_5" };

COherServeConnMan::COherServeConnMan()
	:m_olineTimer(GetOnlineUserByTimer, this, GetCfgManObj()->GetRefreshTime() * 1000),
	m_realTimeNew(GetRealTimeNewByTimer, this, GetCfgManObj()->GetRefrNewTime() * 1000),
	//
	m_reflushLogTimer(ReflushOnlineLogByTimer, this, 60 * 60 * 1000),
	m_isRun(TRUE)
{
	if (!GetCfgManObj()->IsTest())
	{
		//���߳�;
		OSCreateThread(&m_osthrd, &m_osthrdId, CheckEndOfEveryDayThread, this, 0);
		m_olineTimer.Start();
		m_realTimeNew.Start();

	}

}

COherServeConnMan::~COherServeConnMan()
{
	m_isRun = FALSE;
	OSCloseThread(m_osthrd);
	m_olineTimer.Stop(TRUE);
	m_reflushLogTimer.Stop(TRUE);
	DestroyServeConnObj();

}

int COherServeConnMan::CheckEndOfEveryDayThread(void* p)
{
	ASSERT(p != NULL);
	COherServeConnMan* pThis = (COherServeConnMan*)p;
	pThis->OnCheckRunning();
	return 0;
}

void COherServeConnMan::OnCheckRunning()
{
	char nowTime[20] = { 0 };
	char stime[20] = { 0 };

	while (FALSE)
	{
		if (!m_isRun)
			break;
		GetSysCurrentTime(nowTime);
		GetTime(nowTime, stime, FALSE);
		/*memset(nowTime, 0, 20);
		//strcpy(nowTime, "2015-09-17 23:59:59");
		strcpy(nowTime, "0000-00-00 00:00:00");*/

		if (strcmp(stime, GetCfgManObj()->GetdbsTime()) == 0)
		{
			if (IS_RUN_STORE_LK)
				m_update.UpdateAddupNewData(nowTime);
		}
		else
			Sleep(800);
	}

}

//ʵʱ����;
void COherServeConnMan::OnNewRuning()
{
	char updateTime[20] = { 0 };
	GetSysCurrentTime(updateTime);
	std::vector<UINT16> platid = GetCfgManObj()->GetAllPlatId();
	std::vector<UINT16> ChannelId = GetCfgManObj()->GetAllchannel();
	for (int j = 0; j<(int)platid.size(); j++)
	{
		for (int i = 0; i<(int)ChannelId.size(); i++)
		{
			//ʵʱ���µ�����;
			//���������ʺ�����
			m_update.UpdateAddupNewAccountMan(platid[j], ChannelId[i], updateTime, "accountid", 1);
			//���������豸����
			m_update.UpdateAddupNewAccountMan(platid[j], ChannelId[i], updateTime, "accountid", 2);
			//����������ɫ����
			m_update.UpdateAddupNewActorMan(platid[j], ChannelId[i], updateTime);
			//�����ʺ�����
			m_update.UpdateAccountData(platid[j], ChannelId[i], updateTime);
			//���½�ɫ����
			m_update.UpdateActorData(platid[j], ChannelId[i], updateTime);
		}
	}
}

void COherServeConnMan::OnGetOnlineUser()
{
	char stime[20] = { 0 };
	GetSysCurrentTime(stime);
	std::vector<UINT16> platid = GetCfgManObj()->GetAllPlatId();
	std::vector<UINT16> channelId = GetCfgManObj()->GetAllchannel();
	for (int j = 0; j<(int)platid.size(); j++)
	{
		for (int i = 0; i < (int)channelId.size(); i++)
			m_update.UpdateCommOnlineMan(platid[j], channelId[i], stime);
	}
}

int COherServeConnMan::QLoginCount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count, BYTE type)
{
	char sql[1024] = { 0 };
	int ret = 0;
	if (type == 1)
		return GetAccount(platId, channelId, partId, startTime, endTime, "lasttts", count);
	else if (type == 2)
		return GetDevice(platId, channelId, partId, startTime, endTime, "lasttts", count);
	else if (type == 3)
		return GetActor(platId, channelId, partId, startTime, endTime, "lvtts", count);

	if (*count != 0)
		return ret;

	std::string str;
	if (type == 1)
		str = "account_leavekeep";
	else if (type == 2)
		str = "device_leavekeep";
	else if (type == 3)
		str = "actor_leavekeep";

	sprintf(sql, "select sum(uplgcount) from %s  where  uptts >= '%s' \
				 		and uptts <= '%s'", str.c_str(), startTime, endTime);

	std::map<UINT16, UINT32> extRes;
	ret = m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGSINFO, sql, extRes, 0);
	std::map<UINT16, UINT32>::iterator it = extRes.find(partId);
	if (it != extRes.end())
		*count = it->second;
	extRes.clear();
	m_update.ClearNewDBSResult();
	return ret;
}

BOOL COherServeConnMan::IsNow(const char* startTime, const char* endTime)
{
	char nowTime[20] = { 0 };
	char bgTime[20] = { 0 };
	char edTime[20] = { 0 };

	GetDate(nowTime, nowTime, TRUE);
	if (strcmp(startTime, "") != 0 && strcmp(endTime, "") == 0)
	{
		if (strcmp(nowTime, bgTime) == 0)
			return TRUE;

	}
	else if (strcmp(startTime, "") == 0 && strcmp(endTime, "") != 0)
	{
		if (strcmp(nowTime, bgTime) == 0)
			return TRUE;
	}
	else if (strcmp(startTime, "") == 0 && strcmp(endTime, "") == 0)
		return TRUE;



	GetDate(startTime, bgTime, FALSE);
	GetDate(endTime, edTime, FALSE);

	if (strcmp(nowTime, bgTime) == 0 && strcmp(nowTime, edTime) == 0)
		return TRUE;
	return FALSE;
}

BOOL COherServeConnMan::IsSameDay(const char* startTime, const char* endTime)
{
	char bgTime[20] = { 0 };
	char edTime[20] = { 0 };

	GetDate(startTime, bgTime, FALSE);
	GetDate(endTime, edTime, FALSE);

	if (strcmp(bgTime, edTime) == 0)
		return TRUE;
	return FALSE;

}

///����ͳ�ơ������ݽӿ�;
int COherServeConnMan::GetLeaveKeepStatistics(UINT16 platId, UINT16 channelId/*����*/, UINT16 partId, const char* starTime, const char* endTime, BYTE lktype, BYTE lkTimeType, std::vector<GMQLeavekeepDateInfo>&LeaveKeepInfo, UINT32* getcount)
{
	std::vector<UINT16> channedl = GetCfgManObj()->GetAllchannel();
	int i = 0;
	bool addswitch = false;
	UINT16 ifchannelid = channelId;

	do
	{
		addswitch = false;
		if (channelId == 0)
		{
			addswitch = true;
			if (i<channedl.size())
				ifchannelid = channedl[i];
			else	break;

		}

		char FromTime[20] = { 0 };	//��ʼ��ѯ����,����from������;0���ʾΪstartTime
		char FromToTime[20] = { 0 };//������ѯʱ�䣬����to��������0���ʾendTime����ʱ��;
		//��ѯʱ��ȷ��;
		int ret = 0;
		*getcount = (UINT32)ResolveTimeNum(starTime, endTime/*,from,to,FromTime,FromToTime*/);
		if (*getcount == GM_QUERY_TIME)
			return GM_QUERY_TIME;
		strcpy(FromTime, starTime);		//���ƿ�ʼ��ѯʱ��;
		strcpy(FromToTime, starTime);

		for (int i = 0; i<*getcount; i++)
		{
			GoToTime(FromTime, FromToTime, i, true);
			if (lktype == 1)//�ʺ�����
				ret = GetIfLeaveKeep(platId, ifchannelid, partId, FromTime, LK_ACCOUNT, lkTimeType, LeaveKeepInfo, i);
			else if (lktype == 2)//�豸����
				ret = GetIfLeaveKeep(platId, ifchannelid, partId, FromTime, LK_DEVICE, lkTimeType, LeaveKeepInfo, i);
			else if (lktype == 3)//��ɫ����
				ret = GetIfLeaveKeep(platId, ifchannelid, partId, FromTime, LK_ACTOR, lkTimeType, LeaveKeepInfo, i);

			if (ret != GM_OK && lktype == 1)
			{
				LogInfo(("��ѯ[��ȡ�ʺ�����]ʧ��:[%d]\n", GM_QUERY_ACCOUNT_LK_FAILED));
				return GM_QUERY_ACCOUNT_LK_FAILED;
			}
			if (ret != GM_OK && lktype == 2)
			{
				LogInfo(("��ѯ[��ȡ�豸����]ʧ��:[%d]\n", GM_QUERY_DEVICE_LK_FAILED));
				return GM_QUERY_DEVICE_LK_FAILED;
			}
			if (ret != GM_OK && lktype == 3)
			{
				LogInfo(("��ѯ[��ȡ��ɫ����]ʧ��:[%d]\n", GM_QUERY_ACTOR_LK_FAILED));
				return GM_QUERY_ACTOR_LK_FAILED;
			}

		}

	} while (addswitch);

	channedl.clear();
	return 0;
}

///���桪�������ѯ����;
int COherServeConnMan::GetIfLeaveKeep(UINT16 platId, UINT16 channelId, UINT16 partId, const char* stime, const char* tbName, BYTE lkTimeType, std::vector<GMQLeavekeepDateInfo>&LeaveKeepInfo, int ic)
{

	GMQLeavekeepDateInfo DateInfo;
	memset(&DateInfo, 0, sizeof(GMQLeavekeepDateInfo));
	LeaveKeepInfo.push_back(DateInfo);
	UINT32 NewRecount = 0;
	int GetLkep = 0;
	int ret = 0;
	for (int i = 0; GetLkep <= lkTimeType; i++)
	{
		GetLkep = GetCfgManObj()->GetLeaveKeepTimeType()[i] + 1;
		//��ȡ��ѯʱ�䣬����;ѭ���������;
		LeaveKeepInfo[ic].retNum = LeaveKeepInfo[ic].retNum + 1;
		ret = GetLeaveKeep(platId, channelId, partId, stime, tbName, GetCfgManObj()->GetLeaveKeepTimeType()[i], &LeaveKeepInfo[ic].Retained[i], i, &NewRecount);
		//��ȡ������;
		ret = LeaveRate(platId, channelId, partId, stime, tbName, GetCfgManObj()->GetLeaveKeepTimeType()[i], ic, &LeaveKeepInfo[ic].rate[i]);
	}
	strcpy(LeaveKeepInfo[ic].LeaveTime, stime);
	LeaveKeepInfo[ic].NewRecount = NewRecount;
	if (ret != GM_OK)
	{
		return ret;
	}
	return 0;
}

///���Ѵ���
int COherServeConnMan::CashDataMan(UINT16 platId, UINT16 channelId, UINT16 partId, BYTE operType, const char* startTime, const char* endTime, UINT32& count)
{

	char start[125] = { 0 };
	char endt[125] = { 0 };
	GetOtherServeManObj()->CheckTimeB(startTime, endTime, NULL, "confirmtts", start, endt, NULL, NULL);
	char sql[1024 * 2] = { 0 };
	sprintf(sql, "select ");
	for (int i = 1; i < 6; i++)
	{
		switch (operType)
		{
		case PY_CASH_ACTOR:
			if (i == 5)
				sprintf(sql, "%s (select count(distinct actorid) from player_cash_%u where  \
							 							 (status = 1)  and (partid=%d) %s %s)  ", sql, i, partId, start, endt);
			else
				sprintf(sql, "%s (select count(distinct actorid) from player_cash_%u where  \
							 							 (status = 1)  and (partid=%d) %s %s) + ", sql, i, partId, start, endt);
			break;
		case PY_CASH_COUNT:
			if (i == 5)
				sprintf(sql, "%s (select count(*) from player_cash_%u where  \
							 							 (status = 1)  and (partid=%d) %s %s)", sql, i, partId, start, endt);
			else
				sprintf(sql, "%s (select count(*) from player_cash_%u where  \
							 							 (status = 1)  and (partid=%d) %s %s) + ", sql, i, partId, start, endt);
			break;
		case PY_CASH_TOTAL:
			if (i == 5)
				sprintf(sql, "%s (select count(distinct accountid) from player_cash_%u where  \
							 							 (status = 1)  and (partid=%d) %s %s)", sql, i, partId, start, endt);
			else
				sprintf(sql, "%s (select count(distinct accountid) from player_cash_%u where  \
							 							 (status = 1)  and (partid=%d) %s %s) + ", sql, i, partId, start, endt);
			break;
		case PY_CASH_NRMB:
			if (i == 5)
				sprintf(sql, "%s (select ifnull(sum(rmb),0) from player_cash_%u where  \
							 							 (status = 1)  and (partid=%d) %s %s)", sql, i, partId, start, endt);
			else
				sprintf(sql, "%s (select ifnull(sum(rmb),0) from player_cash_%u where  \
							 							 (status = 1)  and (partid=%d) %s %s) + ", sql, i, partId, start, endt);
			break;
		case PY_CASH_CRMB:
			if (i == 5)
				sprintf(sql, "%s (select ifnull(sum(rmb),0) from player_cash_%u where  \
							 							 (status= 1)  and (partid=%d) %s %s)", sql, i, partId, start, endt);
			else
				sprintf(sql, "%s (select ifnull(sum(rmb),0) from player_cash_%u where  \
							 							 (status= 1)  and (partid=%d) %s %s) + ", sql, i, partId, start, endt);
			break;
		case PY_CASH_TORMB:
			if (i == 5)
				sprintf(sql, "%s (select ifnull(sum(rmb),0) from player_cash_%u where  \
							 							 (status = 1)  and (partid=%d) %s %s)", sql, i, partId, start, endt);
			else
				sprintf(sql, "%s (select ifnull(sum(rmb),0) from player_cash_%u where  \
							 							 (status = 1)  and (partid=%d) %s %s) + ", sql, i, partId, start, endt);
			break;
		default:
			break;
		}

	}
	m_update.ExecuteNewRechargeDbQuerySql(DBS_MSG_QUERY, DB_SGSRECHARGE, sql, &count);
	memset(sql, 0, 1024 * 2);
	m_update.ClearNewDBSResult();
	return count;
}


///�׳��û�����
int COherServeConnMan::FirstRechargeMan(UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count)
{
	*count = 0;
	char start[125] = { 0 };
	char endt[125] = { 0 };
	GetOtherServeManObj()->CheckTime(startTime, endTime, "confirmtts", start, endt);
	char sql[1024] = { 0 };
	std::map<UINT16, UINT32> extRes;
	sprintf(sql, "select count(distinct actorid) from recharge where  status=1 %s %s", start, endt);
	m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGS, sql, extRes, 0);
	std::map<UINT16, UINT32>::iterator it = extRes.find(partId);
	if (it != extRes.end())
		*count = it->second;
	m_update.ClearNewDBSResult();
	return 0;
}

int COherServeConnMan::GetAccountInfo(UINT16 partId, UINT32 from, BYTE to, BYTE isNow, const char* start, const char* endt, char extSql[1024 * 2], char sql[1024 * 2])
{
	if (isNow)
	{
		for (int i = 1; i < 6; i++)
		{
			if (i == 5)
				sprintf(sql, "%s (select id, user, pwd, ip, regtts, platsource, os, model, \
							 							 regdev, ver, visit, lasttts, partid, status from player_account_%u \
														 							 where  (partid=%d) %s %s ) limit %u, %u", sql, i, partId, start, endt, from, to);
			else
				sprintf(sql, "%s (select id, user, pwd, ip, regtts, platsource, os, model, regdev, \
							 							 ver, visit, lasttts, partid, status from player_account_%u \
														 							 where  (partid=%d) %s %s ) union  ", sql, i, partId, start, endt);
		}
	}
	else
	{
		if (from == 0)
			sprintf(extSql, "select count(*) from account_info where partid=%d %s %s ", partId, start, endt);

		sprintf(sql, "select accountid, user, pwd, ip, regtts, platsource, os, model, regdev, ver, visit, \
					 						 lasttts, partid, status from account_info where partid=%d %s %s  limit %u, %u", partId, start, endt, from, to);
	}
	return 0;
}

int COherServeConnMan::GetActorInfo(UINT16 partId, UINT32 from, BYTE to, BYTE isNow, const char* start, const char* endt, char extSql[1024 * 2], char sql[1024 * 2])
{
	if (isNow)
	{
		if (from == 0)
			sprintf(extSql, "select count(*) from actor where 1=1 %s %s ", start, endt);

		sprintf(sql, "select id, createtts, accountid, nick, sex, gold, level, occu, exp, \
					 						 actortype, hp, mp, attack, def, vit, cp, ap, faction, vist, online, \
											 						 lvtts from actor where 1=1 %s %s limit %u, %u", start, endt, from, to);
	}
	else
	{
		if (from == 0)
			sprintf(extSql, "select count(*) from actor_info where 1=1 %s %s ", start, endt);

		sprintf(sql, "select actorid, createtts, accountid, nick, sex, gold, level, occu, exp, \
					 						 actortype, hp, mp, attack, def, vit, cp, ap, faction, vist, online, lvtts \
											 						 from actor_info where 1=1 %s %s limit %u, %u", start, endt, from, to);
	}
	return 0;
}

void COherServeConnMan::GetTotal(UINT16 channelId, UINT16 partId, UINT32 from, const char* db, const char* sql, UINT32* total)
{
	std::map<UINT16, UINT32> extRes;
	*total = 0;
	if (from == 0)
	{
		m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, db, sql, extRes, 0);
		GetExtSqlResult(extRes, partId, total);
		m_update.ClearNewDBSResult();
	}
}

void COherServeConnMan::GetAccountTotal(UINT16 channelId, UINT16 partId, UINT32 from, const char* start, const char* endt, UINT32* total)
{
	char sql[1024 * 2] = { 0 };
	std::map<UINT16, UINT32> extRes;
	for (int i = 1; i < 6; i++)
	{
		UINT32 num = 0;
		sprintf(sql, "select count(*) from player_account_%u where partid=%d %s %s", i, partId, start, endt);
		m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGSVERIFY, sql, extRes, 0);
		GetExtSqlResult(extRes, partId, &num);
		*total += num;
		memset(sql, 0, 1024);
		extRes.clear();
		m_update.ClearNewDBSResult();
	}
}

void COherServeConnMan::GetActivatedDevice(UINT32 from, const char* start, const char* endt, UINT32* total)
{
	char sql[1024 * 2] = { 0 };
	std::map<UINT16, UINT32> extRes;
	for (int i = 1; i < 6; i++)
	{
		UINT32 num = 0;
		sprintf(sql, "select count(*) from activated_device%u where 1=1 %s %s", i, start, endt);
		m_update.ExecuteNewRechargeDbQuerySql(DBS_MSG_QUERY, DB_SGSRECHARGE, sql, &num);
		*total += num;
		memset(sql, 0, 1024);
		extRes.clear();
		m_update.ClearNewDBSResult();
	}
}


int COherServeConnMan::PaymentMan(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, const char* keyWord, UINT32& count)
{
	char sql[1024] = { 0 };
	char start[128] = { 0 };
	char endt[128] = { 0 };
	CheckTimeB(startTime, endTime, NULL, "tts", start, endt, NULL, NULL);
	count = 0;
	sprintf(sql, "select sum(%s) from cash where 1=1 %s %s", keyWord, start, endt);
	std::map<UINT16, UINT32> extRes;
	int ret = m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGSINFO, sql, extRes, 0);
	std::map<UINT16, UINT32>::iterator it = extRes.find(partId);
	if (it != extRes.end())
		count = it->second;
	m_update.ClearNewDBSResult();
	return ret;
}

void COherServeConnMan::CheckTime(const char* startTime, const char* endTime, const char* keyWord, char start[125], char endt[125])
{
	if (!StrIsNull(startTime))
		sprintf(start, " and %s >= '%s' ", keyWord, startTime);
	if (!StrIsNull(endTime))
		sprintf(endt, " and %s <= '%s' ", keyWord, endTime);
	if (StrIsNull(startTime) && StrIsNull(endTime))
		sprintf(endt, " and %s <= now() ", keyWord);
}

void COherServeConnMan::CheckTimeB(const char* startTime, const char* endTime, const char* TendTime, const char* keyWord, char start[125], char endt[125], const char* TkeyWord, char Tendt[125])
{
	if (!StrIsNull(startTime))
		sprintf(start, " and date(%s) >=date('%s') ", keyWord, startTime);
	if (!StrIsNull(endTime))
		sprintf(endt, " and date(%s) <=date('%s') ", keyWord, endTime);
	if (!StrIsNull(TendTime))
		sprintf(Tendt, " and date(%s)=date('%s') ", TkeyWord, TendTime);
	if (StrIsNull(startTime) && StrIsNull(endTime))
		sprintf(endt, " and date(%s) <= now()", keyWord);
}

//����ʱ�䴦��;
void COherServeConnMan::OnlineTimeMan(UINT16 platId, UINT16 partId, UINT32 from, BYTE to, char extSql[1024], char sql[1024], const char* startTime, const char* endTime, BYTE stype)
{
	char start[125] = { 0 };
	char endt[125] = { 0 };
	CheckTime(startTime, endTime, "tts", start, endt);

	sprintf(sql, "select count, tts from online where  type=%u   %s  %s   and partid =%u order by tts limit %u, %u", stype, start, endt, partId, from, to);
	if (from == 0)
		sprintf(extSql, "select count(*) from online where  type=%u  %s  %s  and partid =%u order by tts ", stype, start, endt, partId);
}

///��ȡ���
void COherServeConnMan::GetExtSqlResult(std::map<UINT16, UINT32>& extRes, UINT16 partId, UINT32* count)
{
	std::map<UINT16, UINT32>::iterator it = extRes.find(partId);
	if (it != extRes.end())
		*count = it->second;
	else
		*count = 0;
}
///��ȡ���
void COherServeConnMan::GetExtSqlResult(std::map<UINT16, float>& extRes, UINT16 partId, float* count)
{
	std::map<UINT16, float>::iterator it = extRes.find(partId);
	if (it != extRes.end())
		*count = it->second;
	else
		*count = 0.0;
}

void COherServeConnMan::GetExtSqlResult(std::map<UINT16, DBS_RESULT>& extRes, UINT16 partId, DBS_RESULT& result)
{
	std::map<UINT16, DBS_RESULT>::iterator it = extRes.find(partId);
	if (it != extRes.end())
		result = it->second;
	else
		result = NULL;
}

///��ȡ������������
void COherServeConnMan::OnlineMan(UINT32 from, UINT16 channelId, UINT16 partId, const char* extSql, const char* sql, std::vector<OnlineInfo>& olInfo, UINT32* count)
{
	GetTotal(channelId, partId, from, DB_SGSINFO, extSql, count);
	m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGSINFO, sql);
	std::map<UINT16, DBS_RESULT>::iterator it = m_update.GetNewDBSResult().begin();
	UINT32 i = 0;
	for (it; it != m_update.GetNewDBSResult().end(); it++)
	{
		DBS_RESULT result = it->second;
		if (result == NULL)
			continue;

		DBS_ROW row = NULL;
		while ((row = DBS_FetchRow(result)) != NULL)
		{
			if (i >= MAX_LIMIT_COLUMS)
				break;

			OnlineInfo info;
			olInfo.push_back(info);
			olInfo[i].onlineCount = atoi(row[0]);
			strcpy(olInfo[i].olTime, row[1]);
			i++;
		}
	}

	m_update.ClearNewDBSResult();
}


///���桪����ȡ���ݴ���;
int COherServeConnMan::GetLeaveKeep(UINT16 platId, UINT16 channelId, UINT16 partId, const char* stime, const char* tbName, BYTE lkTimeType, UINT16* Retained, int i, UINT32* NewRecount)
{
	char sql[1024] = { 0 };
	char refreshTime[20] = { 0 };
	DBS_RESULT result = NULL;
	DBS_ROW row = NULL;
	char* p;
	GetDate(stime, refreshTime, FALSE);
	*(refreshTime + 4) = *(refreshTime + 5);
	*(refreshTime + 5) = *(refreshTime + 6);
	*(refreshTime + 6) = *(refreshTime + 8);
	*(refreshTime + 7) = *(refreshTime + 9);
	*(refreshTime + 8) = NULL;
	*(refreshTime + 9) = NULL;
	p = addDays(refreshTime, lkTimeType);
	memset(&refreshTime, 0, 20);
	*(refreshTime) = *(p);
	*(refreshTime + 1) = *(p + 1);
	*(refreshTime + 2) = *(p + 2);
	*(refreshTime + 3) = *(p + 3);
	*(refreshTime + 4) = '-';
	*(refreshTime + 5) = *(p + 4);
	*(refreshTime + 6) = *(p + 5);
	*(refreshTime + 7) = '-';
	*(refreshTime + 8) = *(p + 6);
	*(refreshTime + 9) = *(p + 7);

	sprintf(sql, "select regcount,lgcount from %s where date(tts) = date('%s') and date(uptts)=date('%s') and type=%u and partid=%d and ver=%d", tbName, stime, refreshTime, lkTimeType, partId, channelId);

	m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGSINFO, sql);
	std::map<UINT16, DBS_RESULT>::iterator it = m_update.GetNewDBSResult().find(partId);
	if (it != m_update.GetNewDBSResult().end())
		result = it->second;

	if (result == NULL)
		return GM_ERROR;

	while ((row = DBS_FetchRow(result)) != NULL)
	{
		if (i >= MAX_LIMIT_COLUMS)
			break;
		*NewRecount = (UINT32)atoi(row[0]);
		*Retained = (UINT32)atoi(row[1]);
	}
	m_update.ClearNewDBSResult();
	return 0;
}

//��ȡ������;
int COherServeConnMan::LeaveRate(UINT16 platId, UINT16 channelId, UINT16 partId, const char* stime, const char* tbName, BYTE lkTimeType, int ic, float* Getrate)
{
	char sql[1024] = { 0 };
	char refreshTime[20] = { 0 };
	char* p;
	GetDate(stime, refreshTime, FALSE);
	*(refreshTime + 4) = *(refreshTime + 5);
	*(refreshTime + 5) = *(refreshTime + 6);
	*(refreshTime + 6) = *(refreshTime + 8);
	*(refreshTime + 7) = *(refreshTime + 9);
	*(refreshTime + 8) = NULL;
	*(refreshTime + 9) = NULL;
	p = addDays(refreshTime, lkTimeType);
	memset(&refreshTime, 0, 20);
	*(refreshTime) = *(p);
	*(refreshTime + 1) = *(p + 1);
	*(refreshTime + 2) = *(p + 2);
	*(refreshTime + 3) = *(p + 3);
	*(refreshTime + 4) = '-';
	*(refreshTime + 5) = *(p + 4);
	*(refreshTime + 6) = *(p + 5);
	*(refreshTime + 7) = '-';
	*(refreshTime + 8) = *(p + 6);
	*(refreshTime + 9) = *(p + 7);
	p = NULL;
	sprintf(sql, "select rate from %s where date(tts) = date('%s') and date(uptts)=date('%s') and type=%u and partid=%d and ver=%d ", tbName, stime, refreshTime, lkTimeType, partId, channelId);
	std::map<UINT16, float> extRes;

	int ret = m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGSRECHARGE, sql, extRes);
	GetExtSqlResult(extRes, partId, Getrate);
	memset(sql, 0, 1024);
	extRes.clear();
	m_update.ClearNewDBSResult();

	return ret;


}


//��ʱ��ȡ��������;
void COherServeConnMan::GetOnlineUserByTimer(void* p, TTimerID id)
{

	COherServeConnMan* ponline = (COherServeConnMan*)p;
	ASSERT(ponline != NULL);
	ponline->OnGetOnlineUser();


}
//ʵʱ����;
void COherServeConnMan::GetRealTimeNewByTimer(void *p, TTimerID id)
{
	if (!GetCfgManObj()->IsWriteIntoDb())
	{
		COherServeConnMan* ponewTime = (COherServeConnMan*)p;
		ASSERT(ponewTime != NULL);
		ponewTime->OnNewRuning();
	}
}

void COherServeConnMan::ReflushOnlineLogByTimer(void* p, TTimerID id)
{
	COherServeConnMan* ponline = (COherServeConnMan*)p;
	ponline;
	ASSERT(ponline != NULL);

}
void InitOtherServeManObj()
{
	if (g_otherServeConnObj != NULL)
		return;
	InitServeConnObj();
	g_otherServeConnObj = new COherServeConnMan();
	ASSERT(g_otherServeConnObj != NULL);
}

COherServeConnMan* GetOtherServeManObj()
{
	return g_otherServeConnObj;
}

void DestroyOtherServeManObj()
{
	DestroyServeConnObj();
	if (g_otherServeConnObj == NULL)
		return;
	delete g_otherServeConnObj;
	g_otherServeConnObj = NULL;
}

///��ѯ�����豸������;
int COherServeConnMan::QueryAddNewDevice(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count)
{
	char sql[1024] = { 0 };

	int ret = GetDevice(platId, channelId, partId, startTime, endTime, "regtts", count);
	if (*count != 0)
		return ret;

	sprintf(sql, "select sum(count) from active_device where activetts >= '%s' \
				 				  and activetts <= '%s' and type=%u and platformid =%d and partid=%d ", startTime, endTime, 1, channelId, partId);
	std::map<UINT16, UINT32> extRes;
	ret = m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGSINFO, sql, extRes, 0);
	std::map<UINT16, UINT32>::iterator it = extRes.find(partId);
	if (it != extRes.end())
		*count = it->second;
	extRes.clear();
	m_update.ClearNewDBSResult();
	return ret;
}

///��ѯ�����˺�������;
int COherServeConnMan::QueryAddNewAccount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count)
{
	char sql[1024] = { 0 };

	int ret = GetAccount(platId, channelId, partId, startTime, endTime, "regtts", count);


	if (*count != 0)
		return ret;
	sprintf(sql, "select sum(count) from regist_account where regtts >= '%s' \
				 				  and regtts <= '%s' and platformid=%d and partid=%d and type=%u ", startTime, endTime, channelId, partId, 1);
	std::map<UINT16, UINT32> extRes;
	ret = m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGSINFO, sql, extRes, 0);
	std::map<UINT16, UINT32>::iterator it = extRes.find(partId);
	if (it != extRes.end())
		*count = it->second;
	m_update.ClearNewDBSResult();
	return ret;
}

///��������;
int COherServeConnMan::QueryAddNewActor(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count)
{
	char sql[1024] = { 0 };
	int ret = GetActor(platId, channelId, partId, startTime, endTime, "createtts", count);
	if (*count != 0)
		return ret;

	sprintf(sql, "select sum(count) from create_actor where createtts >= '%s' \
				 				  and createtts <= '%s' and type=%u and platformid=%d and partid= %d", startTime, endTime, 1, channelId, partId);
	std::map<UINT16, UINT32> extRes;
	ret = m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGSINFO, sql, extRes, 0);
	std::map<UINT16, UINT32>::iterator it = extRes.find(partId);
	if (it != extRes.end())
		*count = it->second;
	extRes.clear();
	m_update.ClearNewDBSResult();
	return ret;
}

///��ѯ��¼�豸������;
int COherServeConnMan::QueryLogDevice(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count)
{
	return QLoginCount(platId, channelId, partId, startTime, endTime, count, 1);
}
///��¼�豸������;
int COherServeConnMan::QueryLogAccount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count)
{
	return QLoginCount(platId, channelId, partId, startTime, endTime, count, 2);
}
///��¼��ɫ������;
int COherServeConnMan::QueryLogActor(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count)
{
	return QLoginCount(platId, channelId, partId, startTime, endTime, count, 3);
}

///��Ծ�豸������;
int COherServeConnMan::QueryActiveDevice(UINT32 countA, UINT32 countB, UINT32* count)
{
	if (countA>0 && countB>0 && countA>countB)
		*count = (countA - countB);
	else
		*count = 0;
	return 0;
}

/// ��Ծ�˺�������;
int COherServeConnMan::QueryActiveAccount(UINT32 countA, UINT32 countB, UINT32* count)
{

	if (countA>0 && countB>0 && countA>countB)

		*count = (countA - countB);
	else
		*count = 0;
	return 0;
}

/// ��Ծ��ɫ������;
int COherServeConnMan::QueryAcitveActor(UINT32 countA, UINT32 countB, UINT32* count)
{
	if (countA>0 && countB>0 && countA>countB)
		*count = (countA - countB);
	else
		*count = 0;
	return 0;
}

///��ȡ�ʺ�����
int COherServeConnMan::GetAccount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, const char* keyWord, UINT32* count)
{

	std::map<UINT16, UINT32> extRes;
	char sql[1024 * 2] = { 0 };


	sprintf(sql, "select ");
	for (int i = 1; i < 6; i++)
	{
		if (i == 5)
			sprintf(sql, "%s (SELECT COUNT(spa1.id) FROM sgsverify.player_account_%u AS spa1 WHERE spa1.ver=%d and \
						 						  spa1.%s BETWEEN('%s') AND ('%s')) ", sql, i, channelId, keyWord, startTime, endTime);
		else
			sprintf(sql, "%s (SELECT COUNT(spa1.id) FROM sgsverify.player_account_%u AS spa1 WHERE spa1.ver=%d and \
						 						  spa1.%s BETWEEN('%s') AND ('%s')) + ", sql, i, channelId, keyWord, startTime, endTime);
	}
	int c = 0;
	c = m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGSVERIFY, sql, extRes, 0);
	std::map<UINT16, UINT32>::iterator it = extRes.find(partId);
	if (it != extRes.end())
		*count = it->second;
	m_update.ClearNewDBSResult();
	return 0;
}

///��ȡ�豸����
int COherServeConnMan::GetDevice(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, const char* keyWord, UINT32* count)
{

	std::map<UINT16, UINT32> extRes;
	char sql[1024 * 2] = { 0 };
	sprintf(sql, "select ");
	for (int i = 1; i < 6; i++)
	{

		if (i == 5)
			sprintf(sql, "%s (SELECT COUNT(spa1.regdev) FROM sgsverify.player_account_%u AS spa1 WHERE spa1.ver=%d and \
						 						  spa1.%s BETWEEN('%s') AND ('%s')) ", sql, i, channelId, keyWord, startTime, endTime);
		else
			sprintf(sql, "%s (SELECT COUNT(spa1.regdev) FROM sgsverify.player_account_%u AS spa1 WHERE spa1.ver=%d and \
						 						  spa1.%s BETWEEN('%s') AND ('%s')) + ", sql, i, channelId, keyWord, startTime, endTime);

	}
	m_update.ExecuteNewQuerySql(channelId, 0, DBS_MSG_QUERY, DB_SGSVERIFY, sql, extRes, 0);
	std::map<UINT16, UINT32>::iterator it = extRes.find(partId);
	if (it != extRes.end())
		*count = it->second;
	m_update.ClearNewDBSResult();
	return 0;

}

///��ȡ��ɫ����
int  COherServeConnMan::GetActor(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, const char* keyWord, UINT32* count)
{

	std::map<UINT16, UINT32> extRes;
	char sql[1024 * 2] = { 0 };

	sprintf(sql, "select count(*) from actor where  sgs.actor.%s BETWEEN('%s') AND ('%s') ", keyWord, startTime, endTime);

	int ret = m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGS, sql, extRes, 0);
	std::map<UINT16, UINT32>::iterator it = extRes.find(partId);
	if (it != extRes.end())
		*count = it->second;
	else
		*count = 0;
	return ret;
}

///ʵʱ����ͳ�����ݶ���ӿ�;
int COherServeConnMan::QueryAddNewStatistics(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, std::vector<TimeAddedinfo>&Addedinfo, UINT32*count)
{

	std::vector<UINT16> channedl = GetCfgManObj()->GetAllchannel();
	int i = 0;
	bool addswitch = false;
	UINT16 ifchannelid = channelId;
	do
	{
		addswitch = false;
		if (channelId == 0)
		{
			addswitch = true;
			if (i<channedl.size())
				ifchannelid = channedl[i];
			else	break;

		}

		TimeAddedinfo addinfo;
		memset(&addinfo, 0, sizeof(TimeAddedinfo));
		////�����豸������
		QueryAddNewDevice(platId, ifchannelid, partId, startTime, endTime, &addinfo.EquipmentAddedNum);
		////�����ʺ�������
		QueryAddNewAccount(platId, ifchannelid, partId, startTime, endTime, &addinfo.AccountAddedNum);
		////������ɫ������
		QueryAddNewActor(platId, ifchannelid, partId, startTime, endTime, &addinfo.ActorAddedNum);
		//��¼�豸������
		QueryLogDevice(platId, ifchannelid, partId, startTime, endTime, &addinfo.EquipmentLogInNum);
		//��¼�˺�������
		QueryLogAccount(platId, ifchannelid, partId, startTime, endTime, &addinfo.AccountLogInNum);
		//��¼��ɫ������
		QueryLogActor(platId, ifchannelid, partId, startTime, endTime, &addinfo.ActorLogInNum);
		//��Ծ�豸������
		QueryActiveDevice(addinfo.EquipmentLogInNum, addinfo.EquipmentAddedNum, &addinfo.EquipmentActiveNum);
		//��Ծ�˺�������
		QueryActiveAccount(addinfo.AccountLogInNum, addinfo.AccountAddedNum, &addinfo.AccountActiveNum);
		//��Ծ��ɫ������
		QueryAcitveActor(addinfo.ActorLogInNum, addinfo.ActorAddedNum, &addinfo.ActorActiveNum);


		i++;
		Addedinfo.push_back(addinfo);
	} while (addswitch);

	channedl.clear();
	return 0;
}

///������ѯ�������ݴ���;
int COherServeConnMan::QueryRechargeOrderDealwith(UINT16 platId, UINT16 channelId, UINT16 partId, UINT32 from, BYTE to, const char* startTime, const char* endTime, std::vector<RecharegeStatisticeIofo>&RecharnInfo, UINT32*count)
{
	char sql[1024 * 2] = { 0 };
	char start[128] = { 0 };
	char endt[128] = { 0 };
	DBS_RESULT result = NULL;
	DBS_ROW row = NULL;
	int i = 0;

	CheckTime(startTime, endTime, "confirmtts", start, endt);//playey_cash��ȷ��ʱ�䣩;
	GetOrderInfo(partId, from, to, start, endt, sql);
	GetOrderTotal(platId, channelId, partId, from, start, endt, count); //��ѯ������������;
	m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGSRECHARGE, sql);

	std::map<UINT16, DBS_RESULT>::iterator it = m_update.GetNewDBSResult().find(partId);
	if (it != m_update.GetNewDBSResult().end())
		result = it->second;

	if (result == NULL)
		return GM_ERROR;

	while ((row = DBS_FetchRow(result)) != NULL)
	{
		if (i >= MAX_LIMIT_COLUMS)
			break;
		RecharegeStatisticeIofo accoinfo;
		memset(&accoinfo, 0, sizeof(RecharegeStatisticeIofo));
		RecharnInfo.push_back(accoinfo);
		RecharnInfo[i].actorId = atoi(row[0]);
		strcpy(RecharnInfo[i].RechargeTime, row[1]);
		strcpy(RecharnInfo[i].actorName, row[2]);
		RecharnInfo[i].rmb = atoi(row[3]);
		RecharnInfo[i].starus = atoi(row[4]);
		RecharnInfo[i].platformid = atoi(row[5]);
		i++;
	}
	m_update.ClearNewDBSResult();
	return 0;
}

/////��ֵ��ѯͳ�����ݴ���;
int COherServeConnMan::QueryRechargeStatistics(UINT16 platId, UINT16 channelId, UINT16 partId, UINT32 from, BYTE to, const char* startTime, const char* endTime, std::vector<StartisticsInfo>&RecharnInfo, UINT32*count)
{
	char FromTime[20] = { 0 };	//��ʼ��ѯ����,����from������;0���ʾΪstartTime
	char FromToTime[20] = { 0 };//������ѯʱ�䣬����to��������0���ʾendTime����ʱ��;
	//��ѯʱ��ȷ��;
	int ret = 0;
	*count = ResolveTimeNum(startTime, endTime/*,from,to,FromTime,FromToTime*/);
	strcpy(FromTime, startTime);		//���ƿ�ʼ��ѯʱ��;
	strcpy(FromToTime, startTime);
	//���Ʋ�ѯ�������չ���������;
	for (UINT32 i = 0; i<*count; i++)
	{
		StartisticsInfo GetRechartregeInfo;
		memset(&GetRechartregeInfo, 0, sizeof(StartisticsInfo));//��սṹ��
		GoToTime(FromTime, FromToTime, i, false);
		//�����ѯ���;
		ret = GetPaymentMoneySQL(channelId, channelId, partId, FromTime, FromToTime, GetRechartregeInfo);
		RecharnInfo.push_back(GetRechartregeInfo);
		strcpy(RecharnInfo[i].dateTime, FromTime);
	}

	return ret;
}

///��ѯ��ֵͳ�ơ�����ѯ�����û���;
int COherServeConnMan::QueryPaymentActor(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32& count)
{
	int ret = CashDataMan(platId, channelId, partId, PY_CASH_COUNT, startTime, endTime, count);
	if (count == 0)
		return PaymentMan(platId, channelId, partId, startTime, endTime, "actorcount", count);
	return ret;
}
///��ѯ��ֵͳ�ơ������������û���(��һ������Ϸ�г�ֵ�Ľ�ɫ);
int COherServeConnMan::QueryFirstRecharge(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32& count)
{
	int ret = CashDataMan(platId, channelId, partId, PY_CASH_COUNT, startTime, endTime, count);
	if (count == 0)
		return PaymentMan(platId, channelId, partId, startTime, endTime, "paymentcount", count);
	return ret;
}
///��ѯ��ֵͳ�ơ�����ѯ���Ѵ���;
int COherServeConnMan::QueryPaymentCount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32& count)
{
	int ret = CashDataMan(platId, channelId, partId, PY_CASH_COUNT, startTime, endTime, count);
	if (count == 0)
		return PaymentMan(platId, channelId, partId, startTime, endTime, "paymentcount", count);

	return ret;
}

///��ѯ��ֵͳ�ơ�����ѯ���ѽ��(���ո��ѽ��);
int COherServeConnMan::QueryPaymentMoney(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32& count)
{
	int ret = CashDataMan(platId, channelId, partId, PY_CASH_CRMB, startTime, endTime, count);
	if (count == 0)
		return PaymentMan(platId, channelId, partId, startTime, endTime, "rmb", count);
	return ret;
}

///��ѯ��¼������
int COherServeConnMan::QueryPaymentLogin(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, float& retArpu)
{
	UINT32 PaymentNum = 0;
	UINT32 LoginNum = 0;
	QueryPaymentActor(platId, channelId, partId, startTime, endTime, PaymentNum);
	QLoginCount(platId, channelId, partId, startTime, endTime, &LoginNum, 2);
	if (PaymentNum != 0 && LoginNum != 0)
	{
		retArpu = (float)PaymentNum / LoginNum * 100;
		return retArpu;
	}

	return 0;
}

///��ѯע�Ḷ����;
int COherServeConnMan::QueryPaymentAdd(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, float& retArpu)
{

	UINT32 FirstRechargeNum = 0;
	UINT32 AddNewAccountNum = 0;
	//���ñ���С�������λ��
	QueryAddNewAccount(platId, channelId, partId, startTime, endTime, &AddNewAccountNum);
	QueryFirstRecharge(platId, channelId, partId, startTime, endTime, FirstRechargeNum);
	if (FirstRechargeNum != 0 && AddNewAccountNum != 0)
	{
		retArpu = (float)FirstRechargeNum / AddNewAccountNum * 100;
		return retArpu;
	}

	return 0;
}

///��ѯ����ƽ����ARPU;
int COherServeConnMan::QuerypaymentArpu(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, float& retArpu)
{
	UINT32 QueryPaymentMoneyNum = 0;
	UINT32 QueryPaymentActorNum = 0;
	QueryPaymentMoney(platId, channelId, partId, startTime, endTime, QueryPaymentMoneyNum);
	QueryPaymentActor(platId, channelId, partId, startTime, endTime, QueryPaymentActorNum);
	if (QueryPaymentMoneyNum != 0 && QueryPaymentActorNum != 0)
	{
		retArpu = (float)QueryPaymentMoneyNum / QueryPaymentActorNum;
		return retArpu;
	}
	return 0;
}

///��ѯ��¼ƽ����ARPU;
int COherServeConnMan::QueryLoginArpu(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, float& retArpu)
{
	UINT32 QueryPaymentMoneyNum = 0;
	UINT32 QueryLogAccountNum = 0;
	QueryPaymentMoney(platId, channelId, partId, startTime, endTime, QueryPaymentMoneyNum);
	QueryLogAccount(platId, channelId, partId, startTime, endTime, &QueryLogAccountNum);
	if (QueryPaymentMoneyNum != 0 && QueryLogAccountNum != 0)
	{
		retArpu = (float)(QueryPaymentMoneyNum*1.0f) / QueryLogAccountNum;
		return retArpu;
	}
	return 0;
}

///���충����ѯsql��Ϣ�����˺����������충����ѯ��Sql��䣩
int COherServeConnMan::GetOrderInfo(UINT16 partId, UINT32 from, BYTE to, const char* start, const char* endt, char sql[1024 * 2])
{
	for (int i = 1; i < 6; i++)
	{
		if (i == 5)
			sprintf(sql, "%s (select distinct b.actorid,a.confirmtts,b.nick,a.rmb,c.cash,a.platformid from sgsinfo.actor_info b,sgsrecharge.player_cash_%u a,\
						 							 sgscash.cash_%u c where (a.actorid=b.actorid) and (a.status=1)  and (a.accountid=c.accountid) and (a.partid=%d) %s %s ) limit %u, %u", sql, i, i, partId, start, endt, from, to);
		else
			sprintf(sql, "%s (select distinct b.actorid,a.confirmtts,b.nick,a.rmb,c.cash,a.platformid from sgsinfo.actor_info b,sgsrecharge.player_cash_%u a,\
						 							 sgscash.cash_%u c where (a.actorid=b.actorid) and (a.status=1) and (a.accountid=c.accountid) and (a.partid=%d) %s %s ) union  ", sql, i, i, partId, start, endt);
	}
	return 0;
}

//��ȡ��������;
void COherServeConnMan::GetOrderTotal(UINT16 platId, UINT16 channelId, UINT16 partId, UINT32 from, const char* start, const char* endt, UINT32* total)
{
	char sql[1024 * 2] = { 0 };
	std::map<UINT16, UINT32> extRes;
	for (int i = 1; i < 6; i++)
	{
		UINT32 num = 0;
		sprintf(sql, "select count(distinct b.actorid) from player_cash_%u a,\
					 						 sgsinfo.actor_info b,sgscash.cash_%u c where (a.actorid=b.actorid) and (a.status=1)  and (a.accountid=c.accountid) and (a.partid=%d) %s %s ", i, i, partId, start, endt);

		m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGSRECHARGE, sql, extRes, 0);
		GetExtSqlResult(extRes, partId, &num);
		*total += num;
		memset(sql, 0, 1024);
		extRes.clear();
		m_update.ClearNewDBSResult();
	}
}

///��ȡʵʱ���߼�¼;
int COherServeConnMan::GetOnlineHistory(UINT16 platId, UINT16 channelId, UINT16 partId, const char* starTime, const char* endTime, std::vector<OnlineInfo>& olInfo, UINT32 from, BYTE to, UINT32* total)
{
	char sql[1024] = { 0 };
	char extSql[1024] = { 0 };
	SqlOnelineTimeMan(platId, channelId, partId, extSql, sql, starTime, endTime, from, to, OL_COMM);
	//������������;
	SqlOneLineUserMan(platId, channelId, partId, extSql, sql, olInfo, total);

	return 0;
}

//sql����ʱ�䴦��;
void COherServeConnMan::SqlOnelineTimeMan(UINT16 platId, UINT16 channelId, UINT16 partId, char extSql[1024], char sql[1024], const char* startTime, const char* endTime, UINT32 from, BYTE to, BYTE stype)
{
	char start[125] = { 0 };
	char endt[125] = { 0 };
	CheckTime(startTime, endTime, "tts", start, endt);

	sprintf(sql, "select tts,count from online where  type=%u   %s  %s   and  platformid=%u and ver=%u and partid =%u limit %u,%u ", stype, start, endt, platId, channelId, partId, from, to);

	sprintf(extSql, "select count(tts),count from online where  type=%u   %s  %s   and  platformid=%u and ver=%u and partid =%u", stype, start, endt, platId, channelId, partId);

}
//��ȡ������������;
int COherServeConnMan::SqlOneLineUserMan(UINT16 platId, UINT16 channelId, UINT16 partId, const char* extSql, const char* sql, std::vector<OnlineInfo>& olInfo, UINT32* total)
{
	GetTotal(channelId, partId, 0, DB_SGSINFO, extSql, total);
	m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGSINFO, sql);
	std::map<UINT16, DBS_RESULT>::iterator it = m_update.GetNewDBSResult().begin();
	UINT32 i = 0;
	for (it; it != m_update.GetNewDBSResult().end(); it++)
	{
		DBS_RESULT result = it->second;
		if (result == NULL)
			continue;

		DBS_ROW row = NULL;
		while ((row = DBS_FetchRow(result)) != NULL)
		{
			if (i >= MAX_LIMIT_COLUMS)
				break;

			OnlineInfo info;
			olInfo.push_back(info);
			strcpy(olInfo[i].olTime, row[0]);
			olInfo[i].onlineCount = atoi(row[1]);
			i++;
		}

	}

	m_update.ClearNewDBSResult();
	if (i == 0)
	{
		return -1;
	}
	return 0;
}

///�ȼ��ֲ�ͳ��;
int COherServeConnMan::Grade_RatingStatistics(UINT16 platId, UINT16 channelId, UINT16 partId, const char* starTime, const char* endTime, std::vector<GM_Grade_RatingStatisticsInfo>& OnlineStatisticsInfo, BYTE* rentNum, UINT32* Grade_sum)
{
	//��ѯÿ���ȼ���������;Ȼ�����ã���ɫ����/��ǰ�ȼ�����*100)��õ�ǰ�ȼ��ٷ�ռ��;
	//��ȡ��ǰ�ȼ�����;

	/*select count(*) from sgs.actor;*/
	//����;
	GetcoutNum(platId, channelId, partId, starTime, endTime, Grade_sum);
	return Get_Grade_RattingStatistics(platId, channelId, partId, starTime, endTime, OnlineStatisticsInfo, rentNum, Grade_sum);


}

///��ȡ�ȼ��ֲ�ͳ�ƴ���;
int COherServeConnMan::Get_Grade_RattingStatistics(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, std::vector<GM_Grade_RatingStatisticsInfo> & OnlineStatisticsInfo, BYTE* rentNum, UINT32* Grade_sum)
{
	char sql[1024] = { 0 };
	DBS_RESULT result = NULL;
	int i = 0;

	sprintf(sql, "select sgs.actor.`level`,count(*) from sgs.actor where unix_timestamp(sgs.actor.createtts)>=UNIX_TIMESTAMP('%s') and date(sgs.actor.lvtts)<=date('%s') GROUP BY sgs.actor.`level`", startTime, endTime);

	int ret = m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGS, sql);

	GetExtSqlResult(m_update.GetNewDBSResult(), partId, result);
	if (result == NULL)
		return 1;
	DBS_ROW row = NULL;

	while ((row = DBS_FetchRow(result)) != NULL)
	{
		if (i >= MAX_LIMIT_COLUMS)
			break;
		GM_Grade_RatingStatisticsInfo info;
		memset(&info, 0, sizeof(GM_Grade_RatingStatisticsInfo));
		OnlineStatisticsInfo.push_back(info);
		OnlineStatisticsInfo[i].grade = atoi(row[0]);
		OnlineStatisticsInfo[i].acterNum = atoi(row[1]);
		OnlineStatisticsInfo[i].proportion = (float)OnlineStatisticsInfo[i].acterNum / (int)(*Grade_sum) * 100;
		i++;
		*rentNum = i;
	}
	m_update.ClearNewDBSResult();

	return ret;

}

//��ɫ����;
int COherServeConnMan::GetcoutNum(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* Grade_sum)
{
	char sql[1024] = { 0 };

	sprintf(sql, "select count(*) from sgs.actor where unix_timestamp(sgs.actor.createtts)>=UNIX_TIMESTAMP('%s') and date(sgs.actor.lvtts)<=date('%s') ", startTime, endTime);

	std::map<UINT16, UINT32> extRes;
	int ret = m_update.ExecuteNewQuerySql(channelId, partId, DBS_MSG_QUERY, DB_SGS, sql, extRes, 0);
	std::map<UINT16, UINT32>::iterator it = extRes.find(partId);
	if (it != extRes.end())
		*Grade_sum = it->second;
	extRes.clear();
	m_update.ClearNewDBSResult();
	return ret;
}

///��ѯ��ֵͳ�ơ�����ȡ��ѯ����;//�����from,to,FromTime,FromTotime,��ʱ��ΪԤ���ӿڣ��ݲ�����;
int COherServeConnMan::ResolveTimeNum(const char* startTime, const char* endtime/*,const UINT32 from,BYTE to,char * FromTime, char*FromTotime*/)
{
	int Getyear = 0;//���ڴ��ת���ַ����е����;
	int Getmonth = 0;//�·�;
	int Getday = 0;	//����;
	int Sumday = 0;	//������;
	int monthNumber[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };//��ƽ���㣬ÿ���µ�����;�����ǰ��ѯΪ���꣬��2�·�+1��;
	//��ȡ�·�;
	SYSTEMTIME sysSTime = ToTime(StrToMsel1(startTime, TRUE), TRUE);
	SYSTEMTIME sysETime = ToTime(StrToMsel1(endtime, TRUE), TRUE);
	//�ж��Ƿ�Ϊ����;
	monthNumber[1] += sysSTime.wYear % 4 == 0 && sysSTime.wYear % 100 != 0 || sysSTime.wYear % 400 == 0 ? 1 : 0;

	char bgTime[20] = { 0 };
	char edTime[20] = { 0 };

	GetDate(startTime, bgTime, FALSE);
	GetDate(endtime, edTime, FALSE);
	if (strcmp(bgTime, edTime) != 1)
	{
		Getyear = sysETime.wYear - sysSTime.wYear;	 //��ȡ�������;
		Getmonth = sysETime.wMonth - sysSTime.wMonth;//��ȡ�����·�;
		Getday = sysETime.wDay - sysSTime.wDay + 1;		 //��ȡ��������;

		///ͨ��ѭ����ȥ��ݺ��·ݵ�������;
		for (int i = 0; Getyear != 0 || Getmonth != 0 && Getmonth >= 0; i++)
		{
			if (Getyear>0 && sysSTime.wYear + i != sysETime.wYear)//��ȡ���;
			{
				Getyear--;
				Sumday += 365 + sysSTime.wYear + i % 4 == 0 && sysSTime.wYear + i % 100 != 0 || sysSTime.wYear + i % 400 == 0 ? 1 : 0;
			}
			if (Getmonth>0 && sysSTime.wMonth + i != sysETime.wMonth)//��ȡ�·�;
			{
				Getmonth--;
				Sumday += monthNumber[sysSTime.wMonth + i - 1];
			}
		}
		return Sumday += Getday;
	}
	return GM_QUERY_TIME;
}

//���Ʋ�ѯ����;
int COherServeConnMan::GoToTime(char* STime, char* ETime, int i, bool trues)
{
	///���ƣ����һ��;
	if (trues)
	{
		if (i)
		{
			if (*(STime + 9) + 1<58)  //���С��9�򣬽�һ������ʮλ�������һ;	
			{
				*(STime + 9) = *(STime + 9) + 1;
			}
			else
				if (*(STime + 8) + 1<'3')//ʮλ��������3���һ;
				{
					*(STime + 8) = *(STime + 8) + 1;
					*(STime + 9) = '0';
				}
			//�������·�;���������;
		}
		///���ѯʱ�����;
		if (*(ETime + 9) + 1<58)  //���С��9�򣬽�һ������ʮλ�������һ;
		{
			*(ETime + 9) = *(ETime + 9) + 1;
		}
		else
			if (*(STime + 8) + 1<'3')//ʮλ��������3���һ;
			{
				*(ETime + 8) = *(ETime + 8) + 1;
				*(ETime + 9) = '0';
			}

	}
	else	//����ǰ��һ������������;
	{
		if (i)
		{
			if (*(STime + 9) + 1<58)  //���С��9�򣬽�һ������ʮλ�������һ;	
				*(STime + 9) = *(STime + 9) + 1;
			else
				if (*(STime + 8) + 1<'3')//ʮλ��������3���һ;
				{
					*(STime + 8) = *(STime + 8) + 1;
					*(STime + 9) = '0';
				}
			//�������·�;���������;

			///���ѯʱ�����;
			if (*(ETime + 9) + 1<58)  //���С��9�򣬽�һ������ʮλ�������һ;
			{
				*(ETime + 9) = *(ETime + 9) + 1;
			}
			else
				if (*(STime + 8) + 1<'3')//ʮλ��������3���һ;
				{
					*(ETime + 8) = *(ETime + 8) + 1;
					*(ETime + 9) = '0';
				}
		}
	}

	return 0;
}

int COherServeConnMan::GetPaymentMoneySQL(UINT16 platId, UINT16 channelId, UINT16 partId, const char*startTime, const char* endTime, StartisticsInfo& RecharInfo)
{
	//��sql���룬����;
	///��ѯ�����û���;
	QueryPaymentActor(platId, channelId, partId, startTime, endTime, RecharInfo.PaymentUserNum);
	///���������û���(��һ������Ϸ�г�ֵ�Ľ�ɫ);
	QueryFirstRecharge(platId, channelId, partId, startTime, endTime, RecharInfo.newPaymentUserNum);
	///��ѯ���Ѵ���;
	QueryPaymentCount(platId, channelId, partId, startTime, endTime, RecharInfo.PaymentcountNum);
	///��ѯ���ѽ��(���ո��ѽ��);
	QueryPaymentMoney(platId, channelId, partId, startTime, endTime, RecharInfo.PaymentMoney);

	///��ѯ��¼������;
	QueryPaymentLogin(platId, channelId, partId, startTime, endTime, RecharInfo.LoginPaymentRates);
	///��ѯע�Ḷ����;
	QueryPaymentAdd(platId, channelId, partId, startTime, endTime, RecharInfo.RegiPaymentRates);
	///��ѯ����ƽ����ARPU;
	QuerypaymentArpu(platId, channelId, partId, startTime, endTime, RecharInfo.PaymentARPU);
	///��ѯ��¼ƽ����ARPU;
	QueryLoginArpu(platId, channelId, partId, startTime, endTime, RecharInfo.LoginARPU);


	return 0;
}




/////��������ͳ��;
//int COherServeConnMan::OnlineStatistics( UINT16 platId, UINT16 partId,const char* starTime,const char* endTime, std::vector<GMOnlineDateStatisticsInfo>& olInfo, UINT32* count )
//{
//
//	char sql[1024] = { 0 };
//	char extSql[1024] = { 0 };
//
//
//
//	//sql�����������ݴ���;
//	SqlGetOnlineStatistics(partId,partId,extSql,sql,starTime,endTime,count);
//	//��ȡ�������ݴ���;
//	GetSqlOlineStatistics(partId,partId,extSql,sql,olInfo,count);
//
//	return 0;
//}
//
//
/////��������ͳ�ƴ���
// int COherServeConnMan::GetOnlineStatistics( UINT16 platId, UINT16 partId, const char* extSql, const char* sql, std::vector<GMOnlineDateStatisticsInfo>& olInfo, UINT32* count )
//{
//	
//return 0;
//}
//
//
// //Sql��������ͳ�ƴ���
// int COherServeConnMan::SqlGetOnlineStatistics( UINT16 platId, UINT16 partId,char extSql[1024], char sql[1024], const char* startTime, const char* endTime, UINT32* count )
//{
//	char start[125] = { 0 };
//	char endt[125] = { 0 };
//	CheckTime(startTime, endTime, "tts", start, endt);
//
//	sprintf(sql, "select count, tts from online where  type=%u   %s  %s   and partid =%u ",1, start, endt, partId);
//		return 0;
//}	
//
// int COherServeConnMan::GetSqlOlineStatistics( UINT16 platId, UINT16 partId, const char* extSql, const char* sql, std::vector<GMOnlineDateStatisticsInfo>& olInfo, UINT32* count )
//{
//	return 0;
//}
//
//


char* COherServeConnMan::tm2str(tm* pTm)
{
	int year = pTm->tm_year + 1900;
	int mon = pTm->tm_mon + 1;
	int day = pTm->tm_mday;

	char *ret = (char*)malloc(10);
	memset(ret, 0, 10);
	sprintf(ret, "%d", year);
	ret[4] = mon / 10 + '0';
	ret[5] = mon % 10 + '0';
	ret[6] = day / 10 + '0';
	ret[7] = day % 10 + '0';
	return ret;
}
tm* COherServeConnMan::str2tm(char date[])
{
	int year, mon, day;
	sscanf(date, "%4d%2d%2d", &year, &mon, &day);

	tm *pDate = (tm*)malloc(sizeof(tm));
	memset(pDate, 0, sizeof(tm));
	pDate->tm_year = year - 1900;//��ݴ�1900��ʼ
	pDate->tm_mon = mon - 1;//�·ݴ�0��ʼ
	pDate->tm_mday = day;
	return pDate;
}

char* COherServeConnMan::addDays(char date[], int days)
{

	time_t time = mktime(str2tm(date));

	time += days * 24 * 3600;//��������

	tm * res = localtime(&time);

	return tm2str(res);
}

void  COherServeConnMan::DateMan(UINT16 platId, UINT16 partId, const char* registTime, BYTE day, char lkTime[20])
{
	char gTime[20] = { 0 };
	char sql[1024] = { 0 };
	memset(lkTime, 0, 20);

	GetDate(registTime, gTime, FALSE);
	if (day == 60)
		sprintf(sql, "select date_format(date_add('%s',interval 1 month), '%sY-%sm')", gTime, "%", "%");
	else
		sprintf(sql, "select date_add('%s',interval %u day)", gTime, day);
	m_update.ExecuteNewQuerySql(platId, partId, DBS_MSG_QUERY, DB_SGSVERIFY, sql);
	if (IS_RESULT_NULL())
		return;

	std::map<UINT16, DBS_RESULT>::iterator it = m_update.GetNewDBSResult().begin();
	DBS_RESULT result = it->second;
	if (result == NULL)
		return;
	DBS_ROW row = NULL;
	while ((row = DBS_FetchRow(result)) != NULL)
		strcpy(lkTime, row[0]);

	m_update.ClearNewDBSResult();
}