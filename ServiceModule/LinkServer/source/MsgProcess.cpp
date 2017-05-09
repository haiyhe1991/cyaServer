#include "cyaLog.h"

#include "MsgProcess.h"

#include "LinkSession.h"
#include "LinkGlobalObj.h"
#include "LicenceClient.h"
#include "PubFunction.h"
#include "LinkConfig.h"
#include "RankClient.h"


DEFINE_FETCH_OBJ(MsgProcess)


MsgProcess::MsgProcess()
	: m_pMLogicClient(NULL), m_pChatClient(NULL), m_pRankClient(NULL)
{
	InitProc();
}

MsgProcess::~MsgProcess()
{
	FiniProc();
}

void MsgProcess::InitProc()
{
	GWS_Init();
	m_gwsHandle = GWS_FetchHandle(GlobalGetLinkConfig()->GetGWSAddr(),  /// 网关地址
		GlobalGetLinkConfig()->GetGWSPort(),  /// 网关端口
		(EServerType)(GlobalGetLinkConfig()->GetLinkNumber()),    /// Linker编号
		MsgProcess::GWSDataCallback, this);

	LogInfo(("登录网关服务器( %s:%d )%s", GlobalGetLinkConfig()->GetGWSAddr(),
		GlobalGetLinkConfig()->GetGWSPort(),
		GWS_OK == GWS_Login(m_gwsHandle) ? "成功" : "失败"));

	m_pMLogicClient = new MLogicClient();   ASSERT(NULL != m_pMLogicClient);
	m_pChatClient = new ChatClient();     ASSERT(NULL != m_pChatClient);
	m_pRankClient = new RankClient();     ASSERT(NULL != m_pRankClient);
}

void MsgProcess::FiniProc()
{
	OnCleanUserMap();
	OnDestroyClientPtr(m_pMLogicClient);
	OnDestroyClientPtr(m_pChatClient);
	OnDestroyClientPtr(m_pRankClient);

	if (m_gwsHandle)
	{
		GWS_Logout(m_gwsHandle);
		GWS_ReleaseHandle(m_gwsHandle);
		m_gwsHandle = NULL;
		LogInfo(("逻辑代理断开网关服务器"));
	}
	GWS_Cleanup();
}

void MsgProcess::OnCleanUserMap()
{
	CXTAutoLock locke(m_mapUserLocker);
	UserMap::iterator itr = m_users.begin();
	for (itr; m_users.end() != itr; ++itr)
	{
		ASSERT(itr->second);
		itr->second->ReleaseRef();
	}
	m_users.clear();
}

void MsgProcess::OnDestroyClientPtr(BaseClient*& p)
{
	if (NULL != p)
	{
		delete p;
		p = NULL;
	}
}

int MsgProcess::MsgProcessSite(const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID, bool nEncrypt)
{
	if (LICENCE_QUERY_UNLOCK_JOBS != nCmdCode)
	{
		ASSERT(INVALID_USER_ID != nUserID);
		if (INVALID_USER_ID == nUserID)
		{
			InputToSessionError(nCmdCode, LINK_USER_INVALID, nPktType, nUserID);
			return LINK_USER_INVALID;
		}
	}

	int nRetCode = LINK_MEMERY;
	BaseClient* pClient = NULL;
	if (LINK_CHAT_SERVER(nCmdCode) && m_pChatClient)
	{
		pClient = m_pChatClient;
	}
	else if ((LINK_MLOGIC_SERVER(nCmdCode) || LICENCE_QUERY_UNLOCK_JOBS == nCmdCode) && m_pMLogicClient)
	{
		pClient = m_pMLogicClient;
	}
	else if (LINK_OTHER_LOGIC_SERVER(nCmdCode) && m_pRankClient)
	{
		pClient = m_pRankClient;
	}
	/*else
	{
	ASSERT(FALSE);
	}*/

	if (NULL != pClient)
	{
		nRetCode = pClient->ClientMsg(m_gwsHandle, pMsgData, nMsgLen, nCmdCode, nPktType, nUserID, nEncrypt);
		if (0 != LINK_OK)
		{
			InputToSessionError(nCmdCode, nRetCode, nPktType, nUserID);
		}
		return nRetCode;
	}

#pragma compiling_remind("新的消息类型请求在这添加，根据不同的命令选择不同的客户代理")

	LogInfo(("接收到错误的服务器类型请求, UserID[%d] CmdCode[%d]", nUserID, nCmdCode));
	InputToSessionError(nCmdCode, LINK_SERVER_TYPE, nPktType, nUserID);

	return LINK_SERVER_TYPE;
}

#if SEPARATOR("Notify LicenseServer because SocketStausChange or UserSession")
int MsgProcess::NotifyLicenseUserLogin(UINT32 nUserID)
{
	ASSERT(INVALID_USER_ID != nUserID);
	if (INVALID_USER_ID == nUserID)
		return LINK_USER_INVALID;

	return GlobalGetLicenceClient()->ReportUserLogin(nUserID);
}

int MsgProcess::NotifyLicenseUserExit(UINT32 nUserID)
{
	ASSERT(INVALID_USER_ID != nUserID);
	if (INVALID_USER_ID == nUserID)
		return LINK_USER_INVALID;

	GlobalGetLicenceClient()->ReportUserExit(nUserID);

	return LINK_OK;
}

int MsgProcess::NotifyChatUserExit(UINT32 nUserID)
{
	ASSERT(INVALID_USER_ID != nUserID);
	if (INVALID_USER_ID == nUserID)
		return LINK_USER_INVALID;

	m_pChatClient->ClientMsg(m_gwsHandle, NULL, 0, CHAT_USER_EXIT, DATA_PACKET, nUserID, true);
	return LINK_OK;
}

UINT32 MsgProcess::GetOnlineUserNum()
{
	return (UINT32)m_users.size();
}

bool MsgProcess::RegistSession(void* pSessionSite, UINT32 nUserID, bool& repeat)
{
	ASSERT(INVALID_USER_ID != nUserID);
	ASSERT(NULL != pSessionSite);
	if (NULL == pSessionSite || INVALID_USER_ID == nUserID)
		return false;

	bool regFlag = false;
	/// 前一个下线
	CXTAutoLock locke(m_mapUserLocker);
	UserMap::iterator itr = m_users.find(nUserID);
	if (m_users.end() != itr)
	{
		m_pMLogicClient->ClientMsg(m_gwsHandle, NULL, 0, MLS_LEAVE_ROLE, DATA_PACKET, nUserID, true);
		LinkSession* pSession = static_cast<LinkSession*>(itr->second->GetSession());
		pSession->GetServeMan()->CloseSession(pSession);
		itr->second->ReleaseRef();
		m_users.erase(itr);
		//LogInfo(("重复登陆，用户(%d)被踢下线", nUserID));
		repeat = true;
	}

	MsgProcess::UserInfo* pUser = CyaTcp_NewEx(MsgProcess::UserInfo, (pSessionSite));
	ASSERT(NULL != pUser);
	if (NULL == pUser)
	{
		if (repeat)
			LogInfo(("用户[%d] 重复登陆，前面的连接关闭!", nUserID));
		return regFlag;
	}

	VERIFY((regFlag = m_users.insert(std::make_pair(nUserID, pUser)).second));
	if (regFlag)
		LogInfo(("User[%d] login OK!", nUserID));
	if (repeat)
		LogInfo(("用户[%d] 重复登陆，前面的连接关闭!", nUserID));

	return regFlag;
}

void MsgProcess::UnRegistSession(void* pSessionSite, UINT32 nUserID)
{
	ASSERT(INVALID_USER_ID != nUserID);
	ASSERT(NULL != pSessionSite);
	if (NULL == pSessionSite || INVALID_USER_ID == nUserID)
		return;
	LogDebug(("准备注销用户(%d)", nUserID));

	CXTAutoLock locke(m_mapUserLocker);
	UserMap::iterator itr = m_users.begin();
	for (itr; m_users.end() != itr; ++itr)
	{
		if (itr->first == nUserID && itr->second != NULL && itr->second->GetSession() == pSessionSite)
		{
			LogInfo(("User[%d] logout OK!", nUserID));
			m_pMLogicClient->ClientMsg(m_gwsHandle, NULL, 0, MLS_LEAVE_ROLE, DATA_PACKET, nUserID, true);
			itr->second->ReleaseRef();
			m_users.erase(itr);
			break;
		}
	}
}

void MsgProcess::KickSession(UINT32 nUserID)
{
	ASSERT(INVALID_USER_ID != nUserID);
	if (INVALID_USER_ID == nUserID)
	{
		LogDebug(("用户(%d)无效，无法剔除", nUserID));
		return;
	}

	CXTAutoLock locke(m_mapUserLocker);
	UserMap::iterator itr = m_users.find(nUserID);
	if (m_users.end() != itr)
	{
		ASSERT(itr->second);
		LinkSession* pSession = static_cast<LinkSession*>(itr->second->GetSession());
		ASSERT(pSession);
		pSession->GetServeMan()->CloseSession(pSession);
		m_users.erase(itr);
		LogInfo(("用户(%d)其它地方登陆，被踢下线", nUserID));
	}
}

#endif

#if SEPARATOR("send msg to user")
void MsgProcess::InputToSessionMsg(UINT32 nUserID, UINT16 nPktType, UINT16 nCmdCode, UINT16 nRetCode, const void* pPktData, UINT16 nPktLen)
{
	ASSERT(INVALID_USER_ID != nUserID);
	if (INVALID_USER_ID == nUserID)
	{
		LogDebug(("用户(%d)无效，命令(%d)无法回复", nUserID, nCmdCode));
		return;
	}

	CXTAutoLock locke(m_mapUserLocker);
	UserMap::iterator itr = m_users.find(nUserID);
	if (m_users.end() != itr)
	{
		ASSERT(NULL != itr->second);
		LinkSession* pSession = static_cast<LinkSession*>(itr->second->GetSession());
		ASSERT(NULL != pSession);
		pSession->InputPacket(true, nPktType, nCmdCode, nRetCode, pPktData, nPktLen);
	}
}

void MsgProcess::InputToSessionError(UINT16 nCmdCode, UINT16 nRetCode, UINT16 nPktType, UINT32 nUserID)
{
	// 查询解锁职业消息发给验证服务器
	if (LICENCE_QUERY_UNLOCK_JOBS == nCmdCode)
	{
		GlobalGetLicenceClient()->SendCtrlData(nCmdCode, nRetCode, NULL, 0);
		return;
	}
	else
	{
		ASSERT(INVALID_USER_ID != nUserID);
		if (INVALID_USER_ID == nUserID)
		{
			LogDebug(("用户(%d)无效，命令(%d)的错误消息(%d)无法回复", nUserID, nCmdCode, nRetCode));
			return;
		}
	}

	CXTAutoLock locke(m_mapUserLocker);
	UserMap::iterator itr = m_users.find(nUserID);
	if (m_users.end() != itr)
	{
		ASSERT(NULL != itr->second);
		LinkSession* pSession = static_cast<LinkSession*>(itr->second->GetSession());
		ASSERT(NULL != pSession);
		pSession->ResReturnCode(true, nPktType, nCmdCode, nRetCode);
	}
}

#endif

#if SEPARATOR("Callback for LogicClient")
int MsgProcess::RegistRole(UINT32 nUserID, UINT32 roleID)
{
	ASSERT(INVALID_USER_ID != nUserID);
	ASSERT(INVALID_ROLE_ID != roleID);
	if (INVALID_ROLE_ID == roleID || INVALID_USER_ID == nUserID)
	{
		LogDebug(("RegistRole::用户(%d)角色(%d)无效", nUserID, roleID));
		return LINK_USER_INVALID;
	}

	CXTAutoLock locke(m_mapUserLocker);
	UserMap::iterator itr = m_users.find(nUserID);
	if (m_users.end() != itr && nUserID != INVALID_USER_ID)
	{
		ASSERT(itr->second);
		itr->second->SetRoleID(roleID);
		return LINK_OK;
	}

	return LINK_ACCOUNT_TOKEN;
}

void MsgProcess::UnRegistRole(UINT32 nUserID)
{
	ASSERT(INVALID_USER_ID != nUserID);
	if (INVALID_USER_ID == nUserID)
	{
		LogDebug(("UnRegistRole::用户(%d)无效", nUserID));
		return;
	}

	CXTAutoLock locke(m_mapUserLocker);
	UserMap::iterator itr = m_users.find(nUserID);
	if (m_users.end() != itr)
	{
		ASSERT(itr->second);
		itr->second->SetRoleID(INVALID_ROLE_ID);
	}
}

UINT32 MsgProcess::FindRoleIDByUserID(UINT32 nUserID)
{
	ASSERT(INVALID_USER_ID != nUserID);
	if (INVALID_USER_ID == nUserID)
	{
		LogDebug(("用户(%d)无效，找不到角色", nUserID));
		return LINK_USER_INVALID;
	}

	CXTAutoLock locke(m_mapUserLocker);
	UserMap::iterator itr = m_users.find(nUserID);
	if (m_users.end() != itr && nUserID != INVALID_USER_ID)
	{
		ASSERT(itr->second);
		return itr->second->FetchRoleID();
	}

	return INVALID_ROLE_ID;
}

#endif

#if SEPARATOR("Gateway msg process")
void MsgProcess::GroupMsg(UINT16 nPktType, UINT16 nCmdCode, UINT16 nRetCode, const void* pPktData, UINT16 nPktLen)
{
	CXTAutoLock locke(m_mapUserLocker);
	UserMap::iterator itr = m_users.begin();
	for (itr; m_users.end() != itr; ++itr)
	{
		ASSERT(itr->second);
		LinkSession* pSession = static_cast<LinkSession*>(itr->second->GetSession());
		pSession->InputPacket(true, nPktType, nCmdCode, nRetCode, pPktData, nPktLen);
	}
}

void MsgProcess::GWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen, void* context)
{
	MsgProcess* pThis = (MsgProcess*)context;
	if (pThis)
		pThis->OnGwsDataCallBack(pHead, payload, payloadLen);
}

void MsgProcess::OnGwsDataCallBack(SGWSProtocolHead* pHead, const void* payload, int payloadLen)
{
	ASSERT(NULL != pHead && NULL != payload && 0 < payloadLen);
	SServerCommonReply* pComHead = (SServerCommonReply*)payload;
	pComHead->hton();

	BaseClient* pClient = NULL;
	if (LINK_CHAT_SERVER(pComHead->cmdCode) && m_pChatClient)
	{
		pClient = m_pChatClient;
	}
	else if ((LINK_MLOGIC_SERVER(pComHead->cmdCode) || LICENCE_QUERY_UNLOCK_JOBS == pComHead->cmdCode) && m_pMLogicClient)
	{
		pClient = m_pMLogicClient;
	}
	else if (LINK_OTHER_LOGIC_SERVER(pComHead->cmdCode) && m_pRankClient)
	{
		pClient = m_pRankClient;
	}
	else
	{
		ASSERT(FALSE);
	}

	if (NULL != pClient)
	{
		pClient->WGSMsg(pHead, pComHead->cmdCode, pComHead->retCode, pComHead->data, payloadLen - COM_RAW_SIZE);
	}
}

#endif

MsgProcess::UserInfo::UserInfo(void* session)
	: m_roleid(INVALID_ROLE_ID)
	, m_time(GetMsel())
	, m_session(session)
{

}

MsgProcess::UserInfo::~UserInfo()
{
	m_time = 0;
	m_session = NULL;
}

UINT32 MsgProcess::UserInfo::FetchRoleID() const
{
	return m_roleid;
}

void MsgProcess::UserInfo::SetRoleID(UINT32 roleId)
{
	m_roleid = roleId;
}

void MsgProcess::UserInfo::FlushHeartBeatTime()
{
	m_time = GetMsel();
}

bool MsgProcess::UserInfo::SessionBroken(DWORD timeOut) const
{
	return (GetMsel() - m_time > timeOut);
}

void* MsgProcess::UserInfo::GetSession() const
{
	return m_session;
}

void MsgProcess::UserInfo::DeleteObj()
{
	CyaTcp_Delete(this);
}