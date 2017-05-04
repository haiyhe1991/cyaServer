#ifndef __WIN_SERVICE_H__
#define __WIN_SERVICE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if defined(WIN32)
	#include "cyaTypes.h"
	typedef void(*fnServiceStopCallback)(DWORD dwOpt, void* userData);
	void RegistService(const TCHAR* serviceName, fnServiceStopCallback cb = NULL, void* userData = NULL, DWORD idleTick = 10);
	BOOL IsInstallService(const TCHAR* serviceName, DWORD* status = NULL);
#endif

#endif