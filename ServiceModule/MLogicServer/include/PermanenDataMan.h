#ifndef __PERMANEN_DATA_MAN_H__
#define __PERMANEN_DATA_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
角色永久数据(引导信息,魔导器)
*/

#include <map>
#include <vector>
#include "ServiceMLogic.h"
#include "cyaCoretimer.h"

enum EPermanenDataType
{
	PERMANENT_HELP = 1,			//新手引导信息
	PERMANENT_MAGIC = 2,		//魔导器信息
};

#define PERMANENT_DATA_ID_START		PERMANENT_HELP
#define PERMANENT_DATA_ID_END		PERMANENT_MAGIC

class PermanenDataMan
{
	PermanenDataMan(const PermanenDataMan&);
	PermanenDataMan& operator = (const PermanenDataMan&);

	struct SUpdatePermanenData	//需要更新的角色
	{
		UINT32 userId;	//用户id
		UINT32 roleId;	//角色id
		UINT16 type;	//需要更新状态

		SUpdatePermanenData()
		{
			userId = 0;
			roleId = 0;
			type = 0;
		}

		SUpdatePermanenData(UINT32 user, UINT32 role, UINT16 nType)
		{
			userId = user;
			roleId = role;
			type = nType;
		}
	};

public:
	PermanenDataMan();
	~PermanenDataMan();

public:
	//查询已完成新手引导
	int QueryRoleFinishedHelps(UINT32 roleId, SQueryFinishedHelpRes* pHelpRes);
	//查询角色拥有魔导器
	int QueryRoleMagics(UINT32 roleId, SQueryRoleMagicsRes* pMagicRes);
	//初始化新角色永久数据
	int InitNewRolePermanenData(UINT32 roleId, const std::vector<UINT16>& magicVec);
	//更新信息
	void InputUpdatePermanenDataInfo(UINT32 userId, UINT32 roleId, UINT16 type);
	//用户退出时同步
	void ExitSyncUserPermanenData(UINT32 userId, const std::vector<UINT32>& rolesVec);

private:
	static void RefreshPermanenDataTimer(void* param, TTimerID id);
	void OnRefreshPermanenData();
	void OnRefreshPermanenData(const SUpdatePermanenData& info);

private:
	void OnUpdateRoleHelps(UINT32 userId, UINT32 roleId);
	void OnUpdateRoleMagics(UINT32 userId, UINT32 roleId);

private:
	//角色数据信息
	CXTLocker m_locker;
	CyaCoreTimer m_refreshDBTimer;
	std::map<std::string/*key*/, SUpdatePermanenData> m_updateInfoMap;
};

void InitPermanenDataMan();
PermanenDataMan* GetPermanenDataMan();
void DestroyPermanenDataMan();

#endif	//_PermanenDataMan_h__