#include "LvRankingCfg.h"
#include "cyaTime.h"

#define JSON_ROOT_ISNULL(Title)	if(!rootValue.isMember(Title)) return MLS_NOT_EXIST_ACTIVITY;

#define JSON_ROOT_INT(parm,Title) \
if (!rootValue.isMember(Title)) return MLS_NOT_EXIST_ACTIVITY;	\
	else parm = rootValue[Title].asInt();

#define JSON_ROOT_UINT(parm,Title) \
if (!rootValue.isMember(Title)) return MLS_NOT_EXIST_ACTIVITY;	\
	else parm = rootValue[Title].asUInt();

#define JSON_ROOT_STR(parm,Title) \
if (!rootValue.isMember(Title)) return MLS_NOT_EXIST_ACTIVITY;	\
	else parm = rootValue[Title].asCString();

#define JSON_ROOT_BOOL(parm,Title) \
if (!rootValue.isMember(Title)) return MLS_NOT_EXIST_ACTIVITY;	\
	else parm = rootValue[Title].isBool();

LvRankingCfg::LvRankingCfg()
:m_IsValidity(true)
, m_IsOpen(false)
, m_EndTime(0)
{

}


LvRankingCfg::~LvRankingCfg(void)
{

}

BOOL LvRankingCfg::Load(const char* filename)
{
	if (filename == NULL)
		return false;
	m_filename = filename;

	Json::Value rootValue;
	if (!OpenCfgFile(m_filename.c_str(), rootValue))
		return false;

	JSON_ROOT_BOOL(m_IsOpen, "IsOpen");
	std::string strTime;
	JSON_ROOT_STR(strTime, "StartTime");
	m_StartTime = StrToMsel(strTime.c_str());
	JSON_ROOT_INT(m_EndTime, "EndTime");


	if (!rootValue.isMember("LvRanking") || !rootValue["LvRanking"].isArray())
		return false;

	int si = rootValue["LvRanking"].size();
	for (int i = 0; i < si; ++i)
	{
		SLvRankingCV  sRankCV;

		UINT32 id = rootValue["LvRanking"][i]["Ranking"].asUInt();
		sRankCV.cvalue = rootValue["LvRanking"][i]["ConditionValue"].asUInt();

		int n = rootValue["LvRanking"][i]["Rewards"].size();
		for (int j = 0; j < n; ++j)
		{
			SDropPropCfgItem prop;
			prop.num = rootValue["Activity"][i]["Rewards"][j]["Num"].asUInt();
			prop.propId = rootValue["Activity"][i]["Rewards"][j]["ID"].asUInt();
			prop.type = rootValue["Activity"][i]["Rewards"][j]["Type"].asUInt();
			sRankCV.rewardvs.push_back(prop);
		}

		m_LvRewardsMap.insert(std::make_pair(id, sRankCV));
	}
	return true;
}

//是否是活动时间，返回TRUE表示活动继续，FLASE表示无法活动
BOOL LvRankingCfg::IsValidityTime()
{
	if (!m_IsValidity)
		return m_IsValidity;
	LTMSEL nowMSel = GetMsel();
	m_IsValidity = (nowMSel - m_StartTime - m_EndTime * 60 * 60 * 1000)>0 ? false : true;
	return m_IsValidity;
}

BOOL LvRankingCfg::GetLvRanking(const BYTE rank, const int Lv, std::vector<SDropPropCfgItem>*& items)
{
	std::map<BYTE, SLvRankingCV>::const_iterator itr = m_LvRewardsMap.find(rank);
	if (itr != m_LvRewardsMap.end())
	{
		//如果玩家的等级小于等级要求
		if (Lv >= ((SLvRankingCV)itr->second).cvalue)
			return true;
	}
	return false;
}