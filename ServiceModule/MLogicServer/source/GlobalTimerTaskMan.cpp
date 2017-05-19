#include "GlobalTimerTaskMan.h"
#include "EmailCleanMan.h"
#include "ConfigFileMan.h"
#include "InstRankInfoMan.h"
#include "GameCfgFileMan.h"
#include "DBSClient.h"
#include "RolesInfoMan.h"
#include "GWSClientMan.h"
#include "cyaMaxMin.h"
#include "GoldExchangeMan.h"
#include "DailyTaskMan.h"
#include "ActitvityMan.h"
#include "InstFinHistoryMan.h"
#include "VitExchangeMan.h"
#include "RankRewardMan.h"
#include "LadderRankMan.h"
#include "CompetitiveModeMan.h"
#include "OneVsOneMan.h"

static GlobalTimerTaskMan* sg_gtimerTaskMan = NULL;
void InitGlobalTimerTaskMan()
{
	ASSERT(sg_gtimerTaskMan == NULL);
	sg_gtimerTaskMan = new GlobalTimerTaskMan();
	ASSERT(sg_gtimerTaskMan != NULL);
	sg_gtimerTaskMan->Start();
}

GlobalTimerTaskMan* GetGlobalTimerTaskMan()
{
	return sg_gtimerTaskMan;
}

void DestroyGlobalTimerTaskMan()
{
	GlobalTimerTaskMan* gtimerTaskMan = sg_gtimerTaskMan;
	sg_gtimerTaskMan = NULL;
	if (gtimerTaskMan != NULL)
	{
		gtimerTaskMan->Stop();
		delete gtimerTaskMan;
	}
}

GlobalTimerTaskMan::GlobalTimerTaskMan()
: m_thHandle(INVALID_OSTHREAD)
, m_threadExit(true)
, m_lastDay(0)
, m_lastMonth(0)
, m_lastDayOfOffset(0)
{

}

GlobalTimerTaskMan::~GlobalTimerTaskMan()
{

}

void GlobalTimerTaskMan::Start()
{
	/*//delete by hxw 20151201
	SYSTEMTIME lcTime;
	GetLocalTime(&lcTime);
	m_lastDay = lcTime.wDay;
	*///end
	m_threadExit = false;
	OSCreateThread(&m_thHandle, NULL, THWorker, this, 0);
}

void GlobalTimerTaskMan::Stop()
{
	m_threadExit = true;
	m_sleep.Interrupt();
	if (m_thHandle != INVALID_OSTHREAD)
	{
		OSCloseThread(m_thHandle);
		m_thHandle = INVALID_OSTHREAD;
	}
}

int GlobalTimerTaskMan::THWorker(void* param)
{
	GlobalTimerTaskMan* pThis = (GlobalTimerTaskMan*)param;
	ASSERT(pThis != NULL);
	return pThis->OnWorker();
}

int GlobalTimerTaskMan::OnWorker()
{
	while (!m_threadExit)
	{
		SYSTEMTIME lcTime;
		GetLocalTime(&lcTime);
		//add by hxw 20151201 保证0点刷新
		ASSERT(printf("nowtime:%d:%d:%d:%d\n", lcTime.wHour, lcTime.wMinute, lcTime.wSecond, lcTime.wMilliseconds));
		if (m_lastDay == 0)
		{
			m_lastDay = lcTime.wDay;
			m_lastDayOfOffset = lcTime.wDay;
			m_lastMonth = lcTime.wMonth;
			//m_sleep.Sleep(1800*1000);	//半小时
			int minute = 60 - lcTime.wMinute - 1;
			int second = 60 - lcTime.wSecond - 1;
			int millsecond = 1000 - lcTime.wMilliseconds;
			int stime = minute * 60 * 1000 + second * 1000 + millsecond;
			ASSERT(printf("needsleep:%dm%ds%dms，all %dms\n", minute, second, millsecond, stime));
			stime += 30 * 1000;//+30表示在0点30秒的时候刷新
			m_sleep.Sleep(stime);	//半小时			
			continue;
		}

		//end
		if (lcTime.wDay != m_lastDay && lcTime.wHour >= 0)		//凌晨0点以后
		{
			//发放副本占领奖励
			OnSendInstOccupyReward();
			//清理每天金币兑换记录
			OnClearEverydayGoldExchage();
			//清理日常任务
			OnClearDailyTaskRecord();
			//清除轮盘奖品
			OnClearRouletteReward();
			//清除玩家活动副本记录
			OnClearPlayersActiveInstRcd();
			//清理在线保存数据
			GetRolesInfoMan()->CleanEverydayData();
			/* zpy 发送月卡奖励 */
			OnSendMonthCardReward();
			/* zpy 清理每天体力兑换记录 */
			OnClearEverydayVitExchage();
			/* zpy 天梯每日结算 */
			GetLadderRankMan()->DailySettlement();
			/* zpy 竞技模式挑战次数清理 */
			GetCompetitiveModeMan()->ClearEveryday();
			/* zpy 1v1进入限制清理 */
			GetOneVsOneMan()->ClearEveryday();
			//add by hxw20151016
			OnRefreshAndClearRank();
			//ends
			//清理日期
			m_lastDay = lcTime.wDay;

		}

		/* 竞技模式 */
		if (lcTime.wDay != m_lastDayOfOffset && lcTime.wHour >= 3)
		{
			if (lcTime.wDayOfWeek == 1)	//每周第一天
			{
				GetCompetitiveModeMan()->GrantCompetitiveModeReward();
				GetCompetitiveModeMan()->GeneraWeekFreeActorType();
			}

			//清理日期
			m_lastDayOfOffset = lcTime.wDay;
		}

		/* zpy 每日签到，月清理 */
		if (lcTime.wMonth != m_lastMonth && lcTime.wDay == 1)
		{
			GetActitvityMan()->ClearRoleDailySign();
			m_lastMonth = lcTime.wMonth;
		}
		//add by hxw 20151203 处理毫秒数矫正
		GetLocalTime(&lcTime);
		m_sleep.Sleep(3600 * 1000 - lcTime.wMilliseconds);	//1小时
		//end
	}
	return 0;
}

void GlobalTimerTaskMan::OnSendInstOccupyReward()
{
#if 0
	std::list<InstRankInfoMan::SInstOccupyRole> occupyList;
	GetInstRankInfoMan()->GetInstOccupyInfo(occupyList);
	for (std::list<InstRankInfoMan::SInstOccupyRole>::iterator it = occupyList.begin(); it != occupyList.end(); ++it)
	{
		//获取副本占领奖励
		std::vector<InstCfg::SInstOccupyReward> occupyReward;
		int retCode = GetGameCfgFileMan()->GetInstOccupyReward(it->instId, occupyReward);
		if (retCode != MLS_OK)
			continue;

		//获取奖励物品属性
		std::vector<SPropertyAttr> propVec;
		int si = (int)occupyReward.size();
		for (int i = 0; i < si; ++i)
		{
			BYTE maxStack = 0;
			SPropertyAttr propAttr;
			BYTE propType = occupyReward[i].type; //GetPropTypeByRewardType(occupyReward[i].type);
			retCode = GetGameCfgFileMan()->GetPropertyAttr(propType, occupyReward[i].propId, propAttr, maxStack, 1);
			if (retCode != MLS_OK)
				continue;

			propAttr.num = occupyReward[i].num;
			propVec.push_back(propAttr);
		}

		if (propVec.size() == 0)
			continue;

		//发送邮件
		EmailCleanMan::SSyncEmailSendContent email;
		email.receiverUserId = it->userId;
		email.receiverRoleId = it->roleId;
		email.pGBSenderNick = "系统";		//发送者昵称
		email.pGBCaption = "副本占领奖励";	//标题

		std::string instName;
		GetGameCfgFileMan()->GetInstName(it->instId, instName);
		char szBody[MAX_UTF8_EMAIL_BODY_LEN] = { 0 };
		sprintf(szBody, "占领副本%s奖励", instName.c_str());
		email.pGBBody = szBody;
		GetEmailCleanMan()->SyncSendEmail(email);
	}
#endif
}

void GlobalTimerTaskMan::OnClearEverydayGoldExchage()
{
	GetGoldExchangeMan()->ClearGoldExchangeRecord();
}

/* zpy 清理每天体力兑换记录 */
void GlobalTimerTaskMan::OnClearEverydayVitExchage()
{
	GetVitExchangeMan()->ClearVitExchangeRecord();
}

void GlobalTimerTaskMan::OnClearDailyTaskRecord()
{
	GetDailyTaskMan()->ClearDailyTaskFinishRecord();
}

void GlobalTimerTaskMan::OnClearRouletteReward()
{
	GetActitvityMan()->ClearRouletteReward();
}

void GlobalTimerTaskMan::OnClearPlayersActiveInstRcd()
{
	GetInstFinHistoryMan()->ClearPlayersActiveInstRecord();
}

//add by hxw 20151016 
//清理每天玩家挑战副本的次数,并将记录转移到rankinstnum表
void GlobalTimerTaskMan::OnRefreshAndClearRank()
{
	GetRankRewardMan()->RefreshRanks();
}
//end 20151016

/* zpy 发送月卡奖励 */
void GlobalTimerTaskMan::OnSendMonthCardReward()
{
	const std::map<UINT16, RechargeCfg::SRechargeCfgAttr> &config = GetGameCfgFileMan()->GetRechargeConfig();
	for (std::map<UINT16, RechargeCfg::SRechargeCfgAttr>::const_iterator itr = config.begin(); itr != config.end(); ++itr)
	{
		if (itr->second.everydayGive > 0 && itr->second.giveDays > 0)
		{
			char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
			sprintf(szSQL, "SELECT accountid,actorid,DATEDIFF(DATE_ADD(confirmtts,INTERVAL %u DAY),NOW()) FROM recharge WHERE status!=0 AND rechargeid=%u AND DATE_ADD(confirmtts,INTERVAL %u DAY) >= NOW()",
				itr->second.giveDays,
				itr->first,
				itr->second.giveDays);

			DBS_RESULT result = NULL;
			int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
			if (retCode != DBS_OK)
				continue;

			UINT32 nRows = DBS_NumRows(result);
			if (nRows == 0)
			{
				DBS_FreeResult(result);
				continue;
			}

			DBS_ROW row = NULL;
			while ((row = DBS_FetchRow(result)) != NULL)
			{
				UINT32 accountid = (UINT32)_atoi64(row[0]);
				UINT32 actorid = (UINT32)_atoi64(row[1]);
				UINT32 days = (UINT32)_atoi64(row[2]);

				EmailCleanMan::SSyncEmailSendContent email;
				email.pGBSenderNick = "系统";
				email.pGBCaption = "充值每日领取";
				email.receiverRoleId = actorid;
				email.receiverUserId = accountid;
				email.rpcoin = itr->second.everydayGive;

				char szContent[MAX_UTF8_EMAIL_BODY_LEN] = { 0 };
				sprintf(szContent, "你购买的%u元钻石月卡套餐，剩余领取天数%u天", itr->second.rmb, days);
				email.pGBBody = szContent;

				GetEmailCleanMan()->SyncSendEmail(email);
			}

			DBS_FreeResult(result);
		}
	}
}