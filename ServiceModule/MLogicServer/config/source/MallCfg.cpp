#include "MallCfg.h"
#include "cyaTime.h"
#include "ServiceErrorCode.h"

MallCfg::MallCfg()
{

}

MallCfg::~MallCfg()
{

}

BOOL MallCfg::LoadMall(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Mall") || !rootValue["Mall"].isArray())
		return false;

	int si = rootValue["Mall"].size();
	if (si <= 0)
		return false;

	SMallGoodsAttr goodsAttr;
	SYSTEMTIME sysTime;
	memset(&sysTime, 0, sizeof(SYSTEMTIME));
	for (int i = 0; i < si; ++i)
	{
		UINT32 id = rootValue["Mall"][i]["ID"].asUInt();
		std::map<UINT32, SMallGoodsAttr>::iterator it = m_goodsMap.find(id);
		if (it != m_goodsMap.end())
		{
			ASSERT(false);
			continue;
		}

		m_goodsMap.insert(std::make_pair(id, goodsAttr));
		SMallGoodsAttr& goodsAttrRef = m_goodsMap[id];

		goodsAttrRef.discount = rootValue["Mall"][i]["Discount"].asDouble();
		goodsAttrRef.priceToken = rootValue["Mall"][i]["GoodsPrice"].asUInt();

		std::string strDate;
		if (rootValue["Mall"][i].isMember("GoodsTimeLimit"))
			strDate = rootValue["Mall"][i]["GoodsTimeLimit"].asCString();
		else
			strDate = "0";

		if (strDate == "0")
			goodsAttrRef.sellDateLimit = 0;
		else
		{
			int nYear = 0, nMonth = 0, nDay = 0;
			int nHour = 0, nMin = 0, nSec = 0;
			sscanf(strDate.c_str(), "%04d%02d%02d%02d%02d%02d", &nYear, &nMonth, &nDay, &nHour, &nMin, &nSec);
			sysTime.wYear = nYear;
			sysTime.wMonth = nMonth;
			sysTime.wDay = nDay;
			sysTime.wHour = nHour;
			sysTime.wMinute = nMin;
			sysTime.wSecond = nSec;
			goodsAttrRef.sellDateLimit = ToMsel(sysTime);
		}

		goodsAttrRef.type = (BYTE)rootValue["Mall"][i]["GoodsType"].asUInt();
		goodsAttrRef.propId = (UINT16)rootValue["Mall"][i]["GoodsID"].asUInt();
		goodsAttrRef.useDays = (UINT16)rootValue["Mall"][i]["UseCycle"].asUInt();
		goodsAttrRef.vipLvLimit = (BYTE)rootValue["Mall"][i]["VIPLvLimit"].asUInt();
		goodsAttrRef.stackNum = (UINT16)rootValue["Mall"][i]["GoodsNum"].asUInt();
		goodsAttrRef.discountVIPLvLimit = (BYTE)rootValue["Mall"][i]["DiscVIPLvLimit"].asUInt();

		/* zpy add */
		m_goodsIndex.insert(std::make_pair(goodsAttrRef.propId, id));
	}

	return true;
}

int MallCfg::GetMallBuyGoodsAttr(UINT32 buyId, SMallGoodsAttr*& pGoodsAttr)
{
	std::map<UINT32, SMallGoodsAttr>::iterator it = m_goodsMap.find(buyId);
	if (it == m_goodsMap.end())
		return MLS_GOODS_NOT_EXIST;

	pGoodsAttr = &it->second;
	return MLS_OK;
}

int MallCfg::GetMallBuyGoodsAttrByPropId(UINT16 propId, SMallGoodsAttr *&pGoodsAttr)
{
	std::map<UINT16, UINT32>::iterator itr = m_goodsIndex.find(propId);
	if (itr == m_goodsIndex.end())
	{
		return MLS_GOODS_NOT_EXIST;
	}
	return GetMallBuyGoodsAttr(itr->second, pGoodsAttr);
}