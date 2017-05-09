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
	m_gwsHandle = GWS_FetchHandle(GlobalGetLinkConfig()->GetGWSAddr(),  /// ���ص�ַ
		GlobalGetLinkConfig()->GetGWSPort(),  /// ���ض˿�
		(EServerType)(GlobalGetLinkConfig()->GetLinkNumber()),    /// Linker���
		MsgProcess::GWSDataCallback, this);

	LogInfo(("��¼���ط�����( %s:%d )%s", GlobalGetLinkConfig()->GetGWSAddr(),
		GlobalGetLinkConfig()->GetGWSPort(),
		GWS_OK == GWS_Login(m_gwsHandle) ? "�ɹ�" : "ʧ��"));

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
		LogInfo(("�߼�����Ͽ����ط�����"));
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

#pragma compiling_remind("�µ���Ϣ��������������ӣ����ݲ�ͬ������ѡ��ͬ�Ŀͻ�����")

	LogInfo(("���յ�����ķ�������������, UserID[%d] CmdCode[%d]", nUserID, nCmdCode));
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
	/// ǰһ������
	CXTAutoLock locke(m_mapUserLocker);
	UserMap::iterator itr = m_users.find(nUserID);
	if (m_users.end() != itr)
	{
		m_pMLogicClient->ClientMsg(m_gwsHandle, NULL, 0, MLS_LEAVE_ROLE, DATA_PACKET, nUserID, true);
		LinkSession* pSession = static_cast<LinkSession*>(itr->second->GetSession());
		pSession->GetServeMan()->CloseSession(pSession);
		itr->second->ReleaseRef();
		m_users.erase(itr);
		//LogInfo(("�ظ���½���û�(%d)��������", nUserID));
		repeat = true;
	}

	MsgProcess::UserInfo* pUser = CyaTcp_NewEx(MsgProcess::UserInfo, (pSessionSite));
	ASSERT(NULL != pUser);
	if (NULL == pUser)
	{
		if (repeat)
			LogInfo(("�û�[%d] �ظ���½��ǰ������ӹر�!", nUserID));
		return regFlag;
	}

	VERIFY((regFlag = m_users.insert(std::make_pair(nUserID, pUser)).second));
	if (regFlag)
		LogInfo(("User[%d] login OK!", nUserID));
	if (repeat)
		LogInfo(("�û�[%d] �ظ���½��ǰ������ӹر�!", nUserID));

	return regFlag;
}

void MsgProcess::UnRegistSession(void* pSessionSite, UINT32 nUserID)
{
	ASSERT(INVALID_USER_ID != nUserID);
	ASSERT(NULL != pSessionSite);
	if (NULL == pSessionSite || INVALID_USER_ID == nUserID)
		return;
	LogDebug(("׼��ע���û�(%d)", nUserID));

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
		LogDebug(("�û�(%d)��Ч���޷��޳�", nUserID));
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
		LogInfo(("�û�(%d)�����ط���½����������", nUserID));
	}
}

#endif

#if SEPARATOR("send msg to user")
void MsgProcess::InputToSessionMsg(UINT32 nUserID, UINT16 nPktType, UINT16 nCmdCode, UINT16 nRetCode, const void* pPktData, UINT16 nPktLen)
{
	ASSERT(INVALID_USER_ID != nUserID);
	if (INVALID_USER_ID == nUserID)
	{
		LogDebug(("�û�(%d)��Ч������(%d)�޷��ظ�", nUserID, nCmdCode));
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
	// ��ѯ����ְҵ��Ϣ������֤������
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
			LogDebug(("�û�(%d)��Ч������(%d)�Ĵ�����Ϣ(%d)�޷��ظ�", nUserID, nCmdCode, nRetCode));
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
		LogDebug(("RegistRole::�û�(%d)��ɫ(%d)��Ч", nUserID, roleID));
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
		LogDebug(("UnRegistRole::�û�(%d)��Ч", nUserID));
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
		LogDebug(("�û�(%d)��Ч���Ҳ�����ɫ", nUserID));
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