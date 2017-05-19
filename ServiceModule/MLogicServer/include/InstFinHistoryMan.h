#ifndef __INST_FIN_HISTORY_MAN_H__
#define __INST_FIN_HISTORY_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
副本历史记录管理
*/

#include <map>
#include <vector>
#include "cyaCoreTimer.h"
#include "ServiceProtocol.h"

class InstFinHistoryMan
{
	InstFinHistoryMan(const InstFinHistoryMan&);
	InstFinHistoryMan& operator = (const InstFinHistoryMan&);

	struct SInstHistoryRecord	//需要记录的副本完成信息
	{
		UINT32 roleId;	//角色id
		UINT16 instId;	//副本id
		BYTE   star;	//评星
		UINT16 comlatedTimes;	//当前在线进入副本次数
		char   fintts[20];		//完成时间

		SInstHistoryRecord()
		{
			roleId = 0;
			instId = 0;
			star = 0;
			comlatedTimes = 0;
			memset(fintts, 0, sizeof(fintts));
		}

		SInstHistoryRecord(UINT32 finRoleId, UINT16 finInstId, BYTE finStar, UINT16 times = 1)
		{
			roleId = finRoleId;
			instId = finInstId;
			star = finStar;
			comlatedTimes = times;
			GetLocalStrTime(fintts);
		}
	};

public:
	InstFinHistoryMan();
	~InstFinHistoryMan();

	//完成副本历史记录
	void InputInstFinRecord(UINT32 roleId, UINT16 instId, BYTE star);
	void EnforceSyncInstFinRecord(UINT32 roleId);

	//查询已完成副本
	int  QueryFinishedInst(UINT32 roleId, UINT16 from, BYTE num, SQueryFinishedInstRes* pFinInstRes, BYTE maxNum);
	//该副本是否已完成
	BOOL IsFinishedInst(UINT32 roleId, UINT16 instId);

	//查询活动副本挑战次数
	int GetAvtiveInstChallengeTimes(UINT32 roleId, SGetActiveInstChallengeTimesRes* pChallengeTimesRes);
	//更新活动副本完成次数
	int UpdateAvtiveInstFinTimes(UINT32 roleId, UINT16 instId);
	//清理玩家当天活动副本记录
	void ClearPlayersActiveInstRecord();



private:
	static void RefreshInstFinHistoryTimer(void* param, TTimerID id);
	void OnRefreshInstFinHistory();
	int  UpdateInstFinRecord(const SInstHistoryRecord& rec);

private:
	//刷新定时器
	CyaCoreTimer m_refreshTimer;

	//完成副本历史信息
	CXTLocker m_locker;
	std::map<std::string, SInstHistoryRecord> m_instHistoryMap;
};

void InitInstFinHistoryMan();
InstFinHistoryMan* GetInstFinHistoryMan();
void DestroyInstFinHistoryMan();

#endif	//_InstFinHistoryMan_h__