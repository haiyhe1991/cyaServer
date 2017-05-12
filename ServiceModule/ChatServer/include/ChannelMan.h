#ifndef __CHANNEL_MAN_H__
#define __CHANNEL_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include <vector>
#include "cyaTypes.h"
#include "ChatProtocol.h"
#include "cyaLocker.h"
#include "cyaRefCount.h"
#include "ServiceProtocol.h"

class ChannelMan
{
public:
#pragma pack(push, 1)
	struct MemberAttr
	{
		BYTE linkerId;	//所在link服务器id
		char nick[33];	//角色昵称
		UINT32 userId;	//角色账号id

		MemberAttr()
		{
			linkerId = 0;
			memset(nick, 0, sizeof(nick));
			userId = 0;
		}
	};
	struct SChatRecvParam
	{
		UINT32 recvUserId;
		UINT32 recvRoleId;
		SChatRecvParam()
		{
			recvUserId = 0;
			recvRoleId = 0;
		}
	};
#pragma pack(pop)

public:
	ChannelMan();
	~ChannelMan();

	//加入频道
	int JoinChannel(const SLinkerJoinChannelReq* req);
	//离开频道
	int LeaveChannel(const SLinkerLeaveChannelReq* req);
	//用户退出
	int UserExit(UINT32 userId, UINT32 roleId);

	//获取世界频道用户
	int GetWorldChannelMember(UINT32 roleId, MemberAttr& member);
	//获取世界频道中的所有成员
	int GetWorldChannelAllMembers(UINT32 senderRoleId, std::map<BYTE/*linkerId*/, std::vector<SChatRecvParam> >& membersMap);

	//获取公会频道用户
	int GetUnionChannelMember(UINT32 unionId, UINT32 roleId, MemberAttr& member);
	//获取公会频道所有用户
	int GetUnionChannelAllMembers(UINT32 senderRoleId, UINT32 unionId, std::map<BYTE/*linkerId*/, std::vector<SChatRecvParam> >& membersMap);

private:
	//加入世界频道
	int JoinWorldChannel(BYTE linkerId, UINT32 userId, UINT32 roleId, const char* pszNick);
	//加入公会频道
	int JoinUnionChannel(BYTE linkerId, UINT32 userId, UINT32 roleId, UINT32 unionId, const char* pszNick);

	//离开世界频道
	int LeaveWorldChannel(UINT32 roleId);
	//离开公会频道
	int LeaveUnionChannel(UINT32 roleId, UINT32 unionId);

private:
	class ChannelMemberList : public BaseRefCount1	//频道成员列表
	{
	public:
		ChannelMemberList();
		~ChannelMemberList();

		int AddMember(BYTE linkerId, UINT32 userId, UINT32 roleId, const char* pszNick);
		int DelMember(UINT32 roleId, BOOL& needEraseUnion);

		int GetMember(UINT32 roleId, MemberAttr& member);
		int GetAllMembers(UINT32 curRoleId, std::map<BYTE/*linkerId*/, std::vector<SChatRecvParam> >& membersMap);

	private:
		CXTLocker m_memberListLocker;
		std::map<UINT32 /*角色id*/, MemberAttr> m_memberList;
	};

private:
	CXTLocker m_worldMembersLocker;
	CXTLocker m_unionMembersLocker;
	std::map<UINT32 /*角色id*/, MemberAttr> m_worldMembers;			//世界频道成员集合
	std::map<UINT32/*公会id*/, ChannelMemberList*> m_unionMembers;	//公会频道成员列表
};

void InitChannelMan();
ChannelMan* GetChannelMan();
void DestroyChannelMan();

#endif