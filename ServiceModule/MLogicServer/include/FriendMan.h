#ifndef __FRIEND_MAN_H__
#define __FRIEND_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

/*
���ѹ���
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
		add_friend_apply = 1,	//��Ӻ�������
		del_friend_apply,		//ɾ����������
		confirm_friend_apply,	//��֤��������
		friend_on_off_line		//����������
	};

	struct SFriendMsg
	{
		EFriendMsgType msg;
		union
		{
			struct
			{
				UINT32 applyUserId;		//�������˺�id
				UINT32 applyRoleId;		//�����߽�ɫid
				char friendNick[33];	//��Ӻ����ǳ�
			} addFriend;

			struct
			{
				UINT32 confirmUserId;	//ȷ�����˺�id
				UINT32 confirmRoleId;	//ȷ���߽�ɫid
				UINT32 friendUserId;	//ȷ�Ϻ����˺�id
				UINT32 friendRoleId;	//ȷ�Ϻ��ѽ�ɫid
				BYTE status;			//״̬,0-�ܾ�, 1-ͬ��
			} confirmFriend;

			struct
			{
				UINT32 applyUserId;		//�������˺�id
				UINT32 applyRoleId;		//�����߽�ɫid
				char delNick[33];		//ɾ�������ǳ�
			} delFriend;

			struct
			{
				UINT32 userId;			//�����˺�id
				UINT32 roleId;			//���߽�ɫid
				BOOL onoffLine;			//true-����, false-����
			} onofflineFriend;
		};
	};

public:
	FriendMan();
	~FriendMan();

	void Start();
	void Stop();

public:
	//��ѯ����
	int QueryFriends(UINT32 roleId, BYTE from, BYTE num, SQueryFriendsRes* pFriendsRes);
	//��Ӻ���
	int AddFriend(UINT32 userId, UINT32 roleId, const char* friendNick);
	//ȷ�Ϻ�������
	int ConfirmFriendApply(UINT32 userId, UINT32 roleId, UINT32 friendUserId, UINT32 friendRoleId, BYTE status);
	//ɾ������
	int DelFriend(UINT32 userId, UINT32 roleId, const char* friendNick);
	//��ѯ��������
	int QueryFriendApply(UINT32 roleId, BYTE from, BYTE num, SQueryFriendApplyRes* pFriendsRes);
	//��ȡ�Ƽ������б�
	int GetRecommendFriends(BYTE level, UINT32 nTimes, UINT32 exclude, SGetRecommendFriendsRes* pFriendsRes);
	//�������
	int SearchPlayer(const char* pszPlayerNick, SPlayerSearchRes* pPlayerInfo);
	//֪ͨ����������
	int NotifyFriendOnOffLine(UINT32 userId, UINT32 roleId, BOOL onoffLine);

private:
	//��ȡ��������
	BYTE OnGetFriendTotalNum(UINT32 roleId);
	//��ȡ������������
	BYTE OnGetFriendApplyNum(UINT32 roleId);

	//���ͺ�������֪ͨ
	void SendFriendApplyNotify(const SFriendInfo& applyRole, UINT32 notifyUserId, UINT32 notifyRoleId);
	//���ͺ�������ȷ��֪ͨ
	void SendConfirmFriendApplyNotify(const SFriendInfo& friendRole, UINT32 notifyUserId, UINT32 notifyRoleId, BYTE status);
	//���ͺ���������֪ͨ
	void SendFriendOnOffLineNotify(UINT32 notifyUserId, UINT32 notifyRoleId, UINT32 roleId, BOOL onoffLine);
	//���ͺ���ɾ��֪ͨ
	void SendFriendDelNotify(UINT32 notifyUserId, UINT32 notifyRoleId, UINT32 roleId);

	//��ѯ������Ϣ
	int OnQueryFriendInfo(UINT32 friendRoleId, SFriendInfo& friendInfo);
	//��ѯ������Ϣ
	int OnQueryFriendInfo(const char* pszFriendNick, SFriendInfo& friendInfo);
	//Ͷ����Ϣ
	void PostFriendMsg(const SFriendMsg& msg);
	//��ȡ���ߺ�����Ϣ
	int GetOnlineFriend(UINT32 userId, UINT32 roleId, SFriendInfo& friendInfo);
	//�Ƿ��໥����
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