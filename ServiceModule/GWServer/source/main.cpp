// GWServer.cpp : 定义控制台应用程序的入口点。
//
#include "mainloop.h"
#include "minidump.h"
#include "cyaLog.h"

#include "GWSession.h"
#include "ConfigFileMan.h"
#include "RelaySessionMan.h"
#include "GWServeMan.h"


int main(int argc, char* argv[])
{
	//保存core文件
	SaveExceptionDumpFile();

	//设置log目录
#if defined(WIN32)
	CyaLogSetDir("log\\gwslog");
#else
	RoseLogSetDir("log/gwslog");
#endif

	//初始化配置文件管理
	InitConfigFileMan();

	//初始化转发会话管理
	InitRelaySessionMan();

	//启动网络服务
	StartGWServe();

	LogInfo(("========网关服务器[%s][%d]启动成功!========", GetConfigFileMan()->GetVersion(), GetConfigFileMan()->GetServePort()));

	//主循环
	RunMainLoop();

	//停止网络服务
	StopGWServe();

	//销毁转发会话管理
	DestroyRelaySessionMan();

	//销毁配置文件管理
	DestroyConfigFileMan();

	return 0;
}
