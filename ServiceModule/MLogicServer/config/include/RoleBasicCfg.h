#ifndef __ROLE_BASIC_CFG_H__
#define __ROLE_BASIC_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��ɫ��������
*/

#include <map>
#include <vector>
#include "CfgCom.h"
#include "RoleInfos.h"

class RoleBasicCfg
{
public:
	RoleBasicCfg();
	~RoleBasicCfg();

#pragma pack(push, 1)
	//��ɫ������������
	struct SRoleBasicAttrCfg
	{
		UINT64 totalExp;			//��������ǰ�ܾ���
		SRoleAttrPoint basicAttr;	//��ɫ����

		SRoleBasicAttrCfg()
		{
			totalExp = 0;			//��������
		}
	};

	struct SRoleBasicAttrMap
	{
		std::map<BYTE/*����ȼ�*/, SRoleBasicAttrCfg> rolesMap;
		SRoleBasicAttrMap()
		{
			rolesMap.clear();
		}
	};
#pragma pack(pop)

public:
	//���������ļ�
	BOOL Load(const char* pszDir);
	//��ȡ��ɫ����
	int GetRoleCfgBasicAttr(BYTE jobId, BYTE roleLevel, SRoleAttrPoint& attr);
	//�жϽ�ɫ����
	int JudgeRoleUpgrade(BYTE jobId, BYTE curLevel, UINT64 curExp, BYTE& newLevel);
	//���ݾ���������ȼ�
	BYTE GetRoleLevelByExp(BYTE jobId, UINT64 exp);

private:
	//�����ɫ����
	BOOL LoadRoleBasicCfg(const char* filename);

private:
	std::map<BYTE/*ְҵid*/, SRoleBasicAttrMap> m_roleBasicAttrMap;	//��ɫ���ñ�
};


#endif