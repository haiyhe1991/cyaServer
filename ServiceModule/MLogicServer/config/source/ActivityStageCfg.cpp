#include "CfgCom.h"
#include "ActivityStageCfg.h"

ActivityStageCfg::ActivityStageCfg()
{

}

ActivityStageCfg::~ActivityStageCfg()
{

}

//载入配置
BOOL ActivityStageCfg::Load(const char* pszFile)
{
	Json::Value rootValue;
	if (!OpenCfgFile(pszFile, rootValue))
		return false;

	if (!rootValue.isMember("ActivityStage") || !rootValue["ActivityStage"].isArray())
		return false;

	int si = rootValue["ActivityStage"].size();
	if (si <= 0)
		return false;

	for (int i = 0; i < si; ++i)
	{
		int idx = 0;
		Condition condition;
		UINT32 id = rootValue["ActivityStage"][i]["ID"].asUInt();
		const Json::Value &json_object = rootValue["ActivityStage"][i]["Condition"][idx];
		condition.type = json_object["TYPE"].asInt();
		condition.num = json_object["NUM"].asInt();
		m_Condition.insert(std::make_pair(id, condition));
	}

	return TRUE;
}

//获取开启条件
bool ActivityStageCfg::GetActivityStageOpenCondition(UINT16 instId, Condition &ret)
{
	std::map<UINT16, Condition>::iterator itr = m_Condition.find(instId);
	if (itr != m_Condition.end())
	{
		ret = itr->second;
		return true;
	}
	return false;
}