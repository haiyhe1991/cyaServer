#ifndef __SYNC_SERVER_H__
#define __SYNC_SERVER_H__
	
#include "cyaThread.h"
#include "cyaSync.h"
#include "SyncHelper.h"

class SyncServer
{
public:
	explicit SyncServer();
	~SyncServer();

	/// 5分钟做一次同步？
	void OnInitialize(DWORD delayIdleTick = 5 * 60 * 1000);
	void OnUnInitialize();

	void SetSyncDelayIdleTick(DWORD delayIdleTick);
	DWORD GetDefaultSyncDelayIdleTick() const;

	int EnforceSync(const char* dbName);
private:
	static int SyncThreadRoutine(void*);
	int SyncThreadWoker();
private:
	OSThread	m_syncDelayThread;
	OSThreadID	m_syncDelayThreadID;
	bool		m_syncDelayExit;
	DWORD		m_delayIdleTick;
	OSSema		m_syncExitSema;
};

#endif