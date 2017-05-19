// main.cpp : 定义控制台应用程序的入口点。
//

#include "mainLoop.h"
#include "cyaLog.h"
#include "miniDump.h"

#include "ConfigFileMan.h"
#include "GWSClientMan.h"
#include "DBSClient.h"
#include "MLogicProcessMan.h"
#include "GlobalTimerTaskMan.h"
#include "PersonalTimerTaskMan.h"
#include "GameCfgFileMan.h"
#include "FunModuleMan.h"
#include "CashDBMan.h"
//#include "PersonalRandInstMan.h"

#define CONFIG_PATH			"./cyaMLServer.properties"
#if 0// defined(WIN32)
#include "MLTest.h"
#endif

int main(int argc, char* argv[])
{

	//保存core/dump文件
	SaveExceptionDumpFile();

	//设置log目录
#if defined(WIN32)
	CyaLogSetDir("log\\mlslog");
#else
	CyaLogSetDir("log/mlslog");
#endif

	//初始化配置文件
	if (!InitConfigFileMan(CONFIG_PATH))
	{
		LogInfo(("读取配置文件[%s]失败!", CONFIG_PATH));
		goto __end__;
	}

	//20150928 add by hxw 增加服务器开启时间检测
	while (1)
	{
		LTMSEL nowMSel = GetMsel();
		printf("time:%d\n", nowMSel - GetConfigFileMan()->GetPartOpenTime());
		if (nowMSel>GetConfigFileMan()->GetPartOpenTime())
			break;
		else
			Sleep(60);
	}

	//end



	//初始化角色对象分配器
	InitRoleInfosObjAllocator();

	//连接玩家数据服务器
	if (MLS_OK != InitDBSClient())
	{
		LogInfo(("连接玩家数据服务器失败!"));
		goto __end__;
	}

	//连接充值数据服务器
	if (!ConnectRechargeDBServer())
	{
		LogInfo(("连接充值数据服务器失败!"));
		goto __end__;
	}

	//add by hxw 20151117
	//连接代币数据服务器
	if (!ConnectCashDBServer())
	{
		LogInfo(("连接代币数据服务器失败!"));
		goto __end__;
	}
	//end

	//初始化游戏配置文件
	if (!InitGameCfgFileMan())
	{
		LogInfo(("初始化游戏配置文件失败!"));
		goto __end__;
	}

	//初始化功能模块
	InitFunModuleMan();

	//初始化逻辑处理器
	InitMLogicProcessMan();

	//初始化全局任务管理
	InitGlobalTimerTaskMan();

#if 0//defined(WIN32)
	MLTest* test = new MLTest;
	test->GoTest();
	delete test;
#endif

	//初始化/登陆网关服务器
	if (MLS_OK != InitGWSClientMan())
	{
		LogInfo(("初始化/登陆网关服务器失败!"));
		goto __end__;
	}


	//开启系统公告
	OpenSystemNotice();

	//启动单人随机副本
	//StartPersonalRandInst();

	LogInfo(("=========主逻辑服务器[%s]启动成功!=========", GetConfigFileMan()->GetVersion()));





	//主循环
	RunMainLoop();

__end__:
	//停止单人随机副本
	//StopPersonalRandInst();

	//关闭系统公告
	CloseSystemNotice();

	//释放GWSClientMan资源
	DestroyGWSClientMan();

	//释放全局任务管理
	DestroyGlobalTimerTaskMan();

	//释放逻辑处理器对象
	DestroyMLogicProcessMan();

	//释放功能模块
	DestroyFunModuleMan();

	//断开充值数据服务器
	DisConnectRechargeDBServer();

	//add by hxw 20151117
	//断开代币数据服务器
	DisConnectCashDBServer();

	//释放DBSClient资源
	DestroyDBSClient();

	//释放角色对象分配器
	DestroyRoleInfosObjAllocator();

	//释放游戏配置文件管理
	DestroyGameCfgFileMan();

	//释放ConfigFileMan资源
	DestroyConfigFileMan();

	getchar();

	return 0;
}

