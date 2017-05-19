#ifndef __GLOBAL_TIMER_TASK_MAN_H__
#define __GLOBAL_TIMER_TASK_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
ȫ�ֶ�ʱ�������
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

	//���Ÿ���ռ�콱��
	void OnSendInstOccupyReward();
	//����ÿ���Ҷһ���¼
	void OnClearEverydayGoldExchage();
	//�����ճ�����
	void OnClearDailyTaskRecord();
	//�������̳齱
	void OnClearRouletteReward();
	//�����һ������¼
	void OnClearPlayersActiveInstRcd();
	/* zpy �����¿����� */
	void OnSendMonthCardReward();
	/* zpy ����ÿ�������һ���¼ */
	void OnClearEverydayVitExchage();

	//add by hxw 20151016
	//����ÿ�������ս�����Ĵ���,������¼ת�Ƶ�rankinstnum��
	//����LV���а�	
	//����instnum���а�	
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