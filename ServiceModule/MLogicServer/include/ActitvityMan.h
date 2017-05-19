#ifndef __ACTIVITY_MAN_H__
#define __ACTIVITY_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
活动管理
*/

#include <vector>
#include "cyaTypes.h"
#include "ServiceMLogic.h"
#include "RouletteCfg.h"
#include "GameCfgFileMan.h"

class ActitvityMan
{
public:
	struct SDBSign
	{
		BYTE day;

		// 用于排序
		bool operator< (const SDBSign &that) const
		{
			return day < that.day;
		}
	};

public:
	ActitvityMan();
	~ActitvityMan();

	//查询已经领取的活动
	int QueryReceivedActivity(UINT32 roleId, SQueryReceivedActivityRes* pActivityRes);
	//是否领取过的活动
	BOOL IsReceivedActivity(UINT32 roleId, UINT32 activityId);
	//刷新参加活动
	void RefreshReceivedActivity(UINT32 roleId, UINT32 activityId, const char* pszDesc = "");

	//兑换码校验
	int VerifyCashCode(const char* pszCode, UINT32 roleId, UINT16& rewardId);

	//获取轮盘奖品
	int GetRouletteReward(UINT32 roleId, SGetRoulettePropRes* pRoulettePropRes);
	int GetRouletteReward(UINT32 roleId, UINT16& extractNum, std::vector<RouletteCfg::SRoulettePropCfgItem>& itemVec);
	//更新角色轮盘剩余奖品
	int UpdateRoleRouletteReward(UINT32 roleId, UINT16 extractNum, const std::vector<RouletteCfg::SRoulettePropCfgItem>& itemVec);

	//获取轮盘大奖
	int FetchRouletteBigReward(UINT32 roleId, UINT16& extractNum, SRoulettePropItem& bigReward);
	//更新轮盘大奖
	int UpdateRouletteBigReward(UINT32 roleId, const SRoulettePropItem& bigReward);
	//清理轮盘奖品
	void ClearRouletteReward();

	/* zpy 查询签到 */
	int QueryRoleDailySign(UINT32 roleId, SQueryRoleDailySignRes *pQueryRoleSign);
	/* zpy 角色签到 */
	int OnRoleDailySign(INT32 userID, UINT32 roleId, SRoleDailySignRes *pRoleSign, SReceiveRewardRefresh *pRefresh);
	/* zpy 清理每日签到 */
	void ClearRoleDailySign(/*UINT32 roleId delete by hxw 20150929*/);

private:
	/* zpy 查询角色签到信息 */
	int QueryRoleDailySignFromDB(UINT32 roleId, std::vector<SDBSign> &out_days);
	/* zpy 写入角色签到信息 */
	int WriteRoleDailySignToDB(INT32 roleId, std::vector<SDBSign> &days);
};

void InitActitvityMan();
ActitvityMan* GetActitvityMan();
void DestroyActitvityMan();

#endif