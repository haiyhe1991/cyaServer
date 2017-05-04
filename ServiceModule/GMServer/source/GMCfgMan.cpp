#include "GMCfgMan.h"
#include "cyaFdk.h"
#include "cyaLog.h"
#include <fstream>
#include "ServiceErrorCode.h"
#include "RandWords.h"

CGMCfgMan* g_gmcfgManObj = NULL;

CGMCfgMan::CGMCfgMan()
	:m_servePort(0)
{
	InitRandWordsObj();
}

CGMCfgMan::~CGMCfgMan()
{

}

BOOL CGMCfgMan::CheckFile(const char* cfgName, Json::Value& jvalue)
{
	std::ifstream ifs;
	if (!IsExistFile(cfgName))
		return false;

	ifs.open(cfgName);
	if (!ifs.is_open())
		return false;


	Json::Reader reader;
	reader.parse(ifs, jvalue);
	ifs.close();
	return true;
}

int CGMCfgMan::Load(const char* cfgName)
{
	Json::Value jvalue;
	if (!CheckFile(cfgName, jvalue))
		return 1;

	if (!jvalue.isMember("serve_port") || !jvalue["serve_port"].isInt())
		return 1;

	if (!jvalue.isMember("onlinerefresh") || !jvalue["onlinerefresh"].isInt())
		return 1;

	if (!jvalue.isMember("realTime") || !jvalue["realTime"].isInt())
		return 1;

	if (!jvalue.isMember("iswriteintodb") || !jvalue["iswriteintodb"].isBool())
		return 1;

	if (!jvalue.isMember("heartbeatinterval") || !jvalue["heartbeatinterval"].isInt())
		return 1;

	if (!jvalue.isMember("istest") || !jvalue["istest"].isBool())
		return 1;

	if (!jvalue["leavekeep"].isArray())
		return 1;



	//解析留存时间类型
	int num = jvalue["leavekeep"].size(); //jison不一样，所以这儿用size（）；原版是getArraySize（）
	for (int i = 0; i < num; i++)
		m_leaveKeepType.push_back(jvalue["leavekeep"][i].asInt());

	//服务器端口
	m_servePort = jvalue["serve_port"].asInt();

	//在线刷新时间间隔(秒)
	m_onlineRefreshTime = jvalue["onlinerefresh"].asInt();

	//实时更新(秒)
	m_realNewTime = jvalue["realTime"].asInt();

	//是否写入数据库
	m_isWriteIntoDb = jvalue["iswriteintodb"].asBool();
	//当前是否测试
	m_isTest = jvalue["istest"].asBool();
	//心跳间隔时间(秒)
	m_heartbeatInterval = jvalue["heartbeatinterval"].asInt();

	//数据库更新时间;
	strcpy(dbstime, jvalue["times"].asCString());

	if (ParseNewDBSCfgMan(jvalue) != GM_OK)
		return 1;
	if (ParseNewLKSCfgMan(jvalue) != GM_OK)
		return 1;
	if (ParseNewRechargeDBSCfgMan(jvalue) != GM_OK)
		return 1;

	if (ParseNewAccountDBSCfgMan(jvalue) != GM_OK)
		return 1;

	return 0;
}

int CGMCfgMan::ParseNewDBSCfgMan(Json::Value& jvalue)
{
	if (!jvalue["platform"].isArray())				//查看平台是否存在;
		return 1;

	int platNum = jvalue["platform"].size(); //获取平台数;//jison不一样，所以这儿用size（）；原版是getArraySize（）

	for (int j = 0; j<platNum; j++)
	{
		int channelNum = jvalue["platform"][j]["channelform"].size();	//获取渠道数量;//jison不一样，所以这儿用size（）；原版是getArraySize（）
		UINT16 platId = jvalue["platform"][j]["platform_id"].asInt(); //平台ID;
		//存放平台号;
		m_platId.push_back(platId);
		for (int i = 0; i < channelNum; i++)
		{

			UINT16 channelform_id = jvalue["platform"][j]["channelform"][i]["channelform_id"].asInt();//读取配置渠道ID;
			//渠道id存放入动态数组Value中;
			m_channel.push_back(channelform_id);
			DBSServeInfo dbServeInfo;
			PartIDVec pidVec;

			ChannelDBServeInfo::iterator it = m_channelDbServeInfo.find(channelform_id);
			if (it != m_channelDbServeInfo.end())
				ASSERT(false); //渠道ID重复

			if (!jvalue["platform"][j]["channelform"][i].isMember("dbsinfo") || !jvalue["platform"][j]["channelform"][i]["dbsinfo"].isArray())
				return 1;
			int dbsNum = jvalue["platform"][j]["channelform"][i]["dbsinfo"].size();//jison不一样，所以这儿用size（）；原版是getArraySize（）
			for (int j1 = 0; j1 < dbsNum; j1++)
			{
				UINT16 partId = jvalue["platform"][j]["channelform"][i]["dbsinfo"][j1]["partid"].asInt();
				pidVec.push_back(partId);
				DBSServeInfo::iterator its = dbServeInfo.find(partId);
				if (its != dbServeInfo.end())
					ASSERT(false); //分区ID重复

				ServeInfo serveInfo;
				memset(&serveInfo, 0, sizeof(ServeInfo));
				strcpy(serveInfo.sIp, jvalue["platform"][j]["channelform"][i]["dbsinfo"][j1]["dbs_ip"].asCString());
				serveInfo.sPort = jvalue["platform"][j]["channelform"][i]["dbsinfo"][j1]["dbs_port"].asInt();
				dbServeInfo.insert(std::make_pair(partId, serveInfo));
			}
			m_channelPartId.insert(std::make_pair(channelform_id, pidVec));		//渠道分区;
			m_channelDbServeInfo.insert(std::make_pair(channelform_id, dbServeInfo));	//渠道数据库;
		}
		///此处可以做指定平台的所有分区;具体做法，可将m_chanelpartId,加入进来;
	}
	return 0;
}

int CGMCfgMan::ParseNewLKSCfgMan(Json::Value& jvalue)
{
	if (!jvalue["platform"].isArray())
		return 1;
	int platNum = jvalue["platform"].size(); //获取平台数;//jison不一样，所以这儿用size（）；原版是getArraySize（）
	for (int jplatNum = 0; jplatNum<platNum; jplatNum++)
	{
		int channelNum = jvalue["platform"][jplatNum]["channelform"].size();	//获取渠道数量;//jison不一样，所以这儿用size（）；原版是getArraySize（）
		//UINT16 platId = jvalue["platform"][jplatNum]["platform_id"].asInt(); //平台ID;
		for (int i = 0; i < channelNum; i++)
		{
			UINT16 channelform_id = jvalue["platform"][jplatNum]["channelform"][i]["channelform_id"].asInt();//读取配置渠道ID;
			ChannelLinkerServeInfo::iterator it = m_channelLkServeInfo.find(channelform_id);
			if (it != m_channelLkServeInfo.end())
				ASSERT(false); //渠道ID重复
			PlatInfo info;
			memset(&info, 0, sizeof(PlatInfo));
			m_channelpartInfo.push_back(info);
			m_channelpartInfo[i].channelid = channelform_id;
			strcpy(m_channelpartInfo[i].channelname, jvalue["platform"][jplatNum]["channelform"][i]["channelname"].asCString());

			LKServeInfo lkServeInfo;
			int partNum = jvalue["platform"][jplatNum]["channelform"][i]["lksinfo"].size();//jison不一样，所以这儿用size（）；原版是getArraySize（）
			for (int j = 0; j < partNum; j++)
			{

				ServeInfoVec infoVec;
				UINT16 partId = jvalue["platform"][jplatNum]["channelform"][i]["lksinfo"][j]["partid"].asInt();
				m_channelpartInfo[i].partId.push_back(partId);
				int lkippNum = jvalue["platform"][jplatNum]["channelform"][i]["lksinfo"][j]["lksipp"].size();//jison不一样，所以这儿用size（）；原版是getArraySize（）
				LKServeInfo::iterator its = lkServeInfo.find(partId);
				if (its != lkServeInfo.end())
					ASSERT(false); //分区ID重复

				for (int k = 0; k < lkippNum; k++)
				{
					ServeInfo serveInfo;
					memset(&serveInfo, 0, sizeof(ServeInfo));
					infoVec.push_back(serveInfo);
					strcpy(infoVec[k].sIp, jvalue["platform"][jplatNum]["channelform"][i]["lksinfo"][j]["lksipp"][k]["lk_ip"].asCString());
					infoVec[k].sPort = jvalue["platform"][jplatNum]["channelform"][i]["lksinfo"][j]["lksipp"][k]["lk_port"].asInt();
				}
				lkServeInfo.insert(std::make_pair(partId, infoVec));		//linkServer信息;
			}
			m_channelLkServeInfo.insert(std::make_pair(channelform_id, lkServeInfo));	//渠道linkServer信息;
		}
	}
	return 0;
}

int CGMCfgMan::ParseNewRechargeDBSCfgMan(Json::Value& jvalue)
{
	memset(&m_rechargeDBs, 0, sizeof(ServeInfo));

	if (!jvalue.isMember("rechargedbsinfo") || !jvalue["rechargedbsinfo"].isObject())
		return 1;

	if (!jvalue["rechargedbsinfo"].isMember("dbs_ip") || !jvalue["rechargedbsinfo"]["dbs_ip"].isString())
		return 1;

	if (!jvalue["rechargedbsinfo"].isMember("dbs_port") || !jvalue["rechargedbsinfo"]["dbs_port"].isInt())
		return 1;

	strcpy(m_rechargeDBs.sIp, jvalue["rechargedbsinfo"]["dbs_ip"].asCString()); //充值数据库服务器IP
	m_rechargeDBs.sPort = jvalue["rechargedbsinfo"]["dbs_port"].asInt(); //充值数据库服务器端口
	return 0;
}

int CGMCfgMan::ParseNewAccountDBSCfgMan(Json::Value& jvalue)
{
	memset(&m_accountDBs, 0, sizeof(ServeInfo));

	if (!jvalue.isMember("accountdbsinfo") || !jvalue["accountdbsinfo"].isObject())
		return 1;

	if (!jvalue["accountdbsinfo"].isMember("dbs_ip") || !jvalue["accountdbsinfo"]["dbs_ip"].isString())
		return 1;

	if (!jvalue["accountdbsinfo"].isMember("dbs_port") || !jvalue["accountdbsinfo"]["dbs_port"].isInt())
		return 1;

	strcpy(m_accountDBs.sIp, jvalue["accountdbsinfo"]["dbs_ip"].asCString()); //充值数据库服务器IP
	m_accountDBs.sPort = jvalue["accountdbsinfo"]["dbs_port"].asInt(); //充值数据库服务器端口
	return 0;
}

void InitCfgManObj(const char* cfgName)
{
	if (g_gmcfgManObj != NULL)
		return;
	g_gmcfgManObj = new CGMCfgMan();
	ASSERT(g_gmcfgManObj != NULL);
	int ret = g_gmcfgManObj->Load(cfgName);
	if (ret)
		LogInfo(("加载配置文件[%s]失败!", cfgName));
	else
		LogInfo(("加载配置文件[%s]成功!", cfgName));
}
CGMCfgMan* GetCfgManObj()
{
	return g_gmcfgManObj;
}

void DestroyCfgManObj()
{
	if (g_gmcfgManObj == NULL)
		return;
	delete g_gmcfgManObj;
	g_gmcfgManObj = NULL;
}