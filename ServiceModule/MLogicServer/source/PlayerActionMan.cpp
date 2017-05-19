#include "PlayerActionMan.h"
#include "RoleInfos.h"
#include "gb_utf8.h"
#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "GameCfgFileMan.h"

static PlayerActionMan* sg_pActionMan = NULL;
void InitPlayerActionMan()
{
	ASSERT(sg_pActionMan == NULL);
	sg_pActionMan = new PlayerActionMan();
	sg_pActionMan->Start();
	ASSERT(sg_pActionMan != NULL);
}

PlayerActionMan* GetPlayerActionMan()
{
	return sg_pActionMan;
}

void DestroyPlayerActionMan()
{
	PlayerActionMan* pActionMan = sg_pActionMan;
	sg_pActionMan = NULL;
	if (pActionMan)
	{
		pActionMan->Stop();
		delete pActionMan;
	}
}

PlayerActionMan::PlayerActionMan()
: m_exit(true)
, m_thHandle(INVALID_OSTHREAD)
{

}

PlayerActionMan::~PlayerActionMan()
{

}

void PlayerActionMan::Start()
{
	m_exit = false;
	OSCreateThread(&m_thHandle, NULL, THWorker, this, 0);
}

void PlayerActionMan::Stop()
{
	m_exit = true;
	if (m_thHandle != INVALID_OSTHREAD)
	{
		OSCloseThread(m_thHandle);
		m_thHandle = INVALID_OSTHREAD;
	}

	while (!m_enterInstQ.empty())
		OnRecordEnterInst();

	while (!m_finInstQ.empty())
		OnRecordFinishInst();
}

int PlayerActionMan::THWorker(void* param)
{
	PlayerActionMan* pThis = (PlayerActionMan*)param;
	while (!pThis->m_exit)
	{
		BOOL bIdle1 = pThis->OnRecordEnterInst();
		BOOL bIdle2 = pThis->OnRecordFinishInst();

		if (bIdle1 && bIdle2)
			Sleep(1);
	}

	return 0;
}

BOOL PlayerActionMan::OnRecordEnterInst()
{
	if (m_enterInstQ.empty())
		return true;

	int nCount = GetConfigFileMan()->GetActionBatchNum();	//100;
	std::string strSQL = "insert into instenterlog(actorid, actornick, job, level, instid, instname, enter_result, enter_tts, enter_desc, fin_result) values";

	{
		CXTAutoLock lock(m_enterInstQLocker);
		if (m_enterInstQ.empty())
			return true;

		while (nCount-- > 0 && !m_enterInstQ.empty())
		{
			char szDate[32] = { 0 };
			char szUTF8Desc[256] = { 0 };
			char szUTF8InstName[256] = { 0 };
			char szItem[GENERIC_SQL_BUF_LEN] = { 0 };
			std::string strInstName;

			SEnterInstItem& item = m_enterInstQ.front();
			if (item.result == MLS_ROLE_NOT_EXIST)
			{
				m_enterInstQ.pop();
				continue;
			}

			MselToLocalStrTime(item.tts, szDate);
			GetGameCfgFileMan()->GetInstName(item.instId, strInstName);
			if (strInstName != "")
				gb2312_utf8(strInstName.c_str(), (int)strInstName.length(), szUTF8InstName, sizeof(szUTF8InstName)-sizeof(char));

			const char* pszEnterDesc = "未知原因";
			switch (item.result)
			{
			case MLS_OK:
				pszEnterDesc = "进入成功";
				break;
			case MLS_LOWER_INST_LIMIT_LEVEL:
				pszEnterDesc = "低于副本限制角色等级";
				break;
			case MLS_VIT_NOT_ENOUGH:
				pszEnterDesc = "体力不足";
				break;
			case MLS_HAS_INST_DROP_EMAIL:
				pszEnterDesc = "副本掉落邮件未处理";
				break;
			case MLS_NOT_EXIST_INST:
				pszEnterDesc = "不存在的副本";
				break;
			case MLS_NOT_OPEN_RAND_INST:
				pszEnterDesc = "未开启的随机副本";
				break;
			case MLS_RAND_INST_FULL:
				pszEnterDesc = "随机副本人数已满";
				break;
			default:
				break;
			}

			gb2312_utf8(pszEnterDesc, strlen(pszEnterDesc), szUTF8Desc, sizeof(szUTF8Desc)-sizeof(char));
			sprintf(szItem, "(%u, '%s', %d, %d, %d, '%s', %d, '%s', '%s', %d),",
				item.roleId, item.strNick.c_str(), item.job, item.level, item.instId, szUTF8InstName, item.result, szDate, szUTF8Desc, 1);
			m_enterInstQ.pop();
			strSQL += szItem;
		}
	}

	GetDBSClient()->Insert(GetConfigFileMan()->GetPlayerActionDBName(), strSQL.c_str(), NULL, (int)strSQL.length() - 1);
	return false;
}

BOOL PlayerActionMan::OnRecordFinishInst()
{
	if (m_finInstQ.empty())
		return true;

	SFinishInstItem finInstItem;

	{
		CXTAutoLock lock(m_finInstQLocker);
		if (m_finInstQ.empty())
			return true;
		finInstItem = m_finInstQ.front();
		m_finInstQ.pop();
	}

	char szUTF8Desc[256] = { 0 };
	char szEnterDate[32] = { 0 };
	char szFinishDate[32] = { 0 };

	const char* pszFinishDesc = "未知原因";
	switch (finInstItem.result)
	{
	case MLS_OK:
		pszFinishDesc = "完成成功";
		break;
	case MLS_INST_FAILED_TIMEOVER:
		pszFinishDesc = "超出副本完成时间";
		break;
	case MLS_INST_FAILED_DEAD:
		pszFinishDesc = "角色死亡";
		break;
	case MLS_INST_FAILED_EXIT:
		pszFinishDesc = "中途退出副本";
		break;
	case MLS_INST_FAILED_STONE_BROKEN:
		pszFinishDesc = "石碑破碎";
		break;
	default:
		break;
	}

	MselToLocalStrTime(finInstItem.enter_tts, szEnterDate);
	MselToLocalStrTime(finInstItem.fin_tts, szFinishDate);

	gb2312_utf8(pszFinishDesc, strlen(pszFinishDesc), szUTF8Desc, sizeof(szUTF8Desc)-sizeof(char));

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update instenterlog set fin_result=%d, fin_tts='%s', fin_desc='%s' where actorid=%u and instid=%u and enter_tts='%s'",
		finInstItem.result, szFinishDate, szUTF8Desc, finInstItem.roleId, finInstItem.instId, szEnterDate);
	GetDBSClient()->Update(GetConfigFileMan()->GetPlayerActionDBName(), szSQL, NULL);
	return false;
}

void PlayerActionMan::InputEnterInstLog(const SEnterInstItem& enterItem)
{
	CXTAutoLock lock(m_enterInstQLocker);
	m_enterInstQ.push(enterItem);
}

void PlayerActionMan::InputFinishInstLog(const SFinishInstItem& finItem)
{
	CXTAutoLock lock(m_finInstQLocker);
	m_finInstQ.push(finItem);
}