#ifndef __STRENGTH_ATTRICFG_H__
#define __STRENGTH_ATTRICFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
װ��λǿ������
*/

#include <map>
#include "CfgCom.h"

class StrengthenAttriCfg
{
public:
	struct SStrengthenAttr
	{
		UINT32 hp;
		UINT32 mp;
		UINT32 attack;
		UINT32 def;
		UINT32 crit;
		UINT32 strong;
	};

public:
	StrengthenAttriCfg();
	~StrengthenAttriCfg();

public:
	//��������
	BOOL Load(const char* filename);
	//��ȡװ��λǿ������
	BOOL GetStrengthenAttri(BYTE level, SStrengthenAttr &ret);

private:
	std::map<BYTE, SStrengthenAttr> m_strengthenAttr;
};


#endif