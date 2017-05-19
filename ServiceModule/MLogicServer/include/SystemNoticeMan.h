#ifndef __SYSTEM_NOTICE_MAN_H__
#define __SYSTEM_NOTICE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
系统公告
*/

#include "cyaTypes.h"
#include "cyaCoreTimer.h"

class SystemNoticeMan
{
	SystemNoticeMan(const SystemNoticeMan&);
	SystemNoticeMan& operator = (const SystemNoticeMan&);

public:
	SystemNoticeMan();
	~SystemNoticeMan();

	void Start();
	void Stop();

public:
	//碎片合成公告
	void FragmentComposeNotice(const char* playerNick, BYTE jobId, UINT16 composeId);
	//传送门打开公告
	void TransferOpenNotice();
	//传送门关闭公告
	void TransferCloseNotice();

	/*zpy 2015.10.10新增 */
	void BroadcastSysyemNoticeUTF8(const char *content);
	void BroadcastSysyemNoticeGB2312(const char *content);
	//zpy 世界Boss死亡公告
	void WorldBossDeathNotice();

private:
	void OnBroadcastNotice(void* buf, int bufLen);
	static void NoticeTimerCallback(void* param, TTimerID id);
	void OnNotice();

private:
	CyaCoreTimer m_timer;
	UINT32 m_lastInterval;
};

void InitSystemNoticeMan();
void OpenSystemNotice();
void CloseSystemNotice();
SystemNoticeMan* GetSystemNoticeMan();
void DestroySystemNoticeMan();

#endif	//_SystemNoticeMan_h__