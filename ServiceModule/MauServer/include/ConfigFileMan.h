#ifndef __CONFIG_FILE_MAN_H__
#define __CONFIG_FILE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#include "cyaTypes.h"

class ConfigFileMan
{
public:
	ConfigFileMan();
	~ConfigFileMan();

	BOOL LoadConfigFile(const char* pszFilePath);

	const char* GetGwsIp() const
	{
		return m_gwsIp.c_str();
	}

	int GetGwsPort() const
	{
		return m_gwsPort;
	}

	const char* GetVersion() const
	{
		return m_version.c_str();
	}

	int GetInitGWConnections() const
	{
		return m_initGWConnections;
	}

private:
	std::string m_gwsIp;
	int m_gwsPort;
	std::string m_version;
	int m_initGWConnections;
};

BOOL InitConfigFileMan(const char* pszFile = "cyaMauServer.properties");
ConfigFileMan* GetConfigFileMan();
void DestroyConfigFileMan();

#endif
