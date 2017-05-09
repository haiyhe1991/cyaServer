
#include "mainLoop.h"
#include "minidump.h"
#include "cyaLog.h"

#include "LinkServer.h"
#include "LinkGlobalObj.h"
#include "LicenceClient.h"


int main()
{
	//保存core文件
	SaveExceptionDumpFile();

	//设置log目录
#if defined(WIN32)
	CyaLogSetDir("log\\linklog");
#else
	RoseLogSetDir("log/linklog");
#endif

	SockInit();
	if (LINK_OK != InitConfig())
	{
		UnitConfig();
		return LINK_ERROR;
	}

	InitLSLink();

	if (LINK_OK != InitLinkServer())
	{
		UnitLinkServer();
		UnitLSLink();
		UnitConfig();
		return LINK_ERROR;
	}
	if (GlobalGetLicenceClient()->LSClientStatus())
	{
		const int MaxTryGetStatusNum = 10;
		BYTE lkStatus = LicenceClient::LINK_STATUS_UNKOWN;	/// 0--开启 1--停止
		for (int trycount = 0; trycount < MaxTryGetStatusNum; ++trycount)
		{
			if (LINK_OK == GlobalGetLicenceClient()->GetLinkStatus(lkStatus))
			{
				break;
			}
			Sleep(10000);
		}

		if (LicenceClient::LINK_STATUS_ON == lkStatus)
		{
			GlobalGetLinkServer()->StartLinkServer();
		}
	}

	RunMainLoop(NULL);

	UnitLinkServer();
	UnitLSLink();
	UnitConfig();

	return 0;
}