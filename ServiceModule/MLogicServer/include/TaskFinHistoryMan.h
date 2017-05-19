#ifndef __TASK_FIN_HISTORY_MAN_H__
#define __TASK_FIN_HISTORY_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
任务完成历史记录
*/

#include <map>
#include <list>
#include "cyaCoreTimer.h"
#include "ServiceProtocol.h"
#include "RoleInfos.h"

class TaskFinHistoryMan
{
	TaskFinHistoryMan(const TaskFinHistoryMan&);
	TaskFinHistoryMan& operator = (const TaskFinHistoryMan&);

public:
	TaskFinHistoryMan();
	~TaskFinHistoryMan();

public:
	//查询角色完成 人物
	int  QueryRoleFinTaskHistory(UINT32 roleId, UINT16 from, UINT16 num, SQueryFinishedTaskRes* pFinTasks);
	void InputRoleFinTask(INT32 roleId, UINT16 taskId, BYTE taskType, UINT16 chapterId);
	BOOL IsRoleFinishedMainTasks(INT32 roleId, const std::vector<UINT16>& tasksVec);

	//获取当前开放章节
	UINT16 GetCurrentOpenChapter(UINT32 roleId);

private:
	static void SyncDBFinTskTimer(void* param, TTimerID id);
	void OnSyncDBFinTsk();
	void InsertDBFinTask(INT32 roleId, UINT16 taskId, BYTE taskType, UINT16 chapterId, const char* pszDate);
	BOOL IsFinishedTask(INT32 roleId, UINT16 taskId);

private:
	struct SFinTaskInfo
	{
		UINT16 taskId;	//完成任务id
		BYTE   taskType;	//任务类型
		UINT16 chapterId;	//所在章节id
		LTMSEL fintts;	//完成任务时间
	};
	CXTLocker m_finTaskMapLocker;
	std::map<UINT32/*角色id*/, std::list<SFinTaskInfo> > m_finTaskMap;
	CyaCoreTimer m_refreshTaskTimer;
};

void InitTaskFinHistoryMan();
TaskFinHistoryMan* GetTaskFinHistoryMan();
void DestroyTaskFinHistoryMan();

#endif	//_TaskFinHistoryMan_h__