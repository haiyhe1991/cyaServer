#include "moduleInfo.h"
#include "FragmentCfg.h"
#include "ServiceErrorCode.h"

FragmentCfg::FragmentCfg()
{

}

FragmentCfg::~FragmentCfg()
{

}

BOOL FragmentCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Fragment") || !rootValue["Fragment"].isArray())
		return false;

	SFragmentCfgAttr frag;
	int si = rootValue["Fragment"].size();
	for (int i = 0; i < si; ++i)
	{
		UINT16 id = rootValue["Fragment"][i]["ID"].asUInt();
		std::map<UINT16/*ËéÆ¬id*/, SFragmentCfgAttr>::iterator it = m_fragmentMap.find(id);
		if (it != m_fragmentMap.end())
		{
			ASSERT(false);
			continue;
		}
		m_fragmentMap.insert(std::make_pair(id, frag));
		SFragmentCfgAttr& fragRef = m_fragmentMap[id];

		fragRef.quality = (BYTE)rootValue["Fragment"][i]["Quality"].asUInt();
		fragRef.maxStack = (UINT16)rootValue["Fragment"][i]["StackCount"].asUInt();
	}

	return true;
}

BOOL FragmentCfg::LoadGiveFragment(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("FragmentGiving") || !rootValue["FragmentGiving"].isArray())
		return false;

	SGiveFragmentInfo giveObj;
	int si = rootValue["FragmentGiving"].size();
	for (int i = 0; i < si; ++i)
	{
		giveObj.id = (UINT16)rootValue["FragmentGiving"][i]["ID"].asUInt();
		giveObj.num = (UINT16)rootValue["FragmentGiving"][i]["Num"].asUInt();
		if (giveObj.id == 0 || giveObj.num == 0)
			continue;

		m_giveFragmentVec.push_back(giveObj);
	}

	return true;
}

int FragmentCfg::GetFragmentUserSellPrice(UINT16 /*fragmentId*/, UINT64& price)
{
	price = 0;
	return MLS_OK;
}

int FragmentCfg::GetFragmentAttr(UINT16 fragmentId, SFragmentCfgAttr& fragmentAttr)
{
	std::map<UINT16/*ËéÆ¬id*/, SFragmentCfgAttr>::iterator it = m_fragmentMap.find(fragmentId);
	if (it == m_fragmentMap.end())
		return MLS_PROPERTY_NOT_EXIST;

	fragmentAttr.maxStack = it->second.maxStack;
	fragmentAttr.quality = it->second.quality;
	return MLS_OK;
}

int FragmentCfg::GetGiveFragment(std::vector<SGiveFragmentInfo>& giveVec)
{
	giveVec.assign(m_giveFragmentVec.begin(), m_giveFragmentVec.end());
	return MLS_OK;
}