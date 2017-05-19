#include "WorldBossMan.h"

#include <algorithm>
#include "gb_utf8.h"
#include "DBSClient.h"
#include "ServiceErrorCode.h"
#include "RolesInfoMan.h"
#include "ConfigFileMan.h"
#include "SystemNoticeMan.h"
#include "EmailCleanMan.h"

// 无效分钟数
#define INVALID_MINUTE -1

static WorldBossMan* sg_pWorldBossMan = NULL;
void InitWorldBossMan()
{
	ASSERT(sg_pWorldBossMan == NULL);
	sg_pWorldBossMan = new WorldBossMan();
	ASSERT(sg_pWorldBossMan != NULL);
}

WorldBossMan* GetWorldBossMan()
{
	return sg_pWorldBossMan;
}

void DestroyWorldBossMan()
{
	WorldBossMan* pWorldBossMan = sg_pWorldBossMan;
	sg_pWorldBossMan = NULL;
	if (pWorldBossMan != NULL)
		delete pWorldBossMan;
}

WorldBossMan::WorldBossMan()
: m_isOpen(FALSE)
, m_isEnd(FALSE)
, m_lastActivityTime(0)
, m_lastSendNoTiceTime(INVALID_MINUTE)
{
	m_refreshTimer.Start(RefreshWorldBossTimer, this, 5000);
}

WorldBossMan::~WorldBossMan()
{
	if (m_refreshTimer.IsStarted())
		m_refreshTimer.Stop();
}

void WorldBossMan::RefreshWorldBossTimer(void* param, TTimerID id)
{
	WorldBossMan* pThis = (WorldBossMan*)param;
	if (pThis)
		pThis->OnRefreshWorldBoss();
}

// 发送奖励邮件
void WorldBossMan::SendRewardEmail(UINT32 userId, UINT32 roleId, const std::vector<GameTypeCfg::SRewardInfo> &rewardVec, const char *body)
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
		email.type = ESGS_EMAIL_WORLD_BOSS_REWARD;
		email.pGBSenderNick = "系统";
		email.pGBCaption = "讨伐世界Boss奖励";
		email.pGBBody = body;
		if (propVec.size() > 0)
		{
			email.attachmentsNum = propVec.size();
			email.pAttachments = propVec.data();
		}
		GetEmailCleanMan()->AsyncSendEmail(email);
	}
}

// 领取世界Boss奖励
void WorldBossMan::ReceiveWorldBossReward(UINT32 userId, UINT32 roleId, UINT32 hurtValue, UINT32 rank, bool kill)
{
	//const std::vector<GameTypeCfg::SRewardInfo> *involvedVec = NULL;
	const std::vector<GameTypeCfg::SRewardInfo> *killVec = NULL;
	const std::vector<GameTypeCfg::SRewardInfo> *hurtVec = NULL;
	const std::vector<GameTypeCfg::SRewardInfo> *rankingVec = NULL;
	std::vector<GameTypeCfg::SRewardCondition> *conditionVec_ptr = NULL;
	if (GetGameCfgFileMan()->GetConditionAndReward(ESGS_INST_WORLD_BOSS, conditionVec_ptr))
	{
		std::vector<GameTypeCfg::SRewardCondition> &conditionVec = *conditionVec_ptr;
		for (std::vector<GameTypeCfg::SRewardCondition>::const_iterator itr = conditionVec.begin(); itr != conditionVec.end(); ++itr)
		{
			switch (itr->type)
			{
				//case ESGS_CONDITION_HURT_EVERY:		// 每造成伤害
				//	{
				//		if (hurtValue >= itr->num)
				//		{
				//			involvedVec = &itr->reward;
				//		}
				//		break;
				//	}
			case ESGS_CONDITION_RANKING:		// 获得排名
			{
													if (rank == itr->num)
													{
														rankingVec = &itr->reward;
													}
													break;
			}
			case ESGS_CONDITION_LAST_STRAW:		// 最后一击
			{
													if (kill)
													{
														killVec = &itr->reward;
													}
													break;
			}
			case ESGS_CONDITION_HURT_EXTREMELY:	// 伤害万分比
			{
													float proportion = (float)hurtValue / GetGameCfgFileMan()->GetWorldBossBlood() * 100000;
													if (proportion >= itr->num)
													{
														hurtVec = &itr->reward;
													}
													break;
			}
			}
		}
	}

	// 获取系统当前时间
	SYSTEMTIME lcTime;
	GetLocalTime(&lcTime);
	char szBody[MAX_UTF8_EMAIL_BODY_LEN] = { 0 };
	LimitedTimeActivityCfg::SActivityConfig config;
	GetGameCfgFileMan()->GetLimitedTimeActivityCfg(LimitedTimeActivityCfg::WORLD_BOSS, config);

	// 发送参与奖
	/*if (involvedVec != NULL && !involvedVec->empty())
	{
	sprintf(szBody, "感谢你参与%u-%02u-%02u %02u:%02u到%u-%02u-%02u %02u:%02u的讨伐世界BOSS挑战，造成了%u点伤害，获得参与奖励。",
	lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.StartTimeHour, config.StartTimeMinute,
	lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.EndTimeHour, config.EndTimeHourMinute,
	hurtValue);
	SendRewardEmail(userId, roleId, *involvedVec, szBody);
	}*/

	// 发送排名奖励
	if (rankingVec != NULL && !rankingVec->empty())
	{
		sprintf(szBody, "你在%u-%02u-%02u %02u:%02u到%u-%02u-%02u %02u:%02u的讨伐世界BOSS挑战中功勋卓著，造成了%u点伤害，排名%u，获得了第%u名奖励。",
			lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.StartTimeHour, config.StartTimeMinute,
			lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.EndTimeHour, config.EndTimeHourMinute,
			hurtValue, rank, rank);
		std::vector<GameTypeCfg::SRewardInfo> rewards;
		GetGameCfgFileMan()->UnlockRewardNesting(*rankingVec, &rewards);
		SendRewardEmail(userId, roleId, rewards, szBody);
	}

	// 发送尾刀奖励
	if (killVec != NULL && !killVec->empty())
	{
		sprintf(szBody, "你在%u-%02u-%02u %02u:%02u到%u-%02u-%02u %02u:%02u的讨伐世界BOSS挑战中力拔头筹，成功击杀了世界BOSS，获得世界BOSS斩杀奖励。",
			lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.StartTimeHour, config.StartTimeMinute,
			lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.EndTimeHour, config.EndTimeHourMinute);
		std::vector<GameTypeCfg::SRewardInfo> rewards;
		GetGameCfgFileMan()->UnlockRewardNesting(*killVec, &rewards);
		SendRewardEmail(userId, roleId, rewards, szBody);
	}

	// 发送伤害万分比奖励
	if (hurtVec != NULL && !hurtVec->empty())
	{
		float proportion = (float)hurtValue / GetGameCfgFileMan()->GetWorldBossBlood() * 10000;
		sprintf(szBody, "感谢你参与%u-%02u-%02u %02u:%02u到%u-%02u-%02u %02u:%02u的讨伐世界BOSS挑战，造成了%u点伤害，占Boss血量的%u/10000，获得伤害奖励。",
			lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.StartTimeHour, config.StartTimeMinute,
			lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.EndTimeHour, config.EndTimeHourMinute,
			hurtValue, (UINT32)proportion);
		std::vector<GameTypeCfg::SRewardInfo> rewards;
		GetGameCfgFileMan()->UnlockRewardNesting(*hurtVec, &rewards);
		SendRewardEmail(userId, roleId, rewards, szBody);
	}
}

// 检测活动是否开启
void WorldBossMan::OnRefreshWorldBoss()
{
	// 获取系统当前时间
	SYSTEMTIME lcTime;
	GetLocalTime(&lcTime);

	// 发放奖励
	if (m_isOpen == TRUE && m_isEnd == TRUE)
	{
		m_isOpen = FALSE;

		// 击杀公告
		char format[256] = { 0 };
		char content[256] = { 0 };
		const char *description = "勇士%s成功击杀了世界BOSS，获得全服唯一世界BOSS奖励。";
		gb2312_utf8(description, strlen(description), format, sizeof(format));
		sprintf(content, format, m_killNickName.c_str());
		GetSystemNoticeMan()->BroadcastSysyemNoticeUTF8(content);

		// 死亡公告
		GetSystemNoticeMan()->WorldBossDeathNotice();

		// 重组数据
		std::map<UINT32/*角色ID*/, UINT32/*名次*/> role_rank;
		CXTAutoLock lock(m_locker);
		{
			std::sort(m_rankInfos.begin(), m_rankInfos.end(), SRankSortPred());
			for (size_t i = 0; i < m_rankInfos.size(); ++i)
			{
				role_rank.insert(std::make_pair(m_rankInfos[i].user.roleId, i + 1));
			}

			// 发送奖励
			for (std::map<SAccountInfo, SRoleAction>::const_iterator itr = m_roleInfos.begin(); itr != m_roleInfos.end(); ++itr)
			{
				std::map<UINT32/*角色ID*/, UINT32/*名次*/>::const_iterator result = role_rank.find(itr->first.roleId);
				ReceiveWorldBossReward(itr->first.userId, itr->first.roleId, itr->second.hurt, result == role_rank.end() ? 0 : result->second, itr->first.roleId == m_killRoleId);
			}
		}
	}

	// 活动已开始huozhe今天已进行过了
	if (m_isOpen == TRUE || m_lastActivityTime == lcTime.wDay)
	{
		return;
	}

	// 开场公告
	LimitedTimeActivityCfg::SActivityConfig notice_config;
	if (GetGameCfgFileMan()->GetLimitedTimeActivityCfg(LimitedTimeActivityCfg::WORLD_BOSS_NOTICE, notice_config))
	{
		if (notice_config.IsOpen)
		{
			int start_minute = notice_config.StartTimeHour * 60 + notice_config.StartTimeMinute;
			int end_minute = notice_config.EndTimeHour * 60 + notice_config.EndTimeHour;
			int current_minute = lcTime.wHour * 60 + lcTime.wMinute;

			// 推送公告
			if (current_minute >= start_minute && current_minute <= end_minute)
			{
				if (m_lastSendNoTiceTime == INVALID_MINUTE || (notice_config.Interval != 0 && (current_minute - m_lastSendNoTiceTime) >= notice_config.Interval))
				{
					GetSystemNoticeMan()->BroadcastSysyemNoticeGB2312(notice_config.Desc.c_str());
					m_lastSendNoTiceTime = current_minute;
				}
			}
			else
			{
				m_lastSendNoTiceTime = INVALID_MINUTE;
			}
		}
	}

	// 检测活动是否开启
	LimitedTimeActivityCfg::SActivityConfig config;
	if (GetGameCfgFileMan()->GetLimitedTimeActivityCfg(LimitedTimeActivityCfg::WORLD_BOSS, config))
	{
		if (config.IsOpen)
		{
			int start_minute = config.StartTimeHour * 60 + config.StartTimeMinute;
			int end_minute = config.EndTimeHour * 60 + config.EndTimeHour;
			int current_minute = lcTime.wHour * 60 + lcTime.wMinute;

			// 开启世界Boss
			if (current_minute >= start_minute && current_minute <= end_minute)
			{
				// 初始化击杀者
				m_killRoleId = 0;

				// 初始化Boos血量
				m_blood = GetGameCfgFileMan()->GetWorldBossBlood();

				// 初始化排行榜信息
				{
					CXTAutoLock lock(m_locker);
					m_rankInfos.clear();
					m_roleInfos.clear();
				}
				m_isOpen = TRUE;
				m_isEnd = FALSE;
				m_lastActivityTime = lcTime.wDay;
			}
		}
	}
}

// 进入世界Boss副本
int WorldBossMan::EnterWorldBossInst(UINT32 userId, UINT32 roleId)
{
	// 副本是否已开启
	if (m_isOpen == FALSE)
	{
		return MLS_NOT_OPEN_WORLD_BOSS;
	}

	// 获取复活时间
	BYTE reviveID = GetGameCfgFileMan()->GetGameTypeReviveRule(ESGS_INST_WORLD_BOSS);
	UINT32 interval = GetGameCfgFileMan()->GetReviveCDTimes(reviveID);

	// 插入玩家
	{
		CXTAutoLock lock(m_locker);
		SAccountInfo roleInfo(userId, roleId);
		std::map<SAccountInfo, SRoleAction>::iterator itr = m_roleInfos.find(roleInfo);
		if (itr == m_roleInfos.end())
		{
			m_roleInfos.insert(std::make_pair(roleInfo, SRoleAction()));
		}
		else
		{
			// 判断是否已经复活
			if (itr->second.death_time != 0)
			{
				if (interval != 0)
				{
					if ((GetMsel() - itr->second.death_time) / 1000 < interval)
					{
						return MLS_INVALID_REVIVE;
					}
				}
				itr->second.death_time = 0;
			}
		}
	}
	return MLS_OK;
}

// 攻击世界Boss
int WorldBossMan::AttackWorldBoss(UINT32 userId, UINT32 roleId, UINT32 hurt, SAttackWorldBossRes *pAttackWorldBoss)
{
	// Boss已死亡
	if (m_blood == 0)
	{
		pAttackWorldBoss->boss_blood = 0;
		pAttackWorldBoss->kill_roleId = m_killRoleId;
		return MLS_OK;
	}

	{
		// 获取复活时间
		BYTE reviveID = GetGameCfgFileMan()->GetGameTypeReviveRule(ESGS_INST_WORLD_BOSS);
		UINT32 interval = GetGameCfgFileMan()->GetReviveCDTimes(reviveID);

		UINT32 hurt_sum = 0;
		CXTAutoLock lock(m_locker);
		{
			std::map<SAccountInfo, SRoleAction>::iterator itr = m_roleInfos.find(SAccountInfo(userId, roleId));
			if (itr == m_roleInfos.end())
			{
				return MLS_NOT_ENTER_INST;
			}

			// 判断是否已经复活
			if (itr->second.death_time != 0)
			{
				if (interval != 0)
				{
					if ((GetMsel() - itr->second.death_time) / 1000 < interval)
					{
						return MLS_INVALID_REVIVE;
					}
				}
				itr->second.death_time = 0;
			}

			// 是否击杀Boss
			bool kill_boss = m_blood <= hurt;
			if (kill_boss)
			{
				itr->second.hurt += m_blood;
				hurt_sum = itr->second.hurt;
				m_blood = 0;
				m_killRoleId = roleId;

				char nickName[33];
				GetRolesInfoMan()->GetRoleNick(userId, roleId, nickName);
				m_killNickName = nickName;
			}
			else
			{
				itr->second.hurt += hurt;
				m_blood -= hurt;
				hurt_sum = itr->second.hurt;
			}
		}

		// 更新排行榜
		{
			RankVector::iterator roleItr = std::find_if(m_rankInfos.begin(), m_rankInfos.end(), SFindRankPred(SAccountInfo(userId, roleId)));
			if (roleItr != m_rankInfos.end())
			{
				roleItr->hurt_sum = hurt_sum;
			}
			else if (m_rankInfos.size() < 5)
			{
				char nickName[33];
				GetRolesInfoMan()->GetRoleNick(userId, roleId, nickName);
				m_rankInfos.resize(m_rankInfos.size() + 1);
				m_rankInfos.back().nick = nickName;
				m_rankInfos.back().hurt_sum = hurt_sum;
				m_rankInfos.back().user.userId = userId;
				m_rankInfos.back().user.roleId = roleId;
			}
			else
			{
				RankVector::iterator minHurtItr = std::min_element(m_rankInfos.begin(), m_rankInfos.end(), SMinRankPred());
				if (hurt_sum > minHurtItr->hurt_sum)
				{
					char nickName[33];
					GetRolesInfoMan()->GetRoleNick(userId, roleId, nickName);
					minHurtItr->nick = nickName;
					minHurtItr->hurt_sum = hurt_sum;
					minHurtItr->user.userId = userId;
					minHurtItr->user.roleId = roleId;
				}
			}
		}

		pAttackWorldBoss->hurt_sum = hurt_sum;
		pAttackWorldBoss->kill_roleId = m_killRoleId;
		pAttackWorldBoss->boss_blood = m_blood;
	}

	if (m_blood == 0)
	{
		m_isEnd = TRUE;
	}

	return MLS_OK;
}

// 查询排行榜
int WorldBossMan::QueryWorldBossHurtRank(UINT32 userId, UINT32 roleId, SQueryWorldBossRankRes *pWorldBossRank)
{
	RankVector rankVec;
	{
		// 基础信息赋值
		CXTAutoLock lock(m_locker);
		pWorldBossRank->boss_blood = m_blood;
		pWorldBossRank->kill_roleId = m_killRoleId;
		pWorldBossRank->number = m_rankInfos.size();

		// 拷贝排行榜
		rankVec = m_rankInfos;
	}

	// 查询排行信息
	pWorldBossRank->num = 0;
	std::sort(rankVec.begin(), rankVec.end(), SRankSortPred());
	for (size_t i = 0; i < rankVec.size(); ++i)
	{
		pWorldBossRank->data[i].id = rankVec[i].user.roleId;
		pWorldBossRank->data[i].hurt_sum = rankVec[i].hurt_sum;
		memcpy(pWorldBossRank->data[i].nick, rankVec[i].nick.c_str(), sizeof(pWorldBossRank->data[i].nick));
		++pWorldBossRank->num;
	}

	return MLS_OK;
}

// 被世界Boss杀死
int WorldBossMan::WasKilledByWorldBoss(UINT32 userId, UINT32 roleId)
{
	// 副本是否已开启
	if (m_isOpen == FALSE)
	{
		return MLS_NOT_OPEN_WORLD_BOSS;
	}

	CXTAutoLock lock(m_locker);
	std::map<SAccountInfo, SRoleAction>::iterator itr = m_roleInfos.find(SAccountInfo(userId, roleId));
	if (itr == m_roleInfos.end())
	{
		return MLS_NOT_ENTER_INST;
	}
	itr->second.death_time = GetMsel();
	return MLS_OK;
}

// 角色复活
int WorldBossMan::ResurrectionInWorldBoss(UINT32 userId, UINT32 roleId, SResurrectionInWorldBossRes *pResurrectionInWorldBos)
{
	// 副本是否已开启
	if (m_isOpen == FALSE)
	{
		return MLS_NOT_OPEN_WORLD_BOSS;
	}

	BYTE reviveId = GetGameCfgFileMan()->GetGameTypeReviveRule(ESGS_INST_WORLD_BOSS);
	UINT32 interval = GetGameCfgFileMan()->GetReviveCDTimes(reviveId);

	CXTAutoLock lock(m_locker);
	std::map<SAccountInfo, SRoleAction>::iterator itr = m_roleInfos.find(SAccountInfo(userId, roleId));
	if (itr == m_roleInfos.end()) return MLS_NOT_ENTER_INST;

	// 没有死亡/没有冷却时间
	if (itr->second.death_time == 0 || interval == 0 || ((GetMsel() - itr->second.death_time) / 1000 > interval))
	{
		SRoleInfos role_info;
		int retCode = GetRolesInfoMan()->GetRoleInfo(userId, roleId, &role_info);
		if (MLS_OK != retCode) return retCode;
		itr->second.death_time = 0;
		pResurrectionInWorldBos->rpcoin = role_info.cash;
		pResurrectionInWorldBos->consume_count = itr->second.consume_count;
		return MLS_OK;
	}

	// 复活冷却时间
	int retCode = GetRolesInfoMan()->ResurrectionInWorldBoss(userId, roleId, reviveId, itr->second.consume_count, pResurrectionInWorldBos->rpcoin);
	if (retCode != MLS_OK) return retCode;
	itr->second.death_time = 0;
	pResurrectionInWorldBos->consume_count = ++itr->second.consume_count;
	return MLS_OK;
}

// 查询世界Boss信息
int WorldBossMan::QueryWorldBossInfo(UINT32 userId, UINT32 roleId, SQueryWorldBossInfoRes *pQueryWorldBossInfo)
{
	// 副本是否已开启
	if (m_isOpen == FALSE)
	{
		return MLS_NOT_OPEN_WORLD_BOSS;
	}

	// 初始化
	pQueryWorldBossInfo->consume_count = 0;
	pQueryWorldBossInfo->resurrection_time = 0;

	// 获取复活时间
	BYTE reviveID = GetGameCfgFileMan()->GetGameTypeReviveRule(ESGS_INST_WORLD_BOSS);
	UINT32 interval = GetGameCfgFileMan()->GetReviveCDTimes(reviveID);

	CXTAutoLock lock(m_locker);
	std::map<SAccountInfo, SRoleAction>::iterator itr = m_roleInfos.find(SAccountInfo(userId, roleId));
	if (itr == m_roleInfos.end())
	{
		return MLS_OK;
	}

	// 消费复活次数
	pQueryWorldBossInfo->consume_count = itr->second.consume_count;

	// 判断是否已经复活
	if (itr->second.death_time != 0)
	{
		if (interval != 0)
		{
			UINT32 seconds = (GetMsel() - itr->second.death_time) / 1000;
			pQueryWorldBossInfo->resurrection_time = seconds > interval ? 0 : interval - seconds;
		}
		else
		{
			pQueryWorldBossInfo->resurrection_time = 0;
		}
	}

	return MLS_OK;
}