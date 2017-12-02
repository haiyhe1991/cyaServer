#include "ChannelMan.h"
#include "cyaMaxMin.h"

static ChannelMan* sg_channelMan = NULL;
void InitChannelMan()
{
	ASSERT(sg_channelMan == NULL);
	sg_channelMan = new ChannelMan();
	ASSERT(sg_channelMan != NULL);
}

ChannelMan* GetChannelMan()
{
	return sg_channelMan;
}

void DestroyChannelMan()
{
	ChannelMan* channelMan = sg_channelMan;
	sg_channelMan = NULL;
	if (channelMan != NULL)
		delete channelMan;
}

ChannelMan::ChannelMan()
{

}

ChannelMan::~ChannelMan()
{

}

int ChannelMan::JoinChannel(const SLinkerJoinChannelReq* req)
{
	int retCode = CHAT_CHANNEL_NOT_ALLOW_EL;

	if (req->channel == CHAT_WORLD_CHANNEL)
		retCode = JoinWorldChannel(req->linkerId, req->userId, req->roleId, req->nick);
	else if (req->channel == CHAT_UNIONS_CHANNEL)
		retCode = JoinUnionChannel(req->linkerId, req->userId, req->roleId, req->joinId, req->nick);

	return retCode;
}

int ChannelMan::LeaveChannel(const SLinkerLeaveChannelReq* req)
{
	int retCode = CHAT_CHANNEL_NOT_ALLOW_EL;

	if (req->channel == CHAT_WORLD_CHANNEL)
		retCode = LeaveWorldChannel(req->roleId);
	else if (req->channel == CHAT_UNIONS_CHANNEL)
		retCode = LeaveUnionChannel(req->roleId, req->joinId);

	return retCode;
}

int ChannelMan::UserExit(UINT32 userId, UINT32 roleId)
{
	{
		CXTAutoLock lock(m_worldMembersLocker);
		std::map<UINT32 /*角色id*/, MemberAttr>::iterator it = m_worldMembers.find(roleId);
		if (it != m_worldMembers.end())
			m_worldMembers.erase(it);
	}

	{
	CXTAutoLock lock(m_unionMembersLocker);
	std::map<UINT32/*公会id*/, ChannelMemberList*>::iterator it = m_unionMembers.begin();
	for (; it != m_unionMembers.end();)
	{
		BOOL needEraseUnion = false;
		it->second->DelMember(roleId, needEraseUnion);
		if (needEraseUnion)
		{
			ChannelMemberList* pChnMemberList = it->second;
			m_unionMembers.erase(it++);
			pChnMemberList->ReleaseRef();
		}
		else
			++it;
	}
}

	return CHAT_OK;
}

int ChannelMan::GetWorldChannelAllMembers(UINT32 senderRoleId, std::map<BYTE/*linkerId*/, std::vector<SChatRecvParam> >& membersMap)
{
	CXTAutoLock lock(m_worldMembersLocker);
	std::map<UINT32 /*角色id*/, MemberAttr>::iterator it = m_worldMembers.find(senderRoleId);
	if (it == m_worldMembers.end())
		return CHAT_ID_NOT_EXIST_IN_CHANNEL;

	it = m_worldMembers.begin();
	for (; it != m_worldMembers.end(); ++it)
	{
		UINT32 roleId = it->first;
		if (senderRoleId == roleId)
			continue;

		SChatRecvParam param;
		BYTE linkerId = it->second.linkerId;
		param.recvUserId = it->second.userId;
		param.recvRoleId = roleId;
		std::map<BYTE/*linkerId*/, std::vector<SChatRecvParam> >::iterator it2 = membersMap.find(linkerId);
		if (it2 != membersMap.end())
			it2->second.push_back(param);
		else
		{
			std::vector<SChatRecvParam> vec;
			vec.push_back(param);
			membersMap.insert(std::make_pair(linkerId, vec));
		}
	}
	return membersMap.empty() ? CHAT_CHANNEL_MEMBERS_EMPTY : CHAT_OK;
}

int ChannelMan::GetWorldChannelMember(UINT32 roleId, MemberAttr& member)
{
	CXTAutoLock lock(m_worldMembersLocker);
	std::map<UINT32 /*角色id*/, MemberAttr>::iterator it = m_worldMembers.find(roleId);
	if (it == m_worldMembers.end())
		return CHAT_ID_NOT_EXIST_IN_CHANNEL;

	member.linkerId = it->second.linkerId;
	member.userId = it->second.userId;
	strcpy(member.nick, it->second.nick);
	return CHAT_OK;
}

int ChannelMan::GetUnionChannelAllMembers(UINT32 senderRoleId, UINT32 unionId, std::map<BYTE/*linkerId*/, std::vector<SChatRecvParam> >& membersMap)
{
	ChannelMemberList* pChnMemberList = NULL;
	{
		CXTAutoLock lock(m_unionMembersLocker);
		std::map<UINT32/*公会id*/, ChannelMemberList*>::iterator it = m_unionMembers.find(unionId);
		if (it != m_unionMembers.end())
		{
			pChnMemberList = it->second;
			pChnMemberList->AddRef();
		}
	}

	if (pChnMemberList == NULL)
		return CHAT_ID_NOT_EXIST_IN_CHANNEL;	//该公会id不存在

	AutoRefCountObjTmpl<ChannelMemberList> pAutoObj(pChnMemberList);
	return pAutoObj->GetAllMembers(senderRoleId, membersMap);
}

int ChannelMan::GetUnionChannelMember(UINT32 unionId, UINT32 roleId, MemberAttr& member)
{
	ChannelMemberList* pChnMemberList = NULL;
	{
		CXTAutoLock lock(m_unionMembersLocker);
		std::map<UINT32/*公会id*/, ChannelMemberList*>::iterator it = m_unionMembers.find(unionId);
		if (it != m_unionMembers.end())
		{
			pChnMemberList = it->second;
			pChnMemberList->AddRef();
		}
	}

	if (pChnMemberList == NULL)
		return CHAT_ID_NOT_EXIST_IN_CHANNEL;	//该公会id不存在

	AutoRefCountObjTmpl<ChannelMemberList> pAutoObj(pChnMemberList);
	return pAutoObj->GetMember(roleId, member);
}

int ChannelMan::JoinWorldChannel(BYTE linkerId, UINT32 userId, UINT32 roleId, const char* pszNick)
{
	CXTAutoLock lock(m_worldMembersLocker);
	std::map<UINT32 /*角色id*/, MemberAttr>::iterator it = m_worldMembers.find(roleId);
	if (it != m_worldMembers.end())
	{
		it->second.linkerId = linkerId;
		it->second.userId = userId;
		memset(it->second.nick, 0, sizeof(it->second.nick));
		strncpy(it->second.nick, pszNick, min(strlen(pszNick), sizeof(it->second.nick) - sizeof(char)));
	}
	else
	{
		MemberAttr member;
		member.linkerId = linkerId;
		member.userId = userId;
		strncpy(member.nick, pszNick, min(strlen(pszNick), sizeof(member.nick) - sizeof(char)));
		m_worldMembers.insert(std::make_pair(roleId, member));
	}

	return CHAT_OK;
}

int ChannelMan::JoinUnionChannel(BYTE linkerId, UINT32 userId, UINT32 roleId, UINT32 unionId, const char* pszNick)
{
	ChannelMemberList* pChnMemberList = NULL;
	{
		CXTAutoLock lock(m_unionMembersLocker);
		std::map<UINT32/*公会id*/, ChannelMemberList*>::iterator it = m_unionMembers.find(unionId);
		if (it != m_unionMembers.end())
		{
			pChnMemberList = it->second;
			pChnMemberList->AddRef();
		}
		else
		{
			pChnMemberList = new ChannelMemberList();
			ASSERT(pChnMemberList != NULL);
			pChnMemberList->AddRef();
			m_unionMembers.insert(std::make_pair(unionId, pChnMemberList));
		}
	}

	ASSERT(pChnMemberList != NULL);
	AutoRefCountObjTmpl<ChannelMemberList> pAutoObj(pChnMemberList);
	return pAutoObj->AddMember(linkerId, userId, roleId, pszNick);
}

int ChannelMan::LeaveWorldChannel(UINT32 roleId)
{
	CXTAutoLock lock(m_worldMembersLocker);
	std::map<UINT32 /*角色id*/, MemberAttr>::iterator it = m_worldMembers.find(roleId);
	if (it != m_worldMembers.end())
		m_worldMembers.erase(it);
	return CHAT_OK;
}

int ChannelMan::LeaveUnionChannel(UINT32 roleId, UINT32 unionId)
{
	ChannelMemberList* pChnMemberList = NULL;

	CXTAutoLock lock(m_unionMembersLocker);
	std::map<UINT32/*公会id*/, ChannelMemberList*>::iterator it = m_unionMembers.find(unionId);
	if (it == m_unionMembers.end())
		return CHAT_OK;

	pChnMemberList = it->second;
	BOOL needEraseUnion = false;
	pChnMemberList->DelMember(roleId, needEraseUnion);
	if (needEraseUnion)
	{
		m_unionMembers.erase(it);
		pChnMemberList->ReleaseRef();
	}

	return CHAT_OK;
}

ChannelMan::ChannelMemberList::ChannelMemberList()
{

}

ChannelMan::ChannelMemberList::~ChannelMemberList()
{

}

int ChannelMan::ChannelMemberList::AddMember(BYTE linkerId, UINT32 userId, UINT32 roleId, const char* pszNick)
{
	CXTAutoLock lock(m_memberListLocker);
	std::map<UINT32 /*角色id*/, MemberAttr>::iterator it = m_memberList.find(roleId);
	if (it != m_memberList.end())
	{
		it->second.linkerId = linkerId;
		it->second.userId = userId;
		memset(it->second.nick, 0, sizeof(it->second.nick));
		strncpy(it->second.nick, pszNick, min(strlen(pszNick), sizeof(it->second.nick) - sizeof(char)));
	}
	else
	{
		MemberAttr member;
		member.linkerId = linkerId;
		member.userId = userId;
		strncpy(member.nick, pszNick, min(strlen(pszNick), sizeof(member.nick) - sizeof(char)));
		m_memberList.insert(std::make_pair(roleId, member));
	}
	return CHAT_OK;
}

int ChannelMan::ChannelMemberList::DelMember(UINT32 roleId, BOOL& needEraseUnion)
{
	int ret = CHAT_OK;
	needEraseUnion = false;
	CXTAutoLock lock(m_memberListLocker);
	std::map<UINT32 /*角色id*/, MemberAttr>::iterator it = m_memberList.find(roleId);
	if (it != m_memberList.end())
		m_memberList.erase(it);
	else
		ret = CHAT_ID_NOT_EXIST_IN_CHANNEL;

	if (m_memberList.empty())
		needEraseUnion = true;
	return ret;
}

int ChannelMan::ChannelMemberList::GetMember(UINT32 roleId, MemberAttr& member)
{
	std::map<UINT32 /*角色id*/, MemberAttr>::iterator it = m_memberList.find(roleId);
	if (it == m_memberList.end())
		return CHAT_ID_NOT_EXIST_IN_CHANNEL;

	member.linkerId = it->second.linkerId;
	member.userId = it->second.userId;
	strcpy(member.nick, it->second.nick);
	return CHAT_OK;
}

int ChannelMan::ChannelMemberList::GetAllMembers(UINT32 curRoleId, std::map<BYTE/*linkerId*/, std::vector<SChatRecvParam> >& membersMap)
{
	CXTAutoLock lock(m_memberListLocker);
	std::map<UINT32 /*角色id*/, MemberAttr>::iterator it = m_memberList.find(curRoleId);
	if (it == m_memberList.end())
		return CHAT_ID_NOT_EXIST_IN_CHANNEL;

	it = m_memberList.begin();
	for (; it != m_memberList.end(); ++it)
	{
		UINT32 roleId = it->first;
		if (curRoleId == roleId)
			continue;

		SChatRecvParam recvParam;
		recvParam.recvRoleId = roleId;
		recvParam.recvUserId = it->second.userId;
		BYTE linkerId = it->second.linkerId;
		std::map<BYTE/*linkerId*/, std::vector<SChatRecvParam> >::iterator it2 = membersMap.find(linkerId);
		if (it2 != membersMap.end())
			it2->second.push_back(recvParam);
		else
		{
			std::vector<SChatRecvParam> vec;
			vec.push_back(recvParam);
			membersMap.insert(std::make_pair(linkerId, vec));
		}
	}
	return membersMap.empty() ? CHAT_CHANNEL_MEMBERS_EMPTY : CHAT_OK;
}