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
		BYTE linkerId;	//����link������id
		char nick[33];	//��ɫ�ǳ�
		UINT32 userId;	//��ɫ�˺�id

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

	//����Ƶ��
	int JoinChannel(const SLinkerJoinChannelReq* req);
	//�뿪Ƶ��
	int LeaveChannel(const SLinkerLeaveChannelReq* req);
	//�û��˳�
	int UserExit(UINT32 userId, UINT32 roleId);

	//��ȡ����Ƶ���û�
	int GetWorldChannelMember(UINT32 roleId, MemberAttr& member);
	//��ȡ����Ƶ���е����г�Ա
	int GetWorldChannelAllMembers(UINT32 senderRoleId, std::map<BYTE/*linkerId*/, std::vector<SChatRecvParam> >& membersMap);

	//��ȡ����Ƶ���û�
	int GetUnionChannelMember(UINT32 unionId, UINT32 roleId, MemberAttr& member);
	//��ȡ����Ƶ�������û�
	int GetUnionChannelAllMembers(UINT32 senderRoleId, UINT32 unionId, std::map<BYTE/*linkerId*/, std::vector<SChatRecvParam> >& membersMap);

private:
	//��������Ƶ��
	int JoinWorldChannel(BYTE linkerId, UINT32 userId, UINT32 roleId, const char* pszNick);
	//���빫��Ƶ��
	int JoinUnionChannel(BYTE linkerId, UINT32 userId, UINT32 roleId, UINT32 unionId, const char* pszNick);

	//�뿪����Ƶ��
	int LeaveWorldChannel(UINT32 roleId);
	//�뿪����Ƶ��
	int LeaveUnionChannel(UINT32 roleId, UINT32 unionId);

private:
	class ChannelMemberList : public BaseRefCount1	//Ƶ����Ա�б�
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
		std::map<UINT32 /*��ɫid*/, MemberAttr> m_memberList;
	};

private:
	CXTLocker m_worldMembersLocker;
	CXTLocker m_unionMembersLocker;
	std::map<UINT32 /*��ɫid*/, MemberAttr> m_worldMembers;			//����Ƶ����Ա����
	std::map<UINT32/*����id*/, ChannelMemberList*> m_unionMembers;	//����Ƶ����Ա�б�
};

void InitChannelMan();
ChannelMan* GetChannelMan();
void DestroyChannelMan();

#endif