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

	void LoadConfigFile(const char* pszFilePath);

	BOOL IsEnableHeartbeat() const
	{
		return m_heartInterval > 0 ? true : false;
	}

	DWORD GetHeartbeatInterval() const
	{
		return m_heartInterval;
	}

	int GetServePort() const
	{
		return m_servePort;
	}

	const char* GetVersion() const
	{
		return m_version.c_str();
	}

private:
	void DefaultConfigParam();

private:
	DWORD m_heartInterval;
	int m_servePort;
	std::string m_version;
};

void InitConfigFileMan(const char* filename = "risegwserver.properties");
void DestroyConfigFileMan();
ConfigFileMan* GetConfigFileMan();


#endif