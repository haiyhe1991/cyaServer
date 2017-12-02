// ChatServer.cpp : 定义控制台应用程序的入口点。
//
#include "mainloop.h"
#include "minidump.h"
#include "cyaLog.h"

#include "MauGWSClientMan.h"
#include "ConfigFileMan.h"
#include "MauTableMan.h"
#include "MauMsgMan.h"

#define CONFIG_PATH			"./cyaMauServer.properties"

int main(int argc, char* argv[])
{
	//保存core文件
	SaveExceptionDumpFile();

	//设置log目录
#if defined(WIN32)
	CyaLogSetDir("log\\maulog");
#else
	CyaLogSetDir("log/maulog");
#endif

	//读取配置文件
	if (!InitConfigFileMan(CONFIG_PATH))
	{
		LogInfo(("读取配置文件失败!"));
		goto __end__;
	}

	//初始化频道管理
	InitMauTableMan();

	//初始化mau逻辑处理器
	InitMauMsgMan();

	//初始化,登陆网关服务器
	if (MAU_OK != InitGWSClientMan())
	{
		LogInfo(("登录网关服务器失败!"));
		goto __end__;
	}

	LogInfo(("===========变色龙棋牌服务器[%s]启动成功!===========", GetConfigFileMan()->GetVersion()));

__end__:
	//主循环
	RunMainLoop();

	//销毁GWSClient资源
	DestroyGWSClientMan();

	//销毁mau逻辑处理器
	DestroyMauMsgMan();

	//销毁频道管理资源
	DestroyMauTableMan();

	//销毁配置管理资源
	DestroyConfigFileMan();

	return 0;
}

