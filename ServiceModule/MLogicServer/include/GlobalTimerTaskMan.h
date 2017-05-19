#ifndef __GLOBAL_TIMER_TASK_MAN_H__
#define __GLOBAL_TIMER_TASK_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
全局定时任务管理
*/


#include "cyaCoreTimer.h"

class GlobalTimerTaskMan
{
	GlobalTimerTaskMan(const GlobalTimerTaskMan&);
	GlobalTimerTaskMan& operator = (const GlobalTimerTaskMan&);

public:
	GlobalTimerTaskMan();
	~GlobalTimerTaskMan();

	void Start();
	void Stop();

private:
	static int THWorker(void* param);
	int OnWorker();

	//发放副本占领奖励
	void OnSendInstOccupyReward();
	//清理每天金币兑换记录
	void OnClearEverydayGoldExchage();
	//清理日常任务
	void OnClearDailyTaskRecord();
	//清理轮盘抽奖
	void OnClearRouletteReward();
	//清除玩家活动副本记录
	void OnClearPlayersActiveInstRcd();
	/* zpy 发送月卡奖励 */
	void OnSendMonthCardReward();
	/* zpy 清理每天体力兑换记录 */
	void OnClearEverydayVitExchage();

	//add by hxw 20151016
	//清理每天玩家挑战副本的次数,并将记录转移到rankinstnum表
	//更新LV排行榜	
	//更新instnum排行榜	
	void OnRefreshAndClearRank();
	//end 20151016

private:
	OSThread m_thHandle;
	BOOL m_threadExit;
	InterruptableSleep m_sleep;
	int m_lastDay;
	int m_lastMonth;
	int m_lastDayOfOffset;

};

void InitGlobalTimerTaskMan();
GlobalTimerTaskMan* GetGlobalTimerTaskMan();
void DestroyGlobalTimerTaskMan();

#endif