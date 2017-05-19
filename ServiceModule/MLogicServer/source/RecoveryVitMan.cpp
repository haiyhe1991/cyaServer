#include "RecoveryVitMan.h"
#include "GameCfgFileMan.h"
#include "RolesInfoMan.h"
#include "GWSClientMan.h"
#include "ServiceErrorCode.h"

static RecoveryVitMan* sg_recoveryMan = NULL;
void InitRecoveryVitMan()
{
	ASSERT(sg_recoveryMan == NULL);
	sg_recoveryMan = new RecoveryVitMan();
	ASSERT(sg_recoveryMan != NULL);
	sg_recoveryMan->Start();
}

RecoveryVitMan* GetRecoveryVitMan()
{
	return sg_recoveryMan;
}

void DestroyRecoveryVitMan()
{
	RecoveryVitMan* recoveryMan = sg_recoveryMan;
	sg_recoveryMan = NULL;
	if (recoveryMan)
	{
		recoveryMan->Stop();
		delete recoveryMan;
	}
}

RecoveryVitMan::RecoveryVitMan()
{

}

RecoveryVitMan::~RecoveryVitMan()
{

}

void RecoveryVitMan::Start()
{
	DWORD sec = GetGameCfgFileMan()->GetVitRecoveryInterval();
	if (sec > 0)
		m_recoveryVitTimer.Start(RecoveryVitTimer, this, sec * 1000);
}

void RecoveryVitMan::Stop()
{
	m_recoveryVitTimer.Stop();
}

void RecoveryVitMan::RecoveryVitTimer(void* param, TTimerID id)
{
	RecoveryVitMan* pThis = (RecoveryVitMan*)param;
	if (pThis)
		pThis->OnRecoveryVit();
}

void RecoveryVitMan::OnRecoveryVit()
{
	if (m_recoveryRolesMap.empty())
		return;

	std::map<UINT32/*角色id*/, SRecoveryVitInfo> roles;
	{
		CXTAutoLock lock(m_locker);
		roles.insert(m_recoveryRolesMap.begin(), m_recoveryRolesMap.end());
	}

	UINT32 recoveryVit = GetGameCfgFileMan()->GetVitRecoveryVal();

	for (std::map<UINT32/*角色id*/, SRecoveryVitInfo>::iterator it = roles.begin(); it != roles.end(); ++it)
	{
		UINT32 vit = recoveryVit;
		BOOL needNotify = false;
		int retCode = GetRolesInfoMan()->RecoveryOnlineRoleVit(it->second.userId, it->first, vit, needNotify);
		if (retCode != MLS_OK || !needNotify)
			continue;

		GWSClient* gwsSession = GetGWSClientMan()->RandGWSClient();
		if (gwsSession == NULL)
			continue;

		//推送体力恢复
		char buf[GWS_COMMON_REPLY_LEN + sizeof(SVitUpdateNotify)] = { 0 };
		SVitUpdateNotify* pVitUpdate = (SVitUpdateNotify*)(buf + GWS_COMMON_REPLY_LEN);
		pVitUpdate->vit = vit;
		pVitUpdate->hton();
		gwsSession->SendResponse(it->second.linkerId, it->second.userId, MLS_OK, MLS_VIT_UPDATE_NOTIFY, buf, GWS_COMMON_REPLY_LEN + sizeof(SVitUpdateNotify));
	}
}

void RecoveryVitMan::InsertRecoveryVitRole(UINT32 userId, UINT32 roleId, BYTE linkerId)
{
	SRecoveryVitInfo item;
	item.linkerId = linkerId;
	item.userId = userId;

	CXTAutoLock lock(m_locker);
	std::map<UINT32/*角色id*/, SRecoveryVitInfo>::iterator it = m_recoveryRolesMap.find(roleId);
	if (it == m_recoveryRolesMap.end())
		m_recoveryRolesMap.insert(std::make_pair(roleId, item));
}

void RecoveryVitMan::RemoveRecoveryVitRole(UINT32 roleId)
{
	CXTAutoLock lock(m_locker);
	std::map<UINT32/*角色id*/, SRecoveryVitInfo>::iterator it = m_recoveryRolesMap.find(roleId);
	if (it != m_recoveryRolesMap.end())
		m_recoveryRolesMap.erase(it);
}