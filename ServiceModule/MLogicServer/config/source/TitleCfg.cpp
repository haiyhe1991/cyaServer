#include "TitleCfg.h"
#include "ServiceErrorCode.h"

TitleCfg::TitleCfg()
{

}

TitleCfg::~TitleCfg()
{

}

BOOL TitleCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Titles") || !rootValue["Titles"].isArray())
		return false;

	int si = rootValue["Titles"].size();
	if (si <= 0)
		return false;

	for (int i = 0; i < si; ++i)
	{
		std::vector<STitleAttr>* pTitleAttrVec = NULL;
		BYTE jobId = rootValue["Titles"][i]["BelongTo"].asUInt();
		UINT16 titleId = (UINT16)rootValue["Titles"][i]["ID"].asUInt();

		std::map<BYTE/*Ö°Òµid*/, STitleCfg>::iterator it = m_roleTitleCfg.find(jobId);
		if (jobId != 0)
		{
			STitleCfg* pTitleCfg = NULL;
			if (it == m_roleTitleCfg.end())
			{
				STitleCfg title;
				m_roleTitleCfg.insert(std::make_pair(jobId, title));
				pTitleCfg = &m_roleTitleCfg[jobId];
			}
			else
				pTitleCfg = &m_roleTitleCfg[jobId];

			std::vector<STitleAttr> vec;
			pTitleCfg->titleAttrMap.insert(std::make_pair(titleId, vec));
			pTitleAttrVec = &pTitleCfg->titleAttrMap[titleId];
		}
		else
		{
			std::vector<STitleAttr> vec;
			m_pubTitleCfg.titleAttrMap.insert(std::make_pair(titleId, vec));
			pTitleAttrVec = &m_pubTitleCfg.titleAttrMap[titleId];
		}

		UINT32 u32HP = rootValue["Titles"][i]["HP"].asUInt();			//HP
		UINT32 u32MP = rootValue["Titles"][i]["MP"].asUInt();			//MP
		UINT32 crit = rootValue["Titles"][i]["Crit"].asUInt();			//±©»÷
		UINT32 tenacity = rootValue["Titles"][i]["Toughness"].asUInt();	//ÈÍÐÔ
		UINT32 attack = rootValue["Titles"][i]["Attack"].asUInt();		//¹¥»÷
		UINT32 def = rootValue["Titles"][i]["Def"].asUInt();				//·ÀÓù

		if (u32HP != 0)
		{
			STitleAttr attr;
			attr.val = u32HP;
			attr.type = ESGS_EQUIP_ATTACH_HP;
			pTitleAttrVec->push_back(attr);
		}
		if (u32MP != 0)
		{
			STitleAttr attr;
			attr.val = u32MP;
			attr.type = ESGS_EQUIP_ATTACH_MP;
			pTitleAttrVec->push_back(attr);
		}
		if (attack != 0)
		{
			STitleAttr attr;
			attr.val = attack;
			attr.type = ESGS_EQUIP_ATTACH_ATTACK;
			pTitleAttrVec->push_back(attr);
		}
		if (def != 0)
		{
			STitleAttr attr;
			attr.val = def;
			attr.type = ESGS_EQUIP_ATTACH_DEF;
			pTitleAttrVec->push_back(attr);
		}
		if (crit != 0)
		{
			STitleAttr attr;
			attr.val = crit;
			attr.type = ESGS_EQUIP_ATTACH_CRIT;
			pTitleAttrVec->push_back(attr);
		}
		if (tenacity != 0)
		{
			STitleAttr attr;
			attr.val = tenacity;
			attr.type = ESGS_EQUIP_ATTACH_TENACITY;
			pTitleAttrVec->push_back(attr);
		}
	}

	return true;
}

int TitleCfg::GetRoleTitleAttr(BYTE jobId, UINT16 titleId, std::vector<STitleAttr>& attrVec)
{
	std::map<BYTE/*Ö°Òµid*/, STitleCfg>::iterator it = m_roleTitleCfg.find(jobId);
	if (it == m_roleTitleCfg.end())
		return MLS_NOT_EXIST_JOB;

	std::map<UINT16/*³ÆºÅid*/, std::vector<STitleAttr> /*³ÆºÅÊôÐÔ*/>::iterator it2 = it->second.titleAttrMap.find(titleId);
	if (it2 == it->second.titleAttrMap.end())
	{
		it2 = m_pubTitleCfg.titleAttrMap.find(titleId);
		if (it2 == m_pubTitleCfg.titleAttrMap.end())
			return MLS_TITLE_NOT_EXIST;
	}

	attrVec.assign(it2->second.begin(), it2->second.end());
	return MLS_OK;
}