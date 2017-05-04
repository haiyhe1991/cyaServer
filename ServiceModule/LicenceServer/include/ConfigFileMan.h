#ifndef __CONFIG_FILE_MAN_H__
#define __CONFIG_FILE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
配置文件管理类
*/

#include "cyaTcp.h"

#define SEPARATOR(x) 1

#define DEFAULT_USER_ONLINE_TIME        2*60*1000   //2min
#define DEFAULT_LINKER_HEART_TIMER      30*1000     //20s

#define SGS_PLAYER_ACCOUNT_TABLE_NUM    5

class ConfigFileMan
{
public:
	ConfigFileMan();
	~ConfigFileMan();

	BOOL LoadConfigFile(const char* pszFilePath);

	int GetServePort() const
	{
		return m_servePort;
	}

	BOOL IsEnableHeartbeat()
	{
		return m_linkerHeartInterval > 0 ? true : false;
	}

	BOOL IsDataEncrypt() const
	{
		return m_dataEncrypt;
	}

	DWORD GetLinkerHeartbeatInterval() const
	{
		return m_linkerHeartInterval;
	}

	DWORD GetMaxUserOnlineTime() const
	{
		return m_maxUserOnlineTime;
	}

	int GetInitDBConnections() const
	{
		return m_initDBConnections;
	}

	int GetMaxDBConnections() const
	{
		return m_maxDBConnections;
	}

	const char* GetDBSIp() const
	{
		return m_dbsIp.c_str();
	}

	int GetDBSPort() const
	{
		return m_dbsPort;
	}

	const char* GetDBName() const
	{
		return m_dbName.c_str();
	}

	const char* GetCashDbIp() const
	{
		return m_dbCashIp.c_str();
	}

	int GetCashDbPort() const
	{
		return m_dbCashPort;
	}

	const char* GetCashDbName() const
	{
		return m_dbCashName.c_str();
	}
	// debug
	const char* GetActionDbIp() const
	{
		return m_dbActionIp.c_str();
	}

	int GetActionDbPort() const
	{
		return m_dbActionPort;
	}

	const char* GetActionDbName() const
	{
		return m_dbActionName.c_str();
	}
	// debug
	const char* GetVersion() const
	{
		return m_version.c_str();
	}

	DWORD GetSyncRoleDBTimer() const
	{
		return m_syncRoleDBTimer;
	}

	DWORD GetMaxUserPayload() const
	{
		return m_maxUserPayload;
	}

	DWORD GetMaxCacheItems() const
	{
		return m_maxCacheItems;
	}

	BOOL IsAutoPartition() const
	{
		return m_isAutoPartition;
	}

	const char* HashTableName(const char* pszKey, std::string& strValue);
	const char* HashTableName(UINT32 id, std::string& strValue);

private:
	std::string m_dbsIp;
	int m_dbsPort;
	std::string m_dbName;
	std::string m_dbCashIp;
	int m_dbCashPort;
	std::string m_dbCashName;
	// debug
	std::string m_dbActionIp;
	int m_dbActionPort;
	std::string m_dbActionName;
	// debug
	int m_servePort;
	DWORD m_maxUserOnlineTime;
	DWORD m_linkerHeartInterval;
	int m_initDBConnections;
	int m_maxDBConnections;
	BOOL m_dataEncrypt;
	std::string m_version;
	DWORD m_syncRoleDBTimer;
	DWORD m_maxUserPayload;
	DWORD m_maxCacheItems;
	BOOL  m_isAutoPartition;
};

BOOL InitConfigFileMan(const char* filename = "riselcserver.properties");
ConfigFileMan* GetConfigFileMan();
void DestroyConfigFileMan();

#endif