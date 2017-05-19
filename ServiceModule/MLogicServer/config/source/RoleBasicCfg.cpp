#include "RoleBasicCfg.h"
#include "cyaFdk.h"
#include "moduleInfo.h"
#include "cyaLog.h"
#include "ServiceErrorCode.h"

RoleBasicCfg::RoleBasicCfg()
{

}

RoleBasicCfg::~RoleBasicCfg()
{

}

BOOL RoleBasicCfg::Load(const char* pszDir)
{
#if defined(WIN32)
	char pszRolePath[MAX_PATH] = { 0 };
	strcpy(pszRolePath, pszDir);
	strcat(pszRolePath, "\\*.role");
	WIN32_FIND_DATAA findData;
	HANDLE hd = ::FindFirstFileA(pszRolePath, &findData);
	if (hd == INVALID_HANDLE_VALUE)
		return false;

	do
	{
		char pszRoleName[MAX_PATH] = { 0 };
		strcpy(pszRoleName, pszDir);
		strcat(pszRoleName, "\\");
		strcat(pszRoleName, findData.cFileName);
		if (!LoadRoleBasicCfg(pszRoleName))
		{
			FindClose(hd);
			return false;
		}

	} while (FindNextFileA(hd, &findData));

	FindClose(hd);
#else
	SCAN_FILE_FILTER scanfilter;
	scanfilter.count = scandir(pszDir, &scanfilter.nameList, SCAN_FILE_FILTER::scandir_filter_, alphasort);
	int count = scanfilter.count;
	struct dirent** nameList = scanfilter.nameList;
	if (count <= 0 || nameList == NULL)
		return true;

	for (int i = 0; i < count; ++i)
	{
		char fullFileName[MAX_PATH] = { 0 };
		strcpy(fullFileName, pszDir);
		strcat(fullFileName, "/");
		strcat(fullFileName, nameList[i]->d_name);

		const char* fileSuffixName = GetFileSuffixName(fullFileName);
		if (strcmp(fileSuffixName, "role") != 0)
			continue;

		if (!LoadRoleBasicCfg(fullFileName))
			return false;
	}
#endif
	return true;
}

int RoleBasicCfg::GetRoleCfgBasicAttr(BYTE jobId, BYTE roleLevel, SRoleAttrPoint& attr)
{
	ASSERT(roleLevel > 0);
	std::map<BYTE/*职业id*/, SRoleBasicAttrMap>::iterator it = m_roleBasicAttrMap.find(jobId);
	if (it == m_roleBasicAttrMap.end())
		return MLS_ROLE_TYPE_INVALID;

	SRoleBasicAttrMap& roleMapRef = it->second;
	std::map<BYTE/*人物等级*/, SRoleBasicAttrCfg>::iterator it2 = roleMapRef.rolesMap.find(roleLevel);
	if (it2 == roleMapRef.rolesMap.end())
		return MLS_OVER_ROLES_MAX_LEVEL;

	attr = it2->second.basicAttr;
	return MLS_OK;
}

int RoleBasicCfg::JudgeRoleUpgrade(BYTE jobId, BYTE curLevel, UINT64 curExp, BYTE& newLevel)
{
	newLevel = curLevel;
	std::map<BYTE/*职业id*/, SRoleBasicAttrMap>::iterator it = m_roleBasicAttrMap.find(jobId);
	if (it == m_roleBasicAttrMap.end())
		return MLS_ROLE_TYPE_INVALID;

	BYTE nextLv = SGSU8Add(curLevel, 1);
	std::map<BYTE/*人物等级*/, SRoleBasicAttrCfg>& rolesMapRef = it->second.rolesMap;
	std::map<BYTE/*人物等级*/, SRoleBasicAttrCfg>::iterator it2 = rolesMapRef.find(nextLv);
	if (it2 == rolesMapRef.end())
		return MLS_OK;

	for (; it2 != rolesMapRef.end(); ++it2)
	{
		if (curExp == it2->second.totalExp)
		{
			newLevel = it2->first;
			break;
		}
		else if (curExp > it2->second.totalExp)
			newLevel = it2->first;
		else
			break;
	}

	return MLS_OK;
}

BOOL RoleBasicCfg::LoadRoleBasicCfg(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Profession") || !rootValue["Profession"].isInt())
		return false;

	if (!rootValue.isMember("role") || !rootValue["role"].isArray())
		return false;

	int si = rootValue["role"].size();
	if (si <= 0)
		return false;

	BYTE jobId = (BYTE)rootValue["Profession"].asUInt();
	std::map<BYTE/*职业id*/, SRoleBasicAttrMap>::iterator it = m_roleBasicAttrMap.find(jobId);
	if (it == m_roleBasicAttrMap.end())
	{
		SRoleBasicAttrMap roleMap;
		m_roleBasicAttrMap.insert(std::make_pair(jobId, roleMap));
	}
	SRoleBasicAttrMap& roleMapRef = m_roleBasicAttrMap[jobId];

	for (int i = 0; i < si; ++i)
	{
		SRoleBasicAttrCfg item;
		BYTE level = (BYTE)rootValue["role"][i]["Level"].asUInt();

		item.totalExp = rootValue["role"][i]["Exp"].asUInt();		//升级总经验
		item.basicAttr.u32HP = (UINT32)rootValue["role"][i]["HP"].asUInt();			//生命
		item.basicAttr.u32MP = (UINT32)rootValue["role"][i]["MP"].asUInt();			//魔力
		item.basicAttr.attack = (UINT32)rootValue["role"][i]["Attack"].asUInt();			//攻击
		item.basicAttr.def = (UINT32)rootValue["role"][i]["Def"].asUInt();				//防御

		std::map<BYTE/*人物等级*/, SRoleBasicAttrCfg>::iterator it2 = roleMapRef.rolesMap.find(level);
		if (it2 != roleMapRef.rolesMap.end())
		{
			ASSERT(false);
			continue;
		}

		roleMapRef.rolesMap.insert(std::make_pair(level, item));
	}

	return true;
}

BYTE RoleBasicCfg::GetRoleLevelByExp(BYTE jobId, UINT64 exp)
{
	BYTE level = 1;
	std::map<BYTE/*职业id*/, SRoleBasicAttrMap>::iterator it = m_roleBasicAttrMap.find(jobId);
	if (it == m_roleBasicAttrMap.end())
		return level;

	std::map<BYTE/*人物等级*/, SRoleBasicAttrCfg>& rolesMapRef = it->second.rolesMap;
	std::map<BYTE/*人物等级*/, SRoleBasicAttrCfg>::iterator it2 = rolesMapRef.begin();
	for (; it2 != rolesMapRef.end(); ++it2)
	{
		if (exp == it2->second.totalExp)
		{
			level = it2->first;
			break;
		}
		else if (exp > it2->second.totalExp)
			level = it2->first;
		else
			break;
	}

	return level;
}