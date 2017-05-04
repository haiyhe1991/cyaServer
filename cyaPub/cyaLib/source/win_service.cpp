#if defined(WIN32)
#include "win_service.h"

static BOOL sg_isRegist = false;
static TCHAR sg_serviceName[256];
static DWORD sg_thdId;
static SERVICE_STATUS_HANDLE sg_hServiceStatus;
static SERVICE_STATUS sg_serviceStatus;
static DWORD sm_idleTick = 1;
static fnServiceStopCallback sm_callback = NULL;
static void* sm_userData = NULL;

static void WINAPI ServiceCtrl__(DWORD dwMsg)
{
	switch (dwMsg)
	{
	case SERVICE_CONTROL_STOP:
		sg_serviceStatus.dwCurrentState = SERVICE_STOPPED;//SERVICE_STOP_PENDING;
		SetServiceStatus(sg_hServiceStatus, &sg_serviceStatus);
		if (sm_callback)
			sm_callback(SERVICE_CONTROL_STOP, sm_userData);
		PostThreadMessage(sg_thdId, WM_CLOSE, 0, 0);
		break;
	case SERVICE_CONTROL_PAUSE:
		break;
	case SERVICE_CONTROL_CONTINUE:
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		break;
	case SERVICE_CONTROL_PARAMCHANGE:
		break;
	default:
		break;
	}
}

static void WINAPI ServiceMain__(DWORD dwNumServicesArgs, LPSTR* lpServiceArgVectors)
{
	sg_serviceStatus.dwCurrentState = SERVICE_START_PENDING;
	sg_serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	//注册服务控制函数
	sg_hServiceStatus = RegisterServiceCtrlHandler(sg_serviceName, ServiceCtrl__);
	if (!sg_hServiceStatus)
		return;

	//设置服务状态
	//SetServiceStatus(sg_hServiceStatus, &sg_serviceStatus);

	sg_serviceStatus.dwWin32ExitCode = S_OK;
	sg_serviceStatus.dwCheckPoint = 0;
	sg_serviceStatus.dwWaitHint = 0;
	sg_serviceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(sg_hServiceStatus, &sg_serviceStatus);

	while (true)
		Sleep(sm_idleTick);
}

void RegistService(const TCHAR* serviceName, fnServiceStopCallback cb /*= NULL*/, void* userData/* = NULL*/, DWORD idleTick/* = 10*/)
{
	if (sg_isRegist)
		return;
	sg_isRegist = true;
	sm_callback = cb;
	sm_userData = userData;
	sm_idleTick = idleTick;
	memset(sg_serviceName, 0, sizeof(sg_serviceName));
	_tcscpy(sg_serviceName, serviceName);
	sg_thdId = GetCurrentThreadId();

	sg_serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	sg_serviceStatus.dwCurrentState = SERVICE_STOPPED;
	sg_serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	sg_serviceStatus.dwWin32ExitCode = 0;
	sg_serviceStatus.dwServiceSpecificExitCode = 0;
	sg_serviceStatus.dwCheckPoint = 0;
	sg_serviceStatus.dwWaitHint = 0;

	SERVICE_TABLE_ENTRY serviceTables[] = {
		{ (TCHAR*)serviceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain__ },
		{ NULL, NULL }
	};
	StartServiceCtrlDispatcher(serviceTables);
}

BOOL IsInstallService(const TCHAR* serviceName, DWORD* status/* = NULL*/)
{
	SC_HANDLE scHandle = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
	DWORD err = GetLastError(); err;
	if (!scHandle)
		return false;

	//打开服务
	SC_HANDLE hService = OpenService(scHandle, serviceName, SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS);
	if (!hService)
	{
		CloseServiceHandle(scHandle);
		return false;
	}
	if (status)
	{
		*status = 0;
		SERVICE_STATUS serviceStatus;
		if (QueryServiceStatus(hService, &serviceStatus))
			*status = serviceStatus.dwCurrentState;
	}
	//释放句柄
	CloseServiceHandle(hService);
	CloseServiceHandle(scHandle);
	return true;
}

#endif	//WIN32