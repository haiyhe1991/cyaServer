#ifndef __LUA_SCRIPT_MAN_H__
#define __LUA_SCRIPT_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
lua�ű�������
*/

#include <map>
#include "LuaGlue.h"
#include "RoleInfos.h"

class LuaScriptMan
{
public:
	LuaScriptMan();
	~LuaScriptMan();

	//����lua�ű��ļ�
	void LoadLuaScriptFile(const char* filepath = "lua", const char* luaSuffixName = "*.lua");
	//ж��lua�ű��ļ�
	void UnLoadLuaScriptFile();

	//�Ƿ�Ϸ��Ľ�ɫ����
	int IsValidRoleType(UINT16 roleType);
	//��ȡ�´�����ɫĬ����Ϣ
	int GetCreateRoleDefaultInfo(UINT16 roleType, SRoleInfos* pInfo, UINT16& guildId, BYTE& sex);

	//�õ��߿���Ƕ���ٿű�ʯ(propId->����id, propGrade->����Ʒ��)
	int GetMaxInlaidStones(UINT16 propId, BYTE propGrade, BYTE& maxStones);

	//���������ü�����Ҫ�ĸ��־���
	int CalcUpgradeTotalExp(int lv, UINT64& totalExp, UINT64& taskExp, UINT64& instExp, UINT64& actionExp);

	//��ȡ���������¼��
	BYTE GetInstRankRecords() const
	{
		return m_instRankRecords;
	}

	//��ȡ�û���ഴ����ɫ����
	BYTE GetMaxCreateRoles() const
	{
		return m_maxCreateRoles;
	}

private:
	LuaGlue* GetLuaGlue(const char* luaScripFile);
	int InitDefaultParams();

private:
	std::map<std::string/*lua�ű��ļ���*/, LuaGlue*> m_luaStateMap;

	int m_baseTaskExp;	//��ʼ������
	int m_taskExpRose;	//�������Ƿ�
	int m_baseInstExp;	//��ʼ��������
	int m_instExpRose;	//���������Ƿ�
	int m_baseActionExp;	//��ʼ�����
	int m_actionExpRose;	//������Ƿ�

	BYTE m_baseTaskLv;	//��Ҫ������ĵȼ�
	BYTE m_baseInstLv;	//��Ҫ��������ĵȼ�
	BYTE m_baseActionLv;	//��Ҫ�����ĵȼ�
	double m_baseConst;	//�����׼����

	BYTE m_instRankRecords;	//ÿ���������������������
	BYTE m_maxCreateRoles;	//�û��ڷ���������ܴ����Ľ�ɫ����
};

void InitLuaScriptMan();
LuaScriptMan* GetLuaScriptMan();
void DestroyLuaScriptMan();

#endif	//_LuaScriptMan_h__