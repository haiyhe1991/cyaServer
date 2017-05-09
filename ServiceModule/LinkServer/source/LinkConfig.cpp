#include <fstream>
#include "LinkConfig.h"
#include "json.h"
#include "cyaLog.h"
#include "cyaFdk.h"
#include "ServiceErrorCode.h"

#define CONFIG_FILE_PATH    "./riselinkserver.properties"
#define INVALID_PORT        0
#define INVALID_ADDR        ""
/// LINK服务器Json字段
#define CONF_LINK_INVALIDIP "link_invalid_ip"
#define CONF_LINK_ADDR  "link_addr"
#define CONF_LINK_PORT  "link_port"
#define CONF_LINK_NUM   "link_num"
#define CONF_LINK_HEART "link_heart"
#define CONF_LINK_CLEAN "link_clean"
/// 网关服务器Json字段
#define CONF_GWS_ADDR   "gws_addr"
#define CONF_GWS_PORT   "gws_port"
/// 验证服务器Json字段
#define CONF_LS_ADDR    "ls_addr"
#define CONF_LS_PORT    "ls_port"
/// 分区管理服务器Json字段
#define CONF_MAN_ADDR   "man_addr"
#define CONF_MAN_PORT   "man_port"
/// 验证服务器通信Token
#define CONF_LS_TOKEN   "ls_token"
#define CONF_LS_REPORT  "report_linknum"
#define CONF_LS_PATITION "ls_patition"

#define LINK_SERVER_NAME    "LinkServer"

LinkConfig::LinkConfig()
	: m_LinkPort(9528)  /// LinkServer默认端口
	, m_LinkNum(0)      /// LinkServer默认的服务器编号
	, m_GWSPort(10020)  /// 网关服务器默认端口
	, m_LSPort(10030)   /// 验证服务器默认端口
	, m_ManPort(10030)  /// 分区管理服务默认端口
	, m_LSReport(300000)/// 默认上报LinkServer客户端连接数时间间隔为5分钟
	, m_PartitionId(0)  /// 所属分区
	, m_LinkHeartSec(3600)  /// 默认心跳间隔时间
	, m_LinkClean(60000)    /// 客户端超时时间
	, m_LinkIpInvalid(false)/// LinkServer地址是网卡上的IP时为false(default), 不是网卡上的IP时为true
{

}

LinkConfig::~LinkConfig()
{

}

/// 初始化配置信息
int LinkConfig::InitConfig(const PathName cfgPath)
{
	return ReadConfigFile(cfgPath) ? LINK_OK : LINK_CONFIG_INIT;
}

/// 获取LinkServer信息
BOOL LinkConfig::GetLinkInvalidIP() const
{
	return m_LinkIpInvalid;
}

const char* LinkConfig::GetLinkAddr() const
{
	return m_LinkAddr.c_str();
}

int LinkConfig::GetLinkListenPort() const
{
	if (m_LinkPort <= INVALID_PORT)
		return INVALID_PORT;
	return m_LinkPort;
}

int LinkConfig::GetLinkNumber() const
{
	return m_LinkNum;
}

int LinkConfig::GetClientHeartBeatTimeout() const
{
	return m_LinkHeartSec;
}

/// 获取网关服务器信息
const char* LinkConfig::GetGWSAddr() const
{
	return m_GWSAddr.c_str();
}

int LinkConfig::GetGWSPort() const
{
	if (m_GWSPort <= INVALID_PORT)
		return INVALID_PORT;
	return m_GWSPort;
}

const char* LinkConfig::GetLSAddr() const
{
	return m_LSAddr.c_str();
}

int LinkConfig::GetLSPort() const
{
	if (m_LSPort <= INVALID_PORT)
		return INVALID_PORT;
	return m_LSPort;
}

const char* LinkConfig::GetManAddr() const
{
	return m_ManAddr.c_str();
}

int LinkConfig::GetManPort() const
{
	if (m_ManPort <= INVALID_PORT)
		return INVALID_PORT;
	return m_ManPort;
}

const char* LinkConfig::GetLSToken() const
{
	return m_LSToken.c_str();
}

int LinkConfig::GetLSReportInvterval() const
{
	return m_LSReport;
}

int LinkConfig::GetLinkCleanInvterval() const
{
	return m_LinkClean;
}

int LinkConfig::GetLinkerPartitionID() const
{
	return m_PartitionId;
}

bool LinkConfig::ReadConfigFile(const PathName cfgPath)
{
	if (!IsExistFile(cfgPath))
	{
		LogInfo(("配置文件不存在"));
		return false;
	}
	Json::Value rootJson;
	Json::Reader myJsonReader;
	std::ifstream ifilestream;
	const char* filePath = cfgPath;
	ifilestream.open(filePath);
	if (!ifilestream.is_open())
	{
		LogInfo(("%s无法打开配置文件", cfgPath));
		return false;
	}
	bool bParseRet = myJsonReader.parse(ifilestream, rootJson);
	ifilestream.close();
	if (!bParseRet)
	{
		LogInfo(("%s 读取配置文件(%s)失败", LINK_SERVER_NAME, filePath));
		return false;
	}

	/// 获取LinkServer自己的配置信息
	if (rootJson.isMember(CONF_LINK_ADDR) && rootJson.isMember(CONF_LINK_PORT) && rootJson.isMember(CONF_LINK_NUM)
		&& rootJson[CONF_LINK_ADDR].isString() && rootJson[CONF_LINK_PORT].isInt() && rootJson[CONF_LINK_NUM].isInt())
	{
		m_LinkAddr = rootJson[CONF_LINK_ADDR].asString();
		m_LinkPort = rootJson[CONF_LINK_PORT].asInt();
		m_LinkNum = rootJson[CONF_LINK_NUM].asInt();

		if (rootJson.isMember(CONF_LINK_INVALIDIP) && rootJson[CONF_LINK_INVALIDIP].isBool())
			m_LinkIpInvalid = !!rootJson[CONF_LINK_INVALIDIP].asBool();
	}
	if (rootJson.isMember(CONF_LINK_HEART) && rootJson[CONF_LINK_HEART].isInt())
	{
		m_LinkHeartSec = rootJson[CONF_LINK_HEART].asInt();
	}
	/// 获取网关服务器的地址信息
	if (rootJson.isMember(CONF_GWS_ADDR) && rootJson.isMember(CONF_GWS_PORT)
		&& rootJson[CONF_GWS_ADDR].isString() && rootJson[CONF_GWS_PORT].isInt())
	{
		m_GWSAddr = rootJson[CONF_GWS_ADDR].asString();
		m_GWSPort = rootJson[CONF_GWS_PORT].asInt();
	}
	/// 获取验证服务器地址信息
	if (rootJson.isMember(CONF_LS_ADDR) && rootJson.isMember(CONF_LS_PORT)
		&& rootJson[CONF_LS_ADDR].isString() && rootJson[CONF_LS_PORT].isInt())
	{
		m_LSAddr = rootJson[CONF_LS_ADDR].asString();
		m_LSPort = rootJson[CONF_LS_PORT].asInt();
	}
	/// 获取分区管理服务器地址信息
	if (rootJson.isMember(CONF_MAN_ADDR) && rootJson.isMember(CONF_MAN_PORT)
		&& rootJson[CONF_MAN_ADDR].isString() && rootJson[CONF_MAN_PORT].isInt())
	{
		m_ManAddr = rootJson[CONF_MAN_ADDR].asString();
		m_ManPort = rootJson[CONF_MAN_PORT].asInt();
	}
	/// 读验证Token
	if (rootJson.isMember(CONF_LS_TOKEN) && rootJson[CONF_LS_TOKEN].isString())
	{
		m_LSToken = rootJson[CONF_LS_TOKEN].asString();
	}
	/// 读取上报连接数的时间间隔
	if (rootJson.isMember(CONF_LS_REPORT) && rootJson[CONF_LS_REPORT].isInt())
	{
		m_LSReport = rootJson[CONF_LS_REPORT].asInt();
	}
	/// 读取分区信息
	if (rootJson.isMember(CONF_LS_PATITION) && rootJson[CONF_LS_PATITION].isInt())
	{
		m_PartitionId = rootJson[CONF_LS_PATITION].asInt();
	}
	if (rootJson.isMember(CONF_LINK_CLEAN) && rootJson[CONF_LINK_CLEAN].isInt())
	{
		m_LinkClean = rootJson[CONF_LINK_CLEAN].asInt();
	}

	if (m_LinkAddr.empty())
		LogInfo(("%s配置信息有误", LINK_SERVER_NAME));
	if (m_GWSAddr.empty())
		LogInfo(("网关服务器配置信息有误"));
	if (m_LSAddr.empty())
		LogInfo(("验证服务器配置信息有误"));
	if (m_ManAddr.empty())
		LogInfo(("分区管理服务器配置信息有误"));
	if (m_LSToken.empty())
		LogInfo(("验证服务器配置TOKEN信息有误"));
	return !m_LinkAddr.empty() && !m_GWSAddr.empty() && !m_LSAddr.empty() && !m_ManAddr.empty() && !m_LSToken.empty();
}
