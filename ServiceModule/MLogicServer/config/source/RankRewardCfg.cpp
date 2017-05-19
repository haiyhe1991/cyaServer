#include "RankRewardCfg.h"
#include "cyaTime.h"
#include "cyaLog.h"

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
	else parm = rootValue[Title].asInt() != 0;

RankRewardCfg::RankRewardCfg()
{
}


RankRewardCfg::~RankRewardCfg(void)
{

}

BOOL RankRewardCfg::Load(std::vector<std::string> vnames)
{
	int len = vnames.size();

	for (int i = 0; i<len; i++)
	{
		if (!m_rankReward[i].Load(vnames[i].c_str()))
		{
			LogInfo(("载入奖励配置[%s]失败!", vnames[i].c_str()));
		}
	}
	return true;
}

//是否是活动时间，返回TRUE表示活动继续，FLASE表示无法活动
BOOL RankRewardCfg::IsValidityTime(const BYTE type)
{
	return m_rankReward[type - 1].bValiTime();
}

BOOL RankRewardCfg::IsOpen(const BYTE type) const
{
	return m_rankReward[type - 1].bOpen();
}

void RankRewardCfg::SetOpen(const BYTE type, const BOOL bOpen)
{
	m_rankReward[type - 1].SetOpen(bOpen);
}

BOOL RankRewardCfg::IsCanGet(const BYTE type, const UINT16 id, const UINT32 value) const
{
	return m_rankReward[type - 1].IsCanGet(id, value);
}

BOOL CRankReward::IsCanGet(const UINT16 id, const UINT32 value) const
{
	std::map<UINT32, SRkRewardRes>::const_iterator it = m_LvRewardsMap.find(id);
	if (it == m_LvRewardsMap.end())
		return false;

	ASSERT(printf("nowvalue=%u,getvalue=%u\n", value, it->second.Value));

	if (it->second.Value == 0 || it->second.Value <= value)
	{
		return true;
	}
	return false;
}


BOOL RankRewardCfg::GetRankReward(const BYTE type, const UINT16 rankid, CRankReward::SRkRewardRes*& items, const int iValue)
{
	return m_rankReward[type - 1].GetReward(rankid, iValue, items);
}

INT RankRewardCfg::GetRewardPlayNum(const BYTE type) const
{
	return m_rankReward[type - 1].GetRewardNum();
}

///////////////////////////////////////

//////////////////////////////////////


//是否是活动时间，返回TRUE表示活动继续，FLASE表示无法活动
BOOL CRankReward::bValiTime()
{
	if (!m_IsValidity)
		return m_IsValidity;
	LTMSEL nowMSel = GetMsel();
	if (nowMSel>m_EndTime)
		m_IsValidity = false;
	return m_IsValidity;
}

////获取对应奖励的奖励物品
BOOL CRankReward::GetReward(const UINT16 rankId, const int iValue, CRankReward::SRkRewardRes*& items)
{
	//if(!m_IsValidity)
	//	return false;
	std::map<UINT32, SRkRewardRes>::const_iterator itr = m_LvRewardsMap.find(rankId);
	if (itr != m_LvRewardsMap.end())
	{
		//如果玩家的等级小于等级要求
		//if(iValue<((SRkRewardRes)itr->second).valueList[rankId]&&((SRkRewardRes)itr->second).valueList[rankId]!=0)			
		//	return false;

		items = (SRkRewardRes*)&itr->second;
		return true;
	}
	return false;
}

BOOL CRankReward::Load(const char* fname)
{
	const char sID[] = "RankID";
	const char sRk[] = "Ranking";
	std::string name = fname;
	std::string str;
	int pos = name.find("OnLine.z");
	if (pos >= 0)
		str = sRk;
	else
		str = sID;

	if (fname == NULL)
	{
		//如果无配置文件，则功能不开放
		m_IsOpen = FALSE;
		return false;
	}
	m_fileName = fname;

	Json::Value rootValue;
	if (!OpenCfgFile(m_fileName.c_str(), rootValue))
		return false;
	JSON_ROOT_BOOL(m_IsOpen, "IsOpen");
	//m_IsOpen = rootValue["IsOpen"].asInt() != 0;	
	std::string strTime;
	JSON_ROOT_STR(strTime, "StartTime");
	m_StartTime = StrToMsel(strTime.c_str());
	JSON_ROOT_STR(strTime, "EndTime");
	m_EndTime = StrToMsel(strTime.c_str());

	//if(!rootValue.isMember("Ranking") )
	//	return false;
	if (!rootValue["Data"].isArray())
		return false;

	int si = rootValue["Data"].size();
	for (int i = 0; i < si; ++i)
	{
		SRkRewardRes  sres;
		UINT32 id = 0;

#define ID_STR(str) id=rootValue["Data"][i][str].asUInt();
		ID_STR(str.c_str());
		//		UINT32 id = rootValue["Data"][i][str.c_str].asUInt();
		sres.Value = rootValue["Data"][i]["ConditionValue"].asUInt();

		int n = rootValue["Data"][i]["Rewards"].size();
		for (int j = 0; j < n; ++j)
		{
			SDropPropCfgItem prop;
			prop.num = rootValue["Data"][i]["Rewards"][j]["NUM"].asUInt();
			prop.propId = rootValue["Data"][i]["Rewards"][j]["ID"].asUInt();
			prop.type = rootValue["Data"][i]["Rewards"][j]["TYPE"].asUInt();
			sres.rewardvs.push_back(prop);
		}

		m_LvRewardsMap.insert(std::make_pair(id, sres));
	}
	return true;
}

int CRankReward::GetValue(UINT16 id)
{
	std::map<UINT32, SRkRewardRes>::const_iterator it = m_LvRewardsMap.find(id);

	if (it == m_LvRewardsMap.end())
	{
		return 0;
	}
	return it->second.Value;
}

int RankRewardCfg::GetLoginfoName(std::string& logstr, BYTE type, UINT16 reward)
{
	char logsz[64] = { 0 };
	switch (type)
	{
	case __Level_Rank_Type:
		sprintf(logsz, "Type:%s,Value:%d", C_REWARD_INFO[type - 1], m_rankReward[type - 1].GetValue(reward));
		break;
	case __InstanceNum_Rank_Type:
		sprintf(logsz, "Type:%s,Value:%d", C_REWARD_INFO[type - 1], m_rankReward[type - 1].GetValue(reward));
		break;
	case __Stronger_Reward_Type:
		sprintf(logsz, "Type:%s,Value:%d", C_REWARD_INFO[type - 1], m_rankReward[type - 1].GetValue(reward));
		break;
	case __Online_Reward_Type:
		sprintf(logsz, "Type:%s,Value:%d", C_REWARD_INFO[type - 1], m_rankReward[type - 1].GetValue(reward));
		break;
	}
	logstr = logsz;
	return LCS_OK;

}