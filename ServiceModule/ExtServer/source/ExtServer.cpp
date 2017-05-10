#include "mainLoop.h"
#include "ExtServerMan.h"

int main(int argc, char* argv[])
{
	ExtServerMan* p = ExtServerMan::InstanceServerMan();
	if (p)
	{
		p->Initialize();
	}
	RunMainLoop(NULL);
	if (p)
	{
		p->DeleteThis();
	}

	return 0;
}