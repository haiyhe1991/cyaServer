#ifndef __LINK_CONFIG_H__
#define __LINK_CONFIG_H__

#include <string>
#include "cyaTypes.h"

#define SEPARATOR(x) 1

class LinkConfig
{
public:
	LinkConfig();
	~LinkConfig();

	/// 初始化配置信息
	int InitConfig(const PathName cfgPath);

	///*********** 获取LinkServer信息 **************/
	BOOL GetLinkInvalidIP() const;
	/// LinkServer地址
	const char* GetLinkAddr() const;
	/// LinkServer端口
	int GetLinkListenPort() const;
	/// LinkServer的编号
	int GetLinkNumber() const;
	/// 客户端心跳超时时间 单位(毫秒)
	int GetClientHeartBeatTimeout() const;

	///*********** 获取网关服务器信息 **************/
	const char* GetGWSAddr() const;
	int GetGWSPort() const;

	/// 获取验证服务器信息
	const char* GetLSAddr() const;
	int GetLSPort() const;
	/// 获取验证服务器通信的Token
	const char* GetLSToken() const;
	/// 上报连接数时间间隔 单位(毫秒)
	int GetLSReportInvterval() const;
	/// 清理超时Token的时间间隔 单位(毫秒)
	int GetLinkCleanInvterval() const;

	/// 获取分区管理服务器信息
	const char* GetManAddr() const;
	int GetManPort() const;

	/// 获取分区ID
	int GetLinkerPartitionID() const;
private:
	bool ReadConfigFile(const PathName cfgPath);

private:
	int m_LinkPort; /// LinkServer端口
	int m_LinkNum;  /// LinkServer的服务器编号
	int m_GWSPort;  /// 网关服务器端口
	int m_LSPort;   /// 验证服务器端口
	int m_ManPort;  /// 分区管理服务端口
	int m_LSReport; /// 上报LinkServer客户端连接数de时间间隔(单位:毫秒)
	int m_PartitionId;  /// 所属分区ID
	int m_LinkHeartSec; /// 心跳间隔
	int m_LinkClean;    /// 客户端超时时间 毫秒
	BOOL m_LinkIpInvalid;   /// LinkServer地址是网卡上的IP时为false(default), 不是网卡上的IP时为true

	std::string m_LinkAddr; /// LinkServer地址
	std::string m_GWSAddr;  /// 网关服务器地址
	std::string m_LSAddr;   /// 验证服务器地址
	std::string m_ManAddr;  /// 分区管理地址
	std::string m_LSToken;  /// 验证服务器验证的Token
};

#endif