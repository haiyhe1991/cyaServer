#ifndef __CONFIG_FILE_MAN_H__
#define __CONFIG_FILE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
配置文件管理类
*/

#define SGS_CASH_TABLE_NUM 5
#define SGS_CASH_TABLE_NAME_PREFIX "cash_"
#define SGS_CASHLIST_TABLE_NUM 5
#define SGS_CASHLIST_TABLE_NAME_PREFIX "cashlist_"

#define CASH_SAVE_TYPE_TO_ACTOR  1
#define CASH_SAVE_TYPE_TO_CASHDB  2

#include "cyaTypes.h"

class ConfigFileMan
{
	ConfigFileMan(const ConfigFileMan&);
	ConfigFileMan& operator = (const ConfigFileMan&);

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

	int GetInitDBConnections() const
	{
		return m_initDBConnections;
	}

	int GetMaxDBConnections() const
	{
		return m_maxDBConnections;
	}

	int GetInitGWConnections() const
	{
		return m_initGWConnections;
	}

	const char* GetDBSIp() const
	{
		return m_dbsIp.c_str();
	}

	int GetDBSPort() const
	{
		return m_dbsPort;
	}

	int GetRefreshDBInterval() const
	{
		return m_refreshDBInterval;
	}

	const char* GetDBName() const
	{
		return m_dbName.c_str();
	}

	int GetMaxUnReadEmailSaveDays() const
	{
		return m_maxUnReadEmailSaveDays;
	}

	int GetMaxReadEmailSaveDays() const
	{
		return m_maxReadEmailSaveDays;
	}

	const char* GetVersion() const
	{
		return m_version.c_str();
	}

	BOOL IsInnerDebugMode() const
	{
		return m_isInnerDebug;
	}

	int GetSyncInstRankTime() const
	{
		return m_syncInstRankTime;
	}

	int GetSyncInstHistoryTime() const
	{
		return m_syncInstHistoryTime;
	}

	int GetMaxCacheItems() const
	{
		return m_maxCacheItems;
	}

	int GetSyncTaskHistoryTime() const
	{
		return m_syncTaskHistoryTime;
	}

	const char* GetGameCfgPath() const
	{
		return m_gameCfgPath.c_str();
	}

	BOOL IsCheckInstDropEmail() const
	{
		return m_instDropEmailChk;
	}

	int GetRechargeDBSPort() const
	{
		return m_rechargeDBSPort;
	}

	const char* GetRechargeDBSIp() const
	{
		return m_rechargeDBSIp.c_str();
	}

	const char* GetRechargeDBName() const
	{
		return m_rechargeDBName.c_str();
	}

	const char* GetPlayerActionDBName() const
	{
		return m_playerActionDBName.c_str();
	}

	int GetActionBatchNum() const
	{
		return m_actionBatchNum;
	}

	BOOL EnableCheckActorOrder() const
	{
		return m_enableCheckActorOrder;
	}

	//by hxw add 20150928 获取分区以及开服时间
	UINT16 GetPartID() const{ return m_partID; }
	std::string GetPartName() const { return m_partName; }
	LTMSEL GetPartOpenTime() const { return m_partStartTime; }
	//end

	//add by hxw 20151111 增加玩家代币数据库
	int GetCashDBSPort() const
	{
		return m_cashDBSPort;
	}

	const char* GetCashDBSIp() const
	{
		return m_cashDBSIp.c_str();
	}

	const char* GetCashDBName() const
	{
		return m_cashDBName.c_str();
	}

	//add by hxw 20151116 增加代币存储类型
	int GetCashSaveType() const
	{
		return m_cashSaveType;
	}

	//add by hxw 20151210 增加 是否上线版本
	BOOL IsRelease() const
	{
		return m_release == 1;
	}

private:
	int m_cashDBSPort;			//充值数据库端口
	std::string m_cashDBSIp;	//充值数据库ip
	std::string m_cashDBName;	//充值数据库名
	//end by 20151111

private:
	std::string m_gwsIp;	//网关ip
	int m_gwsPort;			//网关port
	std::string m_version;	//版本
	std::string m_gameCfgPath;	//游戏配置文件路径

	std::string m_dbsIp;	//数据服务器ip
	int m_dbsPort;			//数据服务器port
	std::string m_dbName;	//数据库名
	int m_initDBConnections;	//初始化数据服务器连接数
	int m_maxDBConnections;		//最大数据服务器连接数

	int m_initGWConnections;	//初始化网关服务器连接数
	int m_refreshDBInterval;	//刷新角色数据信息时间间隔

	int m_maxCacheItems;			//最大缓存数据
	int m_maxUnReadEmailSaveDays;	//未读邮件最大保存天数
	int m_maxReadEmailSaveDays;		//已读邮件最大保存天数
	BOOL m_isInnerDebug;			//是否内部调试模式

	int m_syncInstRankTime;		//刷新副本排行时间
	int m_syncInstHistoryTime;	//刷新副本历史记录时间
	int m_syncTaskHistoryTime;	//刷新任务历史记录时间

	BOOL m_instDropEmailChk;	//是否检测副本掉落邮件

	int m_rechargeDBSPort;			//充值数据库端口
	std::string m_rechargeDBSIp;	//充值数据库ip
	std::string m_rechargeDBName;	//充值数据库名
	std::string m_playerActionDBName;	//玩家行为数据库名
	int m_actionBatchNum;
	BOOL m_enableCheckActorOrder;
	//20150928 add by hxw 分区信息记录
	LTMSEL m_partStartTime;
	std::string m_partName;
	UINT16 m_partID;
	BYTE m_cashSaveType;
	BYTE m_release;	//是否是上线版本
	//end
};

BOOL InitConfigFileMan(const char* pszFilePath);
ConfigFileMan* GetConfigFileMan();
void DestroyConfigFileMan();
#endif