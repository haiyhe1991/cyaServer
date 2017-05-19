#ifndef __TITLE_CFG_H__
#define __TITLE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
称号配置
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class TitleCfg
{
public:
	TitleCfg();
	~TitleCfg();

#pragma pack(push, 1)
	struct STitleAttr
	{
		BYTE type;	//增加属性类型, ESGSEquipAttachType
		UINT32 val;	//增加属性值
		STitleAttr()
		{
			type = 0;
			val = 0;
		}
	};

	struct STitleCfg
	{
		std::map<UINT16/*称号id*/, std::vector<STitleAttr> /*称号属性*/> titleAttrMap;
		STitleCfg()
		{
			titleAttrMap.clear();
		}
	};
#pragma pack(pop)

public:
	//载入称号配置
	BOOL Load(const char* filename);
	//获取称号属性
	int GetRoleTitleAttr(BYTE jobId, UINT16 titleId, std::vector<STitleAttr>& attrVec);

private:
	std::map<BYTE/*职业id*/, STitleCfg> m_roleTitleCfg;		//职业特有称号
	STitleCfg m_pubTitleCfg;	//公共称号
};

#endif