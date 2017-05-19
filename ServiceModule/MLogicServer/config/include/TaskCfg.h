#ifndef __TASK_CFG_H__
#define __TASK_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
任务配置
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
		BYTE limitLv;		//任务限制等级
		std::vector<UINT16> frontTaskVec;	//前置任务id

		STaskLimitCondition()
		{
			limitLv = 0;
			frontTaskVec.clear();
		}
	};
	struct STaskReward
	{
		UINT32 exp;		//经验
		UINT32 gold;	//金币
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
		BYTE type;			//任务类型, 1.主线 2.日常 3.支线
		BYTE targetType;	//目标类型(ESGSTaskTargetType)
		UINT16 chapterId;	//所在章节id
		STaskLimitCondition limit;			//限制条件
		std::vector<UINT32> targetVec;		//任务目标
		STaskReward reward;	//任务奖励

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
		std::vector<UINT16> dailyTasks;	//日常任务id
		SDailyTasks()
		{
			dailyTasks.clear();
		}
	};
#pragma pack(pop)

public:
	//载入任务配置
	BOOL Load(const char* filename, const char* everydayTaskFile);
	//获取角色日常任务
	int GetDailyTasks(BYTE roleLv, SDailyTasks& dailyTasks);
	//获取任务信息
	int GetTaskInfo(UINT16 taskId, STaskAttrCfg*& pTaskInfo);
	//获取任务奖励
	int GetTaskReard(UINT16 taskId, BYTE roleLv, STaskReward& taskReward);

	//测试任务链 add by hxw 20151212
	//只对测试用，服务器不调用
	//int TestTask();
	//end

private:
	BOOL OnLoadTask(const char* filename);
	BOOL OnLoadEverydayTask(const char* filename);

private:
	ChapterCfg* m_pChapterCfg;
	std::map<UINT16/*任务id*/, STaskAttrCfg> m_taskAttrCfg;	//任务配置表
	std::map<BYTE/*人物等级*/, SDailyTasks> m_dailyTaskCfg;	//日常任务
	std::map<BYTE/*人物等级*/, SDailyTaskRewardParam> m_dailyTaskReward;	//日常任务奖励参数
};

#endif