#ifndef __MESSAGE_TIPS_MAN_H__
#define __MESSAGE_TIPS_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��Ϣ��ʾ����
*/

#include "cyaTypes.h"
#include "ServiceMLogic.h"

class MessageTipsMan
{
public:
	// ��ɫ�Ƿ�����
	bool QueryOnlineByRoleId(UINT32 roleId, UINT32 *userId);

	// ������Ϣ��ʾ
	int SendMessageTips(UINT32 roleId, BYTE type, const char *title = NULL, const char *content = NULL);
	int SendMessageTips(UINT32 userId, UINT32 roleId, BYTE type, const char *title = NULL, const char *content = NULL);

	// ȡ������������Ϣ
	int FetchAllOfflineMessage(UINT32 roleId, SMessageTipsNotify *pMessageTips, int *lenght);

private:
	// ����������Ϣ
	int SendOnlineMessage(UINT32 userId, UINT32 roleId, BYTE type, const char *title = NULL, const char *content = NULL);

	// ����������Ϣ
	int SaveOfflineMessage(UINT32 userId, UINT32 roleId, BYTE type, const char *title = NULL, const char *content = NULL);
};

void InitMessageTipsMan();
MessageTipsMan* GetMessageTipsMan();
void DestroyMessageTipsMan();

#endif