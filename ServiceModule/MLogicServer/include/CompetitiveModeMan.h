#ifndef __COMPETITIVE_MODE_MAN_H__
#define __COMPETITIVE_MODE_MAN_H__

#include <vector>
#include "cyaTypes.h"
#include "cyaLocker.h"
#include "cyaCoreTimer.h"
#include "GameTypeCfg.h"

/**
* 竞技模式管理
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

	// 每日清理
	void ClearEveryday();

	// 生成周免角色
	void GeneraWeekFreeActorType();

	// 获取周免角色
	BYTE GetWeekFreeActorType() const;

	// 查询竞技模式进入次数
	int QueryEnterNum(UINT32 userId, UINT32 roleId, SQueryCompEnterNumRes *pEnterNum);

	// 购买竞技模式进入次数(mode 购买模式 1表示购买一次， 2表示购买所有剩余)
	int BuyEnterNum(UINT32 userId, UINT32 roleId, BYTE mode, SBuyCompEnterNumRes *pEnterNum);

	// 进入竞技模式
	int EnterCompetitiveMode(UINT32 userId, UINT32 roleId);

	// 完成竞技模式
	int FinishCompetitiveMode(UINT32 userId, UINT32 roleId);

	// 发放竞技模式奖励
	int GrantCompetitiveModeReward();

private:
	// 领取竞技模式奖励
	void ReceiveCompetitiveModeReward(UINT32 userId, UINT32 roleId, UINT32 rank);

	// 发送奖励邮件
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