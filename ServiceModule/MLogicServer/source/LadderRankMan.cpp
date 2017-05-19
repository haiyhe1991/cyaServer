#include "LadderRankMan.h"

#include "gb_utf8.h"
#include "DBSClient.h"
#include "ServiceErrorCode.h"
#include "RolesInfoMan.h"
#include "ConfigFileMan.h"
#include "GameCfgFileMan.h"
#include "RefreshDBMan.h"
#include "EmailCleanMan.h"
#include "MessageTipsMan.h"

#define LADDER_TIMEOUT 300

static LadderRankMan* sg_pLadderRankMan = NULL;
void InitLadderRankMan()
{
	ASSERT(sg_pLadderRankMan == NULL);
	sg_pLadderRankMan = new LadderRankMan();
	ASSERT(sg_pLadderRankMan != NULL);
}

LadderRankMan* GetLadderRankMan()
{
	return sg_pLadderRankMan;
}

void DestroyLadderRankMan()
{
	LadderRankMan* pLadderRankMan = sg_pLadderRankMan;
	sg_pLadderRankMan = NULL;
	if (pLadderRankMan != NULL)
		delete pLadderRankMan;
}

LadderRankMan::LadderRankMan()
{

}

LadderRankMan::~LadderRankMan()
{

}

// 发送奖励邮件
void LadderRankMan::SendRewardEmail(UINT32 userId, UINT32 roleId, const std::vector<GameTypeCfg::SRewardInfo> &rewardVec, const char *body)
{
	if (!rewardVec.empty())
	{
		UINT32 vit = 0;
		UINT64 exp = 0;
		UINT64 gold = 0;
		UINT32 rpcoin = 0;
		std::vector<SPropertyAttr> propVec;
		for (size_t i = 0; i < rewardVec.size(); ++i)
		{
			switch (rewardVec[i].type)
			{
			case ESGS_PROP_EQUIP:		//装备
			case ESGS_PROP_GEM:			//宝石
			case ESGS_PROP_MATERIAL:	//材料
			{
											BYTE maxStack = 0;
											SPropertyAttr prop;
											memset(&prop, 0, sizeof(SPropertyAttr));
											if (MLS_OK == GetGameCfgFileMan()->GetPropertyAttr(rewardVec[i].type, rewardVec[i].id, prop, maxStack))
											{
												prop.num = rewardVec[i].num;
												propVec.push_back(prop);
											}
											break;
			}
			case ESGS_PROP_GOLD:		//游戏币
			{
											gold = SGSU64Add(gold, rewardVec[i].num);
											break;
			}
			case ESGS_PROP_TOKEN:		//代币
			{
											rpcoin = SGSU32Add(rpcoin, rewardVec[i].num);
											break;
			}
			case ESGS_PROP_VIT:			//体力
			{
											vit = SGSU32Add(vit, rewardVec[i].num);
											break;
			}
			case ESGS_PROP_EXP:			//经验
			{
											exp = SGSU64Add(exp, rewardVec[i].num);
											break;
			}
			default:
			{
					   SPropertyAttr prop;
					   memset(&prop, 0, sizeof(SPropertyAttr));
					   prop.propId = rewardVec[i].id;
					   prop.num = rewardVec[i].num;
					   prop.type = rewardVec[i].type;
					   propVec.push_back(prop);
			}
			}
		}

		EmailCleanMan::SSyncEmailSendContent email;
		email.gold = gold;
		email.rpcoin = rpcoin;
		email.exp = exp;
		email.vit = vit;
		email.receiverUserId = userId;
		email.receiverRoleId = roleId;
		email.type = ESGS_EMAIL_LADDER_REWARD;
		email.pGBSenderNick = "系统";
		email.pGBCaption = "天梯竞赛奖励";
		email.pGBBody = body;
		if (propVec.size() > 0)
		{
			email.attachmentsNum = propVec.size();
			email.pAttachments = propVec.data();
		}
		GetEmailCleanMan()->AsyncSendEmail(email);
	}
}

// 查询天梯称号
int LadderRankMan::QueryLadderRankTitle(UINT32 userId, UINT32 roleId)
{
	//SELECT ladder_title FROM data WHERE actorid=%u
	return MLS_OK;
}

// 领取天梯奖励
void LadderRankMan::ReceiveLadderReward(UINT32 userId, UINT32 roleId, UINT32 rank, UINT16 win_in_row)
{
	const std::vector<GameTypeCfg::SRewardInfo> *rankingVec = NULL;
	const std::vector<GameTypeCfg::SRewardInfo> *winInRowVec = NULL;
	std::vector<GameTypeCfg::SRewardCondition> *conditionVec_ptr = NULL;
	if (GetGameCfgFileMan()->GetConditionAndReward(ESGS_INST_HIGH_LADDER, conditionVec_ptr))
	{
		std::vector<GameTypeCfg::SRewardCondition> &conditionVec = *conditionVec_ptr;
		for (std::vector<GameTypeCfg::SRewardCondition>::const_iterator itr = conditionVec.begin(); itr != conditionVec.end(); ++itr)
		{
			switch (itr->type)
			{
			case ESGS_CONDITION_HIGH_LADDER_RANK:					// 天梯排名
			{
																		if (rank <= itr->num)
																		{
																			rankingVec = &itr->reward;
																		}
																		break;
			}
			case ESGS_CONDITION_HIGH_LADDER_CONTINUOUS_VICTORY_COUNT:// 天梯连胜次数
			{
																		 if (win_in_row >= itr->num)
																		 {
																			 winInRowVec = &itr->reward;
																		 }
																		 break;
			}
			}
		}
	}

	// 发送排名奖励
	char szBody[MAX_UTF8_EMAIL_BODY_LEN] = { 0 };
	if (rankingVec != NULL && !rankingVec->empty())
	{
		sprintf(szBody, "恭喜你在昨日的天梯竞赛中获得%u名，荣获排名奖励。", rank);
		std::vector<GameTypeCfg::SRewardInfo> rewards;
		GetGameCfgFileMan()->UnlockRewardNesting(*rankingVec, &rewards);
		SendRewardEmail(userId, roleId, rewards, szBody);
	}

	// 发送连胜奖励
	if (winInRowVec != NULL && !winInRowVec->empty())
	{
		sprintf(szBody, "恭喜你在天梯竞赛中保持%u连胜不败，获得额外连胜奖励。", win_in_row);
		std::vector<GameTypeCfg::SRewardInfo> rewards;
		GetGameCfgFileMan()->UnlockRewardNesting(*winInRowVec, &rewards);
		SendRewardEmail(userId, roleId, rewards, szBody);
	}
}

// 每日结算
void LadderRankMan::DailySettlement()
{
	std::vector<SLadderItem> ladderVec;

	// 清除今日挑战信息
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), "UPDATE ladder SET refresh_count=0, eliminate_count=0, challenge_count=0, last_finish_time=NULL");

	// 查询天梯排行榜
	{
		CXTAutoLock lock(m_locker);
		DBS_RESULT result = NULL;
		int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(),
			"SELECT b.rank,a.accountid,a.actorid,a.win_in_row FROM ladder AS a JOIN ladder_rank AS b ON (a.actorid=b.actorid) WHERE b.robot=0",
			result);
		if (retCode != MLS_OK)
			return;

		SLadderItem item;
		DBS_ROW row_data = NULL;
		UINT32 nRows = DBS_NumRows(result);
		ladderVec.reserve(nRows);
		while ((row_data = DBS_FetchRow(result)) != NULL)
		{
			item.rank = atol(row_data[0]);
			item.userId = atoi(row_data[1]);
			item.roleId = atoi(row_data[2]);
			item.win_in_row = atoi(row_data[3]);
			ladderVec.push_back(item);
		}
		DBS_FreeResult(result);
	}

	for (size_t i = 0; i < ladderVec.size(); ++i)
	{
		const SLadderItem &ref_item = ladderVec[i];
		ReceiveLadderReward(ref_item.userId, ref_item.roleId, ref_item.rank, ref_item.win_in_row);
	}
}

// 删除角色
int LadderRankMan::DelRole(UINT32 roleId)
{
	// 将角色所在排名替换为机器人
	if (roleId != 0)
	{
		CXTAutoLock lock(m_locker);
		BYTE actortype = 1 + rand() % 3;
		UINT32 cp = 1000 + rand() % 1001;
		BYTE level = 5 + rand() % 6;
		char szSQL[MAX_SQL_BUF_LEN] = { 0 };
		sprintf(szSQL, "UPDATE ladder_rank SET actorid=0, robot=1, actortype=%u, cp=%u, level=%u WHERE actorid=%u",
			actortype, cp, level, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, NULL);

		std::map<UINT32/*角色ID*/, UINT32/*挑战名次*/>::iterator role_itr = m_roleIdOnRank.find(roleId);
		if (role_itr != m_roleIdOnRank.end())
		{
			m_roleIdOnRank.erase(role_itr);
			m_rankOnRoleId.erase(role_itr->second);
		}
	}
	return MLS_OK;
}

// 查询角色天梯信息
int LadderRankMan::QueryRoleLadderInfoFromDB(UINT32 roleId, UINT16 *win_in_row, UINT16 *refresh_count, UINT16 *eliminate_count, UINT16 *challenge_count, LTMSEL *last_finish_time, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM])
{
	*win_in_row = 0;
	*refresh_count = 0;
	*eliminate_count = 0;
	*challenge_count = 0;
	*last_finish_time = 0;

	DBS_RESULT result = NULL;
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT win_in_row,refresh_count,eliminate_count,challenge_count,last_finish_time,user_data FROM ladder WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	DBS_ROWLENGTH rowValLen = NULL;
	DBS_ROW row = DBS_FetchRow(result, &rowValLen);
	if (row != NULL)
	{
		*win_in_row = atol(row[0]);
		*refresh_count = atoi(row[1]);
		*eliminate_count = atoi(row[2]);
		*challenge_count = atoi(row[3]);

		if (rowValLen[4] > 0)
		{
			*last_finish_time = LocalStrTimeToMSel(row[4]);
		}

		if (rowValLen[5] > 0)
		{
			BYTE size = *(BYTE *)row[5];
			ASSERT(size == LADDER_MATCHTING_MAX_NUM);
			UINT32 *rank_data = (UINT32 *)((BYTE *)row[5] + sizeof(BYTE));
			for (int i = 0; i < LADDER_MATCHTING_MAX_NUM; ++i)
			{
				matching_rank[i] = rank_data[i];
			}
		}
	}
	DBS_FreeResult(result);

	return MLS_OK;
}

// 匹配玩家
int LadderRankMan::MatchingPlayerFromDB(UINT32 userId, UINT32 roleId, std::vector<SMatchingLadderItem> &retVec)
{
	retVec.clear();
	retVec.reserve(LADDER_MATCHTING_MAX_NUM);

	// 获取玩家排名
	DBS_ROW row_data = NULL;
	DBS_RESULT result = NULL;
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT rank FROM ladder_rank WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	row_data = DBS_FetchRow(result);
	if (row_data == NULL)
	{
		DBS_FreeResult(result);
		sprintf(szSQL, "INSERT INTO ladder_rank(actorid, robot) VALUES(%u, 0)", roleId);
		retCode = GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
		if (retCode != MLS_OK)
			return retCode;

		sprintf(szSQL, "SELECT rank FROM ladder_rank WHERE actorid=%u", roleId);
		retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
		if (retCode != MLS_OK)
			return retCode;
		row_data = DBS_FetchRow(result);
	}

	// 匹配玩家
	UINT32 current_rank = atoi(row_data[0]);
	DBS_FreeResult(result);

	// 单独处理前五名(张鹏2015.10.30需求)
	if (current_rank <= 5)
	{
		sprintf(szSQL, "SELECT * FROM ladder_rank WHERE rank BETWEEN %u AND %u ORDER BY RAND() LIMIT %u", 1, 5, LADDER_MATCHTING_MAX_NUM);
	}
	else
	{
		UINT32 start_pos = SGSU32Sub(current_rank, GetGameCfgFileMan()->GetLadderMatchingScope(current_rank));
		sprintf(szSQL, "SELECT * FROM ladder_rank WHERE rank BETWEEN %u AND %u ORDER BY RAND() LIMIT %u", start_pos, current_rank - 1, LADDER_MATCHTING_MAX_NUM);
	}
	retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;
	while ((row_data = DBS_FetchRow(result)) != NULL)
	{
		SMatchingLadderItem item;
		item.rank = atoi(row_data[0]);
		item.roleId = atoi(row_data[1]);
		item.robot = atoi(row_data[2]) == 1;
		if (item.robot)
		{
			item.actortype = atoi(row_data[3]);
			item.cp = atoi(row_data[4]);
			item.level = atoi(row_data[5]);
		}
		retVec.push_back(item);
	}
	DBS_FreeResult(result);

	// 补全信息
	UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM];
	memset(matching_rank, 0, sizeof(matching_rank));
	for (size_t i = 0; i < retVec.size(); ++i)
	{
		SMatchingLadderItem &item = retVec[i];
		matching_rank[i] = item.rank;
		if (item.robot)
		{
			const char *description = "天梯勇士";
			memset(item.nick, 0, sizeof(item.nick));
			gb2312_utf8(description, strlen(description), item.nick, sizeof(item.nick));
		}
		else
		{
			sprintf(szSQL, "SELECT nick,level,actortype,cp FROM actor WHERE id=%u", item.roleId);
			retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
			if (retCode != MLS_OK)
				return retCode;

			row_data = DBS_FetchRow(result);
			if (row_data != NULL)
			{
				memcpy(item.nick, row_data[0], sizeof(item.nick));
				item.level = atoi(row_data[1]);
				item.actortype = atoi(row_data[2]);
				item.cp = atol(row_data[3]);
			}
			DBS_FreeResult(result);
		}
	}

	return MLS_OK;
}

// 玩家信息
int LadderRankMan::GetChallengeInfo(UINT32 userId, UINT32 roleId, SLadderRoleInfoRes *pLadderInfo)
{
	SRoleInfos role_info;
	int retCode = GetRolesInfoMan()->GetRoleInfo(userId, roleId, &role_info);
	if (MLS_OK != retCode) return retCode;

	pLadderInfo->win_in_row = role_info.ladder_win_in_row;
	pLadderInfo->refresh_count = role_info.ladder_refresh_count;
	pLadderInfo->eliminate_count = role_info.ladder_eliminate_count;
	pLadderInfo->challenge_count = role_info.ladder_challenge_count;
	if (role_info.ladder_last_finish_time == 0)
	{
		pLadderInfo->cooling_time = 0;
	}
	else
	{
		LTMSEL seconds = (GetMsel() - role_info.ladder_last_finish_time) / 1000;
		if (seconds > GetGameCfgFileMan()->GetLadderChallengeCDTime())
		{
			pLadderInfo->cooling_time = 0;
		}
		else
		{
			pLadderInfo->cooling_time = GetGameCfgFileMan()->GetLadderChallengeCDTime() - seconds;
		}
	}

	// 查询排名
	{
		CXTAutoLock lock(m_locker);

		DBS_ROW row_data = NULL;
		DBS_RESULT result = NULL;
		char szSQL[MAX_SQL_BUF_LEN] = { 0 };
		sprintf(szSQL, "SELECT rank FROM ladder_rank WHERE actorid=%u", roleId);
		retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
		if (retCode != MLS_OK)
			return retCode;

		row_data = DBS_FetchRow(result);
		if (row_data == NULL)
		{
			DBS_FreeResult(result);
			sprintf(szSQL, "INSERT INTO ladder_rank(actorid, robot) VALUES(%u, 0)", roleId);
			retCode = GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
			if (retCode != MLS_OK)
				return retCode;

			sprintf(szSQL, "SELECT rank FROM ladder_rank WHERE actorid=%u", roleId);
			retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
			if (retCode != MLS_OK)
				return retCode;
			row_data = DBS_FetchRow(result);
		}
		pLadderInfo->rank = atol(row_data[0]);
		DBS_FreeResult(result);
	}

	return MLS_OK;
}

// 匹配玩家
int LadderRankMan::MatchingPlayer(UINT32 userId, UINT32 roleId, SMatchingLadderRes *pMatchingLadder)
{
	std::vector<SMatchingLadderItem> rankVec;
	int retCode = MatchingPlayerFromDB(userId, roleId, rankVec);
	if (retCode != MLS_OK)
		return retCode;

	UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM];
	memset(matching_rank, 0, sizeof(matching_rank));
	for (size_t i = 0; i < rankVec.size(); ++i)
	{
		matching_rank[i] = rankVec[i].rank;
	}
	retCode = GetRolesInfoMan()->LadderMatching(userId, roleId, matching_rank, &pMatchingLadder->refresh_count, &pMatchingLadder->rpcoin);
	if (retCode != MLS_OK)
		return retCode;

	pMatchingLadder->num = rankVec.size();
	if (!rankVec.empty())
	{
		memcpy(pMatchingLadder->data, &*rankVec.begin(), sizeof(SMatchingLadderItem)* rankVec.size());
	}

	return MLS_OK;
}

// 刷新天梯排行范围
int LadderRankMan::RefreshLadderRankScope(UINT32 userId, UINT32 roleId, SRefreshLadderRankRes *pRefreshRank)
{
	// 获取玩家信息
	SRoleInfos role_info;
	int retCode = GetRolesInfoMan()->GetRoleInfo(userId, roleId, &role_info);
	if (MLS_OK != retCode)
	{
		return retCode;
	}

	// 是否需要匹配
	bool matching = true;
	for (int i = 0; i < LADDER_MATCHTING_MAX_NUM; ++i)
	{
		if (role_info.ladder_matching_rank[i] != 0)
		{
			matching = false;
			break;
		}
	}

	// 构建数据
	UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM];
	if (matching)
	{
		std::vector<SMatchingLadderItem> rankVec;
		int retCode = MatchingPlayerFromDB(userId, roleId, rankVec);
		if (retCode != MLS_OK)
			return retCode;

		memset(matching_rank, 0, sizeof(matching_rank));
		for (size_t i = 0; i < rankVec.size(); ++i)
		{
			matching_rank[i] = rankVec[i].rank;
		}

		retCode = GetRolesInfoMan()->RefreshLadderRankScope(userId, roleId, matching_rank);
		if (MLS_OK != retCode)
		{
			return retCode;
		}

		pRefreshRank->num = rankVec.size();
		if (!rankVec.empty())
		{
			memcpy(pRefreshRank->data, &*rankVec.begin(), sizeof(SMatchingLadderItem)* rankVec.size());
		}
		return MLS_OK;
	}
	else
	{
		DBS_ROW row_data = NULL;
		DBS_RESULT result = NULL;
		char szSQL[MAX_SQL_BUF_LEN] = { 0 };
		std::vector<SMatchingLadderItem> rankVec;
		rankVec.reserve(LADDER_MATCHTING_MAX_NUM);
		memcpy(matching_rank, role_info.ladder_matching_rank, sizeof(matching_rank));
		for (int i = 0; i < LADDER_MATCHTING_MAX_NUM; ++i)
		{
			if (matching_rank[i] != 0)
			{
				sprintf(szSQL, "SELECT * FROM ladder_rank WHERE rank=%u", matching_rank[i]);
				retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
				if (retCode != MLS_OK)
					return retCode;

				row_data = DBS_FetchRow(result);
				if (row_data == NULL)
				{
					DBS_FreeResult(result);
					continue;
				}

				SMatchingLadderItem item;
				item.rank = atoi(row_data[0]);
				item.roleId = atoi(row_data[1]);
				item.robot = atoi(row_data[2]) == 1;
				if (item.robot)
				{
					item.actortype = atoi(row_data[3]);
					item.cp = atoi(row_data[4]);
					item.level = atoi(row_data[5]);
				}
				rankVec.push_back(item);
				DBS_FreeResult(result);

				// 补全信息
				for (size_t i = 0; i < rankVec.size(); ++i)
				{
					SMatchingLadderItem &item = rankVec[i];
					if (item.robot)
					{
						const char *description = "天梯勇士";
						memset(item.nick, 0, sizeof(item.nick));
						gb2312_utf8(description, strlen(description), item.nick, sizeof(item.nick));
					}
					else
					{
						sprintf(szSQL, "SELECT nick,level,actortype,cp FROM actor WHERE id=%u", item.roleId);
						retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
						if (retCode != MLS_OK)
							return retCode;

						row_data = DBS_FetchRow(result);
						if (row_data != NULL)
						{
							memcpy(item.nick, row_data[0], sizeof(item.nick));
							item.level = atoi(row_data[1]);
							item.actortype = atoi(row_data[2]);
							item.cp = atol(row_data[3]);
						}
						DBS_FreeResult(result);
					}
				}
			}
		}
		pRefreshRank->num = rankVec.size();
		if (!rankVec.empty())
		{
			memcpy(pRefreshRank->data, &*rankVec.begin(), sizeof(SMatchingLadderItem)* rankVec.size());
		}
		return MLS_OK;
	}
}

// 挑战玩家
int LadderRankMan::ChallengePlayer(UINT32 userId, UINT32 roleId, INT32 target_rank, SChallengeLadderRes *pChallengeLadder)
{
	// 排名是否被锁定
	{
		CXTAutoLock lock(m_locker);
		std::map<UINT32/*挑战名次*/, SRankRoleInfo>::iterator rank_itr = m_rankOnRoleId.find(target_rank);
		if (rank_itr != m_rankOnRoleId.end())
		{
			// 是否超时
			if ((GetMsel() - rank_itr->second.challenge_time) / 1000 >= LADDER_TIMEOUT)
			{
				m_roleIdOnRank.erase(rank_itr->second.roleId);
				rank_itr->second.roleId = roleId;
				rank_itr->second.challenge_time = GetMsel();
				m_roleIdOnRank.insert(std::make_pair(roleId, target_rank));
			}
			else
			{
				return MLS_LADDER_RANK_LOCKED;
			}
		}
		else
		{
			m_roleIdOnRank.insert(std::make_pair(roleId, target_rank));
			m_rankOnRoleId.insert(std::make_pair(target_rank, SRankRoleInfo(roleId, GetMsel())));
		}
	}

	// 检查目标排名的有效性
	DBS_ROW row_data = NULL;
	DBS_RESULT result = NULL;
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };

	// 排名是否高于自己
	{
		sprintf(szSQL, "SELECT rank FROM ladder_rank WHERE actorid=%u", roleId);
		int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
		if (retCode != MLS_OK)
		{
			CXTAutoLock lock(m_locker);
			m_roleIdOnRank.erase(roleId);
			m_rankOnRoleId.erase(target_rank);
			return retCode;
		}

		if ((row_data = DBS_FetchRow(result)) != NULL)
		{
			if (atol(row_data[0]) <= target_rank)
			{
				DBS_FreeResult(result);
				CXTAutoLock lock(m_locker);
				m_roleIdOnRank.erase(roleId);
				m_rankOnRoleId.erase(target_rank);
				return MLS_INVALID_LADDER_RANK;
			}
		}
		DBS_FreeResult(result);
	}

	// 获取被挑战者信息
	{
		sprintf(szSQL, "SELECT * FROM ladder_rank WHERE rank=%u", target_rank);
		int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
		if (retCode != MLS_OK)
		{
			CXTAutoLock lock(m_locker);
			m_roleIdOnRank.erase(roleId);
			m_rankOnRoleId.erase(target_rank);
			return retCode;
		}

		if ((row_data = DBS_FetchRow(result)) == NULL)
		{
			DBS_FreeResult(result);
			CXTAutoLock lock(m_locker);
			m_roleIdOnRank.erase(roleId);
			m_rankOnRoleId.erase(target_rank);
			return MLS_INVALID_LADDER_RANK;
		}

		// 获取基本信息
		SMatchingLadderItem &item = pChallengeLadder->player_info;
		item.rank = atol(row_data[0]);
		item.roleId = atoi(row_data[1]);
		item.robot = atoi(row_data[2]) == 1;

		// 如果被挑战者正在挑战其它排名
		{
			CXTAutoLock lock(m_locker);
			std::map<UINT32/*角色ID*/, UINT32/*挑战名次*/>::iterator role_itr = m_roleIdOnRank.find(item.roleId);
			if (role_itr != m_roleIdOnRank.end())
			{
				std::map<UINT32/*挑战名次*/, SRankRoleInfo>::iterator rank_itr = m_rankOnRoleId.find(role_itr->second);
				ASSERT(rank_itr != m_rankOnRoleId.end());
				if (rank_itr != m_rankOnRoleId.end())
				{
					if ((GetMsel() - rank_itr->second.challenge_time) / 1000 <= LADDER_TIMEOUT)
					{
						DBS_FreeResult(result);
						m_roleIdOnRank.erase(roleId);
						m_rankOnRoleId.erase(target_rank);
						return MLS_LADDER_RANK_LOCKED;
					}
					else
					{
						m_roleIdOnRank.erase(role_itr);
						m_rankOnRoleId.erase(rank_itr);
					}
				}
			}
		}

		// 执行天梯挑战
		retCode = GetRolesInfoMan()->DoLadderChallenge(userId, roleId, &pChallengeLadder->challenge_count);
		if (retCode != MLS_OK)
		{
			DBS_FreeResult(result);
			CXTAutoLock lock(m_locker);
			m_roleIdOnRank.erase(roleId);
			m_rankOnRoleId.erase(target_rank);
			return retCode;
		}

		if (!item.robot && item.roleId == roleId)
		{
			DBS_FreeResult(result);
			CXTAutoLock lock(m_locker);
			m_roleIdOnRank.erase(roleId);
			m_rankOnRoleId.erase(target_rank);
			return MLS_DONT_CHALLENGE_SELF;
		}

		// 获取被挑战者角色数据
		if (item.robot)
		{
			item.actortype = atoi(row_data[3]);
			item.cp = atoi(row_data[4]);
			item.level = atoi(row_data[5]);
			const char *description = "天梯勇士";
			memset(item.nick, 0, sizeof(item.nick));
			gb2312_utf8(description, strlen(description), item.nick, sizeof(item.nick));
		}
		DBS_FreeResult(result);

		if (!item.robot)
		{
			sprintf(szSQL, "SELECT nick,level,actortype,cp FROM actor WHERE id=%u", item.roleId);
			retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
			if (retCode != MLS_OK)
			{
				CXTAutoLock lock(m_locker);
				m_roleIdOnRank.erase(roleId);
				m_rankOnRoleId.erase(target_rank);
				return retCode;
			}

			if ((row_data = DBS_FetchRow(result)) != NULL)
			{
				memcpy(item.nick, row_data[0], sizeof(item.nick));
				item.level = atoi(row_data[1]);
				item.actortype = atoi(row_data[2]);
				item.cp = atol(row_data[3]);
			}
			DBS_FreeResult(result);
		}
	}

	return MLS_OK;
}

// 消除冷却时间
int LadderRankMan::EliminateCoolingTime(UINT32 userId, UINT32 roleId, SEliminateLadderCDTimeRes *pEliminate)
{
	return GetRolesInfoMan()->EliminateLadderCoolingTime(userId, roleId, pEliminate);
}

// 挑战完成
int LadderRankMan::FinishChallenge(UINT32 userId, UINT32 roleId, BYTE finishCode/*, SFinishLadderChallengeRes *pFinishLadder*/)
{
	UINT32 targetId = 0;
	UINT32 last_rank = 0;
	UINT32 challenge_rank = 0;

	// 正确性检查
	{
		CXTAutoLock lock(m_locker);
		std::map<UINT32/*角色ID*/, UINT32/*挑战名次*/>::iterator role_itr = m_roleIdOnRank.find(roleId);
		if (role_itr == m_roleIdOnRank.end())
			return MLS_FINISH_CHALLENGE_LADDER_FAIL;

		std::map<UINT32/*挑战名次*/, SRankRoleInfo>::iterator rank_itr = m_rankOnRoleId.find(role_itr->second);
		ASSERT(rank_itr != m_rankOnRoleId.end());
		if (rank_itr == m_rankOnRoleId.end())
			return MLS_FINISH_CHALLENGE_LADDER_FAIL;

		// 是否超时
		if ((GetMsel() - rank_itr->second.challenge_time) / 1000 >= LADDER_TIMEOUT)
		{
			m_roleIdOnRank.erase(role_itr);
			m_rankOnRoleId.erase(rank_itr);
			return MLS_FINISH_CHALLENGE_LADDER_TIMEOUT;
		}
		challenge_rank = rank_itr->first;
	}

	// 是否完成
	if (finishCode != MLS_OK)
	{
		// 更新挑战信息
		int retCode = GetRolesInfoMan()->FinishLadderChallenge(userId, roleId, finishCode/*, pFinishLadder*/);
		if (retCode != MLS_OK)
			return retCode;
		CXTAutoLock lock(m_locker);
		m_roleIdOnRank.erase(roleId);
		m_rankOnRoleId.erase(challenge_rank);
		return MLS_OK;
	}

	// 互换排名
	DBS_RESULT result = NULL;
	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT rank FROM ladder_rank WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return retCode;

	DBS_ROW row_data = DBS_FetchRow(result);
	if (row_data == NULL)
	{
		DBS_FreeResult(result);
		UINT32 affectRows = 0;
		sprintf(szSQL, "UPDATE ladder_rank SET actorid=%u,robot=0,actortype=NULL,cp=NULL,level=NULL WHERE rank=%u", roleId, challenge_rank);
		retCode = GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
		if (retCode != MLS_OK || affectRows != 1)
		{
			return retCode;
		}
	}
	else
	{
		last_rank = atol(row_data[0]);
		DBS_FreeResult(result);

		// 取出被挑战排名的玩家信息
		sprintf(szSQL, "SELECT actorid,robot,actortype,cp,level FROM ladder_rank WHERE rank=%u", challenge_rank);
		retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
		if (retCode != MLS_OK)
			return retCode;
		row_data = DBS_FetchRow(result);
		ASSERT(row_data != NULL);
		if (row_data == NULL)
		{
			DBS_FreeResult(result);
			return MLS_FINISH_CHALLENGE_LADDER_FAIL;
		}

		UINT32 cp = 0;
		BYTE actortype = 0, level = 0;
		targetId = atol(row_data[0]);
		BYTE robot = atoi(row_data[1]);
		if (robot != 0)
		{
			actortype = atoi(row_data[2]);
			cp = atoi(row_data[3]);
			level = atoi(row_data[4]);
		}
		DBS_FreeResult(result);

		// 更新被挑战者的排名
		UINT32 affectRows = 0;
		if (robot == 0)
		{
			sprintf(szSQL, "UPDATE ladder_rank SET actorid=%u,robot=0,actortype=NULL,cp=NULL,level=NULL WHERE rank=%u", targetId, last_rank);
		}
		else
		{
			sprintf(szSQL, "UPDATE ladder_rank SET actorid=%u,robot=1,actortype=%u,cp=%u,level=%u WHERE rank=%u",
				targetId, actortype, cp, level, last_rank);
		}
		retCode = GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
		if (retCode != MLS_OK || affectRows != 1)
			return retCode;

		// 更新挑战者的排名
		affectRows = 0;
		sprintf(szSQL, "UPDATE ladder_rank SET actorid=%u,robot=0,actortype=NULL,cp=NULL,level=NULL WHERE rank=%u", roleId, challenge_rank);
		retCode = GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
		if (retCode != MLS_OK || affectRows != 1)
			return retCode;
	}

	// 更新挑战者信息
	//pFinishLadder->rank = challenge_rank;
	retCode = GetRolesInfoMan()->FinishLadderChallenge(userId, roleId, finishCode/*, pFinishLadder*/);
	if (retCode != MLS_OK)
		return retCode;

	// 更新被挑战者信息
	if (targetId != 0)
	{
		UINT32 accountId = 0;
		sprintf(szSQL, "SELECT accountid FROM ladder WHERE actorid=%u", targetId);
		retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
		if (retCode != MLS_OK)
			return retCode;

		row_data = DBS_FetchRow(result);
		if (row_data != NULL)
		{
			accountId = atoi(row_data[0]);
			GetRolesInfoMan()->ResetChallengerLadderRankScope(accountId, targetId);
			GetRefreshDBMan()->InputUpdateInfo(accountId, targetId, REFRESH_ROLE_RESET_LADDER_RANK_VIEW);
		}
		DBS_FreeResult(result);
	}

	// 解除锁定
	{
		CXTAutoLock lock(m_locker);
		m_roleIdOnRank.erase(roleId);
		m_rankOnRoleId.erase(challenge_rank);
	}

	// 发送消息提示
	char szText[256] = { 0 };
	sprintf(szText, "挑战成功，排名上升至%u名", challenge_rank);
	GetMessageTipsMan()->SendMessageTips(userId, roleId, 0, "天梯模式", szText);
	if (targetId != 0)
	{
		char nickName[33] = { 0 };
		if (MLS_OK == GetRolesInfoMan()->GetRoleNick(userId, roleId, nickName))
		{
			char szGBOccupierNick[MAX_UTF8_NICK_LEN] = { 0 };
			utf8_gb2312(nickName, strlen(nickName), szGBOccupierNick, sizeof(szGBOccupierNick)-sizeof(char));
			sprintf(szText, "%s对你发起挑战，很遗憾您没能获得胜利，排名下降至%u名，是否立即前往挑战？", szGBOccupierNick, last_rank);
			GetMessageTipsMan()->SendMessageTips(targetId, 0, "天梯模式", szText);
		}
	}

	return MLS_OK;
}