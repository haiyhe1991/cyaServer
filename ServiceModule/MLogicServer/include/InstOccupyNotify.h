#ifndef __INST_OCCUPY_NOTIFY_H__
#define __INST_OCCUPY_NOTIFY_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
副本占领邮件通知
*/

#include <list>
#include "cyaCoretimer.h"
#include "RoleInfos.h"

class InstOccupyNotify
{
	InstOccupyNotify(const InstOccupyNotify&);
	InstOccupyNotify& operator = (const InstOccupyNotify&);

public:
	InstOccupyNotify();
	~InstOccupyNotify();

public:
	void Start();
	void Stop();

	void SendInstOccupyEmail(const std::list<SInstOccupyEmail>& emails);
	void SendInstOccupyEmail(const SInstOccupyEmail& occupyEmail);
	void SendInstOccupyNotify(BYTE curUserLinkerId, UINT32 curUserId, char* buf, int nLen, std::list<SInstOccupyEmail>& emails);

private:
	static void SendInstOccupyEmailTimer(void* param, TTimerID id);
	void OnSendInstOccupyEmail();

private:
	CXTLocker m_locker;
	CyaCoreTimer m_sendEmailTimer;
	std::list<SInstOccupyEmail> m_instOccupyEmailList;
};

void InitInstOccupyNotify();
InstOccupyNotify* GetInstOccupyNotify();
void DestroyInstOccupyNotify();

#endif	//_InstOccupyEmailNotify_h__