#include "ServiceRank.h"
#include "RankConfigFileMan.h"
#include "moduleInfo.h"
#include "json.h"
#include <fstream>

static RankConfigMan* sg_cfgRankMan = NULL;
BOOL InitRankConfigFileMan(const char* pszFile /*= "riserankserver.properties"*/)
{
	ASSERT(sg_cfgRankMan == NULL);
	sg_cfgRankMan = new RankConfigMan();
	ASSERT(sg_cfgRankMan != NULL);
	return sg_cfgRankMan->LoadConfigFile(pszFile);
}

RankConfigMan* GetRankConfigFileMan()
{
	return sg_cfgRankMan;
}

void DestroyRankConfigFileMan()
{
	RankConfigMan* cfgMan = sg_cfgRankMan;
	sg_cfgRankMan = NULL;
	if (cfgMan != NULL)
		delete cfgMan;
}

RankConfigMan::RankConfigMan()
{

}

RankConfigMan::~RankConfigMan()
{

}

BOOL RankConfigMan::LoadConfigFile(const char* pszFilePath)
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
	m_initDBConnections = 1;
	m_maxDBConnections = 4;
	m_initGWConnections = 1;
	m_interval = RANK_DEFAULT_UPDATE_INTERVAL;
	m_save_interval = RANK_DEFAULT_UPDATE_INTERVAL * 4;
	m_displaynum = MAX_RANK_NUM;

	if (!rootValue.isMember("dbs_ip") || !rootValue["dbs_ip"].isString() ||
		!rootValue.isMember("dbs_port") || !rootValue["dbs_port"].isInt())
		return false;

	if (!rootValue.isMember("db_name") || !rootValue["db_name"].isString())
		return false;

	//数据服务器
	m_dbsIp = rootValue["dbs_ip"].asCString();
	m_dbsPort = rootValue["dbs_port"].asInt();
	m_dbName = rootValue["db_name"].asCString();

	BOOL ok = true;
	if (rootValue.isMember("gws_ip") && rootValue["gws_ip"].isString())
		m_gwsIp = rootValue["gws_ip"].asCString();
	else
		ok = false;

	if (rootValue.isMember("gws_port") && rootValue["gws_port"].isInt())
		m_gwsPort = rootValue["gws_port"].asInt();
	else
		ok = false;

	if (rootValue.isMember("version") && rootValue["version"].isString())
		m_version = rootValue["version"].asCString();

	if (rootValue.isMember("init_db_connections") && rootValue["init_db_connections"].isInt())
		m_initDBConnections = rootValue["init_db_connections"].asInt();

	if (rootValue.isMember("max_db_connections") && rootValue["max_db_connections"].isInt())
		m_maxDBConnections = rootValue["max_db_connections"].asInt();

	if (rootValue.isMember("init_gw_connections") && rootValue["init_gw_connections"].isInt())
		m_initGWConnections = rootValue["init_gw_connections"].asInt();

	if (rootValue.isMember("update_interval") && rootValue["update_interval"].isInt())
		m_interval = rootValue["update_interval"].asInt();

	if (rootValue.isMember("save_interval") && rootValue["save_interval"].isInt())
		m_save_interval = rootValue["save_interval"].asInt();

	if (rootValue.isMember("rank_disp_num") && rootValue["rank_disp_num"].isInt())
		m_displaynum = rootValue["rank_disp_num"].asInt();

	return ok;
}