#include "StrengthenAttriCfg.h"


StrengthenAttriCfg::StrengthenAttriCfg()
{

}

StrengthenAttriCfg::~StrengthenAttriCfg()
{

}

//载入配置
BOOL StrengthenAttriCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("StrengthensAttri") || !rootValue["StrengthensAttri"].isArray())
		return false;

	for (int i = 0; i < rootValue["StrengthensAttri"].size(); ++i)
	{
		const Json::Value &json_object = rootValue["StrengthensAttri"][i];

		SStrengthenAttr attr;
		BYTE level = json_object["Level"].asUInt();
		attr.hp = json_object["HP"].asUInt();
		attr.mp = json_object["MP"].asUInt();
		attr.attack = json_object["Attack"].asUInt();
		attr.def = json_object["Def"].asUInt();
		attr.crit = json_object["CRIT"].asUInt();
		attr.strong = json_object["STRONG"].asUInt();
		m_strengthenAttr.insert(std::make_pair(level, attr));
	}
	return TRUE;
}

//获取装备位强化属性
BOOL StrengthenAttriCfg::GetStrengthenAttri(BYTE level, SStrengthenAttr &ret)
{
	std::map<BYTE, SStrengthenAttr>::iterator itr = m_strengthenAttr.find(level);
	if (itr == m_strengthenAttr.end())
	{
		return FALSE;
	}
	ret = itr->second;
	return TRUE;
}