// RoomDaemon.cpp : 定义控制台应用程序的入口点。
//
#include "win_service.h"
#include "RoomClient.h"
#include "DaemonServer.h"
#include "RoomConfig.h"
#include "GWSClient.h"

#include "miniDump.h"
#include "mainLoop.h"
#include "cyaLog.h"

CRoomConfig* g_daemonConfig = NULL;
CRoomClient* g_roomClient = NULL;
CDaemonServer* g_daemonServer = NULL;
CGWSClient* g_gwClient = NULL;

void TestServiceStopCallback(DWORD dwOpt, void* userData)
{
	if (dwOpt == SERVICE_CONTROL_STOP)
	{
		if (NULL != g_roomClient)
		{
			g_roomClient->RoomClientStop();
			g_roomClient->ReleaseRoomClientInstance();
			g_roomClient = NULL;
		}
		if (NULL != g_daemonServer)
		{
			g_daemonServer->DaemonServerStop();
			g_daemonServer->ReleaseDaemonServerInstance();
			g_daemonServer = NULL;
		}

		if (NULL != g_gwClient)
		{
			g_gwClient->GateWayClientStop();
			g_gwClient->ReleaseGateWayClientInstance();
			g_gwClient = NULL;
		}
	}
}

//int WINAPI WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)  
int main()
{
	g_daemonConfig = CRoomConfig::FetchConfigInstance();
	ASSERT(NULL != g_daemonConfig);
	g_roomClient = CRoomClient::FetchRoomClientInstance();
	ASSERT(NULL != g_roomClient);
	g_daemonServer = CDaemonServer::FetchDaemonServerInstance();
	ASSERT(NULL != g_daemonServer);

	if (NULL != g_daemonServer)
		g_daemonServer->DaemonServerStart();
	if (NULL != g_roomClient)
		g_roomClient->RoomClientStart();

	g_gwClient = CGWSClient::FetchGateWayClientInstance();
	ASSERT(NULL != g_gwClient);
	if (NULL != g_gwClient)
		g_gwClient->GateWayClientStart();

	LogInfo(("===========房间管理服务器服务器启动成功=========!"));

	RunMainLoop(NULL);

	if (NULL != g_roomClient)
	{
		g_roomClient->RoomClientStop();
		g_roomClient->ReleaseRoomClientInstance();
		g_roomClient = NULL;
	}
	if (NULL != g_daemonServer)
	{
		g_daemonServer->DaemonServerStop();
		g_daemonServer->ReleaseDaemonServerInstance();
		g_daemonServer = NULL;
	}

	if (NULL != g_gwClient)
	{
		g_gwClient->GateWayClientStop();
		g_gwClient->ReleaseGateWayClientInstance();
		g_gwClient = NULL;
	}

	return 0;
}
