#include "FunModuleMan.h"

static FunModuleMan* sg_funModMan = NULL;
void InitFunModuleMan()
{
	ASSERT(sg_funModMan == NULL);
	sg_funModMan = new FunModuleMan();
	ASSERT(sg_funModMan != NULL);
}

void DestroyFunModuleMan()
{
	FunModuleMan* funModMan = sg_funModMan;
	sg_funModMan = NULL;
	if (funModMan)
		delete funModMan;
}

FunModuleMan::FunModuleMan()
{
	//初始化系统公告
	InitSystemNoticeMan();

	//初始化角色引导,魔导器数据管理
	InitPermanenDataMan();

	//初始化用户角色信息
	InitRolesInfoMan();

	//初始化副本排行信息
	InitInstRankInfoMan();

	//初始化数据库刷新管理对象
	InitRefreshDBMan();

	//初始化副本历史记录管理
	InitInstFinHistoryMan();

	//初始化日常任务管理
	InitDailyTaskMan();

	//初始化完成任务历史记录管理
	InitTaskFinHistoryMan();

	//初始化物品出售管理
	InitPropSellMan();

	//初始化邮件管理
	InitEmailCleanMan();

	//初始化好友管理
	InitFriendMan();

	//初始化公会管理
	InitGuildMan();

	//初始化机器人管理
	InitPlayerInfoMan();

	//初始化体力恢复
	InitRecoveryVitMan();

	//初始化充值管理
	//InitRechargeMan();

	//初始化商城管理
	InitShoppingMallMan();

	//初始化副本占领邮件通知
	//InitInstOccupyNotify();

	//初始化消费记录管理
	InitConsumeProduceMan();

	//初始化单人随机副本
	//InitPersonalRandInstMan();

	//初始化金币兑换
	InitGoldExchangeMan();

	//初始化VIP管理
	InitVIPMan();

	//初始化上线签到管理
	InitOnlineSingnInMan();

	//初始化活动管理
	InitActitvityMan();

	//初始化解锁
	InitUnlockContentMan();

	//初始化玩家行为管理
	InitPlayerActionMan();

	//初始化外部管理服务
	InitGMLogicProcMan();

	//初始化成就管理
	InitAchievementMan();

	//初始化体力兑换
	InitVitExchangeMan();

	//初始化世界Boss
	InitWorldBossMan();

	//初始化天梯排行
	InitLadderRankMan();

	//初始化守卫圣像
	InitGuardIconMan();

	//初始化活动副本
	InitActitvityInstMan();

	//add by hxw 20151020
	//初始化排行奖励活动
	InitRankRewardMan();
	//end;

	//初始化竞技模式
	InitCompetitiveModeMan();

	//初始化1v1模式
	InitOneVsOneMan();

	//初始化消息提示
	InitMessageTipsMan();
}

FunModuleMan::~FunModuleMan()
{
	//释放外部管理服务
	DestroyGMLogicProcMan();

	//释放玩家行为管理
	DestroyPlayerActionMan();

	//释放解锁管理
	DestroyUnlockContentMan();

	//清理活动管理
	DestroyActitvityMan();

	//清理上线签到管理
	DestroyOnlineSingnInMan();

	//释放VIP管理
	DestroyVIPMan();

	//释放化金币兑换
	DestroyGoldExchangeMan();

	//释放单人随机副本管理
	//DestroyPersonalRandInstMan();

	//释放消费记录管理
	DestroyConsumeProduceMan();

	//释放副本占领邮件通知
	//DestroyInstOccupyNotify();

	//释放商城管理
	DestroyShoppingMallMan();

	//释放充值管理
	//DestroyRechargeMan();

	//释放体力恢复
	DestroyRecoveryVitMan();

	//释放机器人管理
	DestroyPlayerInfoMan();

	//释放公会管理
	DestroyGuildMan();

	//释放好友管理
	DestroyFriendMan();

	//释放邮件管理
	DestroyEmailCleanMan();

	//销毁物品出售管理
	DestroyPropSellMan();

	//销毁完成任务历史记录管理
	DestroyTaskFinHistoryMan();

	//销毁日常任务管理
	DestroyDailyTaskMan();

	//销毁副本历史记录管理
	DestroyInstFinHistoryMan();

	//销毁数据库刷新管理对象
	DestroyRefreshDBMan();

	//销毁副本排行信息
	DestroyInstRankInfoMan();

	//释放用户角色信息资源
	DestroyRolesInfoMan();

	//释放角色引导,魔导器数据管理
	DestroyPermanenDataMan();

	//销毁系统公告
	DestroySystemNoticeMan();

	//销毁成就管理
	DestroyAchievementMan();

	//销毁世界Boss
	DestroyWorldBossMan();

	//销毁天梯排行
	DestroyLadderRankMan();

	//销毁守卫圣像
	DestroyGuardIconMan();

	//销毁活动副本
	DestroyActitvityInstMan();

	//add by hxw 20151020
	//销毁排行奖励活动模块
	DestroyRankRewardMan();
	//end

	//销毁竞技模式
	DestroyCompetitiveModeMan();

	//销毁1v1模式
	DestroyOneVsOneMan();

	//销毁消息提示
	DestroyMessageTipsMan();
}