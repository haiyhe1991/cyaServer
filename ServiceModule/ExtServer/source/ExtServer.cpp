#include "mainLoop.h"
#include "ExtServerMan.h"
#include "cyaLog.h"

int main(int argc, char* argv[])
{
	ExtServerMan* p = ExtServerMan::InstanceServerMan();
	if (p)
	{
		p->Initialize();
	}

	LogInfo(("=========外部服务器启动成功!========="));

	RunMainLoop(NULL);
	if (p)
	{
		p->DeleteThis();
	}

	return 0;
}