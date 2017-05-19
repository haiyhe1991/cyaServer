#include "moduleInfo.h"
#include "TaskCfg.h"
#include "ServiceErrorCode.h"
#include "cyaMaxMin.h"
#include "ChapterCfg.h"

TaskCfg::TaskCfg(ChapterCfg* pChapterCfg)
: m_pChapterCfg(pChapterCfg)
{

}

TaskCfg::~TaskCfg()
{

}

BOOL TaskCfg::Load(const char* filename, const char* everydayTaskFile)
{
	if (!OnLoadTask(filename))
		return false;

	return OnLoadEverydayTask(everydayTaskFile);
}

BOOL TaskCfg::OnLoadEverydayTask(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("EverydayTasks") || !rootValue["EverydayTasks"].isArray())
		return false;

	SDailyTaskRewardParam item;
	int si = rootValue["EverydayTasks"].size();
	for (int i = 0; i < si; ++i)
	{
		BYTE lv = rootValue["EverydayTasks"][i]["RoleLv"].asUInt();
		m_dailyTaskReward.insert(std::make_pair(lv, item));
		SDailyTaskRewardParam& itemRef = m_dailyTaskReward[lv];

		itemRef.exp = rootValue["EverydayTasks"][i]["MissionExp"].asUInt();
		itemRef.gold = rootValue["EverydayTasks"][i]["MissionGold"].asUInt();
	}

	return true;
}

BOOL TaskCfg::OnLoadTask(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Missions") || !rootValue["Missions"].isArray())
		return false;

	int si = rootValue["Missions"].size();
	if (si <= 0)
		return false;

	STaskAttrCfg task;
	for (int i = 0; i < si; ++i)
	{
		UINT16 id = (UINT16)rootValue["Missions"][i]["ID"].asUInt();
		m_taskAttrCfg.insert(std::make_pair(id, task));
		STaskAttrCfg& taskRef = m_taskAttrCfg[id];

		taskRef.limit.limitLv = (BYTE)rootValue["Missions"][i]["LimitLv"].asUInt();
		taskRef.type = (BYTE)rootValue["Missions"][i]["Type"].asUInt();
		taskRef.targetType = (BYTE)rootValue["Missions"][i]["TargetType"].asUInt();
		taskRef.chapterId = (UINT16)rootValue["Missions"][i]["ChapterID"].asUInt();

		//����Ŀ��
		if (rootValue["Missions"][i].isMember("Target"))
		{
			UINT16 instId = rootValue["Missions"][i]["Target"]["InstID"].asUInt();
			UINT16 monsterId = rootValue["Missions"][i]["Target"]["MonsterID"].asUInt();
			UINT32 num = rootValue["Missions"][i]["Target"]["Num"].asUInt();
			BYTE propType = rootValue["Missions"][i]["Target"]["ItemType"].asUInt();
			UINT16 propId = rootValue["Missions"][i]["Target"]["ItemID"].asUInt();
			UINT32 timeSec = rootValue["Missions"][i]["Target"]["Time"].asUInt();
			UINT32 hp = rootValue["Missions"][i]["Target"]["HP"].asUInt();
			UINT16 NPC = rootValue["Missions"][i]["Target"]["NpcID"].asUInt();

			switch (taskRef.targetType)
			{
			case ESGS_TASK_TARGET_PASS_STAGE:			//ͨ��
				taskRef.targetVec.push_back(instId);
				break;
			case ESGS_TASK_TARGET_KILL_MONSTER:			//ɱ��
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(monsterId);
				taskRef.targetVec.push_back(num);
				break;
			case ESGS_TASK_TARGET_DESTROY_SCENE:		//�ƻ�������
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(propType);
				taskRef.targetVec.push_back(num);
				break;
			case ESGS_TASK_TARGET_FETCH_PROP:			//��ȡ����
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(propType);
				taskRef.targetVec.push_back(propId);
				taskRef.targetVec.push_back(num);
				break;
			case ESGS_TASK_TARGET_LIMIT_TIME:			//��ʱͨ��
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(timeSec);
				break;
			case ESGS_TASK_TARGET_ESCORT:				//����
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(NPC);
				taskRef.targetVec.push_back(hp);
				break;
			case ESGS_TASK_TARGET_FIND_NPC:				//�ҵ�NPC
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(NPC);
				break;
			case ESGS_TASK_TARGET_PASS_NO_HURT:			//����ͨ��
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(hp);
				break;
			case ESGS_TASK_TARGET_PASS_NO_BLOOD:		//��ʹ��Ѫƿͨ��
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(propId);
				break;
			case ESGS_TASK_TARGET_PASS_NO_DIE:			//����ͨ��
				taskRef.targetVec.push_back(instId);
				break;
			case ESGS_TASK_TARGET_BLOCK_TIMES:			//�񵲶��ٴ�
			case ESGS_TASK_TARGET_COMBO_NUM_REACH:		//�ﵽ�㹻���������
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(num);
				break;
			case ESGS_TASK_TARGET_KEEP_BLOOD:			//Ѫ�������ڶ�������/��ͨ�أ�25%/50%/75%)
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(hp);
				break;

			case ESGS_TASK_TARGET_CASH:					//�һ�
			case ESGS_TASK_TARGET_COMPOSE:				//�ϳ�
			case ESGS_TASK_TARGET_STRENGTHEN:			//ǿ��
			case ESGS_TASK_TARGET_RISE_STAR:			//����
				taskRef.targetVec.push_back(propType);
				taskRef.targetVec.push_back(propId);
				taskRef.targetVec.push_back(num);
				break;

			case ESGS_TASK_TARGET_ROLE_LV_REACH:		//��ɫ�ȼ��ﵽ
				taskRef.targetVec.push_back(num);
				break;
			case ESGS_TASK_TARGET_KILL_MONSTER_LIMIT_TIME:	//��ʱɱ��
				taskRef.targetVec.push_back(propId);
				taskRef.targetVec.push_back(monsterId);
				taskRef.targetVec.push_back(timeSec);
				taskRef.targetVec.push_back(num);
				break;
			case ESGS_TASK_TARGET_FETCH_PROP_LIMIT_TIME:	//��ʱ��ȡ���ߣ�������
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(propType);
				taskRef.targetVec.push_back(propId);
				taskRef.targetVec.push_back(timeSec);
				taskRef.targetVec.push_back(num);
				break;
			case ESGS_TASK_TARGET_DESTROY_SCENE_LIMIT_TIME:	//��ʱ�ƻ������������
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(propId);
				taskRef.targetVec.push_back(timeSec);
				taskRef.targetVec.push_back(num);
				break;
			case ESGS_TASK_TARGET_NO_USE_SPECIAL_SKILL:	//��ʹ�����⼼��ͨ��
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(propId);
				break;
			case ESGS_TASK_TARGET_KILL_ALL_MONSTER:		//ɱ�����й�
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(monsterId);
				break;
			case ESGS_TASK_TARGET_DESTROY_ALL_SCENE:		//�ƻ����г�����
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(propType);
			case ESGS_TASK_TARGET_PASS_STAGE_NUM:			//ͨ��������ͨ��������
			case ESGS_TASK_TARGET_PASS_RAND_STAGE_NUM:		//ͨ�������ͨ��������
				taskRef.targetVec.push_back(instId);
				taskRef.targetVec.push_back(num);
				break;
			case ESGS_TASK_TARGET_LOGIN_RECEIVE_VIT:		//��½��ȡ����
				taskRef.targetVec.push_back(propType);
				break;
			case ESGS_TASK_TARGET_OPEN_CHEST_BOX_NUM:		//�ֶ���������
				taskRef.targetVec.push_back(propType);
				taskRef.targetVec.push_back(propId);
				taskRef.targetVec.push_back(num);
				break;
			default:
				ASSERT(0);
				break;
			}
		}

		//�ճ�����
		if (taskRef.type == ESGS_SYS_DAILY_TASK)
		{
			std::map<BYTE/*����ȼ�*/, SDailyTasks>::iterator it = m_dailyTaskCfg.find(taskRef.limit.limitLv);
			if (it == m_dailyTaskCfg.end())
			{
				SDailyTasks dailyTasks;
				m_dailyTaskCfg.insert(std::make_pair(taskRef.limit.limitLv, dailyTasks));
			}
			SDailyTasks& dailyTaskRef = m_dailyTaskCfg[taskRef.limit.limitLv];
			dailyTaskRef.dailyTasks.push_back(id);
		}

		//��������
		if (taskRef.type == ESGS_SYS_MAIN_TASK)
		{
			UINT16 chapterId = (UINT16)rootValue["Missions"][i]["ChapterID"].asUInt();
			m_pChapterCfg->AddChapterMainTask(chapterId, id);
		}

		//����
		taskRef.reward.gold = rootValue["Missions"][i]["Gold"].asUInt();
		taskRef.reward.exp = rootValue["Missions"][i]["Exp"].asUInt();
		if (!rootValue["Missions"][i].isMember("RewardItem") || !rootValue["Missions"][i]["RewardItem"].isArray())
			continue;

		int items = rootValue["Missions"][i]["RewardItem"].size();
		for (int j = 0; j < items; ++j)
		{
			SDropPropCfgItem item;
			item.propId = (UINT16)rootValue["Missions"][i]["RewardItem"][j]["PropID"].asUInt();
			item.num = rootValue["Missions"][i]["RewardItem"][j]["Num"].asUInt();
			item.type = (BYTE)rootValue["Missions"][i]["RewardItem"][j]["Type"].asUInt();
			taskRef.reward.props.push_back(item);
		}
	}

	return true;
}

int TaskCfg::GetDailyTasks(BYTE roleLv, SDailyTasks& dailyTasks)
{
	std::map<BYTE/*����ȼ�*/, SDailyTasks>::iterator it = m_dailyTaskCfg.begin();
	for (; it != m_dailyTaskCfg.end(); ++it)
	{
		if (it->first > roleLv)
			break;

		std::vector<UINT16/*����id*/>& dailyTasksRef = it->second.dailyTasks;
		int si = (int)dailyTasksRef.size();
		for (int i = 0; i < si; ++i)
			dailyTasks.dailyTasks.push_back(dailyTasksRef[i]);
	}
	return MLS_OK;
}

int TaskCfg::GetTaskInfo(UINT16 taskId, STaskAttrCfg*& pTaskInfo)
{
	std::map<UINT16/*����id*/, STaskAttrCfg>::iterator it = m_taskAttrCfg.find(taskId);
	if (it == m_taskAttrCfg.end())
		return MLS_NOT_EXIST_TASK;

	pTaskInfo = &it->second;
	return MLS_OK;
}

int TaskCfg::GetTaskReard(UINT16 taskId, BYTE roleLv, STaskReward& taskReward)
{
	std::map<UINT16/*����id*/, STaskAttrCfg>::iterator it = m_taskAttrCfg.find(taskId);
	if (it == m_taskAttrCfg.end())
		return MLS_NOT_EXIST_TASK;

	if (it->second.type == ESGS_SYS_DAILY_TASK || it->second.type == ESGS_SYS_RAND_TASK)	//�ճ���������������
	{
		taskReward.props.assign(it->second.reward.props.begin(), it->second.reward.props.end());
		std::map<BYTE/*����ȼ�*/, SDailyTaskRewardParam>::iterator it2 = m_dailyTaskReward.find(roleLv);
		if (it2 == m_dailyTaskReward.end())
		{
			taskReward.exp = 0;
			taskReward.gold = 0;
		}
		else
		{
			UINT64 totalGold = it2->second.gold;
			UINT64 totalExp = it2->second.exp;
			taskReward.gold = totalGold * it->second.reward.gold / 100;
			taskReward.exp = totalExp * it->second.reward.exp / 100;
		}
	}
	else
	{
		taskReward.exp = it->second.reward.exp;
		taskReward.gold = it->second.reward.gold;
		taskReward.props.assign(it->second.reward.props.begin(), it->second.reward.props.end());
	}
	return MLS_OK;
}