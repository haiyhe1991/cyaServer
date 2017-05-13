#include <fstream>

#include "cyaFdk.h"
#include "cyaLog.h"
#include "cyaIpCvt.h"
#include "json.h"

#include "RoomConfig.h"

#define CONFIG_PATH	"./cyaRoomServer.properties"
#define INVALID_PORT		0
#define INVALID_ADDR		""

CRoomConfig* CRoomConfig::m_this = NULL;

CRoomConfig* CRoomConfig::FetchConfigInstance()
{
	if (NULL == m_this)
		m_this = new CRoomConfig();
	return m_this;
}

void CRoomConfig::ReleaseConfigInstance()
{
	if (NULL != m_this)
	{
		delete m_this;
		m_this = NULL;
	}
}

CRoomConfig::CRoomConfig()
	: m_nLocalListenPort(9610)
	, m_nRoomServerPort(INVALID_PORT)
	, m_nGWServerPort(INVALID_PORT)
	, m_strRoomServerAddr(INVALID_ADDR)
	, m_strGWServerAddr(INVALID_ADDR)
{
	InitializeConfig();
}

CRoomConfig::~CRoomConfig()
{

}

INT CRoomConfig::GetLocalListenPort() const
{
	if (m_nLocalListenPort <= INVALID_PORT)
		return INVALID_PORT;
	return m_nLocalListenPort;
}

INT CRoomConfig::GetRoomServerPort() const
{
	if (m_nRoomServerPort <= INVALID_PORT)
		return INVALID_PORT;
	return m_nRoomServerPort;
}

DWORD CRoomConfig::GetRoomServerAddr() const
{
	if (m_strRoomServerAddr == INVALID_ADDR)
		return INVALID_PORT;
	const char* pRoomServerAddr = m_strRoomServerAddr.c_str();
	if (NULL == pRoomServerAddr)
		return INVALID_PORT;
	return GetDWordIP(pRoomServerAddr);
}

INT CRoomConfig::GetGWServerPort() const
{
	if (m_nGWServerPort <= INVALID_PORT)
		return INVALID_PORT;
	return m_nGWServerPort;
}

const char* CRoomConfig::GetGWServerAddr() const
{
	return m_strGWServerAddr.c_str();
}

const char* CRoomConfig::GetRoomServerStrAddr() const
{
	return m_strRoomServerAddr.c_str();
}


void CRoomConfig::InitializeConfig()
{
	if (!IsExistFile(CONFIG_PATH))
	{
		LogInfo(("�����ļ�������"));
		return;
	}

	std::ifstream ifilestream;
	ifilestream.open(CONFIG_PATH);
	if (!ifilestream.is_open())
	{
		LogInfo(("�����ļ���ʧ��"));
		return;
	}

	Json::Value iRootJson;
	Json::Reader iJsonReader;
	bool bParseRet = iJsonReader.parse(ifilestream, iRootJson);
	ifilestream.close();
	if (!bParseRet)
	{
		LogInfo(("�����ļ�(%s)����ʧ�ܣ����ܸ�ʽ����ȷ"));
		return;
	}

	if (iRootJson.isMember("LocalPort") && iRootJson["LocalPort"].isInt())
	{
		m_nLocalListenPort = iRootJson["LocalPort"].asInt();
	}
	else
	{
		LogInfo(("���ؼ����˿���������"));
	}

	if (iRootJson.isMember("ServerPort") && iRootJson["ServerPort"].isInt())
	{
		m_nRoomServerPort = iRootJson["ServerPort"].asInt();
	}
	else
	{
		LogInfo(("RoomServer�����˿���������"));
	}

	if (iRootJson.isMember("ServerAddr") && iRootJson["ServerAddr"].isString())
	{
		m_strRoomServerAddr = iRootJson["ServerAddr"].asString();
	}
	else
	{
		LogInfo(("RoomServer��ַ��������"));
	}

	if (iRootJson.isMember("gwaddr") && iRootJson["gwaddr"].isString())
	{
		m_strGWServerAddr = iRootJson["gwaddr"].asString();
	}
	else
	{
		LogInfo(("���ط�������ַ��������"));
	}

	if (iRootJson.isMember("gwport") && iRootJson["gwport"].isInt())
	{
		m_nGWServerPort = iRootJson["gwport"].asInt();
	}
	else
	{
		LogInfo(("���ط������˿���������"));
	}
}
