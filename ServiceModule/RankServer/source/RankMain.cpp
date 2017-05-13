#include "mainLoop.h"
#include "miniDump.h"
#include "cyaLog.h"

#include "ServiceErrorCode.h"
#include "RankDBSClient.h"
#include "RankConfigFileMan.h"
#include "RankGWClientMan.h"
#include "RankHandler.h"

#define CONFIG_PATH			"./cyaRankServer.properties"

int main(int argc, char* argv[])
{
	//保存core文件
	SaveExceptionDumpFile();

	//设置log目录
#if defined(WIN32)
	CyaLogSetDir("log\\rank\\ranklog");
	CyaLogSetDir("log\\rank\\linklog");
	CyaLogSetDir("log\\rank\\mlogiclog");
#else
	RoseLogSetDir(LOG_RANK, "log/rank/ranklog");
	RoseLogSetDir(LOG_LINK, "log/rank/linklog");
	RoseLogSetDir(LOG_MLOGIC, "log/rank/mlogiclog");
#endif
	// 设置每n行flush一次log文件
	CyaLogSetOutScreen(true);
	CyaLogSetFlushLines(2);

	//读取配置文件
	if (!InitRankConfigFileMan(CONFIG_PATH))
	{
		LogDebug(("读取配置文件失败!"));
		goto __end__;
	}

	//初始化数据服务器连接
	if (!InitRankDBSClient())
		goto __end__;

	//初始化排行服务器逻辑处理器
	InitRankHandler();

	//初始化,登陆网关服务器
	if (RANK_OK != InitRankGWClientMan())
		goto __end__;

	LogDebug(("===========排行服务器[%s]启动成功!===========", GetRankConfigFileMan()->GetVersion()));

__end__:
	//主循环
	RunMainLoop();

	//销毁GWSClient资源
	DestroyRankGWClientMan();

	//销毁排行服务器逻辑处理器
	DestroyRankHandler();

	//销毁数据服务器连接对象
	DestroyRankDBSClient();

	//销毁配置管理资源
	DestroyRankConfigFileMan();

	return 0;
}
