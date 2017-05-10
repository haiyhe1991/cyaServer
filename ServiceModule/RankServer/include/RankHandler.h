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

// ��ɫ������Ϣ
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
	std::vector<SRoleCPAttr> m_vecLevel;    // ��ɫ�ȼ���������
	std::vector<SRoleCPAttr> m_vecCP;       // ��ɫս������������
	std::vector<SRoleCPAttr> m_vecInst;     // ��ɫ������������
	std::vector<SRoleCPAttr> m_vecPoint;    // ��ɫ�ɾ͵�����������
	std::vector<SRoleRankInfo> m_vecComp;   // ��ɫ����ģʽ��������

	CXTLocker       m_lock[RANK_ACTOR_TYPE_MAX];
	OSThreadSite    m_Thread;       // ��ѯ��ɫ������Ϣ��ʱ�����߳�
	OSThreadSite    m_LogThread;    // д��־�߳�
	CNoLockQueue    m_LogQueue;
	BOOL            m_ThreadExit;
	BOOL            m_LogThreadExit;
	int             m_interval;     // ���и��¼��(ms)
	int             m_save_interval;// �����������ݼ��(ms)
	int             m_displaynum;   // ���а���ʾ��¼��
};

void InitRankHandler();
RankHandler* GetRankHandler();
void DestroyRankHandler();


#endif