#include <fstream>
#include "ConfigFileMan.h"
#include "moduleInfo.h"
#include "json.h"
#include "cyaLog.h"
#include "cyaMaxMin.h"
#include "cyaStrdk.h"

static ConfigFileMan* sg_cfgMan = NULL;
BOOL InitConfigFileMan(const char* pszFilePath)
{
	if (pszFilePath == NULL || strlen(pszFilePath) <= 0)
		return false;

	ASSERT(sg_cfgMan == NULL);
	sg_cfgMan = new ConfigFileMan();
	ASSERT(sg_cfgMan != NULL);
	return sg_cfgMan->LoadConfigFile(pszFilePath);
}

ConfigFileMan* GetConfigFileMan()
{
	return sg_cfgMan;
}

void DestroyConfigFileMan()
{
	ConfigFileMan* cfgMan = sg_cfgMan;
	sg_cfgMan = NULL;
	if (cfgMan != NULL)
		delete cfgMan;
}

ConfigFileMan::ConfigFileMan()
:m_partID(0)
{

}

ConfigFileMan::~ConfigFileMan()
{

}

BOOL ConfigFileMan::LoadConfigFile(const char* pszFilePath)
{
	if (pszFilePath == NULL || strlen(pszFilePath) <= 0)
		return false;

	char szFilePath[MAX_PATH] = { 0 };
	if (IsAbsolutePath(pszFilePath))
		strcpy(szFilePath, pszFilePath);
	else
		MergeFileName(GetExeDir(), pszFilePath, szFilePath);

	std::ifstream ifs;
#if defined(WIN32)
	WCHAR wszFilePath[MAX_PATH] = { 0 };
	ifs.open(ctowc(szFilePath, wszFilePath, sizeof(wszFilePath)));
#else
	ifs.open(szFilePath);
#endif
	if (!ifs.is_open())
		return false;

	Json::Reader reader;
	Json::Value rootValue;
	if (!reader.parse(ifs, rootValue))
	{
		ifs.close();
		return false;
	}
	ifs.close();

	if (!rootValue.isMember("dbs_ip") || !rootValue["dbs_ip"].isString() ||
		!rootValue.isMember("dbs_port") || !rootValue["dbs_port"].isInt())
		return false;

	if (!rootValue.isMember("db_name") || !rootValue["db_name"].isString())
		return false;

	if (!rootValue.isMember("gws_ip") || !rootValue["gws_ip"].isString() ||
		!rootValue.isMember("gws_port") || !rootValue["gws_port"].isInt())
		return false;

	if (!rootValue.isMember("recharge_dbs_ip") || !rootValue["recharge_dbs_ip"].isString() ||
		!rootValue.isMember("recharge_dbs_port") || !rootValue["recharge_dbs_port"].isInt())
		return false;

	//数据服务器
	m_dbsIp = rootValue["dbs_ip"].asCString();
	m_dbsPort = rootValue["dbs_port"].asInt();
	m_dbName = rootValue["db_name"].asCString();

	//网关服务器
	m_gwsIp = rootValue["gws_ip"].asCString();
	m_gwsPort = rootValue["gws_port"].asInt();

	//充值数据服务器
	m_rechargeDBSIp = rootValue["recharge_dbs_ip"].asCString();
	m_rechargeDBSPort = rootValue["recharge_dbs_port"].asInt();
	m_rechargeDBName = rootValue["recharge_db_name"].asCString();

	//add by hxw 20151111 
	//玩家代币数据库
	m_cashDBSPort = rootValue["cash_dbs_port"].asInt();
	m_cashDBSIp = rootValue["cash_dbs_ip"].asCString();
	m_cashDBName = rootValue["cash_db_name"].asCString();
	//end
	//玩家行为数据库名
	m_playerActionDBName = rootValue["player_action_db_name"].asCString();

	//add by hxw 20150928 分区信息加载
	m_partStartTime = StrToMsel(rootValue["part_open_time"].asString().c_str());
	m_partName = rootValue["part_name"].asString().c_str();
	m_partID = rootValue["part_id"].asInt();
	//end

	//add by hxw 20151116 代币存储类型
	m_cashSaveType = rootValue["cash_save_type"].asInt();
	//add by hxw 20151210 上线版本
	m_release = rootValue["isrelease"].asInt();
	//

	//系统参数
	m_initDBConnections = 2;
	m_maxDBConnections = 32;
	m_initGWConnections = 4;
	m_refreshDBInterval = 5;
	m_maxUnReadEmailSaveDays = 7;
	m_maxReadEmailSaveDays = 3;
	m_isInnerDebug = false;
	m_syncInstRankTime = 120;
	m_syncInstHistoryTime = 5;
	m_syncTaskHistoryTime = 5;
	m_maxCacheItems = 256;
	m_instDropEmailChk = true;
	m_actionBatchNum = 100;
	m_enableCheckActorOrder = true;

	//游戏配置文件路径
	m_gameCfgPath = "./conf";
	if (rootValue.isMember("game_cfg_dir") && rootValue["game_cfg_dir"].isString())
	{
		const char* pszCfgDir = rootValue["game_cfg_dir"].asCString();
		if (!IsAbsolutePath(pszCfgDir))
		{
			char szPath[MAX_PATH] = { 0 };
			MergeFileName(GetExeDir(), pszCfgDir, szPath);
			m_gameCfgPath = szPath;
		}
		else
			m_gameCfgPath = pszCfgDir;
	}
	else
	{
		char szPath[MAX_PATH] = { 0 };
		MergeFileName(GetExeDir(), m_gameCfgPath.c_str(), szPath);
		m_gameCfgPath = szPath;
	}
	if (!IsSlashChar(m_gameCfgPath[m_gameCfgPath.length() - 1]))
		m_gameCfgPath += OS_DIR_SEP_CHAR_STR;

	if (rootValue.isMember("init_db_connections") && rootValue["init_db_connections"].isInt())
		m_initDBConnections = rootValue["init_db_connections"].asInt();

	if (rootValue.isMember("max_db_connections") && rootValue["max_db_connections"].isInt())
		m_maxDBConnections = rootValue["max_db_connections"].asInt();

	if (rootValue.isMember("init_gw_connections") && rootValue["init_gw_connections"].isInt())
		m_initGWConnections = rootValue["init_gw_connections"].asInt();

	if (rootValue.isMember("refresh_db_timer") && rootValue["refresh_db_timer"].isInt())
		m_refreshDBInterval = max(m_refreshDBInterval, rootValue["refresh_db_timer"].asInt());

	if (rootValue.isMember("max_unread_email_save_days") && rootValue["max_unread_email_save_days"].isInt())
		m_maxUnReadEmailSaveDays = rootValue["max_unread_email_save_days"].asInt();

	if (rootValue.isMember("max_read_email_save_days") && rootValue["max_read_email_save_days"].isInt())
		m_maxReadEmailSaveDays = rootValue["max_read_email_save_days"].asInt();

	if (rootValue.isMember("version") && rootValue["version"].isString())
		m_version = rootValue["version"].asCString();

	if (rootValue.isMember("inner_debug") && rootValue["inner_debug"].isBool())
		m_isInnerDebug = !!rootValue["inner_debug"].asBool();

	if (rootValue.isMember("sync_inst_rank_timer") && rootValue["sync_inst_rank_timer"].isInt())
		m_syncInstRankTime = rootValue["sync_inst_rank_timer"].asInt();

	if (rootValue.isMember("sync_inst_history_timer") && rootValue["sync_inst_history_timer"].isInt())
		m_syncInstHistoryTime = rootValue["sync_inst_history_timer"].asInt();

	if (rootValue.isMember("sync_task_history_timer") && rootValue["sync_task_history_timer"].isInt())
		m_syncTaskHistoryTime = rootValue["sync_inst_history_timer"].asInt();

	if (rootValue.isMember("max_cache_items") && rootValue["max_cache_items"].isInt())
		m_maxCacheItems = rootValue["max_cache_items"].asInt();
	if (rootValue.isMember("inst_drop_email") && rootValue["inst_drop_email"].isBool())
		m_instDropEmailChk = !!rootValue["inst_drop_email"].asBool();

	if (rootValue.isMember("action_batch_num") && rootValue["action_batch_num"].isInt())
		m_actionBatchNum = max(1, rootValue["action_batch_num"].asInt());

	if (rootValue.isMember("check_actor_order") && rootValue["check_actor_order"].isBool())
		m_enableCheckActorOrder = rootValue["check_actor_order"].asBool();

	LogInfo(("读取系统配置文件[%s]成功!", pszFilePath));

	return true;
}