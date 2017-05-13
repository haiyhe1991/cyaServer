#include "mainloop.h"
#include "miniDump.h"
#include "cyaLog.h"

#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "LicenceServeMan.h"
#include "LicenceLogicProcMan.h"
#include "PartitionInfoMan.h"
#include "UserInfoMan.h"

#define CONFIG_PATH			"./cyaLCServer.properties"

int main(int argc, char* argv[])
{
	//保存core文件
	SaveExceptionDumpFile();

	//设置log目录
#if defined(WIN32)
	CyaLogSetDir("log\\lcslog");
#else
	CyaLogSetDir("log/lcslog");
#endif

	//初始化配置文件
	if (!InitConfigFileMan(CONFIG_PATH))
	{
		LogInfo(("读取配置文件失败!"));
		goto __end__;
	}

	//初始化数据服务器连接
	if (!InitDBSClient())
	{
		LogInfo(("初始化数据服务器连接失败!"));
		goto __end__;
	}

	//初始化用户信息管理对象
	InitUserInfoMan();

	//初始化分区信息
	if (!InitPartitionInfoMan())
	{
		LogInfo(("初始化分区信息失败!"));
		goto __end__;
	}

	//验证服务器逻辑处理
	InitLicenceLogicProcMan();

	//启动网络服务
	StartLicenceServe();

	LogInfo(("========验证服务器[%s][%d]启动成功!========", GetConfigFileMan()->GetVersion(), GetConfigFileMan()->GetServePort()));

__end__:

	//主循环
	RunMainLoop();


	//停止网络服务
	StopLicenceServe();

	//验证服务器逻辑处理
	DestroyLicenceLogicProcMan();

	//清理分区信息
	DestroyPartitionInfoMan();

	//销毁用户信息管理对象
	DestroyUserInfoMan();

	//销毁数据服务器连接对象
	DestroyDBSClient();

	//销毁配置文件管理对象
	DestroyConfigFileMan();

	return 0;
}