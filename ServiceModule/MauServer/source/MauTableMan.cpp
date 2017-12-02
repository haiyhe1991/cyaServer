#include "MauTableMan.h"
#include "cyaMaxMin.h"
#include "cyaTime.h"

static MauTableMan* sg_mauTabelMan = NULL;
void InitMauTableMan()
{
	ASSERT(sg_mauTabelMan == NULL);
	sg_mauTabelMan = new MauTableMan();
	ASSERT(sg_mauTabelMan != NULL);
}

MauTableMan* GetMauTableMan()
{
	return sg_mauTabelMan;
}

void DestroyMauTableMan()
{
	MauTableMan* mauTableMan = sg_mauTabelMan;
	sg_mauTabelMan = NULL;
	if (mauTableMan != NULL)
		delete mauTableMan;
}

MauTableMan::MauTableMan()
{
	m_unionIdLst.clear();
	for (int i = 100000; i < 999999; i++)
	{
		m_unionIdLst.push_back(i);
	}
}

MauTableMan::~MauTableMan()
{

}

int MauTableMan::CreateMauTabel(const SLinkerCreateMauTabelReq* req, UINT32& unionId, BYTE& tableId)
{
	int retCode = MAU_TABLE_NOT_ALLOW_EL;
	//生成随机工会id
	{
		CXTAutoLock lock(m_unionIdLstLocker);
		unionId = 999999;
		if (m_unionIdLst.size() == 0)
			return retCode;
		srand((unsigned)time(NULL));
		int index = rand() % m_unionIdLst.size();
		std::list<UINT32>::iterator itLst = m_unionIdLst.begin();
		while (index > 0)
		{
			itLst++;
			index--;
		}
		unionId = *itLst;
		m_unionIdLst.erase(itLst);
	}
	retCode = JoinUnionMauTabel(req->linkerId, req->userId, req->roleId, unionId, tableId, req->nick);
	return retCode;
}

int MauTableMan::JoinMauTabel(const SLinkerJoinMauTabelReq* req, BYTE& tableId)
{
	int retCode = MAU_TABLE_NOT_ALLOW_EL;

	{
		CXTAutoLock lock(m_unionMembersLocker);
		std::map<UINT32/*公会id*/, MauTabelMemberList*>::iterator it = m_unionMembers.find(req->joinId);
		if (it == m_unionMembers.end())
		{
			retCode = MAU_TABLE_NOT_ALLOW_EL;
			return retCode;
		}
	}
		retCode = JoinUnionMauTabel(req->linkerId, req->userId, req->roleId, req->joinId, tableId, req->nick);

		return retCode;
}

int MauTableMan::LeaveMauTabel(const SLinkerLeaveMauTabelReq* req, BYTE& tableId)
{
	int retCode = MAU_TABLE_NOT_ALLOW_EL;

	retCode = LeaveUnionMauTabel(req->roleId, req->joinId, tableId);

	return retCode;
}

int MauTableMan::ReadyTabel(const SLinkerReadyTabelReq* req, BYTE& tableId)
{
	int retCode = MAU_TABLE_NOT_ALLOW_EL;

	retCode = ReadyUnionMauTabel(req->roleId, req->joinId, req->ready, tableId);

	return retCode;
}

int MauTableMan::UserExit(UINT32 userId, UINT32 roleId, BYTE& tableId)
{
	{
		CXTAutoLock lock(m_unionMembersLocker);
		std::map<UINT32/*公会id*/, MauTabelMemberList*>::iterator it = m_unionMembers.begin();
		for (; it != m_unionMembers.end();)
		{
			BOOL needEraseUnion = false;
			it->second->DelMember(roleId, tableId, needEraseUnion);
			if (needEraseUnion)
			{
				MauTabelMemberList* pChnMemberList = it->second;
				m_unionMembers.erase(it++);
				pChnMemberList->ReleaseRef();
			}
			else
				++it;
		}
	}

	return MAU_OK;
}

int MauTableMan::GetUnionMauTabelAllMembers(UINT32 senderRoleId, UINT32 unionId, std::map<BYTE/*linkerId*/, std::vector<SMauRecvParam> >& membersMap)
{
	MauTabelMemberList* pChnMemberList = NULL;
	{
		CXTAutoLock lock(m_unionMembersLocker);
		std::map<UINT32/*公会id*/, MauTabelMemberList*>::iterator it = m_unionMembers.find(unionId);
		if (it != m_unionMembers.end())
		{
			pChnMemberList = it->second;
			pChnMemberList->AddRef();
		}
	}

	if (pChnMemberList == NULL)
		return MAU_ID_NOT_EXIST_IN_CHANNEL;	//该公会id不存在

	AutoRefCountObjTmpl<MauTabelMemberList> pAutoObj(pChnMemberList);
	return pAutoObj->GetAllMembers(senderRoleId, membersMap);
}

int MauTableMan::GetUnionMauTabelMember(UINT32 unionId, UINT32 roleId, MemberAttr& member)
{
	MauTabelMemberList* pChnMemberList = NULL;
	{
		CXTAutoLock lock(m_unionMembersLocker);
		std::map<UINT32/*公会id*/, MauTabelMemberList*>::iterator it = m_unionMembers.find(unionId);
		if (it != m_unionMembers.end())
		{
			pChnMemberList = it->second;
			pChnMemberList->AddRef();
		}
	}

	if (pChnMemberList == NULL)
		return MAU_ID_NOT_EXIST_IN_CHANNEL;	//该公会id不存在

	AutoRefCountObjTmpl<MauTabelMemberList> pAutoObj(pChnMemberList);
	return pAutoObj->GetMember(roleId, member);
}

int MauTableMan::JoinUnionMauTabel(BYTE linkerId, UINT32 userId, UINT32 roleId, UINT32 unionId, BYTE& tableId, const char* pszNick)
{
	MauTabelMemberList* pChnMemberList = NULL;
	{
		CXTAutoLock lock(m_unionMembersLocker);
		std::map<UINT32/*公会id*/, MauTabelMemberList*>::iterator it = m_unionMembers.find(unionId);
		if (it != m_unionMembers.end())
		{
			pChnMemberList = it->second;
			pChnMemberList->AddRef();
		}
		else
		{
			pChnMemberList = new MauTabelMemberList();
			ASSERT(pChnMemberList != NULL);
			pChnMemberList->AddRef();
			m_unionMembers.insert(std::make_pair(unionId, pChnMemberList));
		}
	}

	ASSERT(pChnMemberList != NULL);
	AutoRefCountObjTmpl<MauTabelMemberList> pAutoObj(pChnMemberList);
	return pAutoObj->AddMember(linkerId, tableId, userId, roleId, pszNick);
}

int MauTableMan::ReadyUnionMauTabel(UINT32 roleId, UINT32 unionId, BYTE ready, BYTE& tableId)
{
	MauTabelMemberList* pChnMemberList = NULL;

	CXTAutoLock lock(m_unionMembersLocker);
	std::map<UINT32/*公会id*/, MauTabelMemberList*>::iterator it = m_unionMembers.find(unionId);
	if (it == m_unionMembers.end())
		return MAU_OK;

	pChnMemberList = it->second;
	pChnMemberList->ReadyMember(roleId, ready, tableId);

	return MAU_OK;
}

int MauTableMan::LeaveUnionMauTabel(UINT32 roleId, UINT32 unionId, BYTE& tableId)
{
	MauTabelMemberList* pChnMemberList = NULL;

	CXTAutoLock lock(m_unionMembersLocker);
	std::map<UINT32/*公会id*/, MauTabelMemberList*>::iterator it = m_unionMembers.find(unionId);
	if (it == m_unionMembers.end())
		return MAU_OK;

	pChnMemberList = it->second;
	BOOL needEraseUnion = false;
	pChnMemberList->DelMember(roleId, tableId, needEraseUnion);
	if (needEraseUnion)
	{
		m_unionMembers.erase(it);
		pChnMemberList->ReleaseRef();
	}

	return MAU_OK;
}

MauTableMan::MauTabelMemberList::MauTabelMemberList()
{
	m_tableIdLst.clear();
	for (int i = 0; i < 4; i++)
	{
		m_tableIdLst.push_back(i);
	}
}

MauTableMan::MauTabelMemberList::~MauTabelMemberList()
{

}

int MauTableMan::MauTabelMemberList::AddMember(BYTE linkerId, BYTE& tableId, UINT32 userId, UINT32 roleId, const char* pszNick)
{
	CXTAutoLock lock(m_memberListLocker);
	{
		//生成随机工会id
		CXTAutoLock lock(m_tableIdLstLocker);
		if (m_tableIdLst.size() == 0)
			return MAU_TABLE_NOT_ALLOW_EL;
		srand((unsigned)time(NULL));
		int index = rand() % m_tableIdLst.size();
		std::list<BYTE>::iterator itLst = m_tableIdLst.begin();
		while (index > 0)
		{
			itLst++;
			index--;
		}
		tableId = *itLst;
		m_tableIdLst.erase(itLst);
	}
	std::map<UINT32 /*角色id*/, MemberAttr>::iterator it = m_memberList.find(roleId);
	if (it != m_memberList.end())
	{
		it->second.linkerId = linkerId;
		it->second.userId = userId;
		it->second.tableId = tableId;
		it->second.ready = 0;
		memset(it->second.nick, 0, sizeof(it->second.nick));
		strncpy(it->second.nick, pszNick, min(strlen(pszNick), sizeof(it->second.nick) - sizeof(char)));
	}
	else
	{
		MemberAttr member;
		member.linkerId = linkerId;
		member.userId = userId;
		member.tableId = tableId;
		member.ready = 0;
		strncpy(member.nick, pszNick, min(strlen(pszNick), sizeof(member.nick) - sizeof(char)));
		m_memberList.insert(std::make_pair(roleId, member));
	}
	return MAU_OK;
}

int MauTableMan::MauTabelMemberList::DelMember(UINT32 roleId, BYTE& tableId, BOOL& needEraseUnion)
{
	int ret = MAU_OK;
	needEraseUnion = false;
	tableId = 0;
	CXTAutoLock lock(m_memberListLocker);
	std::map<UINT32 /*角色id*/, MemberAttr>::iterator it = m_memberList.find(roleId);
	if (it != m_memberList.end())
	{
		tableId = it->second.tableId;
		{
			CXTAutoLock lock(m_tableIdLstLocker);
			m_tableIdLst.push_back(tableId);
		}
		m_memberList.erase(it);
	}
	else
	{
		ret = MAU_ID_NOT_EXIST_IN_CHANNEL;
	}

	if (m_memberList.empty())
		needEraseUnion = true;
	return ret;
}

int MauTableMan::MauTabelMemberList::ReadyMember(UINT32 roleId, BYTE ready, BYTE& tableId)
{
	int ret = MAU_OK;
	CXTAutoLock lock(m_memberListLocker);
	std::map<UINT32 /*角色id*/, MemberAttr>::iterator it = m_memberList.find(roleId);
	if (it != m_memberList.end())
	{
		tableId = it->second.tableId;
		it->second.ready = ready;
	}
	else
	{
		ret = MAU_ID_NOT_EXIST_IN_CHANNEL;
	}
	return ret;
}

int MauTableMan::MauTabelMemberList::GetMember(UINT32 roleId, MemberAttr& member)
{
	std::map<UINT32 /*角色id*/, MemberAttr>::iterator it = m_memberList.find(roleId);
	if (it == m_memberList.end())
		return MAU_ID_NOT_EXIST_IN_CHANNEL;

	member.linkerId = it->second.linkerId;
	member.userId = it->second.userId;
	member.userId = it->second.tableId;
	member.ready = it->second.ready;
	strcpy(member.nick, it->second.nick);
	return MAU_OK;
}

int MauTableMan::MauTabelMemberList::GetAllMembers(UINT32 curRoleId, std::map<BYTE/*linkerId*/, std::vector<SMauRecvParam> >& membersMap)
{
	CXTAutoLock lock(m_memberListLocker);
	std::map<UINT32 /*角色id*/, MemberAttr>::iterator it = m_memberList.find(curRoleId);
	if (it == m_memberList.end())
		return MAU_ID_NOT_EXIST_IN_CHANNEL;

	it = m_memberList.begin();
	for (; it != m_memberList.end(); ++it)
	{
		SMauRecvParam recvParam;
		recvParam.recvRoleId = it->first;
		recvParam.recvUserId = it->second.userId;
		recvParam.tableId = it->second.tableId;
		recvParam.ready = it->second.ready;
		strcpy(recvParam.nick, it->second.nick);
		BYTE linkerId = it->second.linkerId;
		std::map<BYTE/*linkerId*/, std::vector<SMauRecvParam> >::iterator it2 = membersMap.find(linkerId);
		if (it2 != membersMap.end())
			it2->second.push_back(recvParam);
		else
		{
			std::vector<SMauRecvParam> vec;
			vec.push_back(recvParam);
			membersMap.insert(std::make_pair(linkerId, vec));
		}
	}
	return membersMap.empty() ? MAU_TABLE_MEMBERS_EMPTY : MAU_OK;
}