#include "moduleInfo.h"
#include "ServiceErrorCode.h"
#include "ChainSoulFragmentCfg.h"

ChainSoulFragmentCfg::ChainSoulFragmentCfg()
{

}

ChainSoulFragmentCfg::~ChainSoulFragmentCfg()
{

}

BOOL ChainSoulFragmentCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("HolyDebris") || !rootValue["HolyDebris"].isArray())
		return false;

	ChainSoulFragmentAttr attr;
	for (size_t i = 0; i < rootValue["HolyDebris"].size(); ++i)
	{
		UINT16 id = rootValue["HolyDebris"][i]["ID"].asInt();
		attr.type = rootValue["HolyDebris"][i]["Type"].asInt();
		attr.quality = rootValue["HolyDebris"][i]["Quality"].asInt();
		attr.maxStack = rootValue["HolyDebris"][i]["StackCount"].asInt();
		m_chainSoulFragmentType.insert(std::make_pair(attr.type, id));
		m_chainSoulFragment.insert(std::make_pair(id, attr));
	}

	return true;
}

int ChainSoulFragmentCfg::GetFragmentUserSellPrice(UINT32 /*fragmentId*/, UINT64& price)
{
	price = 0;
	return MLS_OK;
}

//获取炼魂碎片属性
bool ChainSoulFragmentCfg::GetChainSoulFragmentAttr(UINT32 fragmentId, ChainSoulFragmentAttr &out_attr)
{
	std::map<UINT32, ChainSoulFragmentAttr>::iterator itr = m_chainSoulFragment.find(fragmentId);
	if (itr == m_chainSoulFragment.end()) return false;
	out_attr = itr->second;
	return true;
}

//通过类型获取炼魂碎片ID
bool ChainSoulFragmentCfg::GetFragmentIdByType(BYTE type, UINT32 &out_id)
{
	std::map<BYTE, UINT32>::iterator itr = m_chainSoulFragmentType.find(type);
	if (itr == m_chainSoulFragmentType.end()) return false;
	out_id = itr->second;
	return true;
}