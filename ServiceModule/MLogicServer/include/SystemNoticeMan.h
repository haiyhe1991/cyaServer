#ifndef __SYSTEM_NOTICE_MAN_H__
#define __SYSTEM_NOTICE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
ϵͳ����
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
	//��Ƭ�ϳɹ���
	void FragmentComposeNotice(const char* playerNick, BYTE jobId, UINT16 composeId);
	//�����Ŵ򿪹���
	void TransferOpenNotice();
	//�����Źرչ���
	void TransferCloseNotice();

	/*zpy 2015.10.10���� */
	void BroadcastSysyemNoticeUTF8(const char *content);
	void BroadcastSysyemNoticeGB2312(const char *content);
	//zpy ����Boss��������
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