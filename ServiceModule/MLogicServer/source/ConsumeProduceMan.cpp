#include "ConsumeProduceMan.h"
#include "DBSClient.h"
#include "RoleInfos.h"
#include "ConfigFileMan.h"
#include "gb_utf8.h"

static ConsumeProduceMan* sg_pCPMan = NULL;
void InitConsumeProduceMan()
{
	ASSERT(sg_pCPMan == NULL);
	sg_pCPMan = new ConsumeProduceMan();
	ASSERT(sg_pCPMan != NULL);
	sg_pCPMan->Start();
}

ConsumeProduceMan* GetConsumeProduceMan()
{
	return sg_pCPMan;
}

void DestroyConsumeProduceMan()
{
	ConsumeProduceMan* pMan = sg_pCPMan;
	sg_pCPMan = NULL;
	if (pMan != NULL)
	{
		pMan->Stop();
		delete pMan;
	}
}

ConsumeProduceMan::ConsumeProduceMan()
: m_exit(true)
, m_thHandle(INVALID_OSTHREAD)
{

}

ConsumeProduceMan::~ConsumeProduceMan()
{

}

void ConsumeProduceMan::Start()
{
	m_exit = false;
	OSCreateThread(&m_thHandle, NULL, THWorker, this, 0);
}

void ConsumeProduceMan::Stop()
{
	m_exit = true;
	if (m_thHandle != INVALID_OSTHREAD)
	{
		OSCloseThread(m_thHandle);
		m_thHandle = INVALID_OSTHREAD;
	}

	while (!m_consumeLogQ.empty())
		OnWriteConsumeLog();

	while (!m_produceLogQ.empty())
		OnWriteProduceLog();
}

void ConsumeProduceMan::InputConsumeRecord(BYTE moneyType, UINT32 roleId, const char* nick, UINT32 num, const char* consumeAction, UINT64 remain)
{
	SConsumeProduceLogItem consumeItem;
	consumeItem.consumeDate = GetMsel();
	consumeItem.moneyType = moneyType;
	consumeItem.consumeDesc = consumeAction;
	consumeItem.nick = nick;
	consumeItem.roleId = roleId;
	consumeItem.num = num;
	consumeItem.remain = remain;

	CXTAutoLock lock(m_consumeLogQLocker);
	m_consumeLogQ.push(consumeItem);
}

void ConsumeProduceMan::InputProduceRecord(BYTE moneyType, UINT32 roleId, const char* nick, UINT32 num, const char* consumeAction, UINT64 remain)
{
	SConsumeProduceLogItem produceItem;
	produceItem.consumeDate = GetMsel();
	produceItem.moneyType = moneyType;
	produceItem.consumeDesc = consumeAction;
	produceItem.nick = nick;
	produceItem.roleId = roleId;
	produceItem.num = num;
	produceItem.remain = remain;

	CXTAutoLock lock(m_produceLogQLocker);
	m_produceLogQ.push(produceItem);
}

int ConsumeProduceMan::THWorker(void* param)
{
	ConsumeProduceMan* pThis = (ConsumeProduceMan*)param;
	ASSERT(pThis != NULL);
	while (!pThis->m_exit)
	{
		BOOL bIdle1 = pThis->OnWriteConsumeLog();
		BOOL bIdle2 = pThis->OnWriteProduceLog();

		if (bIdle1 && bIdle2)
			Sleep(1);
	}
	return 0;
}

BOOL ConsumeProduceMan::OnWriteConsumeLog()
{
	if (m_consumeLogQ.empty())
		return true;

	int nCount = GetConfigFileMan()->GetActionBatchNum();	//100;
	std::string strSQL = "insert into consumelog(actorid, actornick, action, money, remain, type, tts) values";

	{
		CXTAutoLock lock(m_consumeLogQLocker);
		if (m_consumeLogQ.empty())
			return true;

		while (nCount-- > 0 && !m_consumeLogQ.empty())
		{
			char szDate[32] = { 0 };
			char szUTF8Action[256] = { 0 };
			char szItem[GENERIC_SQL_BUF_LEN] = { 0 };

			SConsumeProduceLogItem& item = m_consumeLogQ.front();
			MselToLocalStrTime(item.consumeDate, szDate);
			if (item.consumeDesc != "")
				gb2312_utf8(item.consumeDesc.c_str(), (int)item.consumeDesc.length(), szUTF8Action, sizeof(szUTF8Action)-sizeof(char));
			sprintf(szItem, "(%u, '%s', '%s', %u, %llu, %d, '%s'),", item.roleId, item.nick.c_str(), szUTF8Action, item.num, item.remain, item.moneyType, szDate);
			m_consumeLogQ.pop();
			strSQL += szItem;
		}
	}

	GetDBSClient()->Insert(GetConfigFileMan()->GetPlayerActionDBName(), strSQL.c_str(), NULL, (int)strSQL.length() - 1);
	return false;
}

BOOL ConsumeProduceMan::OnWriteProduceLog()
{
	if (m_produceLogQ.empty())
		return true;

	int nCount = GetConfigFileMan()->GetActionBatchNum();	//100;
	std::string strSQL = "insert into producelog(actorid, actornick, action, money, remain, type, tts) values";

	{
		CXTAutoLock lock(m_produceLogQLocker);
		if (m_produceLogQ.empty())
			return true;

		while (nCount-- > 0 && !m_produceLogQ.empty())
		{
			char szDate[32] = { 0 };
			char szUTF8Action[256] = { 0 };
			char szItem[GENERIC_SQL_BUF_LEN] = { 0 };

			SConsumeProduceLogItem& item = m_produceLogQ.front();
			MselToLocalStrTime(item.consumeDate, szDate);
			if (item.consumeDesc != "")
				gb2312_utf8(item.consumeDesc.c_str(), (int)item.consumeDesc.length(), szUTF8Action, sizeof(szUTF8Action)-sizeof(char));
			sprintf(szItem, "(%u, '%s', '%s', %u, %llu, %d, '%s'),", item.roleId, item.nick.c_str(), szUTF8Action, item.num, item.remain, item.moneyType, szDate);
			m_produceLogQ.pop();
			strSQL += szItem;
		}
	}

	GetDBSClient()->Insert(GetConfigFileMan()->GetPlayerActionDBName(), strSQL.c_str(), NULL, (int)strSQL.length() - 1);
	return false;
}
