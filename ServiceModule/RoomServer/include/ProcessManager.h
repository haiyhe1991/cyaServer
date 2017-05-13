/************************************************************************/
/// 守护进程和虚幻服务器的通信处理
/// 对进程信息的管理
/************************************************************************/

#ifndef __PROCESS_MANAGER_H__
#define __PROCESS_MANAGER_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include "ServiceRoomDaemon.h"
#include "cyaProcess.h"

class CProcessManager
{
public:
	static CProcessManager* FetchProcessManagerInstance();
	static void ReleaseProcessManagerInstance();

	INT  ProcessRegist(OSProcessID id, void* session);
	void ProcessUnregist(OSProcessID id);

	void* GetProcessSession(OSProcessID id) const;

	INT SendGWData(const void* data, const INT& dataLen, UINT16 cmdCode) const;
private:
	CProcessManager();
	~CProcessManager();

	CProcessManager(const CProcessManager&);
	CProcessManager& operator=(const CProcessManager&);

	class ProcessInfo
	{
	public:
		ProcessInfo();
		~ProcessInfo();

	private:
		OSProcessID m_osProcID;
		BYTE m_usrNum;
		char m_roomID[DAEMON_ID_LEN];
	};
private:
	static CProcessManager* m_this;
	typedef std::map<OSProcessID, ProcessInfo*> DaemonProcessInfo;
	DaemonProcessInfo m_procInfo;

	typedef std::map<OSProcessID, void*> DaemonProcessReg;
	DaemonProcessReg m_procReg;
};

#endif