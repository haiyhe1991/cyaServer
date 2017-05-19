#ifndef __QUALITY_CFG_H__
#define __QUALITY_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
װ��Ʒ������
*/

#include <map>
#include "CfgCom.h"

class QualityCfg
{
public:
	QualityCfg();
	~QualityCfg();

private:
	std::map<BYTE/*Ʒ��id*/, BYTE/*���ǿ���ȼ�*/> m_qualityCfgMap;

public:
	//����Ʒ������
	BOOL Load(const char* filename);
	//��ȡ���ǿ���ȼ�
	int GetMaxStrengthenLevel(BYTE qualityId, BYTE& maxStrengthenLv);
};

#endif