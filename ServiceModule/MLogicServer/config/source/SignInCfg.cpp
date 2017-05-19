#include "SignInCfg.h"
#include "ServiceErrorCode.h"

SignInCfg::SignInCfg()
{

}

SignInCfg::~SignInCfg()
{

}

BOOL SignInCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("SignIn") || !rootValue["SignIn"].isArray())
		return false;

	SSignInItem item;
	int si = rootValue["SignIn"].size();
	for (int i = 0; i < si; ++i)
	{
		BYTE nDays = (BYTE)rootValue["SignIn"][i]["Days"].asUInt();
		m_signInMap.insert(std::make_pair(nDays, item));
		SSignInItem& itemRef = m_signInMap[nDays];

		int n = rootValue["SignIn"][i]["Rewards"].size();
		for (int j = 0; j < n; ++j)
		{
			SDropPropCfgItem prop;
			prop.num = rootValue["SignIn"][i]["Rewards"][j]["Num"].asUInt();
			prop.propId = rootValue["SignIn"][i]["Rewards"][j]["ID"].asUInt();
			prop.type = rootValue["SignIn"][i]["Rewards"][j]["Type"].asUInt();
			itemRef.rewardVec.push_back(prop);
		}
	}

	return true;
}

int SignInCfg::GetSignInItem(BYTE nTimes, SSignInItem*& pItem)
{
	std::map<BYTE/*连续签到次数*/, SSignInItem>::iterator it = m_signInMap.find(nTimes);
	if (it == m_signInMap.end())
		return MLS_INVALID_SIGN_IN;

	pItem = &it->second;
	return MLS_OK;
}