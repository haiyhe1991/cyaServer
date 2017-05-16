#include "EmailCfg.h"
#include "ServiceErrorCode.h"

EmailCfg::EmailCfg()
{

}

EmailCfg::~EmailCfg()
{

}

BOOL EmailCfg::Load(const char* filename)
{
	m_filename = filename;
	return true;

}

int EmailCfg::GetNewPlayerEmailGiveProps(std::vector<SNewPlayerEmailItem>& giveVec)
{
	Json::Value rootValue;
	if (!OpenCfgFile(m_filename.c_str(), rootValue))
		return MLS_OK;

	if (!rootValue.isMember("EmailGiving") || !rootValue["EmailGiving"].isArray())
		return MLS_OK;

	int si = rootValue["EmailGiving"].size();
	for (int i = 0; i < si; ++i)
	{
		SNewPlayerEmailItem item;
		item.gold = rootValue["EmailGiving"][i]["Gold"].asUInt();
		item.rpcoin = rootValue["EmailGiving"][i]["Token"].asUInt();
		item.exp = rootValue["EmailGiving"][i]["Exp"].asUInt();
		item.vit = rootValue["EmailGiving"][i]["Vit"].asUInt();
		item.strTitle = rootValue["EmailGiving"][i]["Title"].asCString();
		item.strBody = rootValue["EmailGiving"][i]["Body"].asCString();
		int n = rootValue["EmailGiving"][i]["Prop"].size();
		for (int j = 0; j < n; ++j)
		{
			SDropPropCfgItem giveProp;
			giveProp.propId = (UINT16)rootValue["EmailGiving"][i]["Prop"][j]["PropID"].asUInt();
			giveProp.num = rootValue["EmailGiving"][i]["Prop"][j]["Num"].asUInt();
			giveProp.type = (BYTE)rootValue["EmailGiving"][i]["Prop"][j]["Type"].asUInt();
			item.props.push_back(giveProp);
		}
		giveVec.push_back(item);
	}

	return MLS_OK;
}