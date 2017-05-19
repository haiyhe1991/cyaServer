#include "AchievementMan.h"

#include "DBSClient.h"
#include "RolesInfoMan.h"
#include "ServiceErrorCode.h"
#include "ConfigFileMan.h"

const UINT16 MAX_BUFFER_SIZE = 1536;

static AchievementMan* sg_pAchievementMan = NULL;
void InitAchievementMan()
{
	ASSERT(sg_pAchievementMan == NULL);
	sg_pAchievementMan = new AchievementMan();
	ASSERT(sg_pAchievementMan != NULL);
}

AchievementMan* GetAchievementMan()
{
	return sg_pAchievementMan;
}

void DestroyAchievementMan()
{
	AchievementMan* pAchievementMan = sg_pAchievementMan;
	sg_pAchievementMan = NULL;
	if (pAchievementMan != NULL)
		delete pAchievementMan;
}

AchievementMan::AchievementMan()
{
	DWORD interval = GetConfigFileMan()->GetRefreshDBInterval();
	m_refreshDBTimer.Start(RefreshRoleTimer, this, interval * 1000);
}

AchievementMan::~AchievementMan()
{
	if (m_refreshDBTimer.IsStarted())
		m_refreshDBTimer.Stop();

	OnRefreshRoleInfo();
}

// 创建缓存
bool AchievementMan::CreateCache(UINT32 roleId, int &error_code)
{
	int ret = 0;
	DBS_RESULT result = NULL;
	SAchievementRecord record;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };

	// 查询成就统计
	{
		sprintf(szSQL, "SELECT common,cream,boss,boxsum,continuous,speed_evaluation,hurt_evaluation,star_evaluation FROM achievement_total WHERE actorid=%u", roleId);
		ret = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
		if (ret == DBS_OK)
		{
			DBS_ROW row = DBS_FetchRow(result);
			if (row != NULL)
			{
				record.kill_common = (UINT32)_atoi64(row[0]);
				record.kill_cream = (UINT32)_atoi64(row[1]);
				record.kill_boss = (UINT32)_atoi64(row[2]);
				record.pickup_box_sum = (UINT32)_atoi64(row[3]);
				record.continuous = (UINT32)_atoi64(row[4]);
				record.speed_evaluation = (UINT32)_atoi64(row[5]);
				record.hurt_evaluation = (UINT32)_atoi64(row[6]);
				record.star_evaluation = (UINT32)_atoi64(row[7]);
			}
			else
			{
				sprintf(szSQL, "INSERT INTO achievement_total(actorid) VALUES(%u)", roleId);
				GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
			}
			DBS_FreeResult(result);
		}
		else
		{
			error_code = ret;
			return false;
		}
	}

	// 查询通关次数
	{
		sprintf(szSQL, "SELECT SUM(complatednum) FROM instancehistory WHERE actorid=%u", roleId);
		ret = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
		if (ret == DBS_OK)
		{
			DBS_ROW row = DBS_FetchRow(result);
			if (row != NULL)
			{
				record.complatednum = (UINT32)_atoi64(row[0]);
			}
			DBS_FreeResult(result);
		}
		else
		{
			error_code = ret;
			return false;
		}
	}

	{
		CXTAutoLock lock(m_total_locker);
		if (m_achievement_total.find(roleId) == m_achievement_total.end())
		{
			m_achievement_total.insert(std::make_pair(roleId, record));
		}
	}

	// 查询成就信息
	{
		CXTAutoLock lock(m_record_locker);
		if (m_achievement_record.find(roleId) == m_achievement_record.end())
		{
			achievement_map achievement;
			std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
			ASSERT(itr == m_achievement_record.end());
			if (!ReadAchievementRecordFromDB(roleId, achievement))
			{
				error_code = DBS_ERROR;
				return false;
			}
			m_achievement_record.insert(std::make_pair(roleId, achievement));
		}
	}

	return true;
}

// 清理缓存
bool AchievementMan::ClearCache(UINT32 roleId)
{
	bool write = false;
	{
		CXTAutoLock lock(m_update_locker);
		std::set<UINT32>::iterator itr = m_updateSet.begin();
		if (itr != m_updateSet.end())
		{
			write = true;
			m_updateSet.erase(itr);
		}
	}

	{
		CXTAutoLock lock(m_total_locker);
		std::map<UINT32, SAchievementRecord>::iterator itr = m_achievement_total.find(roleId);
		if (itr != m_achievement_total.end())
		{
			m_achievement_total.erase(itr);
		}
	}

	{
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr != m_achievement_record.end())
		{
			if (write) WriteAchievementRecordToDB(roleId, itr->second);
			m_achievement_record.erase(itr);
		}
	}

	return true;
}

// 写入成就记录到数据库
bool AchievementMan::WriteAchievementRecordToDB(UINT32 roleId, achievement_map &record)
{
	if (!record.empty())
	{
		achievement_array record_array(record.size());
		{
			size_t index = 0;
			for (achievement_map::const_iterator itr = record.begin(); itr != record.end(); ++itr)
			{
				record_array[index].id = itr->first;
				record_array[index].receive = itr->second;
				++index;
			}
		}

		UINT32 affectRows = 0;
		char szSQL[MAX_SQL_BUF_LEN] = { 0 };
		strcpy(szSQL, "update achievement set userdata='");

		int n = (int)strlen(szSQL);
		DBS_EscapeString(szSQL + n, (const char*)record_array.data(), record_array.size() * sizeof(SDBAchievementItem));

		char szId[64] = { 0 };
		sprintf(szId, "' where actorid=%u", roleId);
		strcat(szSQL, szId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
		if (affectRows == 0)
		{
			memset(szSQL, 0, sizeof(szSQL));
			sprintf(szSQL, "INSERT INTO achievement VALUES(%u,'", roleId);
			n = (int)strlen(szSQL);
			DBS_EscapeString(szSQL + n, (const char*)record_array.data(), record_array.size() * sizeof(SDBAchievementItem));

			strcat(szSQL, "')");
			GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
		}
	}
	return true;
}

// 从数据库中读取成就记录
bool AchievementMan::ReadAchievementRecordFromDB(UINT32 roleId, achievement_map &record)
{
	record.clear();

	DBS_RESULT result = NULL;
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT userdata FROM achievement WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
	{
		return false;
	}

	DBS_ROWLENGTH row_bytes = 0;
	if (DBS_ROW row = DBS_FetchRow(result, &row_bytes))
	{
		const unsigned int modulo = row_bytes[0] % sizeof(SDBAchievementItem);
		if (modulo == 0)
		{
			unsigned int size = row_bytes[0] / sizeof(SDBAchievementItem);
			SDBAchievementItem *pItem = (SDBAchievementItem *)row[0];
			for (int i = 0; i < size; ++i)
			{
				record.insert(std::make_pair(pItem[i].id, pItem[i].receive));
			}
		}
	}
	DBS_FreeResult(result);

	return true;
}

// 激活成就
bool AchievementMan::ActivateAchievement(UINT32 roleId, AchievementCfg::TargetType target_type, UINT32 point, ComparisonType comparison, achievement_map &achievement)
{
	AchievementCfg::SAchievementCondition condition;

	const std::vector<UINT32> *ids = NULL;
	if (!GetGameCfgFileMan()->GetAchievementFromTargetType(target_type, ids))
	{
		return false;
	}

	for (std::vector<UINT32>::const_iterator itr = (*ids).begin(); itr != (*ids).end(); ++itr)
	{
		UINT32 achievement_id = *itr;
		if (!GetGameCfgFileMan()->GetAchievementCondition(achievement_id, condition))
		{
			continue;
		}

		bool activate = false;
		if (comparison == EQUAL)
		{
			activate = point == condition.condition;
		}
		else if (comparison == EQUAL_OR_GREATER)
		{
			activate = point >= condition.condition;
		}

		if (activate)
		{
			if (achievement.find(achievement_id) == achievement.end())
			{
				if (condition.premise_id == 0 || achievement.find(condition.premise_id) != achievement.end())
				{
					achievement.insert(std::make_pair(achievement_id, false));
					return true;
				}
			}
		}
	}
	return false;
}

// 战力更新(不可在此函数查询角色相关信息)
int AchievementMan::OnUpdateCombatPower(UINT32 roleId, UINT32 cp)
{
	bool update = false;
	{
		// 获取玩家成就信息
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr == m_achievement_record.end()) return MLS_ROLE_NOT_EXIST;
		achievement_map &achievement = itr->second;

		// 激活身强力壮成就
		update = ActivateAchievement(roleId, AchievementCfg::SQLZ, cp, EQUAL_OR_GREATER, achievement);
	}

	if (update)
	{
		CXTAutoLock lock(m_update_locker);
		m_updateSet.insert(roleId);
	}

	return MLS_OK;
}

// 获取宝石(不可在此函数查询角色相关信息)
int AchievementMan::OnGotGem(UINT32 roleId, BYTE gem_level)
{
	bool update = false;
	{
		// 获取玩家成就信息
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr == m_achievement_record.end()) return MLS_ROLE_NOT_EXIST;
		achievement_map &achievement = itr->second;

		// 激活闪闪耀耀成就
		update = ActivateAchievement(roleId, AchievementCfg::SSYY, gem_level, EQUAL, achievement);
	}

	if (update)
	{
		CXTAutoLock lock(m_update_locker);
		m_updateSet.insert(roleId);
	}

	return MLS_OK;
}

// 角色升级
int AchievementMan::OnRoleUpgrade(UINT32 userId, UINT32 roleId)
{
	// 获取玩家信息
	SRoleInfos role_info;
	int retCode = GetRolesInfoMan()->GetRoleInfo(userId, roleId, &role_info);
	if (MLS_OK != retCode)
	{
		return retCode;
	}

	bool update = false;
	{
		// 获取玩家成就信息
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr == m_achievement_record.end()) return MLS_ROLE_NOT_EXIST;
		achievement_map &achievement = itr->second;

		// 激活茁壮成长成就
		update = ActivateAchievement(roleId, AchievementCfg::ZZCZ, role_info.level, EQUAL_OR_GREATER, achievement);
	}

	if (update)
	{
		CXTAutoLock lock(m_update_locker);
		m_updateSet.insert(roleId);
	}

	return MLS_OK;
}

// 镶嵌宝石
int AchievementMan::OnInlaidGem(UINT32 userId, UINT32 roleId)
{
	// 获取镶嵌宝石等级之和
	UINT32 inlaid_gem_level_sum = 0;
	int retCode = GetRolesInfoMan()->GetInlaidGemLevelSum(userId, roleId, inlaid_gem_level_sum);
	if (retCode != MLS_OK) return retCode;

	bool update = false;
	{
		// 获取玩家成就信息
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr == m_achievement_record.end()) return MLS_ROLE_NOT_EXIST;
		achievement_map &achievement = itr->second;

		// 激活闪耀夺目成就
		update = ActivateAchievement(roleId, AchievementCfg::SYDM, inlaid_gem_level_sum, EQUAL_OR_GREATER, achievement);
	}

	if (update)
	{
		CXTAutoLock lock(m_update_locker);
		m_updateSet.insert(roleId);
	}

	return MLS_OK;
}

// 装备位升星
int AchievementMan::OnRiseStar(UINT32 userId, UINT32 roleId)
{
	// 获取装备升星等级之和
	UINT32 equip_star_level_sum = 0;
	int retCode = GetRolesInfoMan()->GetEquipStarLevelSum(userId, roleId, equip_star_level_sum);
	if (retCode != MLS_OK) return retCode;

	bool update = false;
	{
		// 获取玩家成就信息
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr == m_achievement_record.end()) return MLS_ROLE_NOT_EXIST;
		achievement_map &achievement = itr->second;

		// 激活星星装备成就
		update = ActivateAchievement(roleId, AchievementCfg::XXZB, equip_star_level_sum, EQUAL_OR_GREATER, achievement);
	}

	if (update)
	{
		CXTAutoLock lock(m_update_locker);
		m_updateSet.insert(roleId);
	}

	return MLS_OK;
}

// 技能升级
int AchievementMan::OnSkillUpgrade(UINT32 userId, UINT32 roleId)
{
	// 获取技能等级等级之和
	UINT32 skill_level_sum = 0;
	int retCode = GetRolesInfoMan()->GetSkillLevelSum(userId, roleId, skill_level_sum);
	if (retCode != MLS_OK) return retCode;

	bool update = false;
	{
		// 获取玩家成就信息
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr == m_achievement_record.end()) return MLS_ROLE_NOT_EXIST;
		achievement_map &achievement = itr->second;

		// 激活武道至尊成就
		update = ActivateAchievement(roleId, AchievementCfg::WDZZ, skill_level_sum, EQUAL_OR_GREATER, achievement);
	}

	if (update)
	{
		CXTAutoLock lock(m_update_locker);
		m_updateSet.insert(roleId);
	}

	return MLS_OK;
}

// 拾取宝箱
int AchievementMan::OnPickupInstBoxDrop(UINT32 userId, UINT32 roleId)
{
	UINT32 pickup_box_sum = 0;

	// 增加宝箱数量
	{
		CXTAutoLock lock(m_total_locker);
		std::map<UINT32, SAchievementRecord>::iterator itr = m_achievement_total.find(roleId);
		if (itr == m_achievement_total.end())
		{
			return MLS_ROLE_NOT_EXIST;
		}
		pickup_box_sum = ++itr->second.pickup_box_sum;
	}

	bool update = false;
	{
		// 获取玩家成就信息
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr == m_achievement_record.end()) return MLS_ROLE_NOT_EXIST;
		achievement_map &achievement = itr->second;

		// 激活夺宝奇兵成就
		update = ActivateAchievement(roleId, AchievementCfg::DBQB, pickup_box_sum, EQUAL_OR_GREATER, achievement);
	}

	if (update)
	{
		CXTAutoLock lock(m_update_locker);
		m_updateSet.insert(roleId);
	}

	return MLS_OK;
}

// 击杀怪物
int AchievementMan::OnKillMonsterRecord(UINT32 userId, UINT32 roleId, UINT16 monsterId)
{
	UINT32 kill_sum = 0;
	const BYTE type = GetGameCfgFileMan()->GetMonsterType(monsterId);

	// 增加杀怪数量
	{
		if ((type != ESGS_MONSTER_COMMON) &&
			(type != ESGS_MONSTER_CREAM) &&
			(type != ESGS_MONSTER_SMALL_BOSS) &&
			(type != ESGS_MONSTER_BIG_BOSS))
		{
			return MLS_OK;
		}

		CXTAutoLock lock(m_total_locker);
		std::map<UINT32, SAchievementRecord>::iterator itr = m_achievement_total.find(roleId);
		if (itr == m_achievement_total.end())
		{
			return MLS_ROLE_NOT_EXIST;
		}

		if (type == ESGS_MONSTER_COMMON)
		{
			kill_sum = ++itr->second.kill_common;
		}
		else if (type == ESGS_MONSTER_CREAM)
		{
			kill_sum = ++itr->second.kill_cream;
		}
		else if (type == ESGS_MONSTER_SMALL_BOSS || type == ESGS_MONSTER_BIG_BOSS)
		{
			kill_sum = ++itr->second.kill_boss;
		}
	}

	bool update = false;
	{
		// 获取玩家成就信息
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr == m_achievement_record.end()) return MLS_ROLE_NOT_EXIST;
		achievement_map &achievement = itr->second;

		if (type == ESGS_MONSTER_COMMON)
		{
			// 激活一骑当千成就
			update = ActivateAchievement(roleId, AchievementCfg::YQDQ, kill_sum, EQUAL_OR_GREATER, achievement);
		}
		else if (type == ESGS_MONSTER_CREAM)
		{
			// 激活势不可挡成就
			update = ActivateAchievement(roleId, AchievementCfg::SBKD, kill_sum, EQUAL_OR_GREATER, achievement);
		}
		else if (type == ESGS_MONSTER_SMALL_BOSS || type == ESGS_MONSTER_BIG_BOSS)
		{
			// 激活勇猛无敌成就
			update = ActivateAchievement(roleId, AchievementCfg::YMWD, kill_sum, EQUAL_OR_GREATER, achievement);
		}
	}

	if (update)
	{
		CXTAutoLock lock(m_update_locker);
		m_updateSet.insert(roleId);
	}

	return MLS_OK;
}

// 完成副本
int AchievementMan::OnFinishInst(UINT32 userId, UINT32 roleId, UINT32 continuous, bool speed_evaluation, bool hurt_evaluation, bool star_evaluation)
{
	UINT32 n_complatednum = 0;
	UINT32 n_speed_evaluation = 0;
	UINT32 n_hurt_evaluation = 0;
	UINT32 n_star_evaluation = 0;

	// 计算数据
	{
		CXTAutoLock lock(m_total_locker);
		std::map<UINT32, SAchievementRecord>::iterator itr = m_achievement_total.find(roleId);
		if (itr == m_achievement_total.end())
		{
			return MLS_ROLE_NOT_EXIST;
		}

		if (continuous > itr->second.continuous)
		{
			itr->second.continuous = continuous;
		}

		n_complatednum = ++itr->second.complatednum;

		if (speed_evaluation)
		{
			n_speed_evaluation = ++itr->second.speed_evaluation;
		}

		if (hurt_evaluation)
		{
			n_hurt_evaluation = ++itr->second.hurt_evaluation;
		}

		if (star_evaluation)
		{
			n_star_evaluation = ++itr->second.star_evaluation;
		}
	}

	int update_count = 0;
	{
		// 获取玩家成就信息
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr == m_achievement_record.end()) return MLS_ROLE_NOT_EXIST;
		achievement_map &achievement = itr->second;

		// 激活百战沙场成就
		if (ActivateAchievement(roleId, AchievementCfg::BZSC, n_complatednum, EQUAL_OR_GREATER, achievement))
		{
			++update_count;
		}

		// 激活连击达人成就
		if (ActivateAchievement(roleId, AchievementCfg::LJDR, continuous, EQUAL_OR_GREATER, achievement))
		{
			++update_count;
		}

		if (speed_evaluation)
		{
			// 激活急速先锋成就
			if (ActivateAchievement(roleId, AchievementCfg::JSXF, n_speed_evaluation, EQUAL_OR_GREATER, achievement))
			{
				++update_count;
			}
		}

		if (hurt_evaluation)
		{
			// 激活优雅武者成就
			if (ActivateAchievement(roleId, AchievementCfg::YYWZ, n_hurt_evaluation, EQUAL_OR_GREATER, achievement))
			{
				++update_count;
			}
		}

		if (star_evaluation)
		{
			// 激活三星强者成就
			if (ActivateAchievement(roleId, AchievementCfg::SXQZ, n_star_evaluation, EQUAL_OR_GREATER, achievement))
			{
				++update_count;
			}
		}
	}

	if (update_count > 0)
	{
		CXTAutoLock lock(m_update_locker);
		m_updateSet.insert(roleId);
	}

	return MLS_OK;
}

// 获取成就记录
int AchievementMan::GetAchievementRecord(UINT32 userId, UINT32 roleId, SAchievementRecord &record)
{
	CXTAutoLock lock(m_total_locker);
	std::map<UINT32, SAchievementRecord>::iterator itr = m_achievement_total.find(roleId);
	if (itr == m_achievement_total.end())
	{
		return MLS_ROLE_NOT_EXIST;
	}
	memcpy(&record, &itr->second, sizeof(SAchievementRecord));
	return MLS_OK;
}

// 查询成就点数
int AchievementMan::QueryAchievementPoint(UINT32 roleId, SQueryAchievementRes *pAchievementInfo)
{
	DBS_RESULT result = NULL;
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT point FROM achievement WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK) return retCode;

	pAchievementInfo->point = 0;
	if (DBS_ROW row = DBS_FetchRow(result))
	{
		pAchievementInfo->point += (UINT32)_atoi64(row[0]);
	}
	DBS_FreeResult(result);
	return MLS_OK;
}

// 查询成长之路
int AchievementMan::QueryRoadToGrowth(UINT32 userId, UINT32 roleId, UINT16 start_pos, SQueryAchievementRes *pAchievementInfo)
{
	SRoleInfos role_info;
	UINT32 achievement_num = 0;
	UINT32 put_achievement_num = 0;
	UINT32 inlaid_gem_level_sum = 0, equip_star_level_sum = 0;

	int retCode = GetRolesInfoMan()->GetRoleInfo(userId, roleId, &role_info);
	if (MLS_OK != retCode) return retCode;
	retCode = GetRolesInfoMan()->GetInlaidGemLevelSum(userId, roleId, inlaid_gem_level_sum);
	if (retCode != MLS_OK) return retCode;
	retCode = GetRolesInfoMan()->GetEquipStarLevelSum(userId, roleId, equip_star_level_sum);
	if (retCode != MLS_OK) return retCode;

	// 获取玩家成就信息
	achievement_map achievement;
	{
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr == m_achievement_record.end()) return MLS_ROLE_NOT_EXIST;
		achievement = itr->second;
	}


	const std::vector<UINT32> *ids = NULL;
	if (!GetGameCfgFileMan()->GetAchievementFromType(AchievementCfg::RoadToGrowth, ids))
	{
		return MLS_INVALID_ACHIEVEMENT_TYPE;
	}

	for (size_t i = 0; i < (*ids).size(); ++i)
	{
		// 超出buffer限制
		if (sizeof(SAchievementInfo)* (achievement_num + 1) > MAX_BUFFER_SIZE)
		{
			break;
		}

		achievement_map::const_iterator result = achievement.find((*ids)[i]);
		if (result != achievement.end())
		{
			if (++achievement_num >= start_pos)
			{
				SAchievementInfo &item = pAchievementInfo->data[put_achievement_num++];
				item.point = 0;
				item.id = result->first;
				item.status = result->second ? 3 : 2;		//1表示正在进行，2表示已达成，3表示已领取
			}
		}
		else
		{
			// 获取正在进行的成就
			AchievementCfg::SAchievementCondition condition;
			if (GetGameCfgFileMan()->GetAchievementCondition((*ids)[i], condition))
			{
				if (condition.premise_id == 0 || achievement.find(condition.premise_id) != achievement.end())
				{
					AchievementCfg::TargetType type;
					if (GetGameCfgFileMan()->GetAchievementTargetType((*ids)[i], type))
					{
						if (++achievement_num >= start_pos)
						{
							SAchievementInfo &item = pAchievementInfo->data[put_achievement_num++];
							item.id = (*ids)[i];
							item.status = 1;

							if (type == AchievementCfg::ZZCZ)
							{
								item.point = role_info.level;
							}
							else if (type == AchievementCfg::SQLZ)
							{
								item.point = role_info.cp;
							}
							else if (type == AchievementCfg::SSYY)
							{
								item.point = 0;
							}
							else if (type == AchievementCfg::SYDM)
							{
								item.point = inlaid_gem_level_sum;
							}
							else if (type == AchievementCfg::XXZB)
							{
								item.point = equip_star_level_sum;
							}
						}
					}
				}
			}
		}
	}
	pAchievementInfo->num = put_achievement_num;
	pAchievementInfo->done = start_pos + put_achievement_num == achievement_num;

	return MLS_OK;
}

// 查询冒险历程
int AchievementMan::QueryAdventureCourse(UINT32 userId, UINT32 roleId, UINT16 start_pos, SQueryAchievementRes *pAchievementInfo)
{
	SAchievementRecord record;
	UINT32 achievement_num = 0;
	UINT32 put_achievement_num = 0;
	{
		CXTAutoLock lock(m_total_locker);
		std::map<UINT32, SAchievementRecord>::iterator itr = m_achievement_total.find(roleId);
		if (itr == m_achievement_total.end())
		{
			return MLS_ROLE_NOT_EXIST;
		}
		record = itr->second;
	}

	// 获取玩家成就信息
	achievement_map achievement;
	{
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr == m_achievement_record.end()) return MLS_ROLE_NOT_EXIST;
		achievement = itr->second;
	}

	const std::vector<UINT32> *ids = NULL;
	if (!GetGameCfgFileMan()->GetAchievementFromType(AchievementCfg::AdventureCourse, ids))
	{
		return MLS_INVALID_ACHIEVEMENT_TYPE;
	}

	for (size_t i = 0; i < (*ids).size(); ++i)
	{
		// 超出buffer限制
		if (sizeof(SAchievementInfo)* (achievement_num + 1) > MAX_BUFFER_SIZE)
		{
			break;
		}

		achievement_map::const_iterator result = achievement.find((*ids)[i]);
		if (result != achievement.end())
		{
			// 获取已达成的成就
			if (++achievement_num >= start_pos)
			{
				SAchievementInfo &item = pAchievementInfo->data[put_achievement_num++];
				item.point = 0;
				item.id = result->first;
				item.status = result->second ? 3 : 2;		//1表示正在进行，2表示已达成，3表示已领取
			}
		}
		else
		{
			// 获取正在进行的成就
			AchievementCfg::SAchievementCondition condition;
			if (GetGameCfgFileMan()->GetAchievementCondition((*ids)[i], condition))
			{
				if (condition.premise_id == 0 || achievement.find(condition.premise_id) != achievement.end())
				{
					AchievementCfg::TargetType type;
					if (GetGameCfgFileMan()->GetAchievementTargetType((*ids)[i], type))
					{
						if (++achievement_num >= start_pos)
						{
							SAchievementInfo &item = pAchievementInfo->data[put_achievement_num++];
							item.id = (*ids)[i];
							item.status = 1;

							if (type == AchievementCfg::YQDQ)
							{
								item.point = record.kill_common;
							}
							else if (type == AchievementCfg::SBKD)
							{
								item.point = record.kill_cream;
							}
							else if (type == AchievementCfg::YMWD)
							{
								item.point = record.kill_boss;
							}
							else if (type == AchievementCfg::BZSC)
							{
								item.point = record.complatednum;
							}
							else if (type == AchievementCfg::DBQB)
							{
								item.point = record.pickup_box_sum;
							}
						}
					}
				}
			}
		}
	}
	pAchievementInfo->num = put_achievement_num;
	pAchievementInfo->done = start_pos + put_achievement_num == achievement_num;

	return MLS_OK;
}

// 查询完美技巧
int AchievementMan::QueryPerfectSkill(UINT32 userId, UINT32 roleId, UINT16 start_pos, SQueryAchievementRes *pAchievementInfo)
{
	SAchievementRecord record;
	UINT32 skill_level_sum = 0;
	UINT32 achievement_num = 0;
	UINT32 put_achievement_num = 0;
	{
		CXTAutoLock lock(m_total_locker);
		std::map<UINT32, SAchievementRecord>::iterator itr = m_achievement_total.find(roleId);
		if (itr == m_achievement_total.end())
		{
			return MLS_ROLE_NOT_EXIST;
		}
		record = itr->second;
	}

	int retCode = GetRolesInfoMan()->GetSkillLevelSum(userId, roleId, skill_level_sum);
	if (retCode != MLS_OK)
	{
		return retCode;
	}

	// 获取玩家成就信息
	achievement_map achievement;
	{
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr == m_achievement_record.end()) return MLS_ROLE_NOT_EXIST;
		achievement = itr->second;
	}

	const std::vector<UINT32> *ids = NULL;
	if (!GetGameCfgFileMan()->GetAchievementFromType(AchievementCfg::PerfectSkill, ids))
	{
		return MLS_INVALID_ACHIEVEMENT_TYPE;
	}

	for (size_t i = 0; i < (*ids).size(); ++i)
	{
		// 超出buffer限制
		if (sizeof(SAchievementInfo)* (achievement_num + 1) > MAX_BUFFER_SIZE)
		{
			break;
		}

		achievement_map::const_iterator result = achievement.find((*ids)[i]);
		if (result != achievement.end())
		{
			// 获取已达成的成就
			if (++achievement_num >= start_pos)
			{
				SAchievementInfo &item = pAchievementInfo->data[put_achievement_num++];
				item.point = 0;
				item.id = result->first;
				item.status = result->second ? 3 : 2;		//1表示正在进行，2表示已达成，3表示已领取
			}
		}
		else
		{
			// 获取正在进行的成就
			AchievementCfg::SAchievementCondition condition;
			if (GetGameCfgFileMan()->GetAchievementCondition((*ids)[i], condition))
			{
				if (condition.premise_id == 0 || achievement.find(condition.premise_id) != achievement.end())
				{
					AchievementCfg::TargetType type;
					if (GetGameCfgFileMan()->GetAchievementTargetType((*ids)[i], type))
					{
						if (++achievement_num >= start_pos)
						{
							SAchievementInfo &item = pAchievementInfo->data[put_achievement_num++];
							item.id = (*ids)[i];
							item.status = 1;

							if (type == AchievementCfg::LJDR)
							{
								item.point = record.continuous;
							}
							else if (type == AchievementCfg::JSXF)
							{
								item.point = record.speed_evaluation;
							}
							else if (type == AchievementCfg::YYWZ)
							{
								item.point = record.hurt_evaluation;
							}
							else if (type == AchievementCfg::SXQZ)
							{
								item.point = record.star_evaluation;
							}
							else if (type == AchievementCfg::WDZZ)
							{
								item.point = skill_level_sum;
							}
						}
					}
				}
			}
		}
	}
	pAchievementInfo->num = put_achievement_num;
	pAchievementInfo->done = start_pos + put_achievement_num == achievement_num;

	return MLS_OK;
}

// 领取成就奖励
int AchievementMan::ReceiveAchievementReward(UINT32 userId, UINT32 roleId, UINT32 achievement_id, SReceiveRewardRes *pAchievementReward, SReceiveRewardRefresh* pRefresh)
{
	// 验证成就是否达成
	{
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr == m_achievement_record.end()) return MLS_ROLE_NOT_EXIST;

		achievement_map &achievement = itr->second;
		achievement_map::iterator result = achievement.find(achievement_id);
		if (result == achievement.end()) return MLS_ACHIEVEMENT_NOT_REACHED;
		if (result->second) return MLS_ACHIEVEMENT_HAVE_RECEIVED;
	}

	// 获取成就奖励
	AchievementCfg::SAchievementReward reward;
	if (GetGameCfgFileMan()->GetAchievementReward(achievement_id, reward))
	{
		// 领取成就物品奖励
		int retCode = GetRolesInfoMan()->ReceiveAchievementReward(userId, roleId, achievement_id, pAchievementReward, pRefresh);
		if (retCode != MLS_OK) return retCode;

		// 领取成就点
		DBS_RESULT result = NULL;
		char szSQL[MAX_SQL_BUF_LEN] = { 0 };
		sprintf(szSQL, "SELECT point FROM achievement WHERE actorid=%u", roleId);
		retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
		if (retCode != DBS_OK) return retCode;

		UINT32 point = reward.achievement_count;
		if (DBS_ROW row = DBS_FetchRow(result))
		{
			point += (UINT32)_atoi64(row[0]);
		}
		DBS_FreeResult(result);

		UINT32 affectRows = 0;
		sprintf(szSQL, "UPDATE achievement SET point=%u WHERE actorid=%u", point, roleId);
		retCode = GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
		if (retCode != DBS_OK) return retCode;

		if (affectRows == 0)
		{
			sprintf(szSQL, "INSERT INTO achievement(actorid,point) VALUES(%u,%u)", roleId, point);
			GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
		}

		pAchievementReward->point = point;
	}

	// 领取成就
	{
		CXTAutoLock lock(m_record_locker);
		std::map<UINT32, achievement_map>::iterator itr = m_achievement_record.find(roleId);
		if (itr != m_achievement_record.end())
		{
			achievement_map &achievement = itr->second;
			achievement_map::iterator result = achievement.find(achievement_id);
			if (result != achievement.end())
			{
				result->second = true;
			}
		}
	}

	// 更新数据
	{
		CXTAutoLock lock(m_update_locker);
		m_updateSet.insert(roleId);
	}

	return MLS_OK;
}

void AchievementMan::RefreshRoleTimer(void* param, TTimerID id)
{
	AchievementMan* pThis = (AchievementMan*)param;
	if (pThis)
		pThis->OnRefreshRoleInfo();
}

void AchievementMan::OnRefreshRoleInfo()
{
	if (m_updateSet.empty())
		return;

	std::set<UINT32> update_set;
	{
		CXTAutoLock lock(m_update_locker);
		update_set = m_updateSet;
		m_updateSet.clear();
	}

	CXTAutoLock lock(m_record_locker);
	for (std::set<UINT32>::const_iterator itr = update_set.begin(); itr != update_set.end(); ++itr)
	{
		std::map<UINT32, achievement_map>::iterator result = m_achievement_record.find(*itr);
		if (result != m_achievement_record.end())
		{
			WriteAchievementRecordToDB(*itr, result->second);
		}
	}
}