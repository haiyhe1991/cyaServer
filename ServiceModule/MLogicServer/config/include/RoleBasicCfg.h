#ifndef __ROLE_BASIC_CFG_H__
#define __ROLE_BASIC_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
角色基本配置
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
	//角色基本属性配置
	struct SRoleBasicAttrCfg
	{
		UINT64 totalExp;			//升级到当前总经验
		SRoleAttrPoint basicAttr;	//角色属性

		SRoleBasicAttrCfg()
		{
			totalExp = 0;			//升级经验
		}
	};

	struct SRoleBasicAttrMap
	{
		std::map<BYTE/*人物等级*/, SRoleBasicAttrCfg> rolesMap;
		SRoleBasicAttrMap()
		{
			rolesMap.clear();
		}
	};
#pragma pack(pop)

public:
	//载入配置文件
	BOOL Load(const char* pszDir);
	//获取角色属性
	int GetRoleCfgBasicAttr(BYTE jobId, BYTE roleLevel, SRoleAttrPoint& attr);
	//判断角色升级
	int JudgeRoleUpgrade(BYTE jobId, BYTE curLevel, UINT64 curExp, BYTE& newLevel);
	//根据经验获得人物等级
	BYTE GetRoleLevelByExp(BYTE jobId, UINT64 exp);

private:
	//载入角色配置
	BOOL LoadRoleBasicCfg(const char* filename);

private:
	std::map<BYTE/*职业id*/, SRoleBasicAttrMap> m_roleBasicAttrMap;	//角色配置表
};


#endif