#ifndef __PERSONAL_TIMER_TASK_H__
#define __PERSONAL_TIMER_TASK_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
个人定时任务管理
*/

#include <map>
#include "cyaLocker.h"
#include "cyaThread.h"

class GWSClient;
class PersonalTimerTaskMan
{
private:
	PersonalTimerTaskMan(const PersonalTimerTaskMan&);
	PersonalTimerTaskMan& operator = (const PersonalTimerTaskMan&);

	struct PersonalTimerTask
	{
		UINT16 taskId;	//定时任务编号
		UINT32 taskSec;	//任务定时时间
		LTMSEL startMSel;	//任务开始时间
		BYTE targetId;	//发送目标id(服务器编号)
		UINT32 userId;	//用户id

		PersonalTimerTask()
		{
			taskId = 0;
			taskSec = 0;
			startMSel = 0;
			targetId = 0;
			userId = 0;
		}
	};

public:
	PersonalTimerTaskMan();
	~PersonalTimerTaskMan();

	void Start();
	void Stop();

	//加入定时任务队列
	int JoinPersonalTimerTaskQ(UINT32 roleId, UINT16 taskId, BYTE targetId, UINT32 userSessionId, UINT32& newId);

private:
	static int THWorker(void* param);
	int OnWorker();
	void LoadPersonalTimerTask();

private:
	CXTLocker m_locker;
	std::map<UINT32, PersonalTimerTask> m_personalTimerTasks;

	OSThread m_timerThread;
	BOOL m_exit;
	InterruptableSleep m_sleep;
};

void InitPersonalTimerTaskMan();
PersonalTimerTaskMan* GetPersonalTimerTaskMan();
void DestroyPersonalTimerTaskMan();

#endif	//_PersonalTimerTaskMan_h__