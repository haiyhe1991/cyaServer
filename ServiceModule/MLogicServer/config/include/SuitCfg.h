#ifndef __SUIT_CFG_H__
#define __SUIT_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��װ����
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class SuitCfg
{
public:
	SuitCfg();
	~SuitCfg();

#pragma pack(push, 1)
	struct SSuitAttrItem
	{
		BYTE type;
		UINT32 val;
		SSuitAttrItem()
		{
			type = 0;
			val = 0;
		}
	};
	struct SSuitAttrObj
	{
		BYTE lv;
		BYTE quality;
		std::vector<SSuitAttrItem> attrVec;
		SSuitAttrObj()
		{
			lv = 0;
			quality = 0;
			attrVec.clear();
		}
	};
#pragma pack(pop)

public:
	//������װ����
	BOOL Load(const char* filename);
	//��ȡ��װ����
	int GetSuitAttr(UINT16 suitId, SSuitAttrObj*& pAttrObj);
	//��ȡ��װ����ֵ
	bool GetSuitAttrValue(UINT16 suitId, BYTE type, UINT32 *value);

private:
	std::map<UINT16, SSuitAttrObj> m_suitAttrMap;
};



#endif