#ifndef __RANK_HANDER_H__
#define __RANK_HANDER_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <vector>
#include "cyaCoreTimer.h"
#include "cyaLocker.h"
#include "cyaThread.h"

#include "GWSProtocol.h"
#include "RankProtocol.h"
#include "NoLockQueue.h"

// 角色排行信息
struct SRoleRankInfo
{
	SRoleCPAttr base;
	union
	{
		UINT32 accountid;
	}ext;
};

typedef void(*pFormat)(SRoleRankInfo &info, char* szSQL, int & len, int j, int index, char* pszRankTime, int main_save);

class RankGWClient;
class RankHandler
{
public:
	RankHandler();
	~RankHandler();

	int  ProcessLogic(RankGWClient* gwsSession, UINT16 cmdCode, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	void OnQueryRoleInfoTimer(char* pszRankTime, int main_save = 0);
	void SetInterval(int updateinterval, int saveinterval);

private:

	int  OnQueryRoleRankInfo(RankGWClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	int  OnQueryRoleInfo(RankGWClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	int  QueryDBRankInfo(ERankType type, const char* pszSQL, UINT32 querynum, std::vector<SRoleCPAttr>& vector);
	int  QueryDBRankInfo(ERankType type, const char* pszSQL, UINT32 querynum, void* pvector);
	int  QueryRankInfo(ERankType type, UINT32 querynum);
	int  GetCacheRankInfo(RankGWClient* gwsSession, SGWSProtocolHead* pHead, SLinkerGetRankInfoReq* & req, int size);
	void SaveRankInfo(ERankType ranktype, char* pszRankTime, int main_save);
	void InsertRankInfoToDB(std::vector<SRoleCPAttr>& vector, char* header, char* pszRankTime, int main_save);
	void InsertRankInfoToDB(std::vector<SRoleRankInfo>& vector, pFormat pf, char* header, char* pszRankTime, int main_save);

	int  OnThreadWorker();
	int  OnLogThreadWorker();
	static int ThreadWorker(void* pParam);
	static int LogThreadWorker(void* pParam);

private:
	std::vector<SRoleCPAttr> m_vecLevel;    // 角色等级排行数据
	std::vector<SRoleCPAttr> m_vecCP;       // 角色战斗力排行数据
	std::vector<SRoleCPAttr> m_vecInst;     // 角色副本排行数据
	std::vector<SRoleCPAttr> m_vecPoint;    // 角色成就点数排行数据
	std::vector<SRoleRankInfo> m_vecComp;   // 角色竞技模式排行数据

	CXTLocker       m_lock[RANK_ACTOR_TYPE_MAX];
	OSThreadSite    m_Thread;       // 查询角色排行信息定时处理线程
	OSThreadSite    m_LogThread;    // 写日志线程
	CNoLockQueue    m_LogQueue;
	BOOL            m_ThreadExit;
	BOOL            m_LogThreadExit;
	int             m_interval;     // 排行更新间隔(ms)
	int             m_save_interval;// 保存排行数据间隔(ms)
	int             m_displaynum;   // 排行榜显示记录数
};

void InitRankHandler();
RankHandler* GetRankHandler();
void DestroyRankHandler();


#endif