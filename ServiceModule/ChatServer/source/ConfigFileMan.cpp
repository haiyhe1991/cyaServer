#include "ConfigFileMan.h"
#include "moduleInfo.h"
#include "json.h"
#include <fstream>

static ConfigFileMan* sg_cfgMan = NULL;
BOOL InitConfigFileMan(const char* pszFile /*= "risechatserver.properties"*/)
{
	ASSERT(sg_cfgMan == NULL);
	sg_cfgMan = new ConfigFileMan();
	ASSERT(sg_cfgMan != NULL);
	return sg_cfgMan->LoadConfigFile(pszFile);
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

	m_initGWConnections = 1;

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

	if (rootValue.isMember("init_gw_connections") && rootValue["init_gw_connections"].isInt())
		m_initGWConnections = rootValue["init_gw_connections"].asInt();

	return ok;
}