#include "FashionCfg.h"
#include "ServiceErrorCode.h"

FashionCfg::FashionCfg()
{

}

FashionCfg::~FashionCfg()
{

}

BOOL FashionCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Fashion") || !rootValue["Fashion"].isArray())
		return false;

	int si = rootValue["Fashion"].size();
	if (si <= 0)
		return false;

	for (int i = 0; i < si; ++i)
	{
		UINT16 fashionId = (UINT16)rootValue["Fashion"][i]["ID"].asUInt();
		int n = rootValue["Fashion"][i]["ModelID"].size();
		for (int j = 0; j < n; ++j)
		{
			BYTE jobId = (BYTE)rootValue["Fashion"][i]["ModelID"][j]["BelongTo"].asUInt();
			std::map<BYTE, std::set<UINT16> >::iterator it = m_jobFashionMap.find(jobId);
			if (it == m_jobFashionMap.end())
			{
				std::set<UINT16> dressSet;
				m_jobFashionMap.insert(std::make_pair(jobId, dressSet));
				std::set<UINT16>& dressSetRef = m_jobFashionMap[jobId];
				dressSetRef.insert(fashionId);
			}
			else
				it->second.insert(fashionId);
		}

		std::vector<SFashionAttr> attrVec;
		n = rootValue["Fashion"][i]["Attr"].size();
		for (int k = 0; k < n; ++k)
		{
			SFashionAttr attr;
			attr.type = (BYTE)rootValue["Fashion"][i]["Attr"][k]["Type"].asUInt();
			attr.val = rootValue["Fashion"][i]["Attr"][k]["Value"].asUInt();
			attrVec.push_back(attr);
		}

		m_fashionAttrMap.insert(std::make_pair(fashionId, attrVec));
	}

	return true;
}

int FashionCfg::GetFashionAttr(UINT16 fashionId, std::vector<SFashionAttr>*& pAttrVec)
{
	std::map<UINT16/*ʱװid*/, std::vector<SFashionAttr> >::iterator it = m_fashionAttrMap.find(fashionId);
	if (it == m_fashionAttrMap.end())
		return MLS_DRESS_NOT_EXIST;

	pAttrVec = &it->second;
	return MLS_OK;
}

int FashionCfg::IsJobFashionMatch(BYTE jobId, UINT16 fashionId)
{
	std::map<BYTE, std::set<UINT16> >::iterator it = m_jobFashionMap.find(jobId);
	if (it == m_jobFashionMap.end())
		return MLS_NOT_EXIST_JOB;

	std::set<UINT16>::iterator it2 = it->second.find(fashionId);
	return it2 == it->second.end() ? MLS_JOB_DRESS_NOT_MATCH : MLS_OK;
}