#include "NoticeCfg.h"
#include "ServiceErrorCode.h"

NoticeCfg::NoticeCfg()
{

}

NoticeCfg::~NoticeCfg()
{

}

BOOL NoticeCfg::Load(const char* filename)
{
	m_filename = filename;
	return true;
}

BOOL NoticeCfg::GetNoticeItems(std::vector<SNoticeItem>& noticeVec, UINT32& timeInterval)
{
	Json::Value rootValue;
	if (!OpenCfgFile(m_filename.c_str(), rootValue))
		return false;

	if (!rootValue.isMember("Notice") || !rootValue["Notice"].isArray())
		return false;

	int si = rootValue["Notice"].size();
	for (int i = 0; i < si; ++i)
	{
		SNoticeItem item;
		item.id = rootValue["Notice"][i]["ID"].asUInt();
		item.interval = rootValue["Notice"][i]["IntervalTime"].asUInt();
		item.strContent = rootValue["Notice"][i]["Body"].asCString();
		noticeVec.push_back(item);
	}

	timeInterval = rootValue["Interval"].asUInt();

	return true;
}

UINT32 NoticeCfg::GetNoticeInterval()
{
	Json::Value rootValue;
	if (!OpenCfgFile(m_filename.c_str(), rootValue))
		return 0;

	UINT32 n = rootValue["Interval"].asUInt();
	return n;
}