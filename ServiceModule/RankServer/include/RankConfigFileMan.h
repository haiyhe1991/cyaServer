#ifndef __RANK_CONFIG_FILE_MAN_H__
#define __RANK_CONFIG_FILE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"

#define SEPARATOR(x) 1

#define RANK_DEFAULT_UPDATE_INTERVAL    (3600*1)   //unit: second

class RankConfigMan
{
public:
	RankConfigMan();
	~RankConfigMan();

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

	int GetUpdateInterval() const
	{
		return m_interval;
	}

	int GetSaveInterval() const
	{
		return m_save_interval;
	}

	int GetDisplayNum() const
	{
		return m_displaynum;
	}

private:
	std::string m_gwsIp;
	std::string m_dbsIp;
	int m_gwsPort;
	int m_dbsPort;
	std::string m_dbName;
	std::string m_version;
	int m_initGWConnections;
	int m_initDBConnections;
	int m_maxDBConnections;
	int m_interval;     // 排行更新间隔
	int m_save_interval;// 保存排行数据间隔
	int m_displaynum;   // 排行榜显示记录数
};

BOOL InitRankConfigFileMan(const char* pszFile = "riserankserver.properties");
RankConfigMan* GetRankConfigFileMan();
void DestroyRankConfigFileMan();

#endif //_RankConfigMan_h__