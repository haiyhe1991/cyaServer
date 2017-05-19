#ifndef __COMPETITIVE_MODE_MAN_H__
#define __COMPETITIVE_MODE_MAN_H__

#include <vector>
#include "cyaTypes.h"
#include "cyaLocker.h"
#include "cyaCoreTimer.h"
#include "GameTypeCfg.h"

/**
* ����ģʽ����
*/
class CompetitiveModeMan
{
	CompetitiveModeMan(const CompetitiveModeMan&);
	CompetitiveModeMan& operator = (const CompetitiveModeMan&);

public:
	CompetitiveModeMan();
	~CompetitiveModeMan();

public:
	void Start();
	void Stop();

	// ÿ������
	void ClearEveryday();

	// ���������ɫ
	void GeneraWeekFreeActorType();

	// ��ȡ�����ɫ
	BYTE GetWeekFreeActorType() const;

	// ��ѯ����ģʽ�������
	int QueryEnterNum(UINT32 userId, UINT32 roleId, SQueryCompEnterNumRes *pEnterNum);

	// ���򾺼�ģʽ�������(mode ����ģʽ 1��ʾ����һ�Σ� 2��ʾ��������ʣ��)
	int BuyEnterNum(UINT32 userId, UINT32 roleId, BYTE mode, SBuyCompEnterNumRes *pEnterNum);

	// ���뾺��ģʽ
	int EnterCompetitiveMode(UINT32 userId, UINT32 roleId);

	// ��ɾ���ģʽ
	int FinishCompetitiveMode(UINT32 userId, UINT32 roleId);

	// ���ž���ģʽ����
	int GrantCompetitiveModeReward();

private:
	// ��ȡ����ģʽ����
	void ReceiveCompetitiveModeReward(UINT32 userId, UINT32 roleId, UINT32 rank);

	// ���ͽ����ʼ�
	void SendRewardEmail(UINT32 userId, UINT32 roleId, const std::vector<GameTypeCfg::SRewardInfo> &rewardVec, const char *body);

private:
	static void CompetitiveModeTimer(void* param, TTimerID id);
	void OnFinishCompetitiveMode();
	void OnRefreshDBFinishCompetitiveMode(UINT32 roleId);

private:
	BYTE m_weekFreeActorType;
	CXTLocker m_locker;
	CyaCoreTimer m_timer;
	std::vector<UINT32> m_finishCompetitive;
};

void InitCompetitiveModeMan();
CompetitiveModeMan* GetCompetitiveModeMan();
void DestroyCompetitiveModeMan();

#endif