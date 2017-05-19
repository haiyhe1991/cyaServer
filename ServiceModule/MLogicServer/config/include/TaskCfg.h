#ifndef __TASK_CFG_H__
#define __TASK_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��������
*/

#include <map>
#include <vector>
#include "CfgCom.h"
#include "ServiceMLogic.h"

class ChapterCfg;
class TaskCfg
{
public:
	TaskCfg(ChapterCfg* pChapterCfg);
	~TaskCfg();

#pragma pack(push, 1)
	struct STaskLimitCondition
	{
		BYTE limitLv;		//�������Ƶȼ�
		std::vector<UINT16> frontTaskVec;	//ǰ������id

		STaskLimitCondition()
		{
			limitLv = 0;
			frontTaskVec.clear();
		}
	};
	struct STaskReward
	{
		UINT32 exp;		//����
		UINT32 gold;	//���
		std::vector<SDropPropCfgItem> props;
		STaskReward()
		{
			exp = 0;
			gold = 0;
		}
	};
	struct SDailyTaskRewardParam
	{
		UINT32 exp;
		UINT32 gold;
		SDailyTaskRewardParam()
		{
			exp = 0;
			gold = 0;
		}
	};

	struct STaskAttrCfg
	{
		BYTE type;			//��������, 1.���� 2.�ճ� 3.֧��
		BYTE targetType;	//Ŀ������(ESGSTaskTargetType)
		UINT16 chapterId;	//�����½�id
		STaskLimitCondition limit;			//��������
		std::vector<UINT32> targetVec;		//����Ŀ��
		STaskReward reward;	//������

		STaskAttrCfg()
		{
			type = 0;
			targetType = 0;
			chapterId = 0;
			targetVec.clear();
		}
	};
	struct SDailyTasks
	{
		std::vector<UINT16> dailyTasks;	//�ճ�����id
		SDailyTasks()
		{
			dailyTasks.clear();
		}
	};
#pragma pack(pop)

public:
	//������������
	BOOL Load(const char* filename, const char* everydayTaskFile);
	//��ȡ��ɫ�ճ�����
	int GetDailyTasks(BYTE roleLv, SDailyTasks& dailyTasks);
	//��ȡ������Ϣ
	int GetTaskInfo(UINT16 taskId, STaskAttrCfg*& pTaskInfo);
	//��ȡ������
	int GetTaskReard(UINT16 taskId, BYTE roleLv, STaskReward& taskReward);

	//���������� add by hxw 20151212
	//ֻ�Բ����ã�������������
	//int TestTask();
	//end

private:
	BOOL OnLoadTask(const char* filename);
	BOOL OnLoadEverydayTask(const char* filename);

private:
	ChapterCfg* m_pChapterCfg;
	std::map<UINT16/*����id*/, STaskAttrCfg> m_taskAttrCfg;	//�������ñ�
	std::map<BYTE/*����ȼ�*/, SDailyTasks> m_dailyTaskCfg;	//�ճ�����
	std::map<BYTE/*����ȼ�*/, SDailyTaskRewardParam> m_dailyTaskReward;	//�ճ�����������
};

#endif