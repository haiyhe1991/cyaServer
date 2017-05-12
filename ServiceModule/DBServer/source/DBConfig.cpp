#include <fstream>
#include "json.h"
#include "cyaLog.h"
#include "DBConfig.h"

#define CONFIG_DBS_ADDR		"dbs_addr"
#define CONFIG_DBS_PORT		"dbs_port"
#define CONFIG_SQL_ADDR		"sql_addr"
#define CONFIG_SQL_PORT		"sql_port"
#define CONFIG_SQL_USER		"sql_user"
#define CONFIG_SQL_PSWD		"sql_pswd"
#define CONFIG_DB_NAME		"db_name"
#define CONFIG_DB_NAMES		"db_names"
#define CONFIG_SQL_CHARSET	"sql_charset"

#define CONFIG_PATH			"./cyaDBServer.properties"

DBConfig::DBConfig()
	: m_dbs_port(9527)
	, m_sql_port(3306)
	, m_sql_count(0)
{

}

DBConfig::~DBConfig()
{
	FinishConfig();
}

BOOL DBConfig::InitializeConfig()
{
	std::ifstream ifilestream;
	ifilestream.open(CONFIG_PATH);

	if (!ifilestream.is_open())
	{
		LogInfo(("打开DBServer配置文件失败"));
		return FALSE;
	}

	Json::Reader myJsonReader;
	Json::Value jsonRoot;
	bool bParseRet = myJsonReader.parse(ifilestream, jsonRoot);
	ifilestream.close();
	if (!bParseRet)
	{
		LogInfo(("DBServer解析配置文件失败"));
		return FALSE;
	}

	if (jsonRoot.isMember(CONFIG_DBS_ADDR) && jsonRoot[CONFIG_DBS_ADDR].isString())
		m_dbs_addr = jsonRoot[CONFIG_DBS_ADDR].asString();

	if (jsonRoot.isMember(CONFIG_DBS_PORT) && jsonRoot[CONFIG_DBS_PORT].isInt())
		m_dbs_port = jsonRoot[CONFIG_DBS_PORT].asInt();

	if (jsonRoot.isMember(CONFIG_SQL_ADDR) && jsonRoot[CONFIG_SQL_ADDR].isString())
		m_sql_addr = jsonRoot[CONFIG_SQL_ADDR].asString();

	if (jsonRoot.isMember(CONFIG_SQL_PORT) && jsonRoot[CONFIG_SQL_PORT].isInt())
		m_sql_port = jsonRoot[CONFIG_SQL_PORT].asUInt();

	if (jsonRoot.isMember(CONFIG_SQL_USER) && jsonRoot[CONFIG_SQL_USER].isString())
		m_sql_user = jsonRoot[CONFIG_SQL_USER].asString();

	if (jsonRoot.isMember(CONFIG_SQL_PSWD) && jsonRoot[CONFIG_SQL_PSWD].isString())
	{
		m_sql_pswd = jsonRoot[CONFIG_SQL_PSWD].asString();
	}

	if (jsonRoot.isMember(CONFIG_DB_NAMES) && jsonRoot[CONFIG_DB_NAMES].isArray())
	{
		m_sql_count = 0;
		INT count = jsonRoot[CONFIG_DB_NAMES].size();
		for (int i = 0; i<count; ++i)
		{
			if (jsonRoot[CONFIG_DB_NAMES][i].isMember(CONFIG_DB_NAME) && jsonRoot[CONFIG_DB_NAMES][i][CONFIG_DB_NAME].isString())
			{
				m_sql_count++;
				m_sql_names.push_back(jsonRoot[CONFIG_DB_NAMES][i][CONFIG_DB_NAME].asString());
			}
		}
	}

	if (jsonRoot.isMember(CONFIG_SQL_CHARSET) && jsonRoot[CONFIG_SQL_CHARSET].isString())
	{
		m_sql_charset = jsonRoot[CONFIG_SQL_CHARSET].asString();
	}

	return !m_sql_names.empty() && !m_sql_addr.empty() && !m_sql_user.empty() && !m_sql_charset.empty();
}

void DBConfig::FinishConfig()
{
	m_dbs_addr.clear();
	m_dbs_port = 9527;
	m_sql_addr.clear();
	m_sql_port = 3306;
	m_sql_user.clear();
	m_sql_pswd.clear();
	m_sql_names.clear();
}

const char* DBConfig::DBS_Addr() const
{
	return m_dbs_addr.c_str();
}

INT DBConfig::DBS_Port() const
{
	return m_dbs_port;
}

const char* DBConfig::DBS_SQL_Addr() const
{
	return m_sql_addr.c_str();
}

INT DBConfig::DBS_SQL_Port() const
{
	return m_sql_port;
}

const char* DBConfig::DBS_SQL_User() const
{
	return m_sql_user.c_str();
}

const char* DBConfig::DBS_SQL_Pswd() const
{
	return m_sql_pswd.c_str();
}

const char* DBConfig::DBS_DB_Name(INT Indx) const
{
	return m_sql_names[Indx].c_str();
}

INT DBConfig::DBS_DB_Count() const
{
	return m_sql_count;
}

const char* DBConfig::DBS_SQL_Charset() const
{
	return m_sql_charset.c_str();
}

DBConfig& GetDBConfigObj()
{
	BEGIN_LOCAL_SAFE_STATIC_OBJ(DBConfig, dbsConfig);
	END_LOCAL_SAFE_STATIC_OBJ(dbsConfig);
}
