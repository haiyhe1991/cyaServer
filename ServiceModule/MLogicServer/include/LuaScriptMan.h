#ifndef __LUA_SCRIPT_MAN_H__
#define __LUA_SCRIPT_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
lua脚本管理类
*/

#include <map>
#include "LuaGlue.h"
#include "RoleInfos.h"

class LuaScriptMan
{
public:
	LuaScriptMan();
	~LuaScriptMan();

	//载入lua脚本文件
	void LoadLuaScriptFile(const char* filepath = "lua", const char* luaSuffixName = "*.lua");
	//卸载lua脚本文件
	void UnLoadLuaScriptFile();

	//是否合法的角色类型
	int IsValidRoleType(UINT16 roleType);
	//获取新创建角色默认信息
	int GetCreateRoleDefaultInfo(UINT16 roleType, SRoleInfos* pInfo, UINT16& guildId, BYTE& sex);

	//该道具可镶嵌多少颗宝石(propId->道具id, propGrade->道具品级)
	int GetMaxInlaidStones(UINT16 propId, BYTE propGrade, BYTE& maxStones);

	//计算升到该级所需要的各种经验
	int CalcUpgradeTotalExp(int lv, UINT64& totalExp, UINT64& taskExp, UINT64& instExp, UINT64& actionExp);

	//获取排名保存记录数
	BYTE GetInstRankRecords() const
	{
		return m_instRankRecords;
	}

	//获取用户最多创建角色个数
	BYTE GetMaxCreateRoles() const
	{
		return m_maxCreateRoles;
	}

private:
	LuaGlue* GetLuaGlue(const char* luaScripFile);
	int InitDefaultParams();

private:
	std::map<std::string/*lua脚本文件名*/, LuaGlue*> m_luaStateMap;

	int m_baseTaskExp;	//初始任务经验
	int m_taskExpRose;	//任务经验涨幅
	int m_baseInstExp;	//初始副本经验
	int m_instExpRose;	//副本经验涨幅
	int m_baseActionExp;	//初始活动经验
	int m_actionExpRose;	//活动经验涨幅

	BYTE m_baseTaskLv;	//需要任务经验的等级
	BYTE m_baseInstLv;	//需要副本经验的等级
	BYTE m_baseActionLv;	//需要活动经验的等级
	double m_baseConst;	//经验基准常量

	BYTE m_instRankRecords;	//每个副本完成排名保存数量
	BYTE m_maxCreateRoles;	//用户在分区中最多能创建的角色数量
};

void InitLuaScriptMan();
LuaScriptMan* GetLuaScriptMan();
void DestroyLuaScriptMan();

#endif	//_LuaScriptMan_h__