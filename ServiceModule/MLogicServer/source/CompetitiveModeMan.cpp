#include "CompetitiveModeMan.h"

#include <fstream>
#include "DBSClient.h"
#include "RolesInfoMan.h"
#include "ServiceErrorCode.h"
#include "ConfigFileMan.h"
#include "GameCfgFileMan.h"
#include "EmailCleanMan.h"

static CompetitiveModeMan* sg_pCompetitiveModeMan = NULL;

void InitCompetitiveModeMan()
{
	ASSERT(sg_pCompetitiveModeMan == NULL);
	sg_pCompetitiveModeMan = new CompetitiveModeMan();
	ASSERT(sg_pCompetitiveModeMan != NULL);
	sg_pCompetitiveModeMan->Start();
}

CompetitiveModeMan* GetCompetitiveModeMan()
{
	return sg_pCompetitiveModeMan;
}

void DestroyCompetitiveModeMan()
{
	CompetitiveModeMan* pCompetitiveModeMan = sg_pCompetitiveModeMan;
	sg_pCompetitiveModeMan = NULL;
	if (pCompetitiveModeMan != NULL)
	{
		pCompetitiveModeMan->Stop();
		delete pCompetitiveModeMan;
	}
}

CompetitiveModeMan::CompetitiveModeMan()
{
	std::ifstream file;
	file.open("competitive.cache", std::ios::binary);
	if (file.is_open())
	{
		m_weekFreeActorType = file.get();
		file.close();
	}
	else
	{
		file.close();
		GeneraWeekFreeActorType();
	}
}

CompetitiveModeMan::~CompetitiveModeMan()
{
}

void CompetitiveModeMan::Start()
{
	DWORD n = GetConfigFileMan()->GetRefreshDBInterval();
	m_timer.Start(CompetitiveModeTimer, this, n * 1000);
}

void CompetitiveModeMan::Stop()
{
	m_timer.Stop();
}

void CompetitiveModeMan::CompetitiveModeTimer(void* param, TTimerID id)
{
	CompetitiveModeMan* pThis = (CompetitiveModeMan*)param;
	ASSERT(pThis != NULL);
	if (pThis)
		pThis->OnFinishCompetitiveMode();
}

void CompetitiveModeMan::OnFinishCompetitiveMode()
{
	CXTAutoLock lock(m_locker);
	for (size_t i = 0; i < m_finishCompetitive.size(); ++i)
	{
		OnRefreshDBFinishCompetitiveMode(m_finishCompetitive[i]);
	}
	m_finishCompetitive.clear();
}

void CompetitiveModeMan::OnRefreshDBFinishCompetitiveMode(UINT32 roleId)
{
	UINT32 affectRows = 0;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "UPDATE competitive SET win=win+1 WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, &affectRows);
	if (retCode != DBS_OK)
		return;

	if (affectRows == 0)
	{
		sprintf(szSQL, "INSERT INTO competitive VALUES(%u, 1)", roleId);
		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}
}

// 每日清理
void CompetitiveModeMan::ClearEveryday()
{
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), "UPDATE competitive SET num=0,buyNum=0");
}

// 获取周免角色
BYTE CompetitiveModeMan::GetWeekFreeActorType() const
{
	return m_weekFreeActorType;
}

// 生成周免角色
void CompetitiveModeMan::GeneraWeekFreeActorType()
{
	m_weekFreeActorType = GetGameCfgFileMan()->GeneraWeekFreeActorType();

	// 写入文件记录
	std::ofstream file;
	file.open("competitive.cache", std::ios::binary);
	file.put(m_weekFreeActorType);
	file.close();
}

// 查询竞技模式进入次数
int CompetitiveModeMan::QueryEnterNum(UINT32 userId, UINT32 roleId, SQueryCompEnterNumRes *pEnterNum)
{
	pEnterNum->buy_num = 0;
	pEnterNum->enter_num = 0;

	// 查询已进入次数
	DBS_ROW row_data = NULL;
	DBS_RESULT result = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT num,buyNum FROM competitive WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;
	if ((row_data = DBS_FetchRow(result)) != NULL)
	{
		pEnterNum->enter_num = atoi(row_data[0]);
		pEnterNum->buy_num = atoi(row_data[1]);
	}
	DBS_FreeResult(result);

	return MLS_OK;
}

// 购买竞技模式进入次数
int CompetitiveModeMan::BuyEnterNum(UINT32 userId, UINT32 roleId, BYTE mode, SBuyCompEnterNumRes *pEnterNum)
{
	pEnterNum->rpcoin = 0;
	pEnterNum->buy_num = 0;

	// 查询已购买次数
	int buy_num = 0;
	DBS_ROW row_data = NULL;
	DBS_RESULT result = NULL;
	bool need_insert = false;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT buyNum FROM competitive WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;
	if ((row_data = DBS_FetchRow(result)) != NULL)
	{
		buy_num = atoi(row_data[0]);
	}
	else
	{
		need_insert = true;
	}
	DBS_FreeResult(result);

	/**
	* 策划文档原文：
	* 竞技模式的购买次数上限与VIP关联，VIP等级越高可购买次数越多，购买次数上限按照每日可挑战次数一半设计，例如：每日挑战次数10次，最高VIP等级可额外购买挑战次数5次
	*/
	std::set<UINT16> *instSet = NULL;
	if (!GetGameCfgFileMan()->GetInstByInstType(ESGS_INST_COMPETITIVEMODE, instSet))
	{
		ASSERT(false);
		return MLS_ERROR;
	}
	UINT16 instId = *instSet->begin();
	int upper_num = GetGameCfgFileMan()->GetInstEnterNum(instId) / 2;
	if (buy_num >= upper_num)
	{
		return MLS_BUY_ACHIEVE_UPPER;
	}

	// 购买进入次数
	if (mode == 1)
	{
		retCode = GetRolesInfoMan()->BuyCompModeEnterNum(userId, roleId, buy_num, pEnterNum);
	}
	else
	{
		retCode = GetRolesInfoMan()->BuyAllCompModeEnterNum(userId, roleId, buy_num, upper_num, pEnterNum);
	}
	if (retCode != MLS_OK)
		return retCode;

	// 更新数据
	if (need_insert)
	{
		sprintf(szSQL, "INSERT INTO competitive VALUES(%u,0,0,%u)", roleId, pEnterNum->buy_num);
		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}
	else
	{
		sprintf(szSQL, "UPDATE competitive SET buyNum=%u WHERE actorid=%u", pEnterNum->buy_num, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}

	return MLS_OK;
}

// 进入竞技模式
int CompetitiveModeMan::EnterCompetitiveMode(UINT32 userId, UINT32 roleId)
{
	int enter_num = 0;
	bool need_insert = false;
	std::set<UINT16> *instSet = NULL;
	if (!GetGameCfgFileMan()->GetInstByInstType(ESGS_INST_COMPETITIVEMODE, instSet))
	{
		ASSERT(false);
		return MLS_ERROR;
	}
	UINT16 instId = *instSet->begin();
	int upper_num = GetGameCfgFileMan()->GetInstEnterNum(instId);

	// 查询已进入次数
	DBS_ROW row_data = NULL;
	DBS_RESULT result = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT num,buyNum FROM competitive WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;
	if ((row_data = DBS_FetchRow(result)) != NULL)
	{
		enter_num = atoi(row_data[0]);
		upper_num += atoi(row_data[1]);
	}
	else
	{
		need_insert = true;
	}
	DBS_FreeResult(result);

	// 是否达到进入上限
	if (enter_num >= upper_num)
	{
		return MLS_ENTER_COMPMODE_UPPER;
	}

	// 写入记录
	++enter_num;
	if (need_insert)
	{
		sprintf(szSQL, "INSERT INTO competitive VALUES(%u,0,%d,0)", roleId, enter_num);
		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}
	else
	{
		sprintf(szSQL, "UPDATE competitive SET num=%d WHERE actorid=%u", enter_num, roleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}

	return MLS_OK;
}

// 完成竞技模式
int CompetitiveModeMan::FinishCompetitiveMode(UINT32 userId, UINT32 roleId)
{
	CXTAutoLock lock(m_locker);
	m_finishCompetitive.push_back(roleId);
	return MLS_OK;
}

// 发放竞技模式奖励
int CompetitiveModeMan::GrantCompetitiveModeReward()
{
	DBS_ROW row_data = NULL;
	DBS_RESULT result = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT id,accountid,actorid,rank FROM rankcomp WHERE competitive>0 AND TO_DAYS(ranktts)=TO_DAYS(CURRENT_DATE) AND msign=1 AND award=0");
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	while ((row_data = DBS_FetchRow(result)) != NULL)
	{
		UINT32 id = atol(row_data[0]);
		ReceiveCompetitiveModeReward(atol(row_data[1]), atol(row_data[2]), atol(row_data[3]));
		sprintf(szSQL, "UPDATE rankcomp SET award=1 WHERE id=%u", id);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}
	DBS_FreeResult(result);

	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), "UPDATE competitive SET win=0");

	return MLS_OK;
}

// 领取竞技模式奖励
void CompetitiveModeMan::ReceiveCompetitiveModeReward(UINT32 userId, UINT32 roleId, UINT32 rank)
{
	const std::vector<GameTypeCfg::SRewardInfo> *rankingVec = NULL;
	std::vector<GameTypeCfg::SRewardCondition> *conditionVec_ptr = NULL;
	if (GetGameCfgFileMan()->GetConditionAndReward(ESGS_INST_COMPETITIVEMODE, conditionVec_ptr))
	{
		std::vector<GameTypeCfg::SRewardCondition> &conditionVec = *conditionVec_ptr;
		for (std::vector<GameTypeCfg::SRewardCondition>::const_iterator itr = conditionVec.begin(); itr != conditionVec.end(); ++itr)
		{
			if (itr->type == ESGS_CONDITION_COMPETITIVE_RANKING)
			{
				if (rank <= itr->num)
				{
					rankingVec = &itr->reward;
				}
			}
		}
	}

	// 发送排名奖励
	if (rankingVec != NULL && !rankingVec->empty())
	{
		char szBody[MAX_UTF8_EMAIL_BODY_LEN] = { 0 };
		sprintf(szBody, "恭喜你在上周竞技模式中夺得第%u名，特此发放奖励。", rank);
		std::vector<GameTypeCfg::SRewardInfo> rewards;
		GetGameCfgFileMan()->UnlockRewardNesting(*rankingVec, &rewards);
		SendRewardEmail(userId, roleId, rewards, szBody);
	}
}

// 发送奖励邮件
void CompetitiveModeMan::SendRewardEmail(UINT32 userId, UINT32 roleId, const std::vector<GameTypeCfg::SRewardInfo> &rewardVec, const char *body)
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
		email.type = ESGS_EMAIL_COMPETITIVE_REWARD;
		email.pGBSenderNick = "系统";
		email.pGBCaption = "竞技模式排名奖励";
		email.pGBBody = body;
		if (propVec.size() > 0)
		{
			email.attachmentsNum = propVec.size();
			email.pAttachments = propVec.data();
		}
		GetEmailCleanMan()->AsyncSendEmail(email);
	}
}