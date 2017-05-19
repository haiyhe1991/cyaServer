// DBServer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "mainloop.h"
#include "minidump.h"
#include "cyaLog.h"

#include "GlobalObj.h"


int main(int argc, char* argv[])
{
	//����core�ļ�
	SaveExceptionDumpFile();
	

	//����logĿ¼
#if defined(WIN32)
 	CyaLogSetDir("log\\dbslog");
#else
	CyaLogSetDir("log/dbslog");
#endif
	if (0 != InitializeDBSConfig())
		return 0;
	if (0 != InitializeDBServer())
		return 0;

	LogInfo(("============���ݷ����������������ɹ�===========!"));

	RunMainLoop(NULL, 10);

	FinishDBServer();
	FinishDBSConfig();

	return 0;
}
