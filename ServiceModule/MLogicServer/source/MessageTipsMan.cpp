#include "MessageTipsMan.h"
#include "gb_utf8.h"
#include "DBSClient.h"
#include "ServiceProtocol.h"
#include "RolesInfoMan.h"
#include "GWSClientMan.h"
#include "ConfigFileMan.h"

static MessageTipsMan* sg_pMessageTipsMan = NULL;

void InitMessageTipsMan()
{
	ASSERT(sg_pMessageTipsMan == NULL);
	sg_pMessageTipsMan = new MessageTipsMan();
	ASSERT(sg_pMessageTipsMan != NULL);
}

MessageTipsMan* GetMessageTipsMan()
{
	return sg_pMessageTipsMan;
}

void DestroyMessageTipsMan()
{
	MessageTipsMan* pMessageTipsMan = sg_pMessageTipsMan;
	sg_pMessageTipsMan = NULL;
	if (pMessageTipsMan)
		delete pMessageTipsMan;
}

// 角色是否在线
bool MessageTipsMan::QueryOnlineByRoleId(UINT32 roleId, UINT32 *userId)
{
	// 查询角色账号id
	*userId = 0;
	DBS_ROW row_data = NULL;
	DBS_RESULT result = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT accountid FROM actor WHERE id=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return false;
	if ((row_data = DBS_FetchRow(result)) != NULL)
	{
		*userId = atol(row_data[0]);
	}
	DBS_FreeResult(result);
	return *userId != 0 ? GetRolesInfoMan()->IsUserRoleOnline(*userId, roleId) == TRUE : false;
}

// 发送消息提示
int MessageTipsMan::SendMessageTips(UINT32 roleId, BYTE type, const char *title, const char *content)
{
	/*UINT32 userId = 0;
	if (QueryOnlineByRoleId(roleId, &userId))
	{
	return SendOnlineMessage(userId, roleId, type, title, content);
	}
	else
	{
	return SaveOfflineMessage(userId, roleId, type, title, content);
	}*/
	return MLS_OK;
}

int MessageTipsMan::SendMessageTips(UINT32 userId, UINT32 roleId, BYTE type, const char *title, const char *content)
{
	/*if (GetRolesInfoMan()->IsUserRoleOnline(userId, roleId))
	{
	return SendOnlineMessage(userId, roleId, type, title, content);
	}
	else
	{
	return SaveOfflineMessage(userId, roleId, type, title, content);
	}*/
	return MLS_OK;
}

// 发送在线消息
int MessageTipsMan::SendOnlineMessage(UINT32 userId, UINT32 roleId, BYTE type, const char *title, const char *content)
{
	// 编码转换
	char szUTF8Title[MAX_UTF8_EMAIL_CAPTION_LEN] = { 0 };
	char szUTF8Content[MAX_UTF8_EMAIL_BODY_LEN] = { 0 };
	if (title != NULL)
	{
		gb2312_utf8(title, strlen(title), szUTF8Title, sizeof(szUTF8Title)-sizeof(char));
	}
	if (content != NULL)
	{
		gb2312_utf8(content, strlen(content), szUTF8Content, sizeof(szUTF8Content)-sizeof(char));
	}

	BYTE linkerId = 0;
	int retCode = GetRolesInfoMan()->GetUserLinkerId(userId, linkerId);
	if (retCode != MLS_OK)
		return MLS_OK;

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SMessageTipsNotify *pMessageTips = (SMessageTipsNotify*)(buf + nLen);
	GWSClient* gwsSession = GetGWSClientMan()->RandGWSClient();
	if (gwsSession == NULL)
		return MLS_OK;

	SMessageTipsItem *pMessageItem = (SMessageTipsItem *)(pMessageTips->data);
	pMessageItem->type = type;
	pMessageItem->title = title ? strlen(szUTF8Title) + 1 : 0;
	pMessageItem->content = content ? strlen(szUTF8Content) + 1 : 0;
	if (pMessageItem->title > 0)
	{
		memcpy(pMessageItem->data, szUTF8Title, pMessageItem->title);
	}

	if (pMessageItem->content > 0)
	{
		memcpy(pMessageItem->data + pMessageItem->title, szUTF8Content, pMessageItem->content);
	}

	nLen += sizeof(SMessageTipsItem)-sizeof(char)+pMessageItem->title + pMessageItem->content;
	pMessageTips->num = 1;
	pMessageItem->hton();
	gwsSession->SendResponse(linkerId, userId, MLS_OK, MLS_MESSAGETIPS_NOTIFY, buf, nLen);

	return MLS_OK;
}

// 保存离线消息
int MessageTipsMan::SaveOfflineMessage(UINT32 userId, UINT32 roleId, BYTE type, const char *title, const char *content)
{
	// 编码转换
	char szUTF8Title[MAX_UTF8_EMAIL_CAPTION_LEN] = { 0 };
	char szUTF8Content[MAX_UTF8_EMAIL_BODY_LEN] = { 0 };
	if (title != NULL)
	{
		gb2312_utf8(title, strlen(title), szUTF8Title, sizeof(szUTF8Title)-sizeof(char));
	}
	if (content != NULL)
	{
		gb2312_utf8(content, strlen(content), szUTF8Content, sizeof(szUTF8Content)-sizeof(char));
	}

	// 同类型消息只保留一条
	int count = 0;
	DBS_ROW row_data = NULL;
	DBS_RESULT result = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT count(*) FROM messagetips WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;
	if ((row_data = DBS_FetchRow(result)) != NULL)
	{
		count = atoi(row_data[0]);
	}
	DBS_FreeResult(result);

	// 覆盖或插入
	if (count > 0)
	{
		sprintf(szSQL, "UPDATE messagetips SET title='%s',content='%s' WHERE actorid=%u AND type=%d",
			title ? szUTF8Title : "", content ? szUTF8Content : "", roleId, (int)type);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}
	else
	{
		sprintf(szSQL, "INSERT INTO messagetips(actorid,type,title,content) VALUES(%u,%d,'%s','%s')",
			roleId, (int)type, title ? szUTF8Title : "", content ? szUTF8Content : "");
		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}

	return MLS_OK;
}

int MessageTipsMan::FetchAllOfflineMessage(UINT32 roleId, SMessageTipsNotify *pMessageTips, int *lenght)
{
	// 取出消息
	int current_pos = 0;
	DBS_ROW row_data = NULL;
	DBS_RESULT result = NULL;
	DBS_ROWLENGTH row_bytes = 0;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT type,title,content FROM messagetips WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;
	while ((row_data = DBS_FetchRow(result, &row_bytes)) != NULL)
	{
		const char *title = NULL;
		const char *content = NULL;
		BYTE type = (BYTE)atoi(row_data[0]);
		if (row_bytes[1] > 0)
		{
			title = row_data[1];
		}

		if (row_bytes[2] > 0)
		{
			content = row_data[2];
		}

		SMessageTipsItem *pMessageItem = (SMessageTipsItem *)(pMessageTips->data + current_pos);
		pMessageItem->type = type;
		pMessageItem->title = title ? strlen(title) + 1 : 0;
		pMessageItem->content = content ? strlen(content) + 1 : 0;

		if (pMessageItem->title > 0)
		{
			memcpy(pMessageItem->data, title, pMessageItem->title);
		}

		if (pMessageItem->content > 0)
		{
			memcpy(pMessageItem->data + pMessageItem->title, content, pMessageItem->content);
		}

		current_pos += sizeof(SMessageTipsItem)-sizeof(char)+pMessageItem->title + pMessageItem->content;
		pMessageItem->hton();
		++pMessageTips->num;
	}
	DBS_FreeResult(result);

	// 清空消息
	if (current_pos > 0)
	{
		sprintf(szSQL, "DELETE FROM messagetips WHERE actorid=%u", roleId);
		GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}
	*lenght += sizeof(SMessageTipsNotify)-sizeof(char)+current_pos;

	return MLS_OK;
}