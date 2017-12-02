#ifndef __MAU_TABLE_MAN_H__
#define __MAU_TABLE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#include <map>
#include <vector>
#include <list>
#include "cyaTypes.h"
#include "MauProtocol.h"
#include "cyaLocker.h"
#include "cyaRefCount.h"
#include "ServiceProtocol.h"

class MauTableMan
{
public:
#pragma pack(push, 1)
	struct MemberAttr
	{
		BYTE linkerId;	//所在link服务器id
		char nick[33];	//角色昵称
		UINT32 userId;	//角色账号id
		BYTE tableId;	//牌桌id
		BYTE ready;		//准备（0取消准备，1准备）

		MemberAttr()
		{
			linkerId = 0;
			memset(nick, 0, sizeof(nick));
			userId = 0;
		}
	};
	struct SMauRecvParam
	{
		UINT32 recvUserId;
		UINT32 recvRoleId;
		char nick[33];	//角色昵称
		BYTE tableId;	//牌桌id
		BYTE ready;		//准备（0取消准备，1准备）
		SMauRecvParam()
		{
			recvUserId = 0;
			recvRoleId = 0;
		}
	};
#pragma pack(pop)

public:
	MauTableMan();
	~MauTableMan();

	//创建牌桌
	int CreateMauTabel(const SLinkerCreateMauTabelReq* req, UINT32& unionId, BYTE& tableId);
	//加入牌桌
	int JoinMauTabel(const SLinkerJoinMauTabelReq* req, BYTE& tableId);
	//玩家准备
	int ReadyTabel(const SLinkerReadyTabelReq* req, BYTE& tableId);
	//离开牌桌
	int LeaveMauTabel(const SLinkerLeaveMauTabelReq* req, BYTE& tableId);
	//用户退出
	int UserExit(UINT32 userId, UINT32 roleId, BYTE& tableId);

	//获取公会频道用户
	int GetUnionMauTabelMember(UINT32 unionId, UINT32 roleId, MemberAttr& member);
	//获取公会频道所有用户
	int GetUnionMauTabelAllMembers(UINT32 senderRoleId, UINT32 unionId, std::map<BYTE/*linkerId*/, std::vector<SMauRecvParam> >& membersMap);

private:
	//加入公会频道
	int JoinUnionMauTabel(BYTE linkerId, UINT32 userId, UINT32 roleId, UINT32 unionId, BYTE& tableId, const char* pszNick);

	//离开公会频道
	int LeaveUnionMauTabel(UINT32 roleId, UINT32 unionId, BYTE& tableId);

	//玩家准备
	int ReadyUnionMauTabel(UINT32 roleId, UINT32 unionId, BYTE ready, BYTE& tableId);

private:
	class MauTabelMemberList : public BaseRefCount1	//频道成员列表
	{
	public:
		MauTabelMemberList();
		~MauTabelMemberList();

		int AddMember(BYTE linkerId, BYTE& tableId, UINT32 userId, UINT32 roleId, const char* pszNick);
		int DelMember(UINT32 roleId, BYTE& tableId, BOOL& needEraseUnion);
		int ReadyMember(UINT32 roleId, BYTE ready, BYTE& tableId);

		int GetMember(UINT32 roleId, MemberAttr& member);
		int GetAllMembers(UINT32 curRoleId, std::map<BYTE/*linkerId*/, std::vector<SMauRecvParam> >& membersMap);

	private:
		CXTLocker m_tableIdLstLocker;
		std::list<BYTE> m_tableIdLst;
		CXTLocker m_memberListLocker;
		std::map<UINT32 /*角色id*/, MemberAttr> m_memberList;
	};

private:
	CXTLocker m_unionIdLstLocker;
	std::list<UINT32> m_unionIdLst;
	CXTLocker m_unionMembersLocker;
	std::map<UINT32/*公会id*/, MauTabelMemberList*> m_unionMembers;	//公会频道成员列表
};

void InitMauTableMan();
MauTableMan* GetMauTableMan();
void DestroyMauTableMan();

#endif