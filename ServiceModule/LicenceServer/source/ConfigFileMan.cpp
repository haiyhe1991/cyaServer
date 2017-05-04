
#include <fstream>
#include "moduleInfo.h"
#include "cyaLog.h"
#include "ConfigFileMan.h"
#include "json.h"
#include "cyaMaxMin.h"
#include "stringHash.h"

#define SGS_PLAYER_ACCOUNT_TABLE_NAME_PREFIX "player_account_"

static ConfigFileMan* sg_cfgMan = NULL;
BOOL InitConfigFileMan(const char* filename /*= "riselcserver.properties"*/)
{
	ASSERT(sg_cfgMan == NULL);
	sg_cfgMan = new ConfigFileMan();
	ASSERT(sg_cfgMan != NULL);
	return sg_cfgMan->LoadConfigFile(filename);
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
	: m_isAutoPartition(true)
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

	//系统参数
	m_servePort = 10030;
	m_maxUserOnlineTime = DEFAULT_USER_ONLINE_TIME;
	m_linkerHeartInterval = DEFAULT_LINKER_HEART_TIMER;
	m_initDBConnections = 2;
	m_maxDBConnections = 32;
	m_dataEncrypt = true;
	m_syncRoleDBTimer = 5;
	m_maxCacheItems = 1024;
	m_maxUserPayload = 0;

	if (!rootValue.isMember("dbs_ip") || !rootValue["dbs_ip"].isString() ||
		!rootValue.isMember("dbs_port") || !rootValue["dbs_port"].isInt())
		return false;

	if (!rootValue.isMember("cash_dbs_ip") || !rootValue["cash_dbs_ip"].isString() ||
		!rootValue.isMember("cash_dbs_port") || !rootValue["cash_dbs_port"].isInt())
		return false;

	if (!rootValue.isMember("db_name") || !rootValue["db_name"].isString())
		return false;

	if (!rootValue.isMember("cash_db_name") || !rootValue["cash_db_name"].isString())
		return false;

	//debug
	if (!rootValue.isMember("action_dbs_ip") || !rootValue["action_dbs_ip"].isString() ||
		!rootValue.isMember("action_dbs_port") || !rootValue["action_dbs_port"].isInt())
		return false;

	if (!rootValue.isMember("action_db_name") || !rootValue["action_db_name"].isString())
		return false;
	//debug

	//数据服务器
	m_dbsIp = rootValue["dbs_ip"].asCString();
	m_dbsPort = rootValue["dbs_port"].asInt();
	m_dbName = rootValue["db_name"].asCString();

	m_dbCashIp = rootValue["cash_dbs_ip"].asCString();;
	m_dbCashPort = rootValue["cash_dbs_port"].asInt();
	m_dbCashName = rootValue["cash_db_name"].asCString();
	//debug
	m_dbActionIp = rootValue["action_dbs_ip"].asCString();;
	m_dbActionPort = rootValue["action_dbs_port"].asInt();
	m_dbActionName = rootValue["action_db_name"].asCString();
	//debug
	if (rootValue.isMember("data_encrypt") && rootValue["data_encrypt"].isBool())
		m_dataEncrypt = !!rootValue["data_encrypt"].asBool();

	if (rootValue.isMember("server_port") && rootValue["server_port"].isInt())
		m_servePort = rootValue["server_port"].asInt();

	if (rootValue.isMember("linker_heartbeat_timer") && rootValue["linker_heartbeat_timer"].isInt())
		m_linkerHeartInterval = rootValue["linker_heartbeat_timer"].asInt() * 1000;

	if (rootValue.isMember("max_user_online_time") && rootValue["max_user_online_time"].isInt())
	{
		DWORD n = rootValue["max_user_online_time"].asInt() * 1000;
		if (n > 0)
			m_maxUserOnlineTime = max(m_maxUserOnlineTime, n);
		else
			m_maxUserOnlineTime = 0;
	}

	if (rootValue.isMember("init_db_connections") && rootValue["init_db_connections"].isInt())
		m_initDBConnections = rootValue["init_db_connections"].asInt();

	if (rootValue.isMember("max_db_connections") && rootValue["max_db_connections"].isInt())
		m_maxDBConnections = rootValue["max_db_connections"].asInt();

	if (rootValue.isMember("version") && rootValue["version"].isString())
		m_version = rootValue["version"].asCString();

	if (rootValue.isMember("sync_role_db_timer") && rootValue["sync_role_db_timer"].isInt())
		m_syncRoleDBTimer = rootValue["sync_role_db_timer"].asInt();

	if (rootValue.isMember("max_user_payload") && rootValue["max_user_payload"].isInt())
		m_maxUserPayload = rootValue["max_user_payload"].asInt();

	if (rootValue.isMember("max_cache_items") && rootValue["max_cache_items"].isInt())
		m_maxCacheItems = rootValue["max_cache_items"].asInt();

	if (rootValue.isMember("auto_choose_partition") && rootValue["auto_choose_partition"].isBool())
		m_isAutoPartition = !!rootValue["auto_choose_partition"].asBool();

	LogInfo(("读取配置文件[%s]成功!", pszFilePath));
	return true;
}

const char* ConfigFileMan::HashTableName(const char* pszKey, std::string& strValue)
{
	UINT32 val = ELFHash(pszKey);
	strValue = SGS_PLAYER_ACCOUNT_TABLE_NAME_PREFIX;
	char buf[32] = { 0 };
	sprintf(buf, "%d", (val % SGS_PLAYER_ACCOUNT_TABLE_NUM) + 1);
	strValue += buf;
	return strValue.c_str();
}

const char* ConfigFileMan::HashTableName(UINT32 id, std::string& strValue)
{
	strValue = SGS_PLAYER_ACCOUNT_TABLE_NAME_PREFIX;
	char buf[32] = { 0 };
	sprintf(buf, "%d", (id % SGS_PLAYER_ACCOUNT_TABLE_NUM) + 1);
	strValue += buf;
	return strValue.c_str();
}

