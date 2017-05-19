#ifndef __STRENGTHEN_CFG_H__
#define __STRENGTHEN_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
װ��ǿ������
*/

#include <map>
#include "CfgCom.h"

class StrengthenCfg
{
public:
	StrengthenCfg();
	~StrengthenCfg();

#pragma pack(push, 1)
	struct SEquipAttrInc
	{
		UINT32 masterInc;
		UINT32 depuInc;
		SEquipAttrInc()
		{
			masterInc = 0;
			depuInc = 0;
		}
	};
#pragma pack(pop)

public:
	//��������
	BOOL Load(const char* filename);
	//��ȡװ����������
	int GetEquipAttrIncreament(BYTE pos, BYTE quality, SEquipAttrInc& attrInc);

private:
	std::map<BYTE/*װ��λ��*/, SEquipAttrInc> m_strengthenCfg;
};

#endif