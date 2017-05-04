// main.cpp : 管理服务器应用程序的入口点。
//
#include "stdafx.h"
#include "GMCfgMan.h"
#include "GMServeMan.h"
#include "cyaIpCvt.h"
#include "mainloop.h"
#include "minidump.h"
#include "cyaLog.h"
#include "OtherServeConnMan.h"
#include "ServeConnMan.h"


int main(int argc, char** argv)
{
	///保存core文件
	SaveExceptionDumpFile();

	///设置日志文件名目录
	CyaLogSetDir("gmserver");

	///设置每隔几行刷新日志文件
	CyaLogSetFlushLines(2);

	///加载管理管理服务器配置
	InitCfgManObj("risegmserver.properties");

	///初始化管理服务器对象
	InitGMServeManObj();

	///启动服务
	int ret = GetGMServeManObj()->GetGMServeMan()->StartServe(IPPADDR(INADDR_ANY, GetCfgManObj()->GetServePort()));
	if (ret != 0)
		LogInfo(("服务器启动失败,[%d]", ret));
	else
	{
#if IS_SCREEN_LOG
		LogInfo(("管理服务器服务器[端口号: %d]启动成功!", GetCfgManObj()->GetServePort()));
#endif
	}



	///初始化时时写入db服务+


	InitOtherServeManObj();

	RunMainLoop();

	///销毁配置管理对象
	DestroyCfgManObj();


	///停止服务
	GetGMServeManObj()->GetGMServeMan()->StopServe();

	///销毁和停止其它写入db服务
	DestroyOtherServeManObj();


	///销毁服务器对象
	DestroyGMServeManObj();

	return 0;
}

