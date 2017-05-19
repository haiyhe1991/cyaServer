#ifndef __INST_FIN_HISTORY_MAN_H__
#define __INST_FIN_HISTORY_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
������ʷ��¼����
*/

#include <map>
#include <vector>
#include "cyaCoreTimer.h"
#include "ServiceProtocol.h"

class InstFinHistoryMan
{
	InstFinHistoryMan(const InstFinHistoryMan&);
	InstFinHistoryMan& operator = (const InstFinHistoryMan&);

	struct SInstHistoryRecord	//��Ҫ��¼�ĸ��������Ϣ
	{
		UINT32 roleId;	//��ɫid
		UINT16 instId;	//����id
		BYTE   star;	//����
		UINT16 comlatedTimes;	//��ǰ���߽��븱������
		char   fintts[20];		//���ʱ��

		SInstHistoryRecord()
		{
			roleId = 0;
			instId = 0;
			star = 0;
			comlatedTimes = 0;
			memset(fintts, 0, sizeof(fintts));
		}

		SInstHistoryRecord(UINT32 finRoleId, UINT16 finInstId, BYTE finStar, UINT16 times = 1)
		{
			roleId = finRoleId;
			instId = finInstId;
			star = finStar;
			comlatedTimes = times;
			GetLocalStrTime(fintts);
		}
	};

public:
	InstFinHistoryMan();
	~InstFinHistoryMan();

	//��ɸ�����ʷ��¼
	void InputInstFinRecord(UINT32 roleId, UINT16 instId, BYTE star);
	void EnforceSyncInstFinRecord(UINT32 roleId);

	//��ѯ����ɸ���
	int  QueryFinishedInst(UINT32 roleId, UINT16 from, BYTE num, SQueryFinishedInstRes* pFinInstRes, BYTE maxNum);
	//�ø����Ƿ������
	BOOL IsFinishedInst(UINT32 roleId, UINT16 instId);

	//��ѯ�������ս����
	int GetAvtiveInstChallengeTimes(UINT32 roleId, SGetActiveInstChallengeTimesRes* pChallengeTimesRes);
	//���»������ɴ���
	int UpdateAvtiveInstFinTimes(UINT32 roleId, UINT16 instId);
	//������ҵ���������¼
	void ClearPlayersActiveInstRecord();



private:
	static void RefreshInstFinHistoryTimer(void* param, TTimerID id);
	void OnRefreshInstFinHistory();
	int  UpdateInstFinRecord(const SInstHistoryRecord& rec);

private:
	//ˢ�¶�ʱ��
	CyaCoreTimer m_refreshTimer;

	//��ɸ�����ʷ��Ϣ
	CXTLocker m_locker;
	std::map<std::string, SInstHistoryRecord> m_instHistoryMap;
};

void InitInstFinHistoryMan();
InstFinHistoryMan* GetInstFinHistoryMan();
void DestroyInstFinHistoryMan();

#endif	//_InstFinHistoryMan_h__