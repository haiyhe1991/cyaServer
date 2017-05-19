#ifndef __FRIEND_MAN_H__
#define __FRIEND_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

/*
好友管理
*/

#include <list>
#include "ServiceProtocol.h"
#include "RoleInfos.h"
#include "cyaThread.h"
#include "cyaLocker.h"

class FriendMan
{
	FriendMan(const FriendMan&);
	FriendMan& operator = (const FriendMan&);

	enum EFriendMsgType
	{
		add_friend_apply = 1,	//添加好友申请
		del_friend_apply,		//删除好友申请
		confirm_friend_apply,	//验证好友申请
		friend_on_off_line		//好友上下线
	};

	struct SFriendMsg
	{
		EFriendMsgType msg;
		union
		{
			struct
			{
				UINT32 applyUserId;		//申请者账号id
				UINT32 applyRoleId;		//申请者角色id
				char friendNick[33];	//添加好友昵称
			} addFriend;

			struct
			{
				UINT32 confirmUserId;	//确认者账号id
				UINT32 confirmRoleId;	//确认者角色id
				UINT32 friendUserId;	//确认好友账号id
				UINT32 friendRoleId;	//确认好友角色id
				BYTE status;			//状态,0-拒绝, 1-同意
			} confirmFriend;

			struct
			{
				UINT32 applyUserId;		//申请者账号id
				UINT32 applyRoleId;		//申请者角色id
				char delNick[33];		//删除好友昵称
			} delFriend;

			struct
			{
				UINT32 userId;			//上线账号id
				UINT32 roleId;			//上线角色id
				BOOL onoffLine;			//true-上线, false-下线
			} onofflineFriend;
		};
	};

public:
	FriendMan();
	~FriendMan();

	void Start();
	void Stop();

public:
	//查询好友
	int QueryFriends(UINT32 roleId, BYTE from, BYTE num, SQueryFriendsRes* pFriendsRes);
	//添加好友
	int AddFriend(UINT32 userId, UINT32 roleId, const char* friendNick);
	//确认好友申请
	int ConfirmFriendApply(UINT32 userId, UINT32 roleId, UINT32 friendUserId, UINT32 friendRoleId, BYTE status);
	//删除好友
	int DelFriend(UINT32 userId, UINT32 roleId, const char* friendNick);
	//查询好友申请
	int QueryFriendApply(UINT32 roleId, BYTE from, BYTE num, SQueryFriendApplyRes* pFriendsRes);
	//获取推荐好友列表
	int GetRecommendFriends(BYTE level, UINT32 nTimes, UINT32 exclude, SGetRecommendFriendsRes* pFriendsRes);
	//搜索玩家
	int SearchPlayer(const char* pszPlayerNick, SPlayerSearchRes* pPlayerInfo);
	//通知好友上下线
	int NotifyFriendOnOffLine(UINT32 userId, UINT32 roleId, BOOL onoffLine);

private:
	//获取好友总数
	BYTE OnGetFriendTotalNum(UINT32 roleId);
	//获取好友申请数量
	BYTE OnGetFriendApplyNum(UINT32 roleId);

	//发送好友申请通知
	void SendFriendApplyNotify(const SFriendInfo& applyRole, UINT32 notifyUserId, UINT32 notifyRoleId);
	//发送好友申请确认通知
	void SendConfirmFriendApplyNotify(const SFriendInfo& friendRole, UINT32 notifyUserId, UINT32 notifyRoleId, BYTE status);
	//发送好友上下线通知
	void SendFriendOnOffLineNotify(UINT32 notifyUserId, UINT32 notifyRoleId, UINT32 roleId, BOOL onoffLine);
	//发送好友删除通知
	void SendFriendDelNotify(UINT32 notifyUserId, UINT32 notifyRoleId, UINT32 roleId);

	//查询好友信息
	int OnQueryFriendInfo(UINT32 friendRoleId, SFriendInfo& friendInfo);
	//查询好友信息
	int OnQueryFriendInfo(const char* pszFriendNick, SFriendInfo& friendInfo);
	//投递消息
	void PostFriendMsg(const SFriendMsg& msg);
	//获取在线好友信息
	int GetOnlineFriend(UINT32 userId, UINT32 roleId, SFriendInfo& friendInfo);
	//是否相互好友
	BOOL IsEachOtherFriend(UINT32 role1, UINT32 role2);

private:
	static int THWorker(void* param);
	int OnWorker();

private:
	void OnProcessMsg(const SFriendMsg& msg);
	void OnAddFriend(UINT32 applyUserId, UINT32 applyRoleId, const char* pszFriendNick);
	void OnConfirmFriendApply(UINT32 confirmUserId, UINT32 confirmRoleId, UINT32 friendUserId, UINT32 friendRoleId, BYTE status);
	void OnDelFriend(UINT32 userId, UINT32 roleId, const char* pszDelNick);
	void OnFriendOnOffline(UINT32 onofflineUserId, UINT32 onofflineRoleId, BOOL onoffLine);

private:
	BOOL m_exit;
	OSThread m_thHandle;
	CXTLocker m_msgListLocker;
	std::list<SFriendMsg> m_msgList;
};

void InitFriendMan();
FriendMan* GetFriendMan();
void DestroyFriendMan();

#endif