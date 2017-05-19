#ifndef __RECOVERY_VIT_MAN_H__
#define __RECOVERY_VIT_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
恢复体力管理
*/

#include <map>
#include "cyaCoretimer.h"
#include "cyaLocker.h"

class RecoveryVitMan
{
	RecoveryVitMan(const RecoveryVitMan&);
	RecoveryVitMan& operator = (const RecoveryVitMan&);

public:
	RecoveryVitMan();
	~RecoveryVitMan();

	struct SRecoveryVitInfo
	{
		UINT32 userId;
		BYTE   linkerId;
	};

public:
	void Start();
	void Stop();

	void InsertRecoveryVitRole(UINT32 userId, UINT32 roleId, BYTE linkerId);
	void RemoveRecoveryVitRole(UINT32 roleId);

private:
	static void RecoveryVitTimer(void* param, TTimerID id);
	void OnRecoveryVit();

private:
	CXTLocker m_locker;
	CyaCoreTimer m_recoveryVitTimer;
	std::map<UINT32/*角色id*/, SRecoveryVitInfo> m_recoveryRolesMap;
};

void InitRecoveryVitMan();
RecoveryVitMan* GetRecoveryVitMan();
void DestroyRecoveryVitMan();

#endif