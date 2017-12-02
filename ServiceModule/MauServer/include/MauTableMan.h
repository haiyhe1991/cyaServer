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
		BYTE linkerId;	//����link������id
		char nick[33];	//��ɫ�ǳ�
		UINT32 userId;	//��ɫ�˺�id
		BYTE tableId;	//����id
		BYTE ready;		//׼����0ȡ��׼����1׼����

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
		char nick[33];	//��ɫ�ǳ�
		BYTE tableId;	//����id
		BYTE ready;		//׼����0ȡ��׼����1׼����
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

	//��������
	int CreateMauTabel(const SLinkerCreateMauTabelReq* req, UINT32& unionId, BYTE& tableId);
	//��������
	int JoinMauTabel(const SLinkerJoinMauTabelReq* req, BYTE& tableId);
	//���׼��
	int ReadyTabel(const SLinkerReadyTabelReq* req, BYTE& tableId);
	//�뿪����
	int LeaveMauTabel(const SLinkerLeaveMauTabelReq* req, BYTE& tableId);
	//�û��˳�
	int UserExit(UINT32 userId, UINT32 roleId, BYTE& tableId);

	//��ȡ����Ƶ���û�
	int GetUnionMauTabelMember(UINT32 unionId, UINT32 roleId, MemberAttr& member);
	//��ȡ����Ƶ�������û�
	int GetUnionMauTabelAllMembers(UINT32 senderRoleId, UINT32 unionId, std::map<BYTE/*linkerId*/, std::vector<SMauRecvParam> >& membersMap);

private:
	//���빫��Ƶ��
	int JoinUnionMauTabel(BYTE linkerId, UINT32 userId, UINT32 roleId, UINT32 unionId, BYTE& tableId, const char* pszNick);

	//�뿪����Ƶ��
	int LeaveUnionMauTabel(UINT32 roleId, UINT32 unionId, BYTE& tableId);

	//���׼��
	int ReadyUnionMauTabel(UINT32 roleId, UINT32 unionId, BYTE ready, BYTE& tableId);

private:
	class MauTabelMemberList : public BaseRefCount1	//Ƶ����Ա�б�
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
		std::map<UINT32 /*��ɫid*/, MemberAttr> m_memberList;
	};

private:
	CXTLocker m_unionIdLstLocker;
	std::list<UINT32> m_unionIdLst;
	CXTLocker m_unionMembersLocker;
	std::map<UINT32/*����id*/, MauTabelMemberList*> m_unionMembers;	//����Ƶ����Ա�б�
};

void InitMauTableMan();
MauTableMan* GetMauTableMan();
void DestroyMauTableMan();

#endif