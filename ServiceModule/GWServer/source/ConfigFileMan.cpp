#include <fstream>
#include "ConfigFileMan.h"
#include "cyaLog.h"
#include "moduleinfo.h"
#include "json.h"
#include "cyaMaxMin.h"

#define MIN_HEARTBEAT_TIMER 10*1000
#define MAX_HEARTBEAT_TIMER 60*60*1000

static ConfigFileMan* sg_cfgMan = NULL;
void InitConfigFileMan(const char* filename/* = "risegwserver.properties"*/)
{
	ASSERT(sg_cfgMan == NULL);
	sg_cfgMan = new ConfigFileMan();
	ASSERT(sg_cfgMan != NULL);
	sg_cfgMan->LoadConfigFile(filename);
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
{

}

ConfigFileMan::~ConfigFileMan()
{

}

void ConfigFileMan::LoadConfigFile(const char* pszFilePath)
{
	if (pszFilePath == NULL || strlen(pszFilePath) <= 0)
	{
		DefaultConfigParam();
		return;
	}
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
	{
		DefaultConfigParam();
		return;
	}

	Json::Reader reader;
	Json::Value rootValue;
	if (!reader.parse(ifs, rootValue))
	{
		ifs.close();
		DefaultConfigParam();
		return;
	}
	ifs.close();

	if (rootValue.isMember("server_port") && rootValue["server_port"].isInt())
		m_servePort = rootValue["server_port"].asInt();

	if (rootValue.isMember("heartbeat_timer") && rootValue["heartbeat_timer"].isInt())
		m_heartInterval = rootValue["heartbeat_timer"].asInt() * 1000;

	if (rootValue.isMember("version") && rootValue["version"].isString())
		m_version = rootValue["version"].asCString();

	if (m_heartInterval > 0)
		m_heartInterval = max(MIN_HEARTBEAT_TIMER, min(MAX_HEARTBEAT_TIMER, m_heartInterval));
	LogInfo(("读取配置文件[%s]成功!", pszFilePath));
}

void ConfigFileMan::DefaultConfigParam()
{
	m_heartInterval = 0;
	m_servePort = 10020;
	LogInfo(("读取配置文件失败,使用默认配置参数!"));
}