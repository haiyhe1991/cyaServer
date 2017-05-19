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
		//add by hxw 20151201 ��֤0��ˢ��
		ASSERT(printf("nowtime:%d:%d:%d:%d\n", lcTime.wHour, lcTime.wMinute, lcTime.wSecond, lcTime.wMilliseconds));
		if (m_lastDay == 0)
		{
			m_lastDay = lcTime.wDay;
			m_lastDayOfOffset = lcTime.wDay;
			m_lastMonth = lcTime.wMonth;
			//m_sleep.Sleep(1800*1000);	//��Сʱ
			int minute = 60 - lcTime.wMinute - 1;
			int second = 60 - lcTime.wSecond - 1;
			int millsecond = 1000 - lcTime.wMilliseconds;
			int stime = minute * 60 * 1000 + second * 1000 + millsecond;
			ASSERT(printf("needsleep:%dm%ds%dms��all %dms\n", minute, second, millsecond, stime));
			stime += 30 * 1000;//+30��ʾ��0��30���ʱ��ˢ��
			m_sleep.Sleep(stime);	//��Сʱ			
			continue;
		}

		//end
		if (lcTime.wDay != m_lastDay && lcTime.wHour >= 0)		//�賿0���Ժ�
		{
			//���Ÿ���ռ�콱��
			OnSendInstOccupyReward();
			//����ÿ���Ҷһ���¼
			OnClearEverydayGoldExchage();
			//�����ճ�����
			OnClearDailyTaskRecord();
			//������̽�Ʒ
			OnClearRouletteReward();
			//�����һ������¼
			OnClearPlayersActiveInstRcd();
			//�������߱�������
			GetRolesInfoMan()->CleanEverydayData();
			/* zpy �����¿����� */
			OnSendMonthCardReward();
			/* zpy ����ÿ�������һ���¼ */
			OnClearEverydayVitExchage();
			/* zpy ����ÿ�ս��� */
			GetLadderRankMan()->DailySettlement();
			/* zpy ����ģʽ��ս�������� */
			GetCompetitiveModeMan()->ClearEveryday();
			/* zpy 1v1������������ */
			GetOneVsOneMan()->ClearEveryday();
			//add by hxw20151016
			OnRefreshAndClearRank();
			//ends
			//��������
			m_lastDay = lcTime.wDay;

		}

		/* ����ģʽ */
		if (lcTime.wDay != m_lastDayOfOffset && lcTime.wHour >= 3)
		{
			if (lcTime.wDayOfWeek == 1)	//ÿ�ܵ�һ��
			{
				GetCompetitiveModeMan()->GrantCompetitiveModeReward();
				GetCompetitiveModeMan()->GeneraWeekFreeActorType();
			}

			//��������
			m_lastDayOfOffset = lcTime.wDay;
		}

		/* zpy ÿ��ǩ���������� */
		if (lcTime.wMonth != m_lastMonth && lcTime.wDay == 1)
		{
			GetActitvityMan()->ClearRoleDailySign();
			m_lastMonth = lcTime.wMonth;
		}
		//add by hxw 20151203 �������������
		GetLocalTime(&lcTime);
		m_sleep.Sleep(3600 * 1000 - lcTime.wMilliseconds);	//1Сʱ
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
		//��ȡ����ռ�콱��
		std::vector<InstCfg::SInstOccupyReward> occupyReward;
		int retCode = GetGameCfgFileMan()->GetInstOccupyReward(it->instId, occupyReward);
		if (retCode != MLS_OK)
			continue;

		//��ȡ������Ʒ����
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

		//�����ʼ�
		EmailCleanMan::SSyncEmailSendContent email;
		email.receiverUserId = it->userId;
		email.receiverRoleId = it->roleId;
		email.pGBSenderNick = "ϵͳ";		//�������ǳ�
		email.pGBCaption = "����ռ�콱��";	//����

		std::string instName;
		GetGameCfgFileMan()->GetInstName(it->instId, instName);
		char szBody[MAX_UTF8_EMAIL_BODY_LEN] = { 0 };
		sprintf(szBody, "ռ�츱��%s����", instName.c_str());
		email.pGBBody = szBody;
		GetEmailCleanMan()->SyncSendEmail(email);
	}
#endif
}

void GlobalTimerTaskMan::OnClearEverydayGoldExchage()
{
	GetGoldExchangeMan()->ClearGoldExchangeRecord();
}

/* zpy ����ÿ�������һ���¼ */
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
//����ÿ�������ս�����Ĵ���,������¼ת�Ƶ�rankinstnum��
void GlobalTimerTaskMan::OnRefreshAndClearRank()
{
	GetRankRewardMan()->RefreshRanks();
}
//end 20151016

/* zpy �����¿����� */
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
				email.pGBSenderNick = "ϵͳ";
				email.pGBCaption = "��ֵÿ����ȡ";
				email.receiverRoleId = actorid;
				email.receiverUserId = accountid;
				email.rpcoin = itr->second.everydayGive;

				char szContent[MAX_UTF8_EMAIL_BODY_LEN] = { 0 };
				sprintf(szContent, "�㹺���%uԪ��ʯ�¿��ײͣ�ʣ����ȡ����%u��", itr->second.rmb, days);
				email.pGBBody = szContent;

				GetEmailCleanMan()->SyncSendEmail(email);
			}

			DBS_FreeResult(result);
		}
	}
}