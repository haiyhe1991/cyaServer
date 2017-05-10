#ifndef __CONFIG_READER_H__
#define __CONFIG_READER_H__

#include "cyaTypes.h"

class ConfigReader
{
public:
	ConfigReader();
	~ConfigReader();

	INT Read(const char* confPath = NULL);

	const char* GWSAddress() const;
	const char* SelfAddress() const;
	INT GWSPort() const;
	INT GWSConnectTimeout() const;
	INT GWSWriteTimeOut() const;
	INT GWSReadTimeOut() const;
	INT SelfPort() const;
private:

private:
	std::string m_config_path;
	std::string m_gws_addr;
	std::string m_self_addr;
	INT m_gws_port;
	INT m_gws_conn_timeout;
	INT m_gws_write_timeout;
	INT m_gws_read_timeout;
	INT m_self_port;
};

#endif