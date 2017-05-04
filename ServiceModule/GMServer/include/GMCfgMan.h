#ifndef __GM_CFG_MAN_H__
#define __GM_CFG_MAN_H__

#include "cyaBase.h"
#include <map>
#include <vector>
#include "json.h"

struct LinkerServeinfo
{
	char lkIp[17]; //Linker IP
	UINT16 lkPort; //Linker 端口
};

struct ServeInfo
{
	char sIp[17]; //IP
	UINT16 sPort; // 端口
};

struct PlatInfo
{
	UINT16 channelid; //渠道ID
	char channelname[33]; //渠道名
	std::vector<UINT16> partId; //渠道分区
};

typedef std::vector<UINT16> PartIDVec;		//分区;
typedef std::vector<LinkerServeinfo*> LinkerSInfoVec;
typedef std::vector<ServeInfo/*连接服务器IP和端口信息*/> ServeInfoVec;
typedef std::map<UINT16/*分区ID*/, ServeInfoVec/*所属分区的ip和端口信息*/> LKServeInfo;
typedef std::map<UINT16/*所属渠道*/, LKServeInfo/*渠道linker信息*/> ChannelLinkerServeInfo;
typedef std::map<UINT16/*所属分区*/, ServeInfo/*DB服务器IP和端口信息*/> DBSServeInfo;
typedef std::map<UINT16/*所属渠道*/, DBSServeInfo/*渠道DB服务器信息*/> ChannelDBServeInfo;
//typedef std::map<UINT16/*平台ID*/,ChannelDBServeInfo/*渠道*/> Platinfo;	//暂时没必须要;

class CGMCfgMan
{
public:
	CGMCfgMan();
	~CGMCfgMan();

	///加载配置
	int Load(const char* cfgName);

	///获取服务器端口号
	UINT16 GetServePort()
	{
		return m_servePort;
	}

	///获取在线刷新间隔时间(单位：秒)
	UINT16 GetRefreshTime()
	{
		return m_onlineRefreshTime;
	}
	///获取实时更新时间间隔(单位:秒)
	UINT16 GetRefrNewTime()
	{
		return m_realNewTime;
	}

	///获取心跳间隔时间(单位：秒)
	UINT16 GetHeartbeatInterval()
	{
		return m_heartbeatInterval;
	}
	///获取数据库更新时间;
	char* GetdbsTime()
	{
		return dbstime;
	}

	///是否写入数据库
	BOOL IsWriteIntoDb()
	{
		return m_isWriteIntoDb;
	}

	///是否测试
	BOOL IsTest()
	{
		return m_isTest;
	}

	///获取留存类型
	std::vector<UINT16>& GetLeaveKeepTimeType()
	{
		return m_leaveKeepType;
	}


	//获取渠道分区服务器连接信息
	ChannelLinkerServeInfo& GetPlatLinkerServeInfo()
	{
		return m_channelLkServeInfo;
	}

	//获取渠道分区数据库服务器连接信息
	ChannelDBServeInfo& GetPlatDBServeInfo()
	{
		return m_channelDbServeInfo;
	}

	//获取充值数据库服务器连接信息
	ServeInfo& GetRechargeDBServeInfo()
	{
		return m_rechargeDBs;
	}

	//获取帐号数据库服务器连接信息
	ServeInfo& GetAccountDBServeInfo()
	{
		return m_accountDBs;
	}

	//获取所有平台/*平台包含IOS与安卓平台*/;
	std::vector<UINT16/*平台ID*/>& GetAllPlatId()
	{
		return m_platId;
	}
	// 获取渠道id
	std::vector<UINT16/*渠道ID*/>& GetAllchannel()
	{
		return m_channel;
	}
	//获取指定渠道的分区
	void GetPlatAllPartId(UINT16 platId, std::vector<UINT16>& partVec)
	{
		partVec.clear();
		std::map<UINT16, PartIDVec>::iterator it = m_channelPartId.find(platId);
		if (it != m_channelPartId.end())
			partVec = it->second;
	}

	///获取渠道分区信息
	std::vector<PlatInfo>& GetPlatInfo()
	{
		return m_channelpartInfo;
	}

private:
	BOOL CheckFile(const char* cfgName, Json::Value& jvalue);

	///解析分区数据库服务器信息
	int ParseNewDBSCfgMan(Json::Value& jvalue);
	///解析分区Linker服务器信息
	int ParseNewLKSCfgMan(Json::Value& jvalue);
	///解析充值数据库服务器信息
	int ParseNewRechargeDBSCfgMan(Json::Value& jvalue);
	///解析帐号数据库服务器信息
	int ParseNewAccountDBSCfgMan(Json::Value& jvalue);

	inline void NewObj(LinkerServeinfo*& p)
	{
		p = new LinkerServeinfo();
	}

private:

	std::map<std::string, int> m_dbsInfo;
	std::map<UINT16, LinkerSInfoVec> m_lksInfo;
	std::vector<UINT16> m_leaveKeepType;			//留存时间类型
	std::vector<UINT16> m_platId;					//平台ID—多个渠道——多个分区;
	std::vector<UINT16> m_channel;					//渠道ID;
	std::map<UINT16, PartIDVec> m_channelPartId;	//渠道分区 
	std::vector<PlatInfo> m_channelpartInfo;		//渠道分区信息


	UINT16 m_servePort;		  //管理服务器端口
	UINT16 m_heartbeatInterval; //心跳间隔时间[单位:秒]
	UINT16 m_onlineRefreshTime; //在线人数刷新间隔时间[单位:秒]
	UINT16 m_realNewTime;		//实时更新[单位:秒];
	BOOL m_isWriteIntoDb;       //是否写入数据库
	BOOL m_isTest;			  //是否测试
	char dbstime[20];		  //数据库更新时间;

	//Platinfo			   m_platinfo;						//平台渠道DBServer信息;
	ChannelLinkerServeInfo m_channelLkServeInfo;			//渠道分区Linker信息
	ChannelDBServeInfo m_channelDbServeInfo;				//渠道分区DBServer信息
	ServeInfo m_rechargeDBs;						//充值数据库服务器信息
	ServeInfo m_accountDBs;						//帐号数据库服务器信息





};

void InitCfgManObj(const char* cfgName);
CGMCfgMan* GetCfgManObj();
void DestroyCfgManObj();

#endif