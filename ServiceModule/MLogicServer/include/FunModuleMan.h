#ifndef __FUN_MODULE_MAN_H__
#define __FUN_MODULE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
功能模块管理
*/

#include "GuildMan.h"
#include "PlayerInfoMan.h"
#include "FriendMan.h"
#include "RefreshDBMan.h"
#include "EmailCleanMan.h"
#include "PropSellMan.h"
#include "RolesInfoMan.h"
#include "InstRankInfoMan.h"
#include "InstFinHistoryMan.h"
#include "TaskFinHistoryMan.h"
#include "RecoveryVitMan.h"
#include "RechargeMan.h"
#include "ShoppingMallMan.h"
#include "InstOccupyNotify.h"
#include "ConsumeProduceMan.h"
#include "AchievementMan.h"
#include "PersonalRandInstMan.h"
#include "SystemNoticeMan.h"
#include "GoldExchangeMan.h"
#include "VIPMan.h"
#include "DailyTaskMan.h"
#include "PermanenDataMan.h"
#include "GMLogicProcMan.h"
#include "OnlineSingnInMan.h"
#include "ActitvityMan.h"
#include "UnlockContentMan.h"
#include "PlayerActionMan.h"
#include "VitExchangeMan.h"
#include "WorldBossMan.h"
#include "LadderRankMan.h"
#include "GuardIconMan.h"
#include "RankRewardMan.h"
#include "LadderRankMan.h"
#include "ActitvityInstMan.h"
#include "CompetitiveModeMan.h"
#include "OneVsOneMan.h"
#include "MessageTipsMan.h"

class FunModuleMan
{
	FunModuleMan(const FunModuleMan&);
	FunModuleMan& operator = (const FunModuleMan&);

public:
	FunModuleMan();
	~FunModuleMan();
};

void InitFunModuleMan();
void DestroyFunModuleMan();

#endif	