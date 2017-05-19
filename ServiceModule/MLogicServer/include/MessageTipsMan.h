#ifndef __MESSAGE_TIPS_MAN_H__
#define __MESSAGE_TIPS_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
消息提示管理
*/

#include "cyaTypes.h"
#include "ServiceMLogic.h"

class MessageTipsMan
{
public:
	// 角色是否在线
	bool QueryOnlineByRoleId(UINT32 roleId, UINT32 *userId);

	// 发送消息提示
	int SendMessageTips(UINT32 roleId, BYTE type, const char *title = NULL, const char *content = NULL);
	int SendMessageTips(UINT32 userId, UINT32 roleId, BYTE type, const char *title = NULL, const char *content = NULL);

	// 取出所有离线消息
	int FetchAllOfflineMessage(UINT32 roleId, SMessageTipsNotify *pMessageTips, int *lenght);

private:
	// 发送在线消息
	int SendOnlineMessage(UINT32 userId, UINT32 roleId, BYTE type, const char *title = NULL, const char *content = NULL);

	// 保存离线消息
	int SaveOfflineMessage(UINT32 userId, UINT32 roleId, BYTE type, const char *title = NULL, const char *content = NULL);
};

void InitMessageTipsMan();
MessageTipsMan* GetMessageTipsMan();
void DestroyMessageTipsMan();

#endif