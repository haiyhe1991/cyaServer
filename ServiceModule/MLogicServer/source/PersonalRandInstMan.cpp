#include "PersonalRandInstMan.h"
#include "GameCfgFileMan.h"
#include "cyaMaxMin.h"
#include "GWSClientMan.h"
#include "cyaLog.h"
#include "SystemNoticeMan.h"

static PersonalRandInstMan* sg_personalInstMan = NULL;
void InitPersonalRandInstMan()
{
	ASSERT(sg_personalInstMan == NULL);
	sg_personalInstMan = new PersonalRandInstMan();
	ASSERT(sg_personalInstMan != NULL);
}

void DestroyPersonalRandInstMan()
{
	PersonalRandInstMan* pMan = sg_personalInstMan;
	sg_personalInstMan = NULL;
	if (pMan != NULL)
		delete pMan;
}

PersonalRandInstMan* GetPersonalRandInstMan()
{
	return sg_personalInstMan;
}

void StartPersonalRandInst()
{
	if (sg_personalInstMan)
		sg_personalInstMan->Start();
}

void StopPersonalRandInst()
{
	if (sg_personalInstMan)
		sg_personalInstMan->Stop();
}

PersonalRandInstMan::PersonalRandInstMan()
: m_thHandle(INVALID_OSTHREAD)
, m_exit(false)
{

}

PersonalRandInstMan::~PersonalRandInstMan()
{

}

void PersonalRandInstMan::Start()
{
	RandInstSysCfg::SRandInstSysParam param;
	BOOL ret = GetGameCfgFileMan()->GetRandInstSysParam(param);
	if (!ret)
	{
		m_openRandInstInterval = 60;
		m_lastTimeSec = 120;
		m_maxEnterNum = 16;
	}
	else
	{
		m_openRandInstInterval = param.timerInterVal;
		m_lastTimeSec = param.openDurationTime;
		m_maxEnterNum = param.maxNumPerTransfer;
	}

	m_timer.Start(RandTransferTimer, this, m_openRandInstInterval * 1000);
	OSCreateThread(&m_thHandle, NULL, THWorker, this, 0);
}

void PersonalRandInstMan::Stop()
{
	m_timer.Stop();
	m_exit = true;
	if (m_thHandle != INVALID_OSTHREAD)
	{
		OSCloseThread(m_thHandle);
		m_thHandle = INVALID_OSTHREAD;
	}
}

void PersonalRandInstMan::RandTransferTimer(void* param, TTimerID id)
{
	PersonalRandInstMan* pThis = (PersonalRandInstMan*)param;
	ASSERT(pThis != NULL);
	pThis->OnRandTransfer();
}

void PersonalRandInstMan::OnRandTransfer()
{
	RandInstSysCfg::SRandInstSysParam param;
	if (!GetGameCfgFileMan()->GetRandInstSysParam(param))
		return;

	if (!param.isOpen)
		return;

	if (!IsInOpenDurationTime(param.startHour, param.startMin, param.endHour, param.endMin))
		return;

	UINT16 nTransferNum = rand() % param.maxOpenTransferNum + 1;
	nTransferNum = min(max(param.minOpenTransferNum, nTransferNum), param.maxOpenTransferNum);

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SRandInstOpenNotify* pRandInstNotify = (SRandInstOpenNotify*)(buf + nLen);

	BYTE idx = 0;
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SRandInstOpenNotify, num, data)) / sizeof(SRandInstItem));
	for (UINT16 i = 1; i <= nTransferNum; ++i)
	{
		int si = (int)param.transferVec.size();
		if (si <= 0)
			break;

		int transferIdx = rand() % si;
		UINT16 transferId = param.transferVec[transferIdx];

		UINT16 randInstId = GetGameCfgFileMan()->GenerateRandInstId();
		if (randInstId == 0)
			continue;

		pRandInstNotify->data[idx].instId = randInstId;
		pRandInstNotify->data[idx].transferId = transferId;
		pRandInstNotify->data[idx].hton();

		for (std::vector<UINT16>::iterator it = param.transferVec.begin(); it != param.transferVec.end(); ++it)
		{
			if (*it != transferId)
				continue;

			param.transferVec.erase(it);
			break;
		}

		int ret = InputOpenTransfer(i, randInstId);
		if (ret != MLS_OK)
			continue;

		LogInfo(("打开单人随机副本传送阵[%d]副本[%d]!", i, randInstId));

		if (++idx >= maxNum)
			break;
	}

	if (idx > 0)
	{
		pRandInstNotify->num = idx;
		nLen += sizeof(SRandInstOpenNotify)+(pRandInstNotify->num - 1) * sizeof(SRandInstItem);
		GWSClient* gwsSession = GetGWSClientMan()->RandGWSClient();
		if (gwsSession != NULL)
		{
			gwsSession->SendResponse(LINK_SERVER_1, 0, MLS_OK, MLS_RAND_INST_OPEN_NOTIFY, buf, nLen, DATA_LINKER_GROUP_PACKET);
			GetSystemNoticeMan()->TransferOpenNotice();		//传送阵打开系统公告
		}
	}

	if (param.openDurationTime != m_lastTimeSec)
		m_lastTimeSec = param.openDurationTime;

	if (param.maxNumPerTransfer != m_maxEnterNum)
		m_maxEnterNum = param.maxNumPerTransfer;

	if (param.timerInterVal != m_openRandInstInterval)
	{
		m_openRandInstInterval = param.timerInterVal;
		m_timer.SetPeriod(m_openRandInstInterval * 1000);
	}
}

int PersonalRandInstMan::InputOpenTransfer(UINT16 transferId, UINT16 instId)
{
	SOpenRandInstStatus item;
	item.enterNum = 0;
	item.transferId = transferId;
	item.startMSel = GetMsel();

	CXTAutoLock lock(m_openTransferMapLocker);
	std::map<UINT16/*副本id*/, SOpenRandInstStatus>::iterator it = m_openTransferMap.find(instId);
	if (it != m_openTransferMap.end())
	{
		it->second.startMSel = item.startMSel;
		return MLS_ERROR;
	}

	m_openTransferMap.insert(std::make_pair(instId, item));
	return MLS_OK;
}

int PersonalRandInstMan::THWorker(void* param)
{
	PersonalRandInstMan* pThis = (PersonalRandInstMan*)param;
	ASSERT(pThis != NULL);
	return pThis->OnWorker();
}

int PersonalRandInstMan::OnWorker()
{
	while (!m_exit)
	{
		if (m_openTransferMap.empty())
		{
			Sleep(1);
			continue;
		}

		LTMSEL nowMSel = GetMsel();
		UINT32 lastTime = m_lastTimeSec * 1000;

		{
			CXTAutoLock lock(m_openTransferMapLocker);
			if (!m_openTransferMap.empty())
			{
				std::map<UINT16, SOpenRandInstStatus>::iterator it = m_openTransferMap.begin();
				for (; it != m_openTransferMap.end();)
				{
					SOpenRandInstStatus& item = it->second;
					if (item.startMSel > nowMSel)
					{
						OnRandInstClose(item.transferId, it->first);
						m_openTransferMap.erase(it++);
					}
					else if (nowMSel - item.startMSel >= lastTime)
					{
						OnRandInstClose(item.transferId, it->first);
						m_openTransferMap.erase(it++);
					}
					else
						++it;
				}
			}
		}

		Sleep(1);
	}

	return 0;
}

void PersonalRandInstMan::OnRandInstClose(UINT16 transferId, UINT16 instId)
{
	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SRandInstCloseNotify* pInstCloseNotify = (SRandInstCloseNotify*)(buf + nLen);
	pInstCloseNotify->inst.instId = instId;
	pInstCloseNotify->inst.transferId = transferId;
	pInstCloseNotify->inst.hton();

	nLen += sizeof(SRandInstCloseNotify);
	GWSClient* gwsSession = GetGWSClientMan()->RandGWSClient();
	if (gwsSession == NULL)
		return;

	gwsSession->SendResponse(LINK_SERVER_1, 0, MLS_OK, MLS_RAND_INST_CLOSE_NOTIFY, buf, nLen, DATA_LINKER_GROUP_PACKET);
	GetSystemNoticeMan()->TransferCloseNotice();		//传送阵关闭系统公告
	LogInfo(("关闭单人随机副本传送阵[%d]副本[%d]!", transferId, instId));
}

int PersonalRandInstMan::EnterRandInstId(UINT16 instId)
{
	UINT16 maxEnterNum = m_maxEnterNum;

	CXTAutoLock lock(m_openTransferMapLocker);
	std::map<UINT16, SOpenRandInstStatus>::iterator it = m_openTransferMap.find(instId);
	if (it == m_openTransferMap.end())
		return MLS_NOT_OPEN_RAND_INST;

	if (it->second.enterNum >= maxEnterNum)
		return MLS_RAND_INST_FULL;

	it->second.enterNum = SGSU16Add(it->second.enterNum, 1);
	return MLS_OK;
}

void PersonalRandInstMan::LeaveRandInstId(UINT16 instId)
{
	CXTAutoLock lock(m_openTransferMapLocker);
	std::map<UINT16, SOpenRandInstStatus>::iterator it = m_openTransferMap.find(instId);
	if (it == m_openTransferMap.end())
		return;

	it->second.enterNum = SGSU16Sub(it->second.enterNum, 1);
}

int PersonalRandInstMan::GetTransferEnterNum(UINT16 transferId, UINT16& enterNum, UINT32& leftSec)
{
	int retCode = MLS_NOT_OPEN_RAND_INST;
	if (m_openTransferMap.empty())
		return retCode;

	//持续时间
	UINT32 lastTimeMSel = m_lastTimeSec * 1000;
	//当前时间
	LTMSEL nowMSel = GetMsel();

	CXTAutoLock lock(m_openTransferMapLocker);
	std::map<UINT16, SOpenRandInstStatus>::iterator it = m_openTransferMap.begin();
	for (; it != m_openTransferMap.end(); ++it)
	{
		if (it->second.transferId != transferId)
			continue;

		enterNum = it->second.enterNum;
		if (it->second.startMSel > nowMSel)
			leftSec = 0;
		else
		{
			UINT32 n = nowMSel - it->second.startMSel;
			if (n >= lastTimeMSel)
				leftSec = 0;
			else
			{
				leftSec = lastTimeMSel - n;
				leftSec = leftSec / 1000;
			}
		}
		retCode = MLS_OK;
		break;
	}

	return retCode;
}

int PersonalRandInstMan::GetOpenedTransfers(SGetOpenPersonalRandInstRes* pOpenRes)
{
	if (m_openTransferMap.empty())
		return MLS_OK;

	BYTE idx = 0;
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SGetOpenPersonalRandInstRes, num, data)) / sizeof(SPersonalRandInstInfo));

	{
		CXTAutoLock lock(m_openTransferMapLocker);
		std::map<UINT16, SOpenRandInstStatus>::iterator it = m_openTransferMap.begin();
		for (; it != m_openTransferMap.end(); ++it)
		{
			pOpenRes->data[idx].instId = it->first;
			pOpenRes->data[idx].transferId = it->second.transferId;
			pOpenRes->data[idx].hton();

			if (++idx >= maxNum)
				break;
		}
	}

	pOpenRes->num = idx;
	return MLS_OK;
}

BOOL PersonalRandInstMan::IsInOpenDurationTime(BYTE startHour, BYTE startMin, BYTE endHour, BYTE endMin)
{
	SYSTEMTIME lcTime;
	GetLocalTime(&lcTime);
	if (lcTime.wHour > startHour && lcTime.wHour < endHour)
		return true;

	if (lcTime.wHour == startHour && lcTime.wMinute >= startMin)
		return true;

	if (lcTime.wHour == endHour && lcTime.wMinute <= endMin)
		return true;

	return false;
}