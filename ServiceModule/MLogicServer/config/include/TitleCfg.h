#ifndef __TITLE_CFG_H__
#define __TITLE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�ƺ�����
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
		BYTE type;	//������������, ESGSEquipAttachType
		UINT32 val;	//��������ֵ
		STitleAttr()
		{
			type = 0;
			val = 0;
		}
	};

	struct STitleCfg
	{
		std::map<UINT16/*�ƺ�id*/, std::vector<STitleAttr> /*�ƺ�����*/> titleAttrMap;
		STitleCfg()
		{
			titleAttrMap.clear();
		}
	};
#pragma pack(pop)

public:
	//����ƺ�����
	BOOL Load(const char* filename);
	//��ȡ�ƺ�����
	int GetRoleTitleAttr(BYTE jobId, UINT16 titleId, std::vector<STitleAttr>& attrVec);

private:
	std::map<BYTE/*ְҵid*/, STitleCfg> m_roleTitleCfg;		//ְҵ���гƺ�
	STitleCfg m_pubTitleCfg;	//�����ƺ�
};

#endif