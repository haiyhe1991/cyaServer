#ifndef __PLAYER_INFO_MAN_H__
#define __PLAYER_INFO_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�����Ϣ����
*/

#include <list>
#include <vector>
#include "cyaLocker.h"
#include "cyaThread.h"
#include "ServiceProtocol.h"

class PlayerInfoMan
{
	// zpy ����
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
	//��ȡ����������
	int FetchRobotData(UINT32 userId, UINT32 roleId, const SFetchRobotReq* req, SFetchRobotRes* pRobotRes);
	//��ѯ����������
	int QueryRobotData(UINT32 roleId, SFetchRobotRes* pRobotRes);
	//��ѯ���������Ϣ
	int QueryPlayerAppearance(UINT32 playerId, SQueryRoleAppearanceRes* pAppearanceRes);
	//��ѯ�����ϸ��Ϣ
	int QueryPlayerExplicit(UINT32 playerId, SQueryRoleExplicitRes* pExplicitRes);

private:
	//zpy���� ���ؽ�ɫս����Ϣ
	int LoadRolesFightingCache();

	//zpy���� ���ؽ�ɫ�ȼ���Ϣ
	int LoadRolesLevelCache();

	//zpy ���ȼ�ƥ�������
	int MatchingByLevel(UINT32 userId, UINT32 roleId, UINT32 *findedRoleId);

	//zpy ��ս��ƥ�������
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

	// zpy ����
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