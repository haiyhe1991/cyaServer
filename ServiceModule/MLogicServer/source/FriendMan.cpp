#include "FriendMan.h"
#include "DBSClient.h"
#include "RolesInfoMan.h"
#include "ConfigFileMan.h"
#include "GWSClientMan.h"
#include "GameCfgFileMan.h"

static FriendMan* sg_friendMan = NULL;
void InitFriendMan()
{
	ASSERT(sg_friendMan == NULL);
	sg_friendMan = new FriendMan();
	ASSERT(sg_friendMan != NULL);
	sg_friendMan->Start();
}

FriendMan* GetFriendMan()
{
	return sg_friendMan;
}

void DestroyFriendMan()
{
	FriendMan* pFriendMan = sg_friendMan;
	sg_friendMan = NULL;
	if (pFriendMan)
	{
		pFriendMan->Stop();
		delete pFriendMan;
	}
}

FriendMan::FriendMan()
: m_exit(true)
, m_thHandle(INVALID_OSTHREAD)
{

}

FriendMan::~FriendMan()
{

}

void FriendMan::Start()
{
	m_exit = false;
	OSCreateThread(&m_thHandle, NULL, THWorker, this, 0);
}

void FriendMan::Stop()
{
	m_exit = true;
	if (m_thHandle != INVALID_OSTHREAD)
	{
		OSCloseThread(m_thHandle);
		m_thHandle = INVALID_OSTHREAD;
	}
}

int FriendMan::SearchPlayer(const char* pszPlayerNick, SPlayerSearchRes* pPlayerInfo)
{
	return OnQueryFriendInfo(pszPlayerNick, pPlayerInfo->player);
}

int FriendMan::NotifyFriendOnOffLine(UINT32 userId, UINT32 roleId, BOOL onoffLine)
{
	SFriendMsg msg;
	msg.msg = friend_on_off_line;
	msg.onofflineFriend.userId = userId;
	msg.onofflineFriend.roleId = roleId;
	msg.onofflineFriend.onoffLine = onoffLine;
	PostFriendMsg(msg);
	return MLS_OK;
}

int FriendMan::AddFriend(UINT32 userId, UINT32 roleId, const char* friendNick)
{
	BYTE totalFriends = OnGetFriendTotalNum(roleId);
	if (totalFriends >= GetGameCfgFileMan()->GetMaxFriendNum())
		return MLS_OVER_MAX_FRIENDS;

	SFriendMsg msg;
	msg.msg = add_friend_apply;
	msg.addFriend.applyUserId = userId;
	msg.addFriend.applyRoleId = roleId;
	strcpy(msg.addFriend.friendNick, friendNick);
	PostFriendMsg(msg);
	return MLS_OK;
}

int FriendMan::ConfirmFriendApply(UINT32 userId, UINT32 roleId, UINT32 friendUserId, UINT32 friendRoleId, BYTE status)
{
	SFriendMsg msg;
	msg.msg = confirm_friend_apply;
	msg.confirmFriend.confirmUserId = userId;
	msg.confirmFriend.confirmRoleId = roleId;
	msg.confirmFriend.friendUserId = friendUserId;
	msg.confirmFriend.friendRoleId = friendRoleId;
	msg.confirmFriend.status = status;
	PostFriendMsg(msg);
	return MLS_OK;
}

int FriendMan::DelFriend(UINT32 userId, UINT32 roleId, const char* friendNick)
{
	SFriendMsg msg;
	msg.msg = del_friend_apply;
	msg.delFriend.applyUserId = userId;
	msg.delFriend.applyRoleId = roleId;
	strcpy(msg.delFriend.delNick, friendNick);
	PostFriendMsg(msg);
	return MLS_OK;
}

int FriendMan::QueryFriends(UINT32 roleId, BYTE from, BYTE num, SQueryFriendsRes* pFriendsRes)
{
	int retCode = MLS_OK;
	DBS_RESULT result = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };

	BYTE total = OnGetFriendTotalNum(roleId);
	if (total == 0)
		return retCode;

	if (num <= 0)
		return MLS_OK;

	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "select F.actorid, F.friendid, A.accountid, A.actortype, A.sex, A.nick, A.level, A.cp from friend as F "
		"join actor as A on (F.friendid=A.id and F.actorid=%u) or (F.actorid=A.id and F.friendid=%u) where F.status=1 limit %d, %d",
		roleId, roleId, from, num);

	retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 8)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	BYTE idx = 0;
	DBS_ROW row = NULL;
	BYTE maxNum = (MAX_RES_USER_BYTES - member_offset(SQueryFriendsRes, total, friends)) / sizeof(SFriendInfo);
	while ((row = DBS_FetchRow(result)) != NULL)
	{
		UINT32 actorId = _atoi64(row[0]);
		UINT32 friendId = _atoi64(row[1]);
		UINT32 userId = _atoi64(row[2]);
		pFriendsRes->friends[idx].userId = userId;
		pFriendsRes->friends[idx].roleId = actorId == roleId ? friendId : actorId;
		pFriendsRes->friends[idx].occuId = atoi(row[3]);
		pFriendsRes->friends[idx].sex = atoi(row[4]);
		strcpy(pFriendsRes->friends[idx].roleNick, row[5]);
		pFriendsRes->friends[idx].roleLevel = atoi(row[6]);
		pFriendsRes->friends[idx].cp = _atoi64(row[7]);
		pFriendsRes->friends[idx].online = GetRolesInfoMan()->IsUserRoleOnline(userId, pFriendsRes->friends[idx].roleId) ? 1 : 0;
		pFriendsRes->friends[idx].hton();

		if (++idx >= maxNum)
			break;
	}
	pFriendsRes->total = total;
	pFriendsRes->num = idx;
	DBS_FreeResult(result);
	return retCode;
}

int FriendMan::QueryFriendApply(UINT32 roleId, BYTE from, BYTE num, SQueryFriendApplyRes* pFriendsRes)
{
	int retCode = MLS_OK;
	DBS_RESULT result = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };

	pFriendsRes->total = OnGetFriendApplyNum(roleId);
	if (pFriendsRes->total == 0)
		return retCode;

	if (num <= 0)
		return retCode;

	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "select A.accountid, F.actorid, A.actortype, A.sex, A.nick, A.level, A.cp from friend as F "
		"join actor as A on (F.actorid=A.id) "
		"where F.friendid=%u and F.status=0 limit %d, %d",
		roleId, from, num);

	retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 7)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	BYTE idx = 0;
	DBS_ROW row = NULL;
	BYTE maxNum = (MAX_RES_USER_BYTES - member_offset(SQueryFriendApplyRes, total, data)) / sizeof(SFriendInfo);
	while ((row = DBS_FetchRow(result)) != NULL)
	{
		UINT32 userId = _atoi64(row[0]);
		UINT32 actorId = _atoi64(row[1]);
		pFriendsRes->data[idx].userId = userId;
		pFriendsRes->data[idx].roleId = actorId;
		pFriendsRes->data[idx].occuId = atoi(row[2]);
		pFriendsRes->data[idx].sex = atoi(row[3]);
		strcpy(pFriendsRes->data[idx].roleNick, row[4]);
		pFriendsRes->data[idx].roleLevel = atoi(row[5]);
		pFriendsRes->data[idx].cp = _atoi64(row[6]);
		pFriendsRes->data[idx].online = GetRolesInfoMan()->IsUserRoleOnline(userId, actorId) ? 1 : 0;
		pFriendsRes->data[idx].hton();
		if (++idx >= maxNum)
			break;
	}
	pFriendsRes->num = idx;
	DBS_FreeResult(result);
	return retCode;
}

int FriendMan::GetRecommendFriends(BYTE level, UINT32 nTimes, UINT32 exclude, SGetRecommendFriendsRes* pFriendsRes)
{
	BYTE recommendNum = GetGameCfgFileMan()->GetRecommendFriendNum();	//推荐数量
	BYTE recommendLv = GetGameCfgFileMan()->GetRecommendFriendLv();		//推荐等级范围

	BYTE minLv = SGSU8Sub(level, recommendLv);
	BYTE maxLv = SGSU8Add(level, recommendLv);
	UINT32 startNum = nTimes * recommendNum;

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select id, accountid, actortype, sex, nick, level, cp from actor where level>=%d and level<=%d limit %u, %d",
		minLv, maxLv, startNum, recommendNum);
	/*
	排除自己账号下的角色
	sprintf(szSQL, "select id, accountid, actortype, sex, nick, level, cp from actor where accountid!=%u and level>=%d and level<=%d limit %u, %d",
	exclude, minLv, maxLv, startNum, recommendNum);
	*/

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 7)
	{
		DBS_FreeResult(result);
		return retCode;
	}

	BYTE idx = 0;
	DBS_ROW row = NULL;
	BYTE maxNum = (MAX_RES_USER_BYTES - member_offset(SGetRecommendFriendsRes, num, data)) / sizeof(SFriendInfo);
	while ((row = DBS_FetchRow(result)) != NULL)
	{
		UINT32 roleId = _atoi64(row[0]);
		UINT32 userId = _atoi64(row[1]);
		pFriendsRes->data[idx].userId = userId;
		pFriendsRes->data[idx].roleId = roleId;
		pFriendsRes->data[idx].occuId = atoi(row[2]);
		pFriendsRes->data[idx].sex = atoi(row[3]);
		strcpy(pFriendsRes->data[idx].roleNick, row[4]);
		pFriendsRes->data[idx].roleLevel = atoi(row[5]);
		pFriendsRes->data[idx].cp = _atoi64(row[6]);
		pFriendsRes->data[idx].online = GetRolesInfoMan()->IsUserRoleOnline(userId, roleId) ? 1 : 0;
		pFriendsRes->data[idx].hton();
		if (++idx >= maxNum)
			break;
	}
	pFriendsRes->num = idx;
	DBS_FreeResult(result);
	return retCode;
}

BYTE FriendMan::OnGetFriendTotalNum(UINT32 roleId)
{
	BYTE total = 0;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select count(id) from friend where actorid=%u or friendid=%u", roleId, roleId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return total;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	DBS_ROW row = DBS_FetchRow(result);
	if (row == NULL || nRows <= 0 || nCols <= 0)
	{
		DBS_FreeResult(result);
		return total;
	}

	total = (BYTE)atoi(row[0]);	//总数
	DBS_FreeResult(result);
	return total;
}

BYTE FriendMan::OnGetFriendApplyNum(UINT32 roleId)
{
	BYTE total = 0;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select count(id) from friend where friendid=%u and status=0", roleId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return total;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	DBS_ROW row = DBS_FetchRow(result);
	if (row == NULL || nRows <= 0 || nCols <= 0)
	{
		DBS_FreeResult(result);
		return total;
	}

	total = (BYTE)atoi(row[0]);	//总数
	DBS_FreeResult(result);
	return total;
}

void FriendMan::SendFriendApplyNotify(const SFriendInfo& applyRole, UINT32 notifyUserId, UINT32 notifyRoleId)
{
	if (!GetRolesInfoMan()->IsUserRoleOnline(notifyUserId, notifyRoleId))
		return;

	BYTE linkerId = 0;
	int retCode = GetRolesInfoMan()->GetUserLinkerId(notifyUserId, linkerId);
	if (retCode != MLS_OK)
		return;

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SFriendApplyNotify* pApplyNotify = (SFriendApplyNotify*)(buf + nLen);
	memcpy(pApplyNotify, &applyRole, sizeof(SFriendApplyNotify));

	GWSClient* gwsSession = GetGWSClientMan()->RandGWSClient();
	if (gwsSession == NULL)
		return;

	nLen += sizeof(SFriendApplyNotify);
	pApplyNotify->hton();
	gwsSession->SendResponse(linkerId, notifyUserId, MLS_OK, MLS_FRIEND_APPLY_NOTIFY, buf, nLen);
}

void FriendMan::SendConfirmFriendApplyNotify(const SFriendInfo& friendRole, UINT32 notifyUserId, UINT32 notifyRoleId, BYTE status)
{
	if (!GetRolesInfoMan()->IsUserRoleOnline(notifyUserId, notifyRoleId))
		return;

	BYTE linkerId = 0;
	int retCode = GetRolesInfoMan()->GetUserLinkerId(notifyUserId, linkerId);
	if (retCode != MLS_OK)
		return;

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SFriendApplyNotify* pApplyNotify = (SFriendApplyNotify*)(buf + nLen);
	memcpy(pApplyNotify, &friendRole, sizeof(SFriendApplyNotify));

	GWSClient* gwsSession = GetGWSClientMan()->RandGWSClient();
	if (gwsSession == NULL)
		return;

	nLen += sizeof(SFriendApplyNotify);
	pApplyNotify->hton();
	gwsSession->SendResponse(linkerId, notifyUserId, MLS_OK, status ? MLS_AGREE_FRIEND_APPLY_NOTIFY : MLS_REFUSE_FRIEND_APPLY_NOTIFY, buf, nLen);
}

void FriendMan::SendFriendOnOffLineNotify(UINT32 notifyUserId, UINT32 notifyRoleId, UINT32 roleId, BOOL onoffLine)
{
	if (!GetRolesInfoMan()->IsUserRoleOnline(notifyUserId, notifyRoleId))
		return;

	BYTE linkerId = 0;
	int retCode = GetRolesInfoMan()->GetUserLinkerId(notifyUserId, linkerId);
	if (retCode != MLS_OK)
		return;

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SFriendOnOffLineNotify* pOnOffNotify = (SFriendOnOffLineNotify*)(buf + nLen);
	pOnOffNotify->friendRoleId = roleId;

	GWSClient* gwsSession = GetGWSClientMan()->RandGWSClient();
	if (gwsSession == NULL)
		return;

	nLen += sizeof(SFriendApplyNotify);
	pOnOffNotify->hton();
	gwsSession->SendResponse(linkerId, notifyUserId, MLS_OK, onoffLine ? MLS_FRIEND_ONLINE_NOTIFY : MLS_FRIEND_OFFLINE_NOTIFY, buf, nLen);
}

void FriendMan::SendFriendDelNotify(UINT32 notifyUserId, UINT32 notifyRoleId, UINT32 roleId)
{
	if (!GetRolesInfoMan()->IsUserRoleOnline(notifyUserId, notifyRoleId))
		return;

	BYTE linkerId = 0;
	int retCode = GetRolesInfoMan()->GetUserLinkerId(notifyUserId, linkerId);
	if (retCode != MLS_OK)
		return;

	char buf[MAX_BUF_LEN] = { 0 };
	int nLen = GWS_COMMON_REPLY_LEN;
	SFriendDelNotify* pDelNotify = (SFriendDelNotify*)(buf + nLen);
	pDelNotify->friendRoleId = roleId;

	GWSClient* gwsSession = GetGWSClientMan()->RandGWSClient();
	if (gwsSession == NULL)
		return;

	nLen += sizeof(SFriendDelNotify);
	pDelNotify->hton();
	gwsSession->SendResponse(linkerId, notifyUserId, MLS_OK, MLS_FRIEND_DEL_NOTIFY, buf, nLen);
}

int FriendMan::OnQueryFriendInfo(UINT32 friendRoleId, SFriendInfo& friendInfo)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select id, accountid, actortype, sex, nick, level, cp from actor where id=%u", friendRoleId);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 7)
	{
		DBS_FreeResult(result);
		return MLS_NOT_FIND_PLAYER;
	}

	DBS_ROW row = DBS_FetchRow(result);
	friendInfo.roleId = _atoi64(row[0]);
	friendInfo.userId = _atoi64(row[1]);
	friendInfo.occuId = atoi(row[2]);
	friendInfo.sex = atoi(row[3]);
	strcpy(friendInfo.roleNick, row[4]);
	friendInfo.roleLevel = atoi(row[5]);
	friendInfo.cp = _atoi64(row[6]);
	friendInfo.online = GetRolesInfoMan()->IsUserRoleOnline(friendInfo.userId, friendInfo.roleId) ? 1 : 0;
	DBS_FreeResult(result);
	return retCode;
}

int FriendMan::OnQueryFriendInfo(const char* pszFriendNick, SFriendInfo& friendInfo)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select id, accountid, actortype, sex, nick, level, cp from actor where nick='%s'", pszFriendNick);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 7)
	{
		DBS_FreeResult(result);
		return MLS_NOT_FIND_PLAYER;
	}

	DBS_ROW row = DBS_FetchRow(result);
	friendInfo.roleId = _atoi64(row[0]);
	friendInfo.userId = _atoi64(row[1]);
	friendInfo.occuId = atoi(row[2]);
	friendInfo.sex = atoi(row[3]);
	strcpy(friendInfo.roleNick, row[4]);
	friendInfo.roleLevel = atoi(row[5]);
	friendInfo.cp = _atoi64(row[6]);
	friendInfo.online = GetRolesInfoMan()->IsUserRoleOnline(friendInfo.userId, friendInfo.roleId) ? 1 : 0;
	DBS_FreeResult(result);
	return retCode;
}

int FriendMan::GetOnlineFriend(UINT32 userId, UINT32 roleId, SFriendInfo& friendInfo)
{
	SRoleInfos role;
	int retCode = GetRolesInfoMan()->GetRoleInfo(userId, roleId, &role);
	if (retCode != MLS_OK)
		return retCode;

	friendInfo.userId = userId;				//好友账户ID;
	friendInfo.roleId = roleId;				//好友角色ID;
	friendInfo.occuId = role.occuId;			//职业id;
	friendInfo.sex = role.sex;				//性别;
	strcpy(friendInfo.roleNick, role.nick);	//角色昵称[33];
	friendInfo.roleLevel = role.level;		//角色等级;
	friendInfo.guildId = role.guildId;		//所在公会ID;
	friendInfo.cp = role.cp;					//战力
	friendInfo.online = 1;						//是否在线
	return retCode;
}

BOOL FriendMan::IsEachOtherFriend(UINT32 role1, UINT32 role2)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select count(id) from friend where (actorid=%u and friendid=%u) or (actorid=%u and friendid=%u)", role1, role2, role2, role1);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return false;

	int n = 0;
	DBS_ROW row = DBS_FetchRow(result);
	if (row != NULL)
		n = atoi(row[0]);
	DBS_FreeResult(result);
	return n > 0 ? true : false;
}

int FriendMan::THWorker(void* param)
{
	FriendMan* pThis = (FriendMan*)param;
	ASSERT(pThis != NULL);
	return pThis->OnWorker();
}

int FriendMan::OnWorker()
{
	while (!m_exit)
	{
		if (m_msgList.empty())
		{
			Sleep(1);
			continue;
		}

		BOOL idle = true;
		std::list<SFriendMsg> msgList;

		{
			CXTAutoLock lock(m_msgListLocker);
			if (!m_msgList.empty())
			{
				msgList.assign(m_msgList.begin(), m_msgList.end());
				m_msgList.clear();
				idle = false;
			}
		}

		std::list<SFriendMsg>::iterator it = msgList.begin();
		for (; it != msgList.end(); ++it)
			OnProcessMsg(*it);

		if (idle)
			Sleep(1);
	}
	return 0;
}

void FriendMan::PostFriendMsg(const SFriendMsg& msg)
{
	CXTAutoLock lock(m_msgListLocker);
	m_msgList.push_back(msg);
}

void FriendMan::OnProcessMsg(const SFriendMsg& msg)
{
	switch (msg.msg)
	{
	case add_friend_apply:	//添加好友申请
		OnAddFriend(msg.addFriend.applyUserId, msg.addFriend.applyRoleId, msg.addFriend.friendNick);
		break;
	case del_friend_apply:		//删除好友申请
		OnDelFriend(msg.delFriend.applyUserId, msg.delFriend.applyRoleId, msg.delFriend.delNick);
		break;
	case confirm_friend_apply:	//验证好友申请
		OnConfirmFriendApply(msg.confirmFriend.confirmUserId, msg.confirmFriend.confirmRoleId, msg.confirmFriend.friendUserId, msg.confirmFriend.friendRoleId, msg.confirmFriend.status);
		break;
	case friend_on_off_line:		//好友上下线
		OnFriendOnOffline(msg.onofflineFriend.userId, msg.onofflineFriend.roleId, msg.onofflineFriend.onoffLine);
		break;
	default:
		break;
	}
}

void FriendMan::OnAddFriend(UINT32 applyUserId, UINT32 applyRoleId, const char* pszFriendNick)
{
	SFriendApplyNotify friendApplyNotify;
	memset(&friendApplyNotify, 0, sizeof(SFriendApplyNotify));
	int retCode = GetOnlineFriend(applyUserId, applyRoleId, friendApplyNotify.friendInfo);
	if (retCode != MLS_OK)
	{
		retCode = OnQueryFriendInfo(applyRoleId, friendApplyNotify.friendInfo);
		if (retCode != MLS_OK)
			return;
	}

	DBS_RESULT result = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select accountid, id from actor where nick='%s'", pszFriendNick);
	retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return;
	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 2)
	{
		DBS_FreeResult(result);
		return;
	}
	DBS_ROW row = DBS_FetchRow(result);
	UINT32 friendUserId = _atoi64(row[0]);
	UINT32 friendId = _atoi64(row[1]);
	DBS_FreeResult(result);
	result = NULL;

	//不能自己加自己
	if (applyRoleId == friendId)
		return;

	//是否已经是好友
	if (IsEachOtherFriend(applyRoleId, friendId))
		return;

	//插入好友列表
	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "insert into friend(actorid, friendid, status) values(%u, %u, %d)", applyRoleId, friendId, 0);
	retCode = GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	if (retCode != MLS_OK)
		return;

	//发送好友申请
	SendFriendApplyNotify(friendApplyNotify.friendInfo, friendUserId, friendId);
}

void FriendMan::OnConfirmFriendApply(UINT32 confirmUserId, UINT32 confirmRoleId, UINT32 friendUserId, UINT32 friendRoleId, BYTE status)
{
	SFriendApplyNotify friendApplyNotify;
	memset(&friendApplyNotify, 0, sizeof(SFriendApplyNotify));
	int retCode = GetOnlineFriend(confirmUserId, confirmRoleId, friendApplyNotify.friendInfo);
	if (retCode != MLS_OK)
	{
		retCode = OnQueryFriendInfo(confirmRoleId, friendApplyNotify.friendInfo);
		if (retCode != MLS_OK)
			return;
	}

	if (status)	//同意
	{
		char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
		sprintf(szSQL, "update friend set status=1 where actorid=%u and friendid=%u", friendRoleId, confirmRoleId);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}
	else	//拒绝
	{
		char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
		sprintf(szSQL, "delete from friend where actorid=%u and friendid=%u", friendRoleId, confirmRoleId);
		GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), szSQL);
	}

	//发送确认好友申请通知
	SendConfirmFriendApplyNotify(friendApplyNotify.friendInfo, friendUserId, friendRoleId, status);
}

void FriendMan::OnDelFriend(UINT32 userId, UINT32 roleId, const char* pszDelNick)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select id, accountid from actor where nick='%s'", pszDelNick);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 2)
	{
		DBS_FreeResult(result);
		return;
	}

	DBS_ROW row = DBS_FetchRow(result);
	UINT32 delRoleId = _atoi64(row[0]);
	UINT32 delUserId = _atoi64(row[1]);
	DBS_FreeResult(result);

	UINT32 delRows = 0;
	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "delete from friend where (actorid=%u and friendid=%u) or (actorid=%u and friendid=%u)", roleId, delRoleId, delRoleId, roleId);
	retCode = GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), szSQL, &delRows);

	//发送好友删除通知
	SendFriendDelNotify(delUserId, delRoleId, roleId);
}

void FriendMan::OnFriendOnOffline(UINT32 onofflineUserId, UINT32 onofflineRoleId, BOOL onoffLine)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "select A.accountid, F.actorid, F.friendid from friend as F "
		"join actor as A on (F.friendid=A.id or F.actorid=A.id) "
		"where (F.actorid=%u or F.friendid=%u) and status=1",
		onofflineRoleId, onofflineRoleId);


	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 3)
	{
		DBS_FreeResult(result);
		return;
	}

	DBS_ROW row = NULL;
	while ((row = DBS_FetchRow(result)) != NULL)
	{
		UINT32 notifyUserId = _atoi64(row[0]);
		UINT32 actorId = _atoi64(row[1]);
		UINT32 friendId = _atoi64(row[2]);
		UINT32 notifyRoleId = actorId == onofflineRoleId ? friendId : actorId;
		if (notifyRoleId == onofflineRoleId)
			continue;

		if (!GetRolesInfoMan()->IsUserRoleOnline(notifyUserId, notifyRoleId))
			continue;

		SendFriendOnOffLineNotify(notifyUserId, notifyRoleId, onofflineRoleId, onoffLine);
	}
}