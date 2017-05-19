#include <fstream>
#include <algorithm>
#include "GameCfgFileMan.h"
#include "ConfigFileMan.h"
#include "cyaLog.h"
#include "cyaFdk.h"
#include "moduleInfo.h"
#include "ServiceErrorCode.h"
#include "cyaMaxMin.h"

#define INIT_GOLD_NUM  10000		//初始化金币
#define INIT_TOKEN_NUM 1000			//初始化代币


#define LOAD_NOW(str)	printf("开始加载配置文件:%s!\n",str);

static GameCfgFileMan* sg_gameCfgMan = NULL;
BOOL InitGameCfgFileMan()
{
	ASSERT(sg_gameCfgMan == NULL);
	sg_gameCfgMan = new GameCfgFileMan();
	ASSERT(sg_gameCfgMan != NULL);
	return sg_gameCfgMan->LoadGameCfgFile();
}

GameCfgFileMan* GetGameCfgFileMan()
{
	return sg_gameCfgMan;
}

void DestroyGameCfgFileMan()
{
	GameCfgFileMan* gameCfgMan = sg_gameCfgMan;
	sg_gameCfgMan = NULL;
	if (gameCfgMan != NULL)
		delete gameCfgMan;
}

GameCfgFileMan::GameCfgFileMan()
: m_instCfg(&m_gameDropCfg, &m_monsterCfg)
, m_taskCfg(&m_chapterCfg)
{
	srand((unsigned int)time(NULL));	//生成随机数种子
}

GameCfgFileMan::~GameCfgFileMan()
{

}

BOOL GameCfgFileMan::LoadGameCfgFile()
{
	char pszFilePath[MAX_PATH] = { 0 };
	char pszFilePath2[MAX_PATH] = { 0 };
	char pszFilePath3[MAX_PATH] = { 0 };
	const char* pszCfgFileDir = GetConfigFileMan()->GetGameCfgPath();

	//游戏全局配置
	try
	{

		MergeFileName(pszCfgFileDir, "game.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!LoadGameGlobalCfg(pszFilePath))
		{
			LogInfo(("载入游戏全局配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入游戏全局配置成功!"));

		//公告配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Notice.json", pszFilePath);
		LOAD_NOW(pszFilePath)
			m_noticeCfg.Load(pszFilePath);
		LogInfo(("载入游戏公告配置成功!"));

		//游戏章节配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Chapter.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_chapterCfg.Load(pszFilePath))
		{
			LogInfo(("载入游戏章节配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入游戏章节配置成功!"));

		//角色配置
		LOAD_NOW("角色属性配置")
		if (!m_roleCfg.Load(pszCfgFileDir))
		{
			LogInfo(("载入角色配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入角色配置成功!"));

		//技能配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		memset(pszFilePath2, 0, sizeof(pszFilePath2));
		MergeFileName(pszCfgFileDir, "Role.skill", pszFilePath);
		MergeFileName(pszCfgFileDir, "Role.combo", pszFilePath2);
		MergeFileName(pszCfgFileDir, "RoleEva.skill", pszFilePath3);
		LOAD_NOW("Role.skill Role.combo RoleEva.skill")
		if (!m_skillCfg.Load(pszFilePath, pszFilePath2, pszFilePath3))
		{
			LogInfo(("载入技能配置失败!"));
			return false;
		}
		LogInfo(("载入技能配置成功!"));

		//游戏落配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "GameDrop.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_gameDropCfg.Load(pszFilePath))
		{
			LogInfo(("载入游戏落配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入游戏落配置成功!"));

		//装备合成配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Compound.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_equipComposeCfg.Load(pszFilePath))
		{
			LogInfo(("载入装备合成配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入装备合成配置成功!"));

		//怪配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "MonsterNew.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_monsterCfg.Load(pszFilePath))
		{
			LogInfo(("载入怪配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入怪配置成功!"));

		//副本配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Stage.json", pszFilePath);
		m_instCfg.SetParamValue(m_goldBranchThrehold, m_goldBranchMaxPercent, m_dropLv1ToBossPercent, m_dropLv1ToCreamPercent);
		LOAD_NOW(pszFilePath)
		if (!m_instCfg.LoadInst(pszFilePath))
		{
			LogInfo(("载入副本配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入副本配置成功!"));

		//单人随机副本配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "SingleRandStage.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_instCfg.LoadRandInst(pszFilePath))
		{
			LogInfo(("载入单人随机副本配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入单人随机副本配置成功!"));

		//任务配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		memset(pszFilePath2, 0, sizeof(pszFilePath2));
		MergeFileName(pszCfgFileDir, "Mission.json", pszFilePath);
		MergeFileName(pszCfgFileDir, "EverydayTasks.z", pszFilePath2);
		LOAD_NOW("Mission.json,EverydayTasks.z")
		if (!m_taskCfg.Load(pszFilePath, pszFilePath2))
		{
			LogInfo(("载入任务配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入任务配置成功!"));

		//装备配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Equipment.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_equipCfg.Load(pszFilePath))
		{
			LogInfo(("载入职业装备配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入职业装备配置成功!"));

		//装备强化配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Strengthen.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_strengthenCfg.Load(pszFilePath))
		{
			LogInfo(("载入装备强化配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入装备强化配置成功!"));

		//装备升星配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "startUpgrade.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_riseStarCfg.Load(pszFilePath))
		{
			LogInfo(("载入装备升星配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入装备升星配置成功!"));

		//装备品质配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "quality.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_qualityCfg.Load(pszFilePath))
		{
			LogInfo(("载入装备品质配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入装备品质配置成功!"));

		//宝石
		memset(pszFilePath, 0, sizeof(pszFilePath));
		memset(pszFilePath2, 0, sizeof(pszFilePath2));
		memset(pszFilePath3, 0, sizeof(pszFilePath3));
		MergeFileName(pszCfgFileDir, "gem.json", pszFilePath);
		MergeFileName(pszCfgFileDir, "gemInlayCost.json", pszFilePath2);
		MergeFileName(pszCfgFileDir, "equipmentGemHole.json", pszFilePath3);
		LOAD_NOW("gem.json,gemInlayCost.json,equipmentGemHole.json")
		if (!m_gemCfg.Load(pszFilePath, pszFilePath2, pszFilePath3))
		{
			LogInfo(("载入宝石配置失败!"));
			return false;
		}
		LogInfo(("载入宝石配置成功!"));

		//材料配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Materials.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_materialCfg.Load(pszFilePath))
		{
			LogInfo(("载入材料配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入材料配置成功!"));

		//碎片配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Fragment.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_fragmentCfg.Load(pszFilePath))
		{
			LogInfo(("载入碎片配置[%s]失败!", pszFilePath));
			return false;
		}
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "FragmentGiving.json", pszFilePath);
		m_fragmentCfg.LoadGiveFragment(pszFilePath);
		LogInfo(("载入碎片配置成功!"));

		//商城配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Mall.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_mallCfg.LoadMall(pszFilePath))
		{
			LogInfo(("载入商城配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入商城配置成功!"));

		//时装配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Fashion.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_fashionCfg.Load(pszFilePath))
		{
			LogInfo(("载入时装配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入时装配置成功!"));

		//充值配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Recharge.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_rechargeCfg.Load(pszFilePath))
		{
			LogInfo(("载入充值配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入充值配置成功!"));

		//称号配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Title.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_titleCfg.Load(pszFilePath))
		{
			LogInfo(("载入称号配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入称号配置成功!"));

		//邮件赠送配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "RoleInitEmail.json", pszFilePath);
		LOAD_NOW(pszFilePath)
			m_emailCfg.Load(pszFilePath);

		//角色初始化赠送配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "RoleInitGive.json", pszFilePath);
		LOAD_NOW(pszFilePath)
			m_newRoleGiveCfg.Load(pszFilePath);

		//单人随机副本
		/*memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "SingleStageSystem.json", pszFilePath);
		if(!m_randInstSysCfg.Load(pszFilePath))
		{
		LogInfo(("载入随机副本系统配置[%s]失败!", pszFilePath));
		return false;
		}
		LogInfo(("载入随机副本系统配置成功!"));*/

		//套装配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Suit.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_suitCfg.Load(pszFilePath))
		{
			LogInfo(("载入套装配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入套装配置成功!"));

		//复活配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Revive.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_reviveCfg.Load(pszFilePath))
		{
			LogInfo(("载入复活配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入复活配置成功!"));

		//金币兑换配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "GoldExchange.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_goldExchangeCfg.Load(pszFilePath))
		{
			LogInfo(("载入金币兑换配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入金币兑换配置成功!"));

		//VIP配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Vip.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_vipCfg.Load(pszFilePath))
		{
			LogInfo(("载入VIP配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入VIP配置成功!"));

		//签到配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "SignIn.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_signInCfg.Load(pszFilePath))
		{
			LogInfo(("载入签到配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入签到配置成功!"));

		//活动配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Activity.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_activityCfg.Load(pszFilePath))
		{
			LogInfo(("载入活动配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入活动配置成功!"));

		//解锁配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Lock.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_unlockCfg.Load(pszFilePath))
		{
			LogInfo(("载入解锁配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入解锁配置成功!"));

		//礼包配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "GiftBag.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_giftBagCfg.Load(pszFilePath, true))
		{
			LogInfo(("载入礼包配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入礼包配置成功!"));

		//轮盘配置
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Roulette.z", pszFilePath);
		LOAD_NOW(pszFilePath)
			m_rouletteCfg.Load(pszFilePath);

		/* zpy 成就系统新增 */
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "Achievement.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_achievementCfg.Load(pszFilePath))
		{
			LogInfo(("载入成就配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入成就配置成功!"));

		/* zpy 体力兑换 */
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "PowerExchange.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_vitExchangeCfg.Load(pszFilePath))
		{
			LogInfo(("载入体力兑换配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入体力兑换配置成功!"));

		/* zpy 每日签到 */
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "EDSignIn.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_dailySignCfg.Load(pszFilePath))
		{
			LogInfo(("载入每日签到配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入每日签到配置成功!"));

		/* zpy 炼魂碎片 */
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "HolyDebris.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_chainSoulFragmentCfg.Load(pszFilePath))
		{
			LogInfo(("载入炼魂碎片配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入炼魂碎片配置成功!"));

		/* zpy 炼魂系统 */
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "GreedTheLive.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_greedTheLiveCfg.Load(pszFilePath))
		{
			LogInfo(("载入炼魂系统配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入炼魂系统配置成功!"));


		//add by hxw 20151013
		//memset(pszFilePath, 0, sizeof(pszFilePath));
		{
			//1 等级排行   2 刷图排行  3 CP领取   4 time领取
			std::string RankRewardCfg_FILE_NAME[RANK_REWARD_TYPE_NUM] = { "LvRanking.z", "EctypeRanking.z", "CapacityRanking.z", "OnLine.z" };
			std::vector<std::string> vNames;
			for (int i = 0; i<RANK_REWARD_TYPE_NUM; i++)
			{
				memset(pszFilePath, 0, sizeof(pszFilePath));
				MergeFileName(pszCfgFileDir, RankRewardCfg_FILE_NAME[i].c_str(), pszFilePath);
				vNames.push_back(pszFilePath);
			}
			LOAD_NOW("LvRanking.z,EctypeRanking.z,CapacityRanking.z,OnLine.z")
			if (!m_rankRewardCfg.Load(vNames))
			{
				LogInfo(("载入排行奖励配置[%s]失败!", pszFilePath));
				return false;
			}
			LogInfo(("载入排行奖励配置成功!"));
		}
		//end


		/* zpy 限时活动 */
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "SingleStageSystem.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_limitedTimeActivity.Load(pszFilePath))
		{
			LogInfo(("载入限时活动配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入限时活动配置成功!"));

		/* zpy 关卡玩法 */
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "GameType.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_gameTypeCfg.Load(pszFilePath))
		{
			LogInfo(("载入关卡玩法配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入关卡玩法配置成功!"));

		/* zpy 怪物属性配置 */
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "MonsterAttri.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_monsterAttributeCfg.Load(pszFilePath))
		{
			LogInfo(("载入怪物属性配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入怪物属性配置成功!"));

		/* zpy 怪物类型配置 */
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "MonsterType.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_monsterTypeCfg.Load(pszFilePath))
		{
			LogInfo(("载入怪物类型配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入怪物类型配置成功!"));

		/* zpy 装备位属性配置 */
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "StrengthenAttri.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_strengthenAttriCfg.Load(pszFilePath))
		{
			LogInfo(("载入装备位属性配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入装备位属性配置成功!"));

		/* zpy 角色属性配置 */
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "PlayerConfig.json", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_playerCfg.Load(pszFilePath))
		{
			LogInfo(("载入角色属性配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入角色属性配置成功!"));

		/* zpy 活动副本配置 */
		memset(pszFilePath, 0, sizeof(pszFilePath));
		MergeFileName(pszCfgFileDir, "ActivityStage.z", pszFilePath);
		LOAD_NOW(pszFilePath)
		if (!m_activityStageCfg.Load(pszFilePath))
		{
			LogInfo(("载入活动副本配置[%s]失败!", pszFilePath));
			return false;
		}
		LogInfo(("载入活动副本配置成功!"));
	}
	catch (UINT16 id)
	{
		printf("\n\n %s Error!!!!!!\n id is=%d\n\n", pszFilePath, id);
		return false;
	}
	catch (UINT32 id)
	{
		printf("\n\n %s Error!!!!!!\n id is=%d\n\n", pszFilePath, id);
		return false;
	}
	catch (UINT8 id)
	{
		printf("\n\n %s Error!!!!!!\n id is=%d\n\n", pszFilePath, id);
		return false;
	}
	catch (...)
	{
		printf("\n\n %s Error!!!!!!\n\n", pszFilePath);
		return false;
	}


	return true;
}

BOOL GameCfgFileMan::OpenCfgFile(const char* filename, Json::Value& rootValue)
{
	std::ifstream ifs;
#if defined(WIN32)
	WCHAR wszFilePath[MAX_PATH] = { 0 };
	ifs.open(ctowc(filename, wszFilePath, sizeof(wszFilePath)));
#else
	ifs.open(filename);
#endif
	if (!ifs.is_open())
		return false;

	Json::Reader reader;
	if (!reader.parse(ifs, rootValue))
	{
		ifs.close();
		return false;
	}
	ifs.close();
	return true;
}

BOOL GameCfgFileMan::LoadGameGlobalCfg(const char* filename)
{
	m_vitRecoveryInterval = 10;
	m_vitRecoveryVal = 1;
	m_maxCreateRoles = 4;
	m_maxInstRankNum = 3;
	m_maxAcceptTasks = 100;
	m_equipMaxStarLv = 20;
	m_equipMaxStrengthenLv = 100;
	m_maxSaveSellProps = 8;
	m_maxEmailAttachments = 4;
	m_maxFriendNum = 50;
	m_recommendFriendNum = 5;
	m_recommendFriendLv = 10;
	m_initBackpackSize = 40;
	m_initWarehouseSize = 0;
	m_maxBackpackSize = 80;
	m_maxWarehouseSize = 0;
	m_strengthenStoneModulus = 1.0;
	m_strengthenGoldModulus = 1.0;
	m_buybackPriceTimes = 1.0;
	m_dropGoldBasicVal = 100;
	m_dropGoldFloat = 10;
	m_goldBranchThrehold = 100;	//金币分堆阀值
	m_goldBranchMaxPercent = 30;	//金币分堆最大百分比
	m_dropLv1ToBossPercent = 20;	//普通物品分到boss百分比
	m_dropLv1ToCreamPercent = 20;	//普通物品分到精英百分比
	m_expCorrectVal = 0;			//经验修正值
	m_riseStarMaxSuccessChance = 90;	//升星最大成功率
	m_riseStarMinDamageChance = 0;		//升星最小破损率
	m_powerCorrectVal = 0;			//战力修正范围值

	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Game"))
		return false;

	m_vitRecoveryInterval = rootValue["Game"]["VitRestoreTime"].asUInt();
	m_vitRecoveryVal = rootValue["Game"]["VitRestoreNum"].asUInt();
	m_maxAutoRecoveryVit = rootValue["Game"]["MaxVit"].asUInt();
	m_maxAcceptTasks = (BYTE)rootValue["Game"]["MaxAcceptTask"].asUInt();
	m_equipMaxStrengthenLv = (BYTE)rootValue["Game"]["MaxStrengthenLv"].asUInt();
	m_equipMaxStarLv = (BYTE)rootValue["Game"]["MaxStarLv"].asUInt();
	m_maxSaveSellProps = (BYTE)rootValue["Game"]["MaxSaveSellProps"].asUInt();
	m_maxEmailAttachments = max(1, (BYTE)rootValue["Game"]["MaxEmailAttachments"].asUInt());
	m_maxFriendNum = (BYTE)rootValue["Game"]["AddFriendsMaxNum"].asUInt();
	m_maxRoleLevel = (BYTE)rootValue["Game"]["MaxLevel"].asUInt();
	m_initBackpackSize = (BYTE)rootValue["Game"]["PackageTotal"].asUInt();
	m_initWarehouseSize = (BYTE)rootValue["Game"]["StorageTotal"].asUInt();
	m_maxBackpackSize = (BYTE)rootValue["Game"]["MaxPackage"].asUInt();
	m_maxWarehouseSize = (BYTE)rootValue["Game"]["MaxStorage"].asUInt();
	m_packParam = (BYTE)rootValue["Game"]["PackParam"].asUInt();
	m_CPBanlance.HPBalance = rootValue["Game"]["HPBalance"].asUInt() / 100.0f;
	m_CPBanlance.MPBalance = rootValue["Game"]["MPBalance"].asUInt() / 100.0f;
	m_CPBanlance.ATKBalance = rootValue["Game"]["ATKBalance"].asUInt() / 100.0f;
	m_CPBanlance.DEFBalance = rootValue["Game"]["DEFBalance"].asUInt() / 100.0f;
	m_CPBanlance.CTBalance = rootValue["Game"]["CTBalance"].asUInt() / 100.0f;
	m_CPBanlance.SGBalance = rootValue["Game"]["SGBalance"].asUInt() / 100.0f;
	m_CPBanlance.SKBalance = rootValue["Game"]["SKBalance"].asUInt() / 100.0f;
	m_CPBanlance.CritHurt = rootValue["Game"]["CritHurt"].asUInt() / 100.0f;
	m_CPBanlance.CriticalRating = rootValue["Game"]["CriticalRating"].asUInt() / 10000.0f;
	m_strengthenStoneModulus = rootValue["Game"]["SModulus"].asUInt() / 100.0;
	m_strengthenGoldModulus = rootValue["Game"]["GModulus"].asUInt() / 100.0;
	m_buybackPriceTimes = rootValue["Game"]["BuyBack"].asUInt() / 100.0;
	m_dropGoldBasicVal = rootValue["Game"]["GoldBase"].asUInt();
	m_dropGoldFloat = min(100, rootValue["Game"]["GoldFloat"].asUInt());
	m_goldBranchThrehold = rootValue["Game"]["GoldAllotMinNum"].asUInt();				//金币分堆阀值
	m_goldBranchMaxPercent = min(100, rootValue["Game"]["GoldAllotMax"].asUInt());		//金币分堆最大百分比
	m_dropLv1ToBossPercent = min(100, rootValue["Game"]["DropLv1ToBoss"].asUInt());		//普通物品分到boss百分比
	m_dropLv1ToCreamPercent = min(100, rootValue["Game"]["DropLv1ToElite"].asUInt());	//普通物品分到精英百分比
	m_expCorrectVal = rootValue["Game"]["ExpCorrect"].asInt();			//经验修正值
	m_stoneConsumeCorrectVal = rootValue["Game"]["SMValue"].asInt();	//石头消耗修正值
	m_goldConsumeCorrectVal = rootValue["Game"]["GMValue"].asInt();		//金币消耗修正值
	m_activityInstUpperLimit = rootValue["Game"]["Activity"].asInt();	//活动副本上限


	int n = rootValue["Game"]["Strengthen"].size();
	for (int i = 0; i < n; ++i)
	{
		UINT32 val = rootValue["Game"]["Strengthen"][i].asUInt();
		m_strengthenBasicConsumeStoneVec.push_back(val);
	}

	n = rootValue["Game"]["GStrengthen"].size();
	for (int i = 0; i < n; ++i)
	{
		UINT32 val = rootValue["Game"]["GStrengthen"][i].asUInt();
		m_strengthenBasicConsumeGoldVec.push_back(val);
	}

	if (rootValue["Game"].isMember("MaxStarSucced") && rootValue["Game"]["MaxStarSucced"].isInt())
		m_riseStarMaxSuccessChance = rootValue["Game"]["MaxStarSucced"].asUInt();

	if (rootValue["Game"].isMember("MinStarBroken") && rootValue["Game"]["MinStarBroken"].isInt())
		m_riseStarMinDamageChance = rootValue["Game"]["MinStarBroken"].asUInt();

	if (rootValue["Game"].isMember("PowerMatching") && rootValue["Game"]["PowerMatching"].isInt())
		m_powerCorrectVal = rootValue["Game"]["PowerMatching"].asUInt();


	if (rootValue["Game"].isMember("PlayerDay") && rootValue["Game"]["PlayerDay"].isInt())
		m_delRoleTimeLimit = rootValue["Game"]["PlayerDay"].asUInt();

	//zpy 读取天梯匹配配置
	const Json::Value &json_array = rootValue["Game"]["LadderRefresh"];
	for (int i = 0; i < json_array.size(); ++i)
	{
		UINT32 key = json_array[i]["Ranking"].asUInt();
		UINT32 value = json_array[i]["NUM"].asUInt();
		m_ladderMatching.insert(std::make_pair(key, value));
	}

	return true;
}

int GameCfgFileMan::GetEquipStrengthenSpend(BYTE pos, BYTE lv, SEquipStrengthenSpend& spend)
{
	int idx = pos - 1;
	if (idx < 0 || idx >= m_strengthenBasicConsumeStoneVec.size() || idx >= m_strengthenBasicConsumeGoldVec.size())
		return MLS_PROPERTY_NOT_EXIST;

	int stones = m_strengthenBasicConsumeStoneVec[idx] * pow(lv, m_strengthenStoneModulus) + m_stoneConsumeCorrectVal;
	INT64 gold = m_strengthenBasicConsumeGoldVec[idx] * pow(lv, m_strengthenGoldModulus) + m_goldConsumeCorrectVal;

	spend.stones = stones > 0 ? stones : 0;
	spend.gold = gold > 0 ? gold : 0;
	return MLS_OK;
}

int GameCfgFileMan::GetRoleCfgBasicAttr(BYTE jobId, BYTE roleLevel, SRoleAttrPoint& attr)
{
	return m_roleCfg.GetRoleCfgBasicAttr(jobId, roleLevel, attr);
}

int GameCfgFileMan::JudgeRoleUpgrade(UINT16 jobId, BYTE curLevel, UINT64 curExp, BYTE& newLevel)
{
	return m_roleCfg.JudgeRoleUpgrade(jobId, curLevel, curExp, newLevel);
}

BYTE GameCfgFileMan::GetRoleLevelByExp(BYTE jobId, UINT64 exp)
{
	return m_roleCfg.GetRoleLevelByExp(jobId, exp);
}

//技能
int GameCfgFileMan::GetRoleAllInitSkills(BYTE jobId, BYTE from, BYTE to, std::vector<SSkillMoveInfo>& skills)
{
	return m_skillCfg.GetRoleAllInitSkills(jobId, from, to, skills);
}

int GameCfgFileMan::GetUpgradeAnySkillAttr(BYTE jobId, UINT16 skillId, BYTE newSkillLevel, SkillCfg::SSkillAttrCfg& skillAttr)
{
	return m_skillCfg.GetUpgradeAnySkillAttr(jobId, skillId, newSkillLevel, skillAttr);
}

//是否是普通技能
bool GameCfgFileMan::IsSkill(BYTE occuId, UINT16 skillID)
{
	return m_skillCfg.IsSkill(occuId, skillID);
}

/* zpy 新增 获取普通技能配置 */
bool GameCfgFileMan::GetSkillAttr(BYTE jobId, UINT16 skillID, SkillCfg::SSkillAttrCfg *&item)
{
	return m_skillCfg.GetSkillAttr(jobId, skillID, item);
}

//副本
int GameCfgFileMan::GetInstSpendVit(UINT16 instId, UINT32& spendVit)
{
	return m_instCfg.GetInstSpendVit(instId, spendVit);
}

int GameCfgFileMan::GetInstName(UINT16 instId, std::string& instName)
{
	return m_instCfg.GetInstName(instId, instName);
}

int GameCfgFileMan::GetInstLvLimit(UINT16 instId, BYTE& lvLimit)
{
	return m_instCfg.GetInstLvLimit(instId, lvLimit);
}

//获取副本配置
int GameCfgFileMan::GetInstConfig(UINT16 instId, InstCfg::SInstAttrCfg *&config)
{
	return m_instCfg.GetInstConfig(instId, config);
}

int GameCfgFileMan::GetInstBasicInfo(UINT16 instId, BYTE&instType, BYTE& lvLimit, UINT32& spendVit)
{
	return m_instCfg.GetInstBasicInfo(instId, instType, lvLimit, spendVit);
}

UINT16 GameCfgFileMan::GenerateRandInstId()
{
	return m_instCfg.GenerateRandInstId();
}

int GameCfgFileMan::GenerateDropBox(UINT16 instId, InstCfg::SInstDropBox& dropBox)
{
	return m_instCfg.GenerateDropBox(instId, dropBox);
}

int GameCfgFileMan::GenerateAreaMonsterDrops(UINT16 instId, BYTE areaId, InstCfg::SInstDropProp& dropProps, BYTE& monsterDrops, BYTE& creamDrops, BYTE& bossDrops)
{
	return m_instCfg.GenerateAreaMonsterDrops(instId, areaId, dropProps, monsterDrops, creamDrops, bossDrops);
}

/* zpy 2015.12.22新增 展开掉落组 */
void GameCfgFileMan::SpreadDropGroups(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem> *ret)
{
	m_instCfg.SpreadDropGroups(dropId, dropNum, ret);
}

int GameCfgFileMan::GetBattleAreaMonster(UINT16 instId, BYTE areaId, InstCfg::SInstBrushMonster& brushMonster)
{
	return m_instCfg.GetBattleAreaMonster(instId, areaId, brushMonster);
}

int GameCfgFileMan::GenerateBrokenDrops(UINT16 instId, InstCfg::SInstDropProp& brokenDrops)
{
	return m_instCfg.GenerateBrokenDrops(instId, brokenDrops);
}

UINT64 GameCfgFileMan::GetInstSweepExp(UINT16 instId)
{
	return m_instCfg.GetInstSweepExp(instId);
}

int GameCfgFileMan::GenerateInstSweepDrops(UINT16 instId, InstCfg::SInstDropProp& drops)
{
	return m_instCfg.GenerateInstSweepDrops(instId, drops);
}

int GameCfgFileMan::GetInstOwnerChapter(UINT16 instId, UINT16& chapterId)
{
	return m_instCfg.GetInstOwnerChapter(instId, chapterId);
}

BYTE GameCfgFileMan::GetInstType(UINT16 instId)
{
	return m_instCfg.GetInstType(instId);
}

//获取进入次数
UINT16 GameCfgFileMan::GetInstEnterNum(UINT16 instId)
{
	return m_instCfg.GetInstEnterNum(instId);
}

//通过类型获取副本id
bool GameCfgFileMan::GetInstByInstType(BYTE type, std::set<UINT16> *&ret)
{
	return m_instCfg.GetInstByInstType(type, ret);
}

//物品
int GameCfgFileMan::GetPropertyAttr(BYTE propType, UINT16 propId, SPropertyAttr& propAttr, BYTE& maxStackCount)
{
	int retCode = MLS_PROPERTY_NOT_EXIST;
	if (propType == ESGS_PROP_EQUIP)
		retCode = m_equipCfg.GetEquipPropAttr(propId, propAttr, maxStackCount);
	else if (propType == ESGS_PROP_GEM)
		retCode = m_gemCfg.GetGemPropAttr(propId, propAttr, maxStackCount);
	else if (propType == ESGS_PROP_MATERIAL)
		retCode = m_materialCfg.GetMaterialPropAttr(propId, propAttr, maxStackCount);

	return retCode;
}

BYTE GameCfgFileMan::GetPropertyMaxStack(BYTE propType, UINT16 propId)
{
	BYTE maxStack = 1;
	if (propType == ESGS_PROP_EQUIP)
		maxStack = m_equipCfg.GetEquipMaxStack(propId);
	else if (propType == ESGS_PROP_GEM)
		maxStack = m_gemCfg.GetGemMaxStack(propId);
	else if (propType == ESGS_PROP_MATERIAL)
		maxStack = m_materialCfg.GetMaterialMaxStack(propId);
	return maxStack;
}

int GameCfgFileMan::GetPropertyUserSellPrice(BYTE propType, UINT16 propId, UINT64& price)
{
	int retCode = MLS_PROPERTY_NOT_EXIST;
	if (propType == ESGS_PROP_EQUIP)
		retCode = m_equipCfg.GetEquipUserSellPrice(propId, price);
	else if (propType == ESGS_PROP_GEM)
		retCode = m_gemCfg.GetGemUserSellPrice(propId, price);
	else if (propType == ESGS_PROP_MATERIAL)
		retCode = m_materialCfg.GetMaterialUserSellPrice(propId, price);
	else if (propType == ESGS_PROP_FRAGMENT)
		retCode = m_fragmentCfg.GetFragmentUserSellPrice(propId, price);
	else if (propType == ESGS_PROP_CHAIN_SOUL_FRAGMENT)
		retCode = m_chainSoulFragmentCfg.GetFragmentUserSellPrice(propId, price);

	return retCode;
}

int GameCfgFileMan::GetPropertyUserBuyBackPrice(BYTE propType, UINT16 propId, UINT64& price)
{
	int retCode = GetPropertyUserSellPrice(propType, propId, price);
	if (retCode != MLS_OK)
		return retCode;

	price = price * m_buybackPriceTimes;
	return retCode;
}

void GameCfgFileMan::GetPropertyName(BYTE propType, UINT16 propId, std::string& name)
{
	if (propType == ESGS_PROP_EQUIP)
		m_equipCfg.GetEquipName(propId, name);
}

//任务
int GameCfgFileMan::GetDailyTasks(BYTE roleLv, TaskCfg::SDailyTasks& dailyTasks)
{
	return m_taskCfg.GetDailyTasks(roleLv, dailyTasks);
}

int GameCfgFileMan::GetTaskInfo(UINT16 taskId, TaskCfg::STaskAttrCfg*& pTaskInfo)
{
	return m_taskCfg.GetTaskInfo(taskId, pTaskInfo);
}

int GameCfgFileMan::GetTaskReard(UINT16 taskId, BYTE roleLv, TaskCfg::STaskReward& taskReward)
{
	return m_taskCfg.GetTaskReard(taskId, roleLv, taskReward);
}

//装备
int GameCfgFileMan::GetEquipPropAttr(UINT16 equipId, SPropertyAttr& propAttr, BYTE& maxStack)
{
	return m_equipCfg.GetEquipPropAttr(equipId, propAttr, maxStack);
}

int GameCfgFileMan::GetEquipAttrPoint(UINT16 equipId, std::vector<EquipCfg::SEquipAddPoint>*& addPoint)
{
	return m_equipCfg.GetEquipAttrPoint(equipId, addPoint);
}

int GameCfgFileMan::GetEquipAttrIncreament(BYTE pos, BYTE quality, StrengthenCfg::SEquipAttrInc& attrInc)
{
	return m_strengthenCfg.GetEquipAttrIncreament(pos, quality, attrInc);
}

//宝石
int GameCfgFileMan::GetGemSpendByLevel(BYTE gemLv, GemCfg::SGemConsumeCfg& spend)
{
	return m_gemCfg.GetGemSpendByLevel(gemLv, spend);
}

int GameCfgFileMan::GetGemSpendById(UINT16 gemId, GemCfg::SGemConsumeCfg& spend)
{
	return m_gemCfg.GetGemSpendById(gemId, spend);
}

int GameCfgFileMan::GetEquipInlaidGemAttr(BYTE equipType, BYTE holeSeq, GemCfg::SEquipGemAttr& attr)
{
	return m_gemCfg.GetEquipInlaidGemAttr(equipType, holeSeq, attr);
}

int GameCfgFileMan::GetGemCfgAttr(UINT16 gemPropId, GemCfg::SGemAttrCfg& gemAttr)
{
	return m_gemCfg.GetGemCfgAttr(gemPropId, gemAttr);
}

int GameCfgFileMan::GetGemIdMaxStackByTypeLevel(BYTE gemType, BYTE gemLv, UINT16& gemId, BYTE& maxStack)
{
	return m_gemCfg.GetGemIdMaxStackByTypeLevel(gemType, gemLv, gemId, maxStack);
}

int GameCfgFileMan::GetGemPropAttr(UINT16 gemPropId, SPropertyAttr& propAttr, BYTE& maxStack)
{
	return m_gemCfg.GetGemPropAttr(gemPropId, propAttr, maxStack);
}

UINT16 GameCfgFileMan::GetComposeGemConsumeGems() const
{
	return m_gemCfg.GetComposeGemConsumeGems();
}

//获取材料功能值
int GameCfgFileMan::GetMaterialFuncValue(UINT16 materiaId, UINT32& funcVal)
{
	return m_materialCfg.GetMaterialFuncValue(materiaId, funcVal);
}

int GameCfgFileMan::GetMaterialFuncValueVec(UINT16 materiaId, std::vector<UINT32>& funcValVec)
{
	return m_materialCfg.GetMaterialFuncValueVec(materiaId, funcValVec);
}

//获取商城物品属性
int GameCfgFileMan::GetMallBuyGoodsAttr(UINT32 buyId, MallCfg::SMallGoodsAttr*& pGoodsAttr)
{
	return m_mallCfg.GetMallBuyGoodsAttr(buyId, pGoodsAttr);
}

int GameCfgFileMan::GetMallBuyGoodsAttrByPropId(UINT16 propId, MallCfg::SMallGoodsAttr *&pGoodsAttr)
{
	return m_mallCfg.GetMallBuyGoodsAttrByPropId(propId, pGoodsAttr);
}

//时装和职业是否匹配
int GameCfgFileMan::IsJobFashionMatch(BYTE jobId, UINT16 fashionId)
{
	return m_fashionCfg.IsJobFashionMatch(jobId, fashionId);
}

int GameCfgFileMan::GetFashionAttr(UINT16 fashionId, std::vector<FashionCfg::SFashionAttr>*& pAttrVec)
{
	return m_fashionCfg.GetFashionAttr(fashionId, pAttrVec);
}

//获取充值属性
int GameCfgFileMan::GetRechargeAttr(UINT16 cashId, RechargeCfg::SRechargeCfgAttr& cashAttr)
{
	return m_rechargeCfg.GetRechargeAttr(cashId, cashAttr);
}

//获取充值配置
const std::map<UINT16, RechargeCfg::SRechargeCfgAttr>& GameCfgFileMan::GetRechargeConfig() const
{
	return m_rechargeCfg.Get();
}

//获取称号属性
int GameCfgFileMan::GetRoleTitleAttr(BYTE jobId, UINT16 titleId, std::vector<TitleCfg::STitleAttr>& attrVec)
{
	return m_titleCfg.GetRoleTitleAttr(jobId, titleId, attrVec);
}

//邮件赠送配置
int GameCfgFileMan::GetNewPlayerEmailGiveProps(std::vector<EmailCfg::SNewPlayerEmailItem>& giveVec)
{
	return m_emailCfg.GetNewPlayerEmailGiveProps(giveVec);
}

//单人随机副本
BOOL GameCfgFileMan::GetRandInstSysParam(RandInstSysCfg::SRandInstSysParam& param)
{
	return m_randInstSysCfg.GetRandInstSysParam(param);
}

//获取碎片属性
int GameCfgFileMan::GetFragmentAttr(UINT16 fragmentId, FragmentCfg::SFragmentCfgAttr& fragmentAttr)
{
	return m_fragmentCfg.GetFragmentAttr(fragmentId, fragmentAttr);
}

int GameCfgFileMan::GetGiveFragment(std::vector<FragmentCfg::SGiveFragmentInfo>& giveVec)
{
	return m_fragmentCfg.GetGiveFragment(giveVec);
}

//装备合成/升星
int GameCfgFileMan::GetEquipComposeItem(UINT16 composeId, EquipComposeCfg::SEquipComposeItem*& pComposeItem)
{
	return m_equipComposeCfg.GetEquipComposeItem(composeId, pComposeItem);
}

int GameCfgFileMan::GetRiseStarSpend(BYTE starLv, RiseStarCfg::SRiseStarSpend& spend)
{
	return m_riseStarCfg.GetRiseStarSpend(starLv, spend);
}

int GameCfgFileMan::GetRiseStarCfg(BYTE starLv, RiseStarCfg::SRiseStarCfg& cfg)
{
	return m_riseStarCfg.GetRiseStarCfg(starLv, cfg);
}

//获取复活CD
int GameCfgFileMan::GetReviveCDTimes(UINT16 reviveId)
{
	return m_reviveCfg.GetReviveCDTimes(reviveId);
}

//获取复活消耗
int GameCfgFileMan::GetReviveConsume(UINT16 reviveId, UINT32 times, UINT16 vipAddTimes, UINT32& reviveCoin)
{
	return m_reviveCfg.GetReviveConsume(reviveId, times, vipAddTimes, reviveCoin);
}

//获取金币兑换
int GameCfgFileMan::GetGoldExchange(UINT16 exchangeTimes, UINT16 vipAddTimes, UINT64& getGold, UINT32& spendToken)
{
	return m_goldExchangeCfg.GetGoldExchange(exchangeTimes, vipAddTimes, getGold, spendToken);
}

/* zpy 兑换体力 */
int GameCfgFileMan::GetVitExchange(UINT16 exchangeTimes, UINT16 vipAddTimes, UINT64& getVit, UINT32& spendToken)
{
	return m_vitExchangeCfg.GetVitExchangeExchange(exchangeTimes, vipAddTimes, getVit, spendToken);
}

//计算VIP等级
BYTE GameCfgFileMan::CalcVIPLevel(UINT32 cashcount)
{
	return m_vipCfg.CalcVIPLevel(cashcount);
}

void GameCfgFileMan::GetVIPGiveProps(BYTE vipLv, std::vector<VIPCfg::SVIPGiveProp>*& pGivePropVec)
{
	m_vipCfg.GetVIPGiveProps(vipLv, pGivePropVec);
}

void GameCfgFileMan::GetVIPMaxVit(BYTE vipLv, UINT32& maxVit)
{
	m_vipCfg.GetVIPMaxVit(vipLv, maxVit);
}

void GameCfgFileMan::GetVIPBuyVitTimes(BYTE vipLv, UINT16& buyVitTimes)
{
	m_vipCfg.GetVIPBuyVitTimes(vipLv, buyVitTimes);
}

void GameCfgFileMan::GetVIPGoldExchangeTimes(BYTE vipLv, UINT16& exchangeTimes)
{
	m_vipCfg.GetVIPGoldExchangeTimes(vipLv, exchangeTimes);
}

/* zpy 体力兑换 */
void GameCfgFileMan::GetVIPVitExchangeTimes(BYTE vipLv, UINT16& exchangeTimes)
{
	m_vipCfg.GetVIPBuyVitTimes(vipLv, exchangeTimes);
}

//获取VIP购买竞技模式次数
void GameCfgFileMan::GetVIPBuyKOFTimes(BYTE vipLv, UINT16& buyKOFTimes)
{
	m_vipCfg.GetVIPBuyKOFTimes(vipLv, buyKOFTimes);
}

//获取1v1进入上限
void GameCfgFileMan::GetOneVsOneEnterTimes(BYTE vipLv, UINT16& enterTimes)
{
	m_vipCfg.GetOneVsOneEnterTimes(vipLv, enterTimes);
}

void GameCfgFileMan::GetVIPDeadReviveTimes(BYTE vipLv, BYTE& reviveTimes)
{
	m_vipCfg.GetVIPDeadReviveTimes(vipLv, reviveTimes);
}

void GameCfgFileMan::GetVIPInstSweepTimes(BYTE vipLv, UINT16& sweepTimes)
{
	m_vipCfg.GetVIPInstSweepTimes(vipLv, sweepTimes);
}

void GameCfgFileMan::GetVIPResourceInstChalNum(BYTE vipLv, UINT16& chalTimes)
{
	m_vipCfg.GetVIPResourceInstChalNum(vipLv, chalTimes);
}

void GameCfgFileMan::GetVIPMallBuyDiscount(BYTE vipLv, BYTE& discount)
{
	m_vipCfg.GetVIPMallBuyDiscount(vipLv, discount);
}

void GameCfgFileMan::GetVIPSignReward(BYTE vipLv, BYTE& signReward)
{
	m_vipCfg.GetVIPSignReward(vipLv, signReward);
}

//获取怪经验
UINT32 GameCfgFileMan::GetMonsterExp(UINT16 momsterId)
{
	return m_monsterCfg.GetMonsterExp(momsterId);
}

/* zpy 成就系统新增 */
BYTE GameCfgFileMan::GetMonsterType(UINT16 momsterId)
{
	return m_monsterCfg.GetMonsterType(momsterId);
}

//获取怪难度系数
UINT32 GameCfgFileMan::GetMonsterDifficulty(UINT16 monsterId)
{
	return m_monsterCfg.GetMonsterDifficulty(monsterId);
}

//获取签到信息
int GameCfgFileMan::GetSignInItem(BYTE nTimes, SignInCfg::SSignInItem*& pItem)
{
	return m_signInCfg.GetSignInItem(nTimes, pItem);
}

//活动信息
int GameCfgFileMan::GetActivityItem(UINT32 activityId, ActivityCfg::SActivityItem& activityItem)
{
	return m_activityCfg.GetActivityItem(activityId, activityItem);
}

//公告
int GameCfgFileMan::GetNoticeItems(std::vector<NoticeCfg::SNoticeItem>& noticeVec, UINT32& timeInterval)
{
	return m_noticeCfg.GetNoticeItems(noticeVec, timeInterval);
}

//获取时间间隔
UINT32 GameCfgFileMan::GetNoticeInterval()
{
	return m_noticeCfg.GetNoticeInterval();
}

//获取解锁参数
int GameCfgFileMan::GetUnlockItem(BYTE type, UINT16 unlockId, UnlockCfg::SUnlockItem*& pUnlockItem)
{
	return m_unlockCfg.GetUnlockItem(type, unlockId, pUnlockItem);
}

int GameCfgFileMan::GetUnlockedItems(BYTE type, std::vector<UINT16>& itemsVec)
{
	return m_unlockCfg.GetUnlockedItems(type, itemsVec);
}

//获取礼包物品
int GameCfgFileMan::GetGiftBagItem(UINT16 id, GiftBagCfg::SGiftBagItem& item)
{
	return m_giftBagCfg.GetGiftBagItem(id, item);
}

//获取章节数据
int GameCfgFileMan::GetChapterItem(UINT16 chapterId, ChapterCfg::SChapterItem*& pChapterItem)
{
	return m_chapterCfg.GetChapterItem(chapterId, pChapterItem);
}

//获取角色初始赠送
BOOL GameCfgFileMan::GetRoleInitGiveItem(InitRoleGiveCfg::SInitRoleGiveItem& giveItem)
{
	return m_newRoleGiveCfg.GetRoleInitGiveItem(giveItem);
}

//生成轮盘奖品
BOOL GameCfgFileMan::GenerateRouletteReward(SGetRoulettePropRes* pRoulettePropRes, std::vector<RouletteCfg::SRoulettePropCfgItem>& saveDBItemVec)
{
	return m_rouletteCfg.GenerateRouletteReward(pRoulettePropRes, saveDBItemVec);
}

BOOL GameCfgFileMan::GenerateRouletteReward(std::vector<RouletteCfg::SRoulettePropCfgItem>& itemVec)
{
	return m_rouletteCfg.GenerateRouletteReward(itemVec);
}

BOOL GameCfgFileMan::GenerateRouletteBigReward(RouletteCfg::SRoulettePropCfgItem& bigReward, UINT32& spendToken)
{
	return m_rouletteCfg.GenerateRouletteBigReward(bigReward, spendToken);
}

BYTE GameCfgFileMan::GetEverydayGiveTimes() const
{
	return m_rouletteCfg.GetEverydayGiveTimes();
}

//获取邮件类型描述
const char* GameCfgFileMan::GetEmailTypeDesc(BYTE type)
{
	const char* pDesc = "";
	switch (type)
	{
	case ESGS_EMAIL_SYSTEM:				//系统邮件
		pDesc = "系统邮件";
		break;
	case ESGS_EMAIL_INST_DROP:			//副本掉落
		pDesc = "副本掉落";
		break;
	case ESGS_EMAIL_TASK_REWARD:		//任务奖励
		pDesc = "任务奖励";
		break;
	case ESGS_EMAIL_FIRST_RECHARGE:		//首充赠送
		pDesc = "首充赠送";
		break;
	case ESGS_EMAIL_VIP_REWARD:			//VIP奖励
		pDesc = "VIP奖励";
		break;
	case ESGS_EMAIL_SIGN_IN_REWARD:		//签到奖励
		pDesc = "签到奖励";
		break;
	case ESGS_EMAIL_ACTIVITY_REWARD:	//活动奖励
		pDesc = "活动奖励";
		break;
	case ESGS_EMAIL_GIFTBAG_REWARD:		//礼包奖励
		pDesc = "礼包奖励";
		break;
	case ESGS_EMAIL_NEW_ACTOR_REWARD:	//新角色奖励
		pDesc = "新角色奖励";
		break;
	case ESGS_EMAIL_EXTRACT_REWARD:		//抽奖
		pDesc = "抽奖";
		break;
	default:
		break;
	}

	return pDesc;
}

//获取成就目标类型
bool GameCfgFileMan::GetAchievementTargetType(UINT32 id, AchievementCfg::TargetType &type) const
{
	return m_achievementCfg.GetAchievementTargetType(id, type);
}

//获取成就奖励
bool GameCfgFileMan::GetAchievementReward(UINT32 id, AchievementCfg::SAchievementReward &reward) const
{
	return m_achievementCfg.GetAchievementReward(id, reward);
}

//获取成就条件
bool GameCfgFileMan::GetAchievementCondition(UINT32 id, AchievementCfg::SAchievementCondition &condition)
{
	return m_achievementCfg.GetAchievementCondition(id, condition);
}

//根据成就类型获取成就
bool GameCfgFileMan::GetAchievementFromType(AchievementCfg::AchievementType type, const std::vector<UINT32> *&out) const
{
	return m_achievementCfg.GetAchievementFromType(type, out);
}

// 根据目标类型获取成就
bool GameCfgFileMan::GetAchievementFromTargetType(AchievementCfg::TargetType type, const std::vector<UINT32> *&out) const
{
	return m_achievementCfg.GetAchievementFromTargetType(type, out);
}

//是否开启每日签到
bool GameCfgFileMan::IsDailySignValid() const
{
	return m_dailySignCfg.IsDailySignValid();
}

//获取每日签到配置
bool GameCfgFileMan::GetDailySignConfig(BYTE days, const DailySignCfg::SDailySignItem *&config)
{
	return m_dailySignCfg.GetDailySignConfig(days, config);
}

//获取炼魂碎片出售价格(单价)
int GameCfgFileMan::GetFragmentUserSellPrice(UINT32 fragmentId, UINT64& price)
{
	return m_chainSoulFragmentCfg.GetFragmentUserSellPrice(fragmentId, price);
}

//获取炼魂碎片属性
bool GameCfgFileMan::GetChainSoulFragmentAttr(UINT32 fragmentId, ChainSoulFragmentCfg::ChainSoulFragmentAttr &out_attr)
{
	return m_chainSoulFragmentCfg.GetChainSoulFragmentAttr(fragmentId, out_attr);
}

//通过类型获取炼魂碎片ID
bool GameCfgFileMan::GetFragmentIdByType(BYTE type, UINT32 &out_id)
{
	return m_chainSoulFragmentCfg.GetFragmentIdByType(type, out_id);
}

//是否开启炼魂系统
bool GameCfgFileMan::IsOpenChainSoulSystem() const
{
	return m_greedTheLiveCfg.IsOpen();
}

//获取部件配置
bool GameCfgFileMan::GetChainSoulPosConfig(BYTE type, BYTE level, GreedTheLiveCfg::ChainSoulPosConfig *&out_config)
{
	return m_greedTheLiveCfg.GetChainSoulPosConfig(type, level, out_config);
}

//获取圣物核心配置
bool GameCfgFileMan::GetChainSoulCoreConfig(BYTE level, GreedTheLiveCfg::ChainSoulCoreConfig *&out_config)
{
	return m_greedTheLiveCfg.GetChainSoulCoreConfig(level, out_config);
}

//根据条件等级获取核心等级
BYTE GameCfgFileMan::GetCoreLevelByConditionLevel(BYTE condition_level)
{
	return m_greedTheLiveCfg.GetCoreLevelByConditionLevel(condition_level);
}

////add by hxw 排行奖励配置信息获取
INT GameCfgFileMan::GetRewardPlayNum(const BYTE type) const
{
	return m_rankRewardCfg.GetRewardPlayNum(type);
}

BOOL GameCfgFileMan::GetRankReward(const BYTE type, const UINT16 rankid, CRankReward::SRkRewardRes*& items, const int iValue)
{
	return m_rankRewardCfg.GetRankReward(type, rankid, items, iValue);
}
//是否是活动时间，返回TRUE表示活动继续，FLASE表示无法活动
BOOL GameCfgFileMan::IsValidityTime(const BYTE type)
{
	return m_rankRewardCfg.IsValidityTime(type);
}

BOOL GameCfgFileMan::IsOpen(const BYTE type) const
{
	return m_rankRewardCfg.IsOpen(type);
}
void GameCfgFileMan::SetOpen(const BYTE type, const BOOL bOpen)
{
	m_rankRewardCfg.SetOpen(type, bOpen);
}

BOOL GameCfgFileMan::IsCanGet(const BYTE type, const UINT16 id, const UINT32 value) const
{
	return m_rankRewardCfg.IsCanGet(type, id, value);
}

int GameCfgFileMan::GetRankLogs(std::string& str, const BYTE type, const UINT16 rewardid)
{
	return m_rankRewardCfg.GetLoginfoName(str, type, rewardid);
}

//获取限时活动配置
bool GameCfgFileMan::GetLimitedTimeActivityCfg(LimitedTimeActivityCfg::ActivityType type, LimitedTimeActivityCfg::SActivityConfig &out)
{
	return m_limitedTimeActivity.GetLimitedTimeActivityCfg(type, out);
}

//生成活动副本
bool GameCfgFileMan::GenerateActivityInst(int level, std::set<UINT16> passInsts, SActivityInstItem &item)
{
	return m_limitedTimeActivity.GenerateActivityInst(level, passInsts, item);
}

//获取世界Boss血量
UINT32 GameCfgFileMan::GetWorldBossBlood() const
{
	InstCfg::SInstBrushMonster brushMonster;
	GetGameCfgFileMan()->GetBattleAreaMonster(LimitedTimeActivityCfg::WORLD_BOSS, 1, brushMonster);
	int id = brushMonster.monsterMap.begin()->first;
	int level = brushMonster.monsterMap.begin()->second.lvMonsterMap.begin()->first;
	int difficulty = GetGameCfgFileMan()->GetMonsterDifficulty(id);
	UINT32 hp = GetGameCfgFileMan()->GetMonsterBlood(level);
	UINT32 proportion = GetGameCfgFileMan()->GetMonsterBloodProportion(ESGS_MONSTER_WORLDBOSS);
	return (difficulty / 100.0f) * (proportion / 100.0f) * hp;
}

//获取cd消耗
UINT32 GameCfgFileMan::GetGameTypeEliminateExpend(BYTE type, BYTE times)
{
	return m_gameTypeCfg.GetGameTypeEliminateExpend(type, times);
}

//获取刷新消耗
UINT32 GameCfgFileMan::GetGameTypeRefurbishExpend(BYTE type, BYTE times)
{
	return m_gameTypeCfg.GetGameTypeRefurbishExpend(type, times);
}

//获取购买价格
UINT32 GameCfgFileMan::GetGameTypeBuyPriceExpend(BYTE type, UINT16 times)
{
	return m_gameTypeCfg.GetGameTypeBuyPriceExpend(type, times);
}

//获取复活规则
BYTE GameCfgFileMan::GetGameTypeReviveRule(BYTE type)
{
	return m_gameTypeCfg.GetGameTypeReviveRule(type);
}

//获取条件和奖励
bool GameCfgFileMan::GetConditionAndReward(BYTE type, std::vector<GameTypeCfg::SRewardCondition> *&out)
{
	return m_gameTypeCfg.GetConditionAndReward(type, out);
}

//获取天梯最大挑战次数
UINT32 GameCfgFileMan::GetLadderMaxChallengeNum() const
{
	return m_gameTypeCfg.GetLadderMaxChallengeNum();
}

//获取天梯挑战CD
UINT32 GameCfgFileMan::GetLadderChallengeCDTime() const
{
	return m_gameTypeCfg.GetLadderChallengeCDTime();
}

//获取1v1奖励比率
BYTE GameCfgFileMan::GetOneOnOneRewardRatio(BYTE finCode) const
{
	return m_gameTypeCfg.GetOneOnOneRewardRatio(finCode);
}


//解除奖励嵌套
void GameCfgFileMan::UnlockRewardNesting(const std::vector<GameTypeCfg::SRewardInfo> &source, std::vector<GameTypeCfg::SRewardInfo> *ret)
{
	m_gameTypeCfg.UnlockNesting(source, ret);
}

//获取怪物血量
UINT32 GameCfgFileMan::GetMonsterBlood(UINT16 level)
{
	return m_monsterAttributeCfg.GetMonsterBlood(level);
}

//获取怪物血比例
UINT32 GameCfgFileMan::GetMonsterBloodProportion(UINT16 id)
{
	return m_monsterTypeCfg.GetMonsterBloodProportion(id);
}

//获取装备位强化属性
BOOL GameCfgFileMan::GetStrengthenAttri(BYTE level, StrengthenAttriCfg::SStrengthenAttr &ret)
{
	return m_strengthenAttriCfg.GetStrengthenAttri(level, ret);
}
//获取天梯匹配范围
UINT32 GameCfgFileMan::GetLadderMatchingScope(UINT32 rank)
{
	for (std::map<UINT32, UINT32>::reverse_iterator itr = m_ladderMatching.rbegin(); itr != m_ladderMatching.rend(); ++itr)
	{
		if (rank >= itr->first)
		{
			return itr->second;
		}
	}
	return 0;
}

// 生成周免角色
BYTE GameCfgFileMan::GeneraWeekFreeActorType()
{
	return m_playerCfg.GeneraWeekFreeActorType();
}

//获取活动副本开启条件
bool GameCfgFileMan::GetActivityStageOpenCondition(UINT16 instId, ActivityStageCfg::Condition &ret)
{
	return m_activityStageCfg.GetActivityStageOpenCondition(instId, ret);
}