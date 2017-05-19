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

	LogInfo(("=========�ⲿ�����������ɹ�!========="));

	RunMainLoop(NULL);
	if (p)
	{
		p->DeleteThis();
	}

	return 0;
}