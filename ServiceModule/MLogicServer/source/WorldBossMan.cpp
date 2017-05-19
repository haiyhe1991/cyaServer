#include "WorldBossMan.h"

#include <algorithm>
#include "gb_utf8.h"
#include "DBSClient.h"
#include "ServiceErrorCode.h"
#include "RolesInfoMan.h"
#include "ConfigFileMan.h"
#include "SystemNoticeMan.h"
#include "EmailCleanMan.h"

// ��Ч������
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

// ���ͽ����ʼ�
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
			case ESGS_PROP_EQUIP:		//װ��
			case ESGS_PROP_GEM:			//��ʯ
			case ESGS_PROP_MATERIAL:	//����
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
			case ESGS_PROP_GOLD:		//��Ϸ��
			{
											gold = SGSU64Add(gold, rewardVec[i].num);
											break;
			}
			case ESGS_PROP_TOKEN:		//����
			{
											rpcoin = SGSU32Add(rpcoin, rewardVec[i].num);
											break;
			}
			case ESGS_PROP_VIT:			//����
			{
											vit = SGSU32Add(vit, rewardVec[i].num);
											break;
			}
			case ESGS_PROP_EXP:			//����
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
		email.pGBSenderNick = "ϵͳ";
		email.pGBCaption = "�ַ�����Boss����";
		email.pGBBody = body;
		if (propVec.size() > 0)
		{
			email.attachmentsNum = propVec.size();
			email.pAttachments = propVec.data();
		}
		GetEmailCleanMan()->AsyncSendEmail(email);
	}
}

// ��ȡ����Boss����
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
				//case ESGS_CONDITION_HURT_EVERY:		// ÿ����˺�
				//	{
				//		if (hurtValue >= itr->num)
				//		{
				//			involvedVec = &itr->reward;
				//		}
				//		break;
				//	}
			case ESGS_CONDITION_RANKING:		// �������
			{
													if (rank == itr->num)
													{
														rankingVec = &itr->reward;
													}
													break;
			}
			case ESGS_CONDITION_LAST_STRAW:		// ���һ��
			{
													if (kill)
													{
														killVec = &itr->reward;
													}
													break;
			}
			case ESGS_CONDITION_HURT_EXTREMELY:	// �˺���ֱ�
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

	// ��ȡϵͳ��ǰʱ��
	SYSTEMTIME lcTime;
	GetLocalTime(&lcTime);
	char szBody[MAX_UTF8_EMAIL_BODY_LEN] = { 0 };
	LimitedTimeActivityCfg::SActivityConfig config;
	GetGameCfgFileMan()->GetLimitedTimeActivityCfg(LimitedTimeActivityCfg::WORLD_BOSS, config);

	// ���Ͳ��뽱
	/*if (involvedVec != NULL && !involvedVec->empty())
	{
	sprintf(szBody, "��л�����%u-%02u-%02u %02u:%02u��%u-%02u-%02u %02u:%02u���ַ�����BOSS��ս�������%u���˺�����ò��뽱����",
	lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.StartTimeHour, config.StartTimeMinute,
	lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.EndTimeHour, config.EndTimeHourMinute,
	hurtValue);
	SendRewardEmail(userId, roleId, *involvedVec, szBody);
	}*/

	// ������������
	if (rankingVec != NULL && !rankingVec->empty())
	{
		sprintf(szBody, "����%u-%02u-%02u %02u:%02u��%u-%02u-%02u %02u:%02u���ַ�����BOSS��ս�й�ѫ׿���������%u���˺�������%u������˵�%u��������",
			lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.StartTimeHour, config.StartTimeMinute,
			lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.EndTimeHour, config.EndTimeHourMinute,
			hurtValue, rank, rank);
		std::vector<GameTypeCfg::SRewardInfo> rewards;
		GetGameCfgFileMan()->UnlockRewardNesting(*rankingVec, &rewards);
		SendRewardEmail(userId, roleId, rewards, szBody);
	}

	// ����β������
	if (killVec != NULL && !killVec->empty())
	{
		sprintf(szBody, "����%u-%02u-%02u %02u:%02u��%u-%02u-%02u %02u:%02u���ַ�����BOSS��ս������ͷ��ɹ���ɱ������BOSS���������BOSSնɱ������",
			lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.StartTimeHour, config.StartTimeMinute,
			lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.EndTimeHour, config.EndTimeHourMinute);
		std::vector<GameTypeCfg::SRewardInfo> rewards;
		GetGameCfgFileMan()->UnlockRewardNesting(*killVec, &rewards);
		SendRewardEmail(userId, roleId, rewards, szBody);
	}

	// �����˺���ֱȽ���
	if (hurtVec != NULL && !hurtVec->empty())
	{
		float proportion = (float)hurtValue / GetGameCfgFileMan()->GetWorldBossBlood() * 10000;
		sprintf(szBody, "��л�����%u-%02u-%02u %02u:%02u��%u-%02u-%02u %02u:%02u���ַ�����BOSS��ս�������%u���˺���ռBossѪ����%u/10000������˺�������",
			lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.StartTimeHour, config.StartTimeMinute,
			lcTime.wYear, lcTime.wMonth, lcTime.wDay, config.EndTimeHour, config.EndTimeHourMinute,
			hurtValue, (UINT32)proportion);
		std::vector<GameTypeCfg::SRewardInfo> rewards;
		GetGameCfgFileMan()->UnlockRewardNesting(*hurtVec, &rewards);
		SendRewardEmail(userId, roleId, rewards, szBody);
	}
}

// ����Ƿ���
void WorldBossMan::OnRefreshWorldBoss()
{
	// ��ȡϵͳ��ǰʱ��
	SYSTEMTIME lcTime;
	GetLocalTime(&lcTime);

	// ���Ž���
	if (m_isOpen == TRUE && m_isEnd == TRUE)
	{
		m_isOpen = FALSE;

		// ��ɱ����
		char format[256] = { 0 };
		char content[256] = { 0 };
		const char *description = "��ʿ%s�ɹ���ɱ������BOSS�����ȫ��Ψһ����BOSS������";
		gb2312_utf8(description, strlen(description), format, sizeof(format));
		sprintf(content, format, m_killNickName.c_str());
		GetSystemNoticeMan()->BroadcastSysyemNoticeUTF8(content);

		// ��������
		GetSystemNoticeMan()->WorldBossDeathNotice();

		// ��������
		std::map<UINT32/*��ɫID*/, UINT32/*����*/> role_rank;
		CXTAutoLock lock(m_locker);
		{
			std::sort(m_rankInfos.begin(), m_rankInfos.end(), SRankSortPred());
			for (size_t i = 0; i < m_rankInfos.size(); ++i)
			{
				role_rank.insert(std::make_pair(m_rankInfos[i].user.roleId, i + 1));
			}

			// ���ͽ���
			for (std::map<SAccountInfo, SRoleAction>::const_iterator itr = m_roleInfos.begin(); itr != m_roleInfos.end(); ++itr)
			{
				std::map<UINT32/*��ɫID*/, UINT32/*����*/>::const_iterator result = role_rank.find(itr->first.roleId);
				ReceiveWorldBossReward(itr->first.userId, itr->first.roleId, itr->second.hurt, result == role_rank.end() ? 0 : result->second, itr->first.roleId == m_killRoleId);
			}
		}
	}

	// ��ѿ�ʼhuozhe�����ѽ��й���
	if (m_isOpen == TRUE || m_lastActivityTime == lcTime.wDay)
	{
		return;
	}

	// ��������
	LimitedTimeActivityCfg::SActivityConfig notice_config;
	if (GetGameCfgFileMan()->GetLimitedTimeActivityCfg(LimitedTimeActivityCfg::WORLD_BOSS_NOTICE, notice_config))
	{
		if (notice_config.IsOpen)
		{
			int start_minute = notice_config.StartTimeHour * 60 + notice_config.StartTimeMinute;
			int end_minute = notice_config.EndTimeHour * 60 + notice_config.EndTimeHour;
			int current_minute = lcTime.wHour * 60 + lcTime.wMinute;

			// ���͹���
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

	// ����Ƿ���
	LimitedTimeActivityCfg::SActivityConfig config;
	if (GetGameCfgFileMan()->GetLimitedTimeActivityCfg(LimitedTimeActivityCfg::WORLD_BOSS, config))
	{
		if (config.IsOpen)
		{
			int start_minute = config.StartTimeHour * 60 + config.StartTimeMinute;
			int end_minute = config.EndTimeHour * 60 + config.EndTimeHour;
			int current_minute = lcTime.wHour * 60 + lcTime.wMinute;

			// ��������Boss
			if (current_minute >= start_minute && current_minute <= end_minute)
			{
				// ��ʼ����ɱ��
				m_killRoleId = 0;

				// ��ʼ��BoosѪ��
				m_blood = GetGameCfgFileMan()->GetWorldBossBlood();

				// ��ʼ�����а���Ϣ
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

// ��������Boss����
int WorldBossMan::EnterWorldBossInst(UINT32 userId, UINT32 roleId)
{
	// �����Ƿ��ѿ���
	if (m_isOpen == FALSE)
	{
		return MLS_NOT_OPEN_WORLD_BOSS;
	}

	// ��ȡ����ʱ��
	BYTE reviveID = GetGameCfgFileMan()->GetGameTypeReviveRule(ESGS_INST_WORLD_BOSS);
	UINT32 interval = GetGameCfgFileMan()->GetReviveCDTimes(reviveID);

	// �������
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
			// �ж��Ƿ��Ѿ�����
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

// ��������Boss
int WorldBossMan::AttackWorldBoss(UINT32 userId, UINT32 roleId, UINT32 hurt, SAttackWorldBossRes *pAttackWorldBoss)
{
	// Boss������
	if (m_blood == 0)
	{
		pAttackWorldBoss->boss_blood = 0;
		pAttackWorldBoss->kill_roleId = m_killRoleId;
		return MLS_OK;
	}

	{
		// ��ȡ����ʱ��
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

			// �ж��Ƿ��Ѿ�����
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

			// �Ƿ��ɱBoss
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

		// �������а�
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

// ��ѯ���а�
int WorldBossMan::QueryWorldBossHurtRank(UINT32 userId, UINT32 roleId, SQueryWorldBossRankRes *pWorldBossRank)
{
	RankVector rankVec;
	{
		// ������Ϣ��ֵ
		CXTAutoLock lock(m_locker);
		pWorldBossRank->boss_blood = m_blood;
		pWorldBossRank->kill_roleId = m_killRoleId;
		pWorldBossRank->number = m_rankInfos.size();

		// �������а�
		rankVec = m_rankInfos;
	}

	// ��ѯ������Ϣ
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

// ������Bossɱ��
int WorldBossMan::WasKilledByWorldBoss(UINT32 userId, UINT32 roleId)
{
	// �����Ƿ��ѿ���
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

// ��ɫ����
int WorldBossMan::ResurrectionInWorldBoss(UINT32 userId, UINT32 roleId, SResurrectionInWorldBossRes *pResurrectionInWorldBos)
{
	// �����Ƿ��ѿ���
	if (m_isOpen == FALSE)
	{
		return MLS_NOT_OPEN_WORLD_BOSS;
	}

	BYTE reviveId = GetGameCfgFileMan()->GetGameTypeReviveRule(ESGS_INST_WORLD_BOSS);
	UINT32 interval = GetGameCfgFileMan()->GetReviveCDTimes(reviveId);

	CXTAutoLock lock(m_locker);
	std::map<SAccountInfo, SRoleAction>::iterator itr = m_roleInfos.find(SAccountInfo(userId, roleId));
	if (itr == m_roleInfos.end()) return MLS_NOT_ENTER_INST;

	// û������/û����ȴʱ��
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

	// ������ȴʱ��
	int retCode = GetRolesInfoMan()->ResurrectionInWorldBoss(userId, roleId, reviveId, itr->second.consume_count, pResurrectionInWorldBos->rpcoin);
	if (retCode != MLS_OK) return retCode;
	itr->second.death_time = 0;
	pResurrectionInWorldBos->consume_count = ++itr->second.consume_count;
	return MLS_OK;
}

// ��ѯ����Boss��Ϣ
int WorldBossMan::QueryWorldBossInfo(UINT32 userId, UINT32 roleId, SQueryWorldBossInfoRes *pQueryWorldBossInfo)
{
	// �����Ƿ��ѿ���
	if (m_isOpen == FALSE)
	{
		return MLS_NOT_OPEN_WORLD_BOSS;
	}

	// ��ʼ��
	pQueryWorldBossInfo->consume_count = 0;
	pQueryWorldBossInfo->resurrection_time = 0;

	// ��ȡ����ʱ��
	BYTE reviveID = GetGameCfgFileMan()->GetGameTypeReviveRule(ESGS_INST_WORLD_BOSS);
	UINT32 interval = GetGameCfgFileMan()->GetReviveCDTimes(reviveID);

	CXTAutoLock lock(m_locker);
	std::map<SAccountInfo, SRoleAction>::iterator itr = m_roleInfos.find(SAccountInfo(userId, roleId));
	if (itr == m_roleInfos.end())
	{
		return MLS_OK;
	}

	// ���Ѹ������
	pQueryWorldBossInfo->consume_count = itr->second.consume_count;

	// �ж��Ƿ��Ѿ�����
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