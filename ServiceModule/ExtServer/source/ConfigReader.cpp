#include <fstream>

#include "json.h"

#include "cyaLog.h"

#include "ServiceErrorCode.h"
#include "ConfigReader.h"

ConfigReader::ConfigReader()
: m_config_path("./riseextserver.properties")
, m_gws_addr("")
, m_self_addr("0.0.0.0")
, m_gws_port(1020)
, m_gws_write_timeout(0)
, m_gws_read_timeout(0)
, m_self_port(9530)
{

}

ConfigReader::~ConfigReader()
{

}

INT ConfigReader::Read(const char* confPath/* = NULL*/)
{
	if (confPath)
	{
		m_config_path = confPath;
	}

	std::ifstream fileStream;
	fileStream.open(m_config_path.c_str());
	if (!fileStream.is_open())
	{
		LogInfo(("打开配置文件(%s)失败", m_config_path.c_str()));
		return EXT_CONFIG_INIT;//MANS_CONFIG_OPEN;
	}

	Json::Value rootValue;
	Json::Reader valueReader;

	BOOL parseSuccess = valueReader.parse(fileStream, rootValue);
	fileStream.close();
	if (!parseSuccess)
	{
		LogInfo(("解析配置文件失败，可能是格式不正确"));
		return EXT_CONFIG_INIT;//MANS_CONFIG_FORMAT;
	}

	if (rootValue.isMember("gws_addr") && rootValue["gws_addr"].isString())
	{
		m_gws_addr = rootValue["gws_addr"].asString();
	}
	if (rootValue.isMember("self_addr") && rootValue["self_addr"].isString())
	{
		m_self_addr = rootValue["self_addr"].asString();
	}
	if (rootValue.isMember("gws_port") && rootValue["gws_port"].isInt())
	{
		m_gws_port = rootValue["gws_port"].asInt();
	}
	if (rootValue.isMember("gws_con_timeout") && rootValue["gws_con_timeout"].isInt())
	{
		m_gws_conn_timeout = rootValue["gws_con_timeout"].asInt();
	}
	if (rootValue.isMember("gws_read_timeout") && rootValue["gws_read_timeout"].isInt())
	{
		m_gws_read_timeout = rootValue["gws_read_timeout"].asInt();
	}
	if (rootValue.isMember("gws_write_timeout") && rootValue["gws_write_timeout"].isInt())
	{
		m_gws_write_timeout = rootValue["gws_write_timeout"].asInt();
	}
	if (rootValue.isMember("self_port") && rootValue["self_port"].isInt())
	{
		m_self_port = rootValue["self_port"].asInt();
	}

	return m_gws_addr.empty();
}

const char* ConfigReader::GWSAddress() const
{
	return m_gws_addr.c_str();
}

const char* ConfigReader::SelfAddress() const
{
	return m_self_addr.c_str();
}

INT ConfigReader::GWSPort() const
{
	return m_gws_port;
}

INT ConfigReader::GWSConnectTimeout() const
{
	return m_gws_conn_timeout;
}

INT ConfigReader::GWSWriteTimeOut() const
{
	return m_gws_write_timeout;
}

INT ConfigReader::GWSReadTimeOut() const
{
	return m_gws_read_timeout;
}

INT ConfigReader::SelfPort() const
{
	return m_self_port;
}
