#ifndef __TASK_FIN_HISTORY_MAN_H__
#define __TASK_FIN_HISTORY_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
���������ʷ��¼
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
	//��ѯ��ɫ��� ����
	int  QueryRoleFinTaskHistory(UINT32 roleId, UINT16 from, UINT16 num, SQueryFinishedTaskRes* pFinTasks);
	void InputRoleFinTask(INT32 roleId, UINT16 taskId, BYTE taskType, UINT16 chapterId);
	BOOL IsRoleFinishedMainTasks(INT32 roleId, const std::vector<UINT16>& tasksVec);

	//��ȡ��ǰ�����½�
	UINT16 GetCurrentOpenChapter(UINT32 roleId);

private:
	static void SyncDBFinTskTimer(void* param, TTimerID id);
	void OnSyncDBFinTsk();
	void InsertDBFinTask(INT32 roleId, UINT16 taskId, BYTE taskType, UINT16 chapterId, const char* pszDate);
	BOOL IsFinishedTask(INT32 roleId, UINT16 taskId);

private:
	struct SFinTaskInfo
	{
		UINT16 taskId;	//�������id
		BYTE   taskType;	//��������
		UINT16 chapterId;	//�����½�id
		LTMSEL fintts;	//�������ʱ��
	};
	CXTLocker m_finTaskMapLocker;
	std::map<UINT32/*��ɫid*/, std::list<SFinTaskInfo> > m_finTaskMap;
	CyaCoreTimer m_refreshTaskTimer;
};

void InitTaskFinHistoryMan();
TaskFinHistoryMan* GetTaskFinHistoryMan();
void DestroyTaskFinHistoryMan();

#endif	//_TaskFinHistoryMan_h__