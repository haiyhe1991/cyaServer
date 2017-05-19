#ifndef __FASHION_CFG_H__
#define __FASHION_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
ʱװ����
*/

#include <map>
#include <set>
#include <vector>
#include "CfgCom.h"

class FashionCfg
{
public:
	FashionCfg();
	~FashionCfg();

#pragma pack(push, 1)
	struct SFashionAttr
	{
		BYTE type;
		UINT32 val;
		SFashionAttr()
		{
			type = 0;
			val = 0;
		}
	};
#pragma pack(pop)

public:
	//����ʱװ����
	BOOL Load(const char* filename);
	//��ȡʱװ����
	int GetFashionAttr(UINT16 fashionId, std::vector<SFashionAttr>*& pAttrVec);
	//ʱװ��ְҵ�Ƿ�ƥ��
	int IsJobFashionMatch(BYTE jobId, UINT16 fashionId);

private:
	std::map<BYTE/*ְҵ*/, std::set<UINT16>/*ʱװ*/ > m_jobFashionMap;
	std::map<UINT16/*ʱװid*/, std::vector<SFashionAttr> > m_fashionAttrMap;
};

#endif