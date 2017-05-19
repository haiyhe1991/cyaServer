#ifndef __DAILY_TASK_MAN_H__
#define __DAILY_TASK_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
日常任务管理
*/

#include "cyaTypes.h"

class DailyTaskMan
{
private:
	DailyTaskMan(const DailyTaskMan&);
	DailyTaskMan& operator = (const DailyTaskMan&);

public:
	DailyTaskMan();
	~DailyTaskMan();

public:
	BOOL IsFinishedDailyTask(UINT32 roleId, UINT16 taskId);
	void FinishedDailyTask(UINT32 roleId, UINT16 taskId);
	void ClearDailyTaskFinishRecord();
};

void InitDailyTaskMan();
DailyTaskMan* GetDailyTaskMan();
void DestroyDailyTaskMan();


#endif