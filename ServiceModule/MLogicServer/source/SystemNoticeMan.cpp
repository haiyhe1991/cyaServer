#include "SystemNoticeMan.h"
#include "GWSClientMan.h"
#include "ServiceMLogic.h"
#include "GameCfgFileMan.h"
#include "gb_utf8.h"

static SystemNoticeMan* sg_noticeMan = NULL;
void InitSystemNoticeMan()
{
	ASSERT(sg_noticeMan == NULL);
	sg_noticeMan = new SystemNoticeMan();
	ASSERT(sg_noticeMan != NULL);
}

SystemNoticeMan* GetSystemNoticeMan()
{
	return sg_noticeMan;
}

void DestroySystemNoticeMan()
{
	SystemNoticeMan* pNoticeMan = sg_noticeMan;
	sg_noticeMan = NULL;
	if (pNoticeMan != NULL)
		delete pNoticeMan;
}

void OpenSystemNotice()
{
	if (sg_noticeMan)
		sg_noticeMan->Start();
}

void CloseSystemNotice()
{
	if (sg_noticeMan)
		sg_noticeMan->Stop();
}

SystemNoticeMan::SystemNoticeMan()
{

}

SystemNoticeMan::~SystemNoticeMan()
{

}

void SystemNoticeMan::Start()
{
	UINT32 timeInterval = GetGameCfgFileMan()->GetNoticeInterval();
	if (timeInterval == 0)
		m_lastInterval = 60;
	else
		m_lastInterval = timeInterval;

	m_timer.Start(NoticeTimerCallback, this, m_lastInterval * 1000);
}

void SystemNoticeMan::Stop()
{
	m_timer.Stop();
}

void SystemNoticeMan::FragmentComposeNotice(const char* playerNick, BYTE jobId, UINT16 composeId)
{
	char buf[MAX_BUF_LEN] = { 0 };
	int bufLen = GWS_COMMON_REPLY_LEN;
	SSysNoticeNotify* pNoticeNotify = (SSysNoticeNotify*)(buf + bufLen);
	pNoticeNotify->type = ESGS_NOTICE_FRAGMENT_COMPOSE;

	std::string name;
	GetGameCfgFileMan()->GetPropertyName(ESGS_PROP_EQUIP, composeId, name);

	int offset = 0;
	int n = (int)strlen(playerNick);
	memcpy(pNoticeNotify->data + offset, playerNick, n);
	offset += n;

	std::string strDesc("通过不懈的努力，终于合出了");
	strDesc += name;
	//玩家X（玩家名字）通过不懈的努力，终于合出了Y（装备名）

	int maxLen = (MAX_RES_USER_BYTES - member_offset(SSysNoticeNotify, type, data)) / sizeof(char);
	int len = gb2312_utf8(strDesc.c_str(), (int)strDesc.length(), pNoticeNotify->data + offset, maxLen - offset);
	ASSERT(len > 0);
	if (len <= 0)
		return;

	pNoticeNotify->len = len + offset;
	bufLen += sizeof(SSysNoticeNotify)+(pNoticeNotify->len - 1) * sizeof(char);

	pNoticeNotify->hton();
	OnBroadcastNotice(buf, bufLen);
}

void SystemNoticeMan::TransferOpenNotice()
{
	char buf[MAX_BUF_LEN] = { 0 };
	int bufLen = GWS_COMMON_REPLY_LEN;
	SSysNoticeNotify* pNoticeNotify = (SSysNoticeNotify*)(buf + bufLen);
	pNoticeNotify->type = ESGS_NOTICE_PERSONAL_RAND_INST_OPEN;

	const char* pszNotice = "庇护所里出现了神秘的传送门";
	int maxLen = (MAX_RES_USER_BYTES - member_offset(SSysNoticeNotify, type, data)) / sizeof(char);
	int len = gb2312_utf8(pszNotice, (int)strlen(pszNotice), pNoticeNotify->data, maxLen);
	ASSERT(len > 0);
	if (len <= 0)
		return;

	pNoticeNotify->len = len;
	bufLen += sizeof(SSysNoticeNotify)+(pNoticeNotify->len - 1) * sizeof(char);

	pNoticeNotify->hton();
	OnBroadcastNotice(buf, bufLen);
}

void SystemNoticeMan::TransferCloseNotice()
{
	char buf[MAX_BUF_LEN] = { 0 };
	int bufLen = GWS_COMMON_REPLY_LEN;
	SSysNoticeNotify* pNoticeNotify = (SSysNoticeNotify*)(buf + bufLen);
	pNoticeNotify->type = ESGS_NOTICE_PERSONAL_RAND_INST_CLOSE;

	const char* pszNotice = "庇护所里神秘的传送门消失了";
	int maxLen = (MAX_RES_USER_BYTES - member_offset(SSysNoticeNotify, type, data)) / sizeof(char);
	int len = gb2312_utf8(pszNotice, (int)strlen(pszNotice), pNoticeNotify->data, maxLen);
	ASSERT(len > 0);
	if (len <= 0)
		return;

	pNoticeNotify->len = len;
	bufLen += sizeof(SSysNoticeNotify)+(pNoticeNotify->len - 1) * sizeof(char);

	pNoticeNotify->hton();
	OnBroadcastNotice(buf, bufLen);
}

/*zpy 2015.10.10新增 */
void SystemNoticeMan::BroadcastSysyemNoticeUTF8(const char *content)
{
	char buf[MAX_BUF_LEN] = { 0 };
	int bufLen = GWS_COMMON_REPLY_LEN;
	SSysNoticeNotify* pNoticeNotify = (SSysNoticeNotify*)(buf + bufLen);
	pNoticeNotify->type = ESGS_NOTICE_SYSTEM;

	int len = strlen(content);
	int maxLen = (MAX_RES_USER_BYTES - member_offset(SSysNoticeNotify, type, data)) / sizeof(char);
	if (len <= 0) return;

	// 内容截断
	if (len < maxLen)
	{
		pNoticeNotify->len = len;
		strcpy(pNoticeNotify->data, content);
	}
	else
	{
		pNoticeNotify->len = maxLen;
		memcpy(pNoticeNotify->data, content, maxLen);
		pNoticeNotify->data[maxLen] = 0;
	}
	bufLen += sizeof(SSysNoticeNotify)+(pNoticeNotify->len - 1) * sizeof(char);

	pNoticeNotify->hton();
	OnBroadcastNotice(buf, bufLen);
}

void SystemNoticeMan::BroadcastSysyemNoticeGB2312(const char *content)
{
	char buf[MAX_BUF_LEN] = { 0 };
	int bufLen = GWS_COMMON_REPLY_LEN;
	SSysNoticeNotify* pNoticeNotify = (SSysNoticeNotify*)(buf + bufLen);
	pNoticeNotify->type = ESGS_NOTICE_SYSTEM;

	int maxLen = (MAX_RES_USER_BYTES - member_offset(SSysNoticeNotify, type, data)) / sizeof(char);
	int len = gb2312_utf8(content, (int)strlen(content), pNoticeNotify->data, maxLen);
	ASSERT(len > 0);
	if (len <= 0)
		return;

	pNoticeNotify->len = len;
	bufLen += sizeof(SSysNoticeNotify)+(pNoticeNotify->len - 1) * sizeof(char);

	pNoticeNotify->hton();
	OnBroadcastNotice(buf, bufLen);
}

//zpy 世界Boss死亡公告
void SystemNoticeMan::WorldBossDeathNotice()
{
	char buf[MAX_BUF_LEN] = { 0 };
	int bufLen = GWS_COMMON_REPLY_LEN;
	SSysNoticeNotify* pNoticeNotify = (SSysNoticeNotify*)(buf + bufLen);
	pNoticeNotify->type = ESGS_NOTICE_WORLDBOSS_END;

	char szBuffer[256];
	LimitedTimeActivityCfg::SActivityConfig config;
	GetGameCfgFileMan()->GetLimitedTimeActivityCfg(LimitedTimeActivityCfg::WORLD_BOSS, config);
	sprintf(szBuffer, "本次世界BOSS讨伐战已经结束，请各位参与活动的勇士查看邮箱收取对应活动奖励，下次活动将在明日%02u:%02u准时开启。", config.StartTimeHour, config.StartTimeMinute);

	int maxLen = (MAX_RES_USER_BYTES - member_offset(SSysNoticeNotify, type, data)) / sizeof(char);
	int len = gb2312_utf8(szBuffer, (int)strlen(szBuffer), pNoticeNotify->data, maxLen);
	ASSERT(len > 0);
	if (len <= 0)
		return;

	pNoticeNotify->len = len;
	bufLen += sizeof(SSysNoticeNotify)+(pNoticeNotify->len - 1) * sizeof(char);

	pNoticeNotify->hton();
	OnBroadcastNotice(buf, bufLen);
}

void SystemNoticeMan::OnBroadcastNotice(void* buf, int bufLen)
{
	GWSClient* gwsSession = GetGWSClientMan()->RandGWSClient();
	if (gwsSession == NULL)
		return;

	gwsSession->SendResponse(LINK_SERVER_1, 0, MLS_OK, MLS_SYS_NOTICE_NOTIFY, buf, bufLen, DATA_LINKER_GROUP_PACKET);
}

void SystemNoticeMan::NoticeTimerCallback(void* param, TTimerID id)
{
	SystemNoticeMan* pThis = (SystemNoticeMan*)param;
	if (pThis)
		pThis->OnNotice();
}

void SystemNoticeMan::OnNotice()
{
	UINT32 timeInterval = 0;
	std::vector<NoticeCfg::SNoticeItem> noticeVec;
	if (!GetGameCfgFileMan()->GetNoticeItems(noticeVec, timeInterval))
		return;

	char buf[MAX_BUF_LEN] = { 0 };
	int bufLen = GWS_COMMON_REPLY_LEN;
	SSysNoticeNotify* pNoticeNotify = (SSysNoticeNotify*)(buf + bufLen);
	pNoticeNotify->type = ESGS_NOTICE_SYSTEM;

	int si = noticeVec.size();
	for (int i = 0; i < si; ++i)
	{
		const char* pszNotice = noticeVec[i].strContent.c_str();
		int maxLen = (MAX_RES_USER_BYTES - member_offset(SSysNoticeNotify, type, data)) / sizeof(char);
		int len = gb2312_utf8(pszNotice, (int)strlen(pszNotice), pNoticeNotify->data, maxLen);
		ASSERT(len > 0);
		if (len <= 0)
			return;

		pNoticeNotify->len = len;
		bufLen += sizeof(SSysNoticeNotify)+(pNoticeNotify->len - 1) * sizeof(char);

		pNoticeNotify->hton();
		OnBroadcastNotice(buf, bufLen);
	}

	if (m_lastInterval != timeInterval)
	{
		m_timer.SetPeriod(timeInterval * 1000);
		m_lastInterval = timeInterval;
	}
}