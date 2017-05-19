// DBServer.cpp : 定义控制台应用程序的入口点。
//

#include "mainloop.h"
#include "minidump.h"
#include "cyaLog.h"

#include "GlobalObj.h"


int main(int argc, char* argv[])
{
	//保存core文件
	SaveExceptionDumpFile();
	

	//设置log目录
#if defined(WIN32)
 	CyaLogSetDir("log\\dbslog");
#else
	CyaLogSetDir("log/dbslog");
#endif
	if (0 != InitializeDBSConfig())
		return 0;
	if (0 != InitializeDBServer())
		return 0;

	LogInfo(("============数据服务器服务器启动成功===========!"));

	RunMainLoop(NULL, 10);

	FinishDBServer();
	FinishDBSConfig();

	return 0;
}
