#include "ActivityCfg.h"
#include "cyaTime.h"
#include "ServiceErrorCode.h"

ActivityCfg::ActivityCfg()
{

}

ActivityCfg::~ActivityCfg()
{

}

BOOL ActivityCfg::Load(const char* filename)
{
	m_filename = filename;
	return true;
}

int ActivityCfg::GetActivityItem(UINT32 activityId, SActivityItem& activityItem)
{
	Json::Value rootValue;
	if (!OpenCfgFile(m_filename.c_str(), rootValue))
		return MLS_NOT_EXIST_ACTIVITY;

	if (!rootValue.isMember("Activity") || !rootValue["Activity"].isArray())
		return MLS_NOT_EXIST_ACTIVITY;

	int retCode = MLS_NOT_EXIST_ACTIVITY;
	int si = rootValue["Activity"].size();
	for (int i = 0; i < si; ++i)
	{
		UINT32 id = rootValue["Activity"][i]["ID"].asUInt();
		if (id != activityId)
			continue;

		activityItem.startMSel = StrToMsel(rootValue["Activity"][i]["StartTime"].asCString());
		activityItem.endMSel = StrToMsel(rootValue["Activity"][i]["EndTime"].asCString());

		int n = rootValue["Activity"][i]["Rewards"].size();
		for (int j = 0; j < n; ++j)
		{
			SDropPropCfgItem prop;
			prop.num = rootValue["Activity"][i]["Rewards"][j]["Num"].asUInt();
			prop.propId = rootValue["Activity"][i]["Rewards"][j]["ID"].asUInt();
			prop.type = rootValue["Activity"][i]["Rewards"][j]["Type"].asUInt();
			activityItem.rewardVec.push_back(prop);
		}
		retCode = MLS_OK;
		break;
	}
	return retCode;
}