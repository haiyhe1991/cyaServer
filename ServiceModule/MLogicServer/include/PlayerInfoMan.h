#ifndef __PLAYER_INFO_MAN_H__
#define __PLAYER_INFO_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
玩家信息管理
*/

#include <list>
#include <vector>
#include "cyaLocker.h"
#include "cyaThread.h"
#include "ServiceProtocol.h"

class PlayerInfoMan
{
	// zpy 新增
	struct SAccountInfo
	{
		UINT32 userId;
		UINT32 roleId;
		SAccountInfo(UINT32 uid, UINT32 rid)
			: userId(uid), roleId(rid)
		{

		}
	};

	PlayerInfoMan(const PlayerInfoMan&);
	PlayerInfoMan& operator = (const PlayerInfoMan&);

public:
	PlayerInfoMan();
	~PlayerInfoMan();

	enum EPlayerMsgType
	{
		E_HAS_CASH_TOKEN = 1,
	};

	struct SPlayerMsg
	{
		int msg;
		union
		{
			struct
			{
				UINT32 userId;
				UINT32 roleId;
			} cash;
		};
	};

	void Start();
	void Stop();

	void InputPlayerMsg(int msgType, UINT32 userId, UINT32 roleId);

public:
	//获取机器人数据
	int FetchRobotData(UINT32 userId, UINT32 roleId, const SFetchRobotReq* req, SFetchRobotRes* pRobotRes);
	//查询机器人数据
	int QueryRobotData(UINT32 roleId, SFetchRobotRes* pRobotRes);
	//查询玩家外形信息
	int QueryPlayerAppearance(UINT32 playerId, SQueryRoleAppearanceRes* pAppearanceRes);
	//查询玩家详细信息
	int QueryPlayerExplicit(UINT32 playerId, SQueryRoleExplicitRes* pExplicitRes);

private:
	//zpy新增 加载角色战力信息
	int LoadRolesFightingCache();

	//zpy新增 加载角色等级信息
	int LoadRolesLevelCache();

	//zpy 按等级匹配机器人
	int MatchingByLevel(UINT32 userId, UINT32 roleId, UINT32 *findedRoleId);

	//zpy 按战力匹配机器人
	int MatchingByFighting(UINT32 userId, UINT32 roleId, UINT32 *findedRoleId);

private:
	static int THWorker(void* param);
	int OnWorker();

	void OnProcessMsg(const SPlayerMsg& msg);
	void CheckOrderStatus(UINT32 userId, UINT32 roleId);

private:
	BOOL m_exit;
	OSThread m_thHandle;

	CXTLocker m_locker;
	std::list<SPlayerMsg> m_playerMsgQ;

	// zpy 新增
	int m_lastHour;
	CXTLocker m_cp_locker;
	CXTLocker m_level_locker;
	std::vector< std::vector<SAccountInfo> > m_allRolesSortByCP;
	std::vector< std::vector<SAccountInfo> > m_allRolesSortByLevel;
};

void InitPlayerInfoMan();
PlayerInfoMan* GetPlayerInfoMan();
void DestroyPlayerInfoMan();

#endif