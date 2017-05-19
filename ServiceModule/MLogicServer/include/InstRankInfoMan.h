#ifndef __INST_RANK_INFO_MAN_H__
#define __INST_RANK_INFO_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
副本完成排行信息
*/

#include <map>
#include <list>
#include "ServiceMLogic.h"
#include "cyaLocker.h"
#include "cyaCoreTimer.h"
#include "RoleInfos.h"

class InstRankInfoMan
{
private:
	InstRankInfoMan(const InstRankInfoMan&);
	InstRankInfoMan& operator = (const InstRankInfoMan&);

	struct SInstRankTable	//副本完成排行表
	{
		UINT32 roleId;		//角色id
		UINT32 userId;		//用户id
		char   roleNick[33];//角色昵称
		UINT32 score;		//评分
		char   tts[20];		//完成时间1970-01-01 00:00:00
		UINT32 uniqueId;	//在数据库中的唯一id
		BYTE   up;			//是否更新排行中的某条记录

		SInstRankTable()
		{
			memset(this, 0, sizeof(SInstRankTable));
		}
	};

	class InstRankSite
	{
	public:
		InstRankSite(UINT16 instId);
		~InstRankSite();

	public:
		void UpdateInstRank(SInstRankTable& rank, std::list<SInstOccupyEmail>& emailList);
		void InsertInstRank(SInstRankTable& rank);
		BYTE QueryInstRankInfo(SInstRankInfo* pRankInfo, BYTE rankInfoNum);
		void SyncDBInstRank();
		int  GetOccupyInstIdByRoleId(UINT32 roleId, UINT16& instId);
		int  GetInstOccupyInfo(UINT32& userId, UINT32& roleId);
		void DelRoleInstRank(UINT32 roleId);

	private:
		UINT16 m_instId;
		BYTE m_instGameMode;
		CXTLocker m_rankListLocker;
		std::list<SInstRankTable> m_rankList;
	};

public:
	InstRankInfoMan();
	~InstRankInfoMan();

	//副本占领角色
	struct SInstOccupyRole
	{
		UINT16 instId;
		UINT32 roleId;
		UINT32 userId;
	};

	//载入副本排行信息
	void LoadInstRankInfo();

	//更新副本完成排行表
	void UpdateInstRankInfo(UINT16 instId, UINT32 userId, const SInstRankInfo& rank, std::list<SInstOccupyEmail>& emailList);

	//查询排行信息
	BYTE QueryInstRankInfo(UINT16 instId, SInstRankInfo* pRankInfo, BYTE rankInfoNum);

	//获取角色占领副本
	int GetRoleOccupyInst(UINT32 userId, UINT32 roleId, SGetRoleOccupyInstRes* pOccupyInstRes, int maxNum);

	//获取副本占领信息
	void GetInstOccupyInfo(std::list<SInstOccupyRole>& occupyList);

	//删除角色副本排名
	void DelRoleInstRank(UINT32 roleId);

private:
	void InsertDBInstRankTable(UINT16 instId, SInstRankTable& instRank);
	static void SyncInstRankTimer(void* param, TTimerID id);
	void OnSyncInstRank();

private:
	CXTLocker m_locker;
	std::map<UINT16/*副本id*/, InstRankSite*> m_instRankMap;
	CyaCoreTimer m_refreshDBTimer;		//刷数据库timer
};

//初始化副本完成排行信息
void InitInstRankInfoMan();
InstRankInfoMan* GetInstRankInfoMan();
void DestroyInstRankInfoMan();

#endif	//_InstRankInfoMan_h__